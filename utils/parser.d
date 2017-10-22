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
module parser is aliced;

import iv.strex;
import iv.unarray;
import iv.vfs.io;


// ////////////////////////////////////////////////////////////////////////// //
struct Loc {
  int line, col;
  string fname;

public:
  string toString () const {
    import std.format : format;
    if (fname.length) return "'%s' (%d,%d)".format(fname, line, col);
    return "'?' (%d,%d)".format(line, col);
  }
}


struct Token {
public:
  enum TT { EOF, IdNum, Str, Delim }

public:
  string toString () const {
    import std.format : format;
    return "Token(%s [%s])".format(type, loc.toString);
  }

public:
  TT type;
  string value;
  Loc loc;

public:
  @property bool isEOF () const nothrow @safe @nogc { return (type == TT.EOF); }
  @property bool isStr () const nothrow @safe @nogc { return (type == TT.Str); }
  @property bool isIdNum () const nothrow @safe @nogc { return (type == TT.IdNum); }
  @property bool isDelim () const nothrow @safe @nogc { return (type == TT.Delim); }

  @property bool isIdNum (const(char)[] s) const nothrow @safe @nogc { return (type == TT.IdNum && value == s); }
  @property bool isDelim (const(char)[] s) const nothrow @safe @nogc { return (type == TT.Delim && value == s); }

  @property string unquote () const nothrow {
    if (!isStr) return value;
    assert(value.length >= 2 && value[0] == '"' && value[$-1] == '"');
    string res;
    usize pos = 1;
    while (pos < value.length-1) {
      if (value[pos] == '\\') {
        ++pos;
        switch (value[pos++]) {
          case 'n': res ~= '\n'; break;
          case 't': res ~= '\t'; break;
          default: res ~= value[pos-1]; break;
        }
      } else {
        res ~= value[pos++];
      }
    }
    return res;
  }

  bool opEquals() (in auto ref Token b) const nothrow @safe @nogc {
    if (type != b.type) return false;
    if (type == TT.EOF) return true;
    return (value == b.value);
  }
}


// ////////////////////////////////////////////////////////////////////////// //
class SimpleIvanParser {
private:
  VFile fl;
  char lastChar, nextChar;

private:
  void skipChar () {
    if (lastChar == 0) return;
    if (lastChar == '\n') { loc.col = 1; ++loc.line; } else ++loc.col;
    lastChar = nextChar;
    if (fl.rawRead((&nextChar)[0..1]).length == 0) { nextChar = 0; return; }
    if (nextChar == 0) nextChar = ' ';
  }

private:
  void openFile (VFile afl) {
    fl = afl;
    lastChar = ' ';
    fl.rawReadExact((&nextChar)[0..1]);
    if (nextChar == 0xEF) {
      skipChar();
      assert(nextChar == 0xBB);
      skipChar();
      assert(nextChar == 0xBF);
      skipChar();
    }
    skipChar();
    loc.col = 1;
    loc.line = 1;
    loc.fname = fl.name.idup;
    nextToken();
  }

public:
  Token tok;
  Loc loc;

public:
  this (VFile afl) {
    openFile(afl);
  }

  final void error (const(char)[] msg) {
    import std.format : format;
    throw new Exception("parser error in '%s' at (%d,%d): %s".format(loc.fname, loc.line, loc.col, msg));
  }

  final void tokerror (const(char)[] msg) {
    import std.format : format;
    throw new Exception("parser error in '%s' at (%d,%d): %s".format(tok.loc.fname, tok.loc.line, tok.loc.col, msg));
  }

  final void errorAt() (in auto ref Loc aloc, const(char)[] msg) {
    import std.format : format;
    throw new Exception("parser error in '%s' at (%d,%d): %s".format(aloc.fname, aloc.line, aloc.col, msg));
  }

  final @property bool eof () const nothrow @safe @nogc { return tok.isEOF; }

  final void expectDelim (const(char)[] s) {
    if (!tok.isDelim || tok.value != s) tokerror("delimiter '"~s~"' expected");
    nextToken();
  }

  final void expectIdNum (const(char)[] s) {
    if (!tok.isIdNum || tok.value != s) tokerror("identifier '"~s~"' expected");
    nextToken();
  }

  final string expectIdNum () {
    if (!tok.isIdNum) tokerror("delimiter expected");
    string res = tok.value;
    nextToken();
    return res;
  }

  final bool eatIdNum (const(char)[] s) { if (tok.isIdNum && tok.value == s) { nextToken(); return true; } return false; }
  final bool eatDelim (const(char)[] s) { if (tok.isDelim && tok.value == s) { nextToken(); return true; } return false; }


  final void skipSpaces () {
    if (lastChar == 0) return;
    while (lastChar) {
      if (lastChar <= ' ') { skipChar(); continue; }
      if (lastChar == '/' && nextChar == '/') {
        while (lastChar && lastChar != '\n') skipChar();
        continue;
      }
      if (lastChar == '/' && nextChar == '*') {
        skipChar();
        skipChar();
        while (lastChar) {
          if (lastChar == '*' && nextChar == '/') { skipChar(); skipChar(); break; }
          skipChar();
        }
        continue;
      }
      break;
    }
  }

  void nextToken () {
    skipSpaces();
    tok = Token(Token.TT.EOF, null, loc);
    if (lastChar == 0) return; // EOF
    if (lastChar == '\'') error("invalid quote");
    // string?
    if (lastChar == '"') {
      tok.type = Token.TT.Str;
      skipChar();
      tok.value ~= '"';
      while (lastChar) {
        if (lastChar == '\\') { tok.value ~= lastChar; skipChar(); assert(lastChar != 0); }
        else if (lastChar == '"') break;
        tok.value ~= lastChar;
        skipChar();
      }
      assert(lastChar == '"');
      tok.value ~= '"';
      skipChar();
      return;
    }
    // id/num
    if (lastChar >= 128 || isalnum(lastChar) || lastChar == '_') {
      tok.type = Token.TT.IdNum;
      while (lastChar >= 128 || isalnum(lastChar) || lastChar == '_') {
        tok.value ~= lastChar;
        skipChar();
      }
      return;
    }
    // delimiter
    tok.type = Token.TT.Delim;
    if (lastChar == '=' && nextChar == '=') { tok.value = "=="; skipChar(); skipChar(); return; }
    if (lastChar == ':' && nextChar == '=') { tok.value = ":="; skipChar(); skipChar(); return; }
    if (lastChar == '<' && nextChar == '<') { tok.value = "<<"; skipChar(); skipChar(); return; }
    if (lastChar == '>' && nextChar == '>') { tok.value = ">>"; skipChar(); skipChar(); return; }
    if (lastChar == '<' && nextChar == '=') { tok.value = "<="; skipChar(); skipChar(); return; }
    if (lastChar == '>' && nextChar == '=') { tok.value = ">="; skipChar(); skipChar(); return; }
    if (lastChar == '!' && nextChar == '=') { tok.value = "!="; skipChar(); skipChar(); return; }
    if (lastChar == '<' && nextChar == '>') { tok.value = "<>"; skipChar(); skipChar(); return; }
    tok.value = ""~lastChar;
    skipChar();
  }
}


// ////////////////////////////////////////////////////////////////////////// //
class IvanParser : SimpleIvanParser {
private:
  static struct State {
    VFile fl;
    char lastChar, nextChar;
    Loc loc;
  }

private:
  State[] mStateStack;
  string basePath;

private:
  bool popState () {
    if (mStateStack.length == 0) return false;
    fl = mStateStack[$-1].fl;
    lastChar = mStateStack[$-1].lastChar;
    nextChar = mStateStack[$-1].nextChar;
    loc = mStateStack[$-1].loc;
    mStateStack.unsafeArraySetLength(mStateStack.length-1);
    return true;
  }

  void pushState (const(char)[] newfile) {
    if (mStateStack.length > 128) throw new Exception("too many includes");
    assert(newfile.length);
    mStateStack.unsafeArraySetLength(mStateStack.length+1);
    mStateStack[$-1].fl = fl;
    mStateStack[$-1].lastChar = lastChar;
    mStateStack[$-1].nextChar = nextChar;
    mStateStack[$-1].loc = loc;
    openFile(VFile(basePath~newfile));
  }

public:
  this (VFile afl) {
    import std.path : dirName;
    super(afl);
    basePath = afl.name.dirName.idup;
    if (basePath.length && basePath[$-1] != '/') basePath ~= '/';
  }

  override void nextToken () {
    super.nextToken();
    for (;;) {
      // eof: pop include
      if (tok.isEOF) {
        if (!popState()) return;
        super.nextToken();
        continue;
      }
      // include
      if (tok.isIdNum("Include")) {
        super.nextToken(); // get file name
        if (!tok.isStr) tokerror("string expected");
        auto fname = tok.unquote;
        super.nextToken(); // get ";"
        if (!tok.isDelim(";")) tokerror("';' expected");
        pushState(fname);
        continue;
      }
      // normal token
      break;
    }
  }
}


// ////////////////////////////////////////////////////////////////////////// //
class SimpleField {
  Loc loc;
  string name;
  Token[] value; // just tokens

  this (SimpleIvanParser par) {
    // "}" added to list
    void collectCurly () {
      int level = 1;
      while (level) {
        if (par.tok.isEOF) par.error("unexpected EOF");
        value ~= par.tok;
             if (par.tok.isDelim("{")) ++level;
        else if (par.tok.isDelim("}")) --level;
        par.nextToken();
      }
    }

    void collectOne () {
      while (!par.eof && !par.tok.isDelim(";")) {
        //{ import iv.vfs.io; writeln("  ONE: ", par.tok); }
        value ~= par.tok;
        par.nextToken();
        if (value[$-1].isDelim("{")) {
          collectCurly();
          return;
        }
      }
      par.expectDelim(";");
      //{ import iv.vfs.io; writeln("  ONEendloc=", par.tok.loc, "; ", name); }
    }

    loc = par.tok.loc;
    name = par.expectIdNum();
    //{ import iv.vfs.io; writeln("fld: loc=", par.tok.loc, "; ", name); }
    if (!par.tok.isDelim) par.expectDelim("=");
    if (par.eatDelim("=")) {
      // array?
      if (par.tok.isDelim("{")) {
        // skip counter and comma
        value ~= par.tok;
        par.expectDelim("{");
        par.expectIdNum();
        par.expectDelim(",");
        collectCurly();
      } else {
        collectOne();
      }
    } else if (par.eatDelim("==")) {
      collectOne();
    } else if (par.eatDelim(":=")) {
      value ~= par.tok;
      par.expectDelim("{");
      collectCurly();
    } else {
      par.expectDelim("=");
    }
    //{ import iv.vfs.io; writeln("fld: ENDloc=", par.tok.loc, "; ", name); }
  }

  override bool opEquals (in Object o) const {
    if (o is null) return false;
    if (o is this) return true;
    if (auto op = cast(SimpleField)o) return (name == op.name && value == op.value);
    return false;
  }

  override string toString () const {
    string res = "<";
    foreach (immutable idx, const ref tk; value) {
      if (idx != 0) res ~= " ";
      res ~= tk.value;
    }
    res ~= ">";
    return res;
  }

  @property bool asBool () const nothrow @safe @nogc {
    if (value.length == 0) return false;
    if (value.length > 1) return true;
    if (value[0].isIdNum("false")) return false;
    if (value[0].isIdNum("0")) return false;
    return true;
  }
}


// ////////////////////////////////////////////////////////////////////////// //
// "{" should be already parsed; will stop after eating ending "}"
class SimpleConfig {
  bool base;
  bool broken;
  Loc loc;
  string name;
  SimpleField[string] fields;

  this () { name = "<default>"; }

  this (SimpleIvanParser par, string aname, bool ignoredups=false) {
    name = aname;
    loc = par.tok.loc;
    for (;;) {
      if (par.eof) par.expectDelim("}");
      if (par.tok.isDelim("{")) par.tokerror("wtf?!");
      if (par.eatDelim("}")) break;
      parseField(par, ignoredups);
    }
  }

  void parseField (SimpleIvanParser par, bool ignoredups=false) {
    auto fld = new SimpleField(par);
    if (fld.name == "CanBeBurned") return;
    if (auto ofp = fld.name in fields) {
      if (*ofp == fld) {
        writeln("WARNING: ", fld.loc, ": duplicate field '"~fld.name~"' (prev vas at "~ofp.loc.toString~")");
      } else {
        if (ignoredups) {
          writeln("WARNING: ", fld.loc, ": duplicate DIFFERENT field '"~fld.name~"' (prev vas at "~ofp.loc.toString~")");
        } else {
          par.errorAt(fld.loc, "duplicate field '"~fld.name~"' (prev vas at "~ofp.loc.toString~")");
        }
      }
    }
    fields[fld.name] = fld;
  }

  override bool opEquals (in Object o) const {
    if (o is null) return false;
    if (o is this) return true;
    if (auto op = cast(SimpleConfig)o) {
      if (name != op.name) return false;
      bool[string] hit;
      foreach (const SimpleField fld; op.fields.byValue) {
        if (auto myfldp = fld.name in fields) {
          if (*myfldp != fld) return false;
          hit[fld.name] = true;
        } else {
          return false;
        }
      }
      foreach (const SimpleField fld; fields.byValue) {
        if (fld.name !in hit) return false;
      }
      return true;
    }
    return false;
  }

  string[] diff (in SimpleConfig other) const {
    assert(other !is null);
    string[] res;
    if (name != other.name) { res ~= "* "~name; res ~= "* "~other.name; }
    // compare fields
    bool[string] hit;
    foreach (const SimpleField myfld; fields.byValue) {
      if (myfld.name !in other.fields) continue;
      hit[myfld.name] = true;
      if (myfld != other.fields[myfld.name]) {
        res ~= "* "~myfld.name;
        res ~= "    "~myfld.toString;
        res ~= "    "~other.fields[myfld.name].toString;
      }
    }
    // remove unhit fields
    foreach (const SimpleField myfld; fields.byValue) {
      if (myfld.name !in hit) {
        res ~= "- "~myfld.name;
        res ~= "    "~myfld.toString;
      }
    }
    // add other unhit fields
    foreach (const SimpleField itfld; other.fields.byValue) {
      if (itfld.name !in fields) {
        res ~= "+ "~itfld.name;
        res ~= "    "~itfld.toString;
      }
    }
    return res;
  }

  inout(SimpleField) opIndex (const(char)[] afldname) inout nothrow @safe @nogc {
    if (auto fp = afldname in fields) return cast(typeof(return))(*fp);
    return null;
  }
}


// ////////////////////////////////////////////////////////////////////////// //
class SimpleDef {
  bool base;
  Loc loc;
  string name;
  SimpleConfig[string] configs;

  //this (string aname) { name = aname; }
  this (SimpleIvanParser par, bool ignoredups=false) {
    loc = par.tok.loc;
    name = par.expectIdNum();
    auto defcfg = new SimpleConfig();
    defcfg.loc = par.tok.loc;
    defcfg.base = true;
    par.expectDelim("{");
    configs["<default>"] = defcfg;
    for (;;) {
      if (par.eof) par.expectDelim("}");
      if (par.tok.isDelim("{")) par.tokerror("wtf?!");
      if (par.eatDelim("}")) break;
      if (par.eatIdNum("Config")) {
        bool broken = false;
        string cfg = par.expectIdNum();
        if (par.eatDelim("|")) {
          if (cfg == "BROKEN") {
            cfg = par.expectIdNum();
          } else {
            par.expectIdNum("BROKEN");
          }
          cfg = "BROKEN|"~cfg;
          broken = true;
        } else if (cfg == "BROKEN") {
          cfg ~= "|<default>";
          broken = true;
        }
        par.expectDelim(";");
        par.expectDelim("{");
        auto cc = new SimpleConfig(par, cfg, ignoredups);
        cc.broken = broken;
        if (auto ocp = cc.name in configs) {
          if (ignoredups) {
            writeln("WARNING: ", cc.loc, ": duplicate DIFFERENT config '"~cc.name~"' (prev vas at "~ocp.loc.toString~")");
          } else {
            par.errorAt(cc.loc, "duplicate config '"~cc.name~"' (prev vas at "~ocp.loc.toString~")");
          }
        }
        configs[cc.name] = cc;
        //{ import iv.vfs.io; writeln("ccend: loc=", par.loc, "; ", cc.name); }
      } else {
        defcfg.parseField(par, ignoredups);
      }
    }
  }

  override bool opEquals (in Object o) const {
    if (o is null) return false;
    if (o is this) return true;
    if (auto op = cast(SimpleDef)o) {
      if (name != op.name) return false;
      bool[string] hit;
      foreach (const SimpleConfig cfg; op.configs.byValue) {
        if (auto mycfgp = cfg.name in configs) {
          if (*mycfgp != cfg) return false;
          hit[cfg.name] = true;
        } else {
          return false;
        }
      }
      foreach (const SimpleConfig cfg; configs.byValue) {
        if (cfg.name !in hit) return false;
      }
      return true;
    }
    return false;
  }

  string[] diff (in SimpleDef other) const {
    assert(other !is null);
    string[] res;
    if (name != other.name) { res ~= "- "~name; res ~= "+ "~other.name; }
    // compare configs
    bool[string] hit;
    foreach (const SimpleConfig mycfg; configs.byValue) {
      if (mycfg.name !in other.configs) continue;
      hit[mycfg.name] = true;
      if (mycfg != other.configs[mycfg.name]) {
        res ~= "*--* "~mycfg.name~" *--*";
        res ~= mycfg.diff(other.configs[mycfg.name]);
      }
    }
    // remove unhit configs
    foreach (const SimpleConfig mycfg; configs.byValue) {
      if (mycfg.name !in hit) {
        res ~= "--- "~mycfg.name;
      }
    }
    // add other unhit fields
    foreach (const SimpleConfig itcfg; other.configs.byValue) {
      if (itcfg.name !in configs) {
        res ~= "+++ "~itcfg.name;
      }
    }
    return res;
  }

  @property bool hasConfig (const(char)[] aconfig) const nothrow @safe @nogc {
    if (aconfig.length == 0) aconfig = "<default>";
    return ((aconfig in configs) !is null);
  }

  @property bool hasBrokenConfig (const(char)[] aconfig) const nothrow @safe {
    if (aconfig.length == 0) aconfig = "<default>";
    if (auto cp = aconfig in configs) { if (cp.broken) return true; }
    auto bkn = "BROKEN|"~aconfig;
    if (auto cp = bkn in configs) return cp.broken;
    if (auto cp = "BROKEN|<default>" in configs) return cp.broken;
    return false;
  }

  inout(SimpleField) opIndex (const(char)[] aconfig, const(char)[] afldname) inout nothrow @safe @nogc {
    if (aconfig.length == 0) aconfig = "<default>";
    if (auto cp = aconfig in configs) {
      if (auto fld = (*cp)[afldname]) return fld;
    }
    // check default config
    if (aconfig != "<default>") {
      if (auto cp = "<default>" in configs) {
        if (auto fld = (*cp)[afldname]) return fld;
      }
    }
    return null;
  }
}


// ////////////////////////////////////////////////////////////////////////// //
class SimpleDefList {
  SimpleDef[string] defs;
  SimpleDef maindef;

  this () {}

  void parse (VFile fl, bool ignoredups=false) { parse(new SimpleIvanParser(fl), ignoredups); }

  void parse (SimpleIvanParser par, bool ignoredups=false) {
    while (!par.eof) {
      auto def = new SimpleDef(par, ignoredups);
      if (auto odp = def.name in defs) {
        if (ignoredups) {
          writeln("WARNING: ", def.loc, ": duplicate DIFFERENT definition '"~def.name~"' (prev vas at "~odp.loc.toString~")");
        } else {
          par.errorAt(def.loc, "duplicate definition '"~def.name~"' (prev vas at "~odp.loc.toString~")");
        }
      }
      defs[def.name] = def;
      if (def.name == "character" || def.name == "item") {
        def.base = true;
        maindef = def;
      }
    }
  }

  override bool opEquals (in Object o) const {
    if (o is null) return false;
    if (o is this) return true;
    if (auto op = cast(SimpleDefList)o) {
      bool[string] hit;
      foreach (const SimpleDef def; op.defs.byValue) {
        if (auto mydefp = def.name in defs) {
          if (*mydefp != def) return false;
          hit[def.name] = true;
        } else {
          return false;
        }
      }
      foreach (const SimpleDef def; defs.byValue) {
        if (def.name !in hit) return false;
      }
      return true;
    }
    return false;
  }

  SimpleField opIndex (const(char)[] adefname, const(char)[] aconfig, const(char)[] afldname) nothrow @safe @nogc {
    SimpleField tryfld (SimpleDef def) {
      if (def is null) return null;
      // try field in given config
      auto fld = def[aconfig, afldname];
      if (fld !is null) return fld;
      // find field in default config
      return def["", afldname];
    }
    // find field in given def
    if (auto dp = adefname in defs) { if (auto fld = tryfld(*dp)) return fld; }
    // find field in base def
    return tryfld(maindef);
  }
}
