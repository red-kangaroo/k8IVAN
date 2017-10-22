/* coded by Ketmar // Invisible Vector <ketmar@ketmar.no-ip.org>
 * Understanding is not required. Only obedience.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */
module idatool is aliced;

import std.path : buildPath;

import iv.cmdcon;
import iv.strex;
import iv.vfs.io;

import parser;


// ////////////////////////////////////////////////////////////////////////// //
__gshared string commIvanRoot = "/home/ketmar/k8prj/I.V.A.N./_community/_git";
__gshared string k8IvanRoot = "/home/ketmar/k8prj/I.V.A.N.";
__gshared bool optIgnoreDups = false;


// ////////////////////////////////////////////////////////////////////////// //
VFile findAndOpenDat(string type) (const(char)[] fname) if (type == "k8" || type == "comm") {
  try {
         static if (type == "k8") auto fl = VFile(buildPath(k8IvanRoot, "script", fname));
    else static if (type == "comm") auto fl = VFile(buildPath(commIvanRoot, "script", fname));
    else static assert(0, "i am just a safeguard");
    return fl;
  } catch (Exception e) {}
  try {
    auto fl = VFile(buildPath("script", fname));
    return fl;
  } catch (Exception e) {}
  throw new Exception("dat file '"~fname.idup~"' not found");
}


// ////////////////////////////////////////////////////////////////////////// //
SimpleDefList loadlist (VFile fl) {
  auto res = new SimpleDefList();
  res.parse(fl, optIgnoreDups);
  return res;
}


// ////////////////////////////////////////////////////////////////////////// //
/*
SimpleDefList loadk8listdir (string path) {
  import std.file;
  auto defs = new SimpleDefList();
  foreach (DirEntry de; dirEntries(path, "*.dat", SpanMode.shallow)) {
    if (!de.isFile) continue;
    defs.parse(VFile(de.name));
  }
  if (defs.maindef is null) assert(0, "k8: main def not found");
  return defs;
}
*/


// ////////////////////////////////////////////////////////////////////////// //
SimpleDefList loadk8list (VFile fl) {
  import std.file;
  auto defs = new SimpleDefList();
  defs.parse(new IvanParser(fl), optIgnoreDups);
  if (defs.maindef is null) assert(0, "k8: main def not found");
  return defs;
}


// ////////////////////////////////////////////////////////////////////////// //
void diffDefs (SimpleDefList d0, SimpleDefList d1) {
  bool[string] hit;
  foreach (SimpleDef def; d0.defs.byValue) {
    if (def.name !in d1.defs) continue;
    hit[def.name] = true;
    auto diff = def.diff(d1.defs[def.name]);
    if (diff.length) {
      writeln("======== ", def.name, " ========");
      foreach (string s; diff) writeln(s);
      writeln;
    }
  }
}


// ////////////////////////////////////////////////////////////////////////// //
void diffItems () {
  auto itdefs = loadlist(findAndOpenDat!"comm"("item.dat"));
  auto mydefs = loadk8list(findAndOpenDat!"k8"("item.dat"));
  diffDefs(mydefs, itdefs);
}


// ////////////////////////////////////////////////////////////////////////// //
void diffChars () {
  auto itdefs = loadlist(findAndOpenDat!"comm"("char.dat"));
  auto mydefs = loadk8list(findAndOpenDat!"k8"("char.dat"));
  diffDefs(mydefs, itdefs);
}


// ////////////////////////////////////////////////////////////////////////// //
void checkBrokenConfigs (SimpleDefList list) {
  foreach (SimpleDef def; list.defs) {
    if (def.name == "potion") continue; // potions always morphed into brokenbottle
    foreach (SimpleConfig cfg; def.configs) {
      if (cfg.broken) continue;
      // skip abstract definitions
      auto fld = cfg["IsAbstract"];
      if (fld !is null && fld.asBool) continue;
      fld = list[def.name, cfg.name, "CanBeBroken"];
      if (fld is null || !fld.asBool) continue;
      if (!def.hasBrokenConfig(cfg.name)) writeln("WARNING: NO BROKEN CONFIG FOR ", def.name, " : ", cfg.name);
      //writeln(def.name, " : ", cfg.name);
    }
  }
}


// ////////////////////////////////////////////////////////////////////////// //
void main (string[] args) {
  vfsIgnoreCaseDisk = true;

  conRegVar!commIvanRoot("comm_root", "community fork root dir");
  conRegVar!k8IvanRoot("k8_root", "k8 fork root dir");
  conRegVar!optIgnoreDups("p_ignore_dups", "don't fail on duplicates");

  conProcessQueue(); // load config
  conProcessArgs!true(args);

  enum Action { None, DiffItems, DiffChars, CheckBrokenComm, CheckBrokenK8 }

  Action action = Action.None;

  foreach (string arg; args[1..$]) {
    switch (arg) {
      case "diff-items":
        if (action != Action.None) assert(0, "duplicate action");
        action = Action.DiffItems;
        break;
      case "diff-chars":
        if (action != Action.None) assert(0, "duplicate action");
        action = Action.DiffChars;
        break;
      case "check-broken-comm":
        if (action != Action.None) assert(0, "duplicate action");
        action = Action.CheckBrokenComm;
        break;
      case "check-broken-k8":
        if (action != Action.None) assert(0, "duplicate action");
        action = Action.CheckBrokenK8;
        break;
      default:
        assert(0, "unknown command: '"~arg~"'");
    }
  }

  if (action == Action.None) assert(0, "sorry, i don't know what to do.");

  writeln("k8 I.V.A.N. root  : ", k8IvanRoot);
  writeln("comm I.V.A.N. root: ", commIvanRoot);

  final switch (action) {
    case Action.None: assert(0, "the thing that should not be");
    case Action.DiffItems:
      writeln("************* DIFF ITEMS *************");
      diffItems();
      break;
    case Action.DiffChars:
      writeln("************* DIFF CHARS *************");
      diffChars();
      break;
    case Action.CheckBrokenComm:
      writeln("************* CHECKING CHARS IN COMM FORK *************");
      loadlist(findAndOpenDat!"comm"("char.dat"));
      writeln("************* CHECKING BROKEN ITEM CONFIGS IN COMM FORK *************");
      checkBrokenConfigs(loadlist(findAndOpenDat!"comm"("item.dat")));
      break;
    case Action.CheckBrokenK8:
      writeln("************* CHECKING CHARS IN K8 FORK *************");
      loadk8list(findAndOpenDat!"k8"("char.dat"));
      writeln("************* CHECKING BROKEN ITEM CONFIGS IN K8 FORK *************");
      checkBrokenConfigs(loadk8list(findAndOpenDat!"k8"("item.dat")));
      break;
  }
}
