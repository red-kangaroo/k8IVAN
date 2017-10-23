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
module zgfxcopy is aliced;

import std.conv : to;
import std.format : format;
import std.path : setExtension;

import arsd.image;

import iv.cmdcon;
import iv.vfs.io;

import pcxwriter;
import pcxgfx;


// ////////////////////////////////////////////////////////////////////////// //
__gshared ubyte optTileWidth = 16;
__gshared ubyte optTileHeight = 16;


// ////////////////////////////////////////////////////////////////////////// //
void main (string[] args) {
  conRegVar!optTileWidth(1, 255, "tile_width", "tile width");
  conRegVar!optTileHeight(1, 255, "tile_height", "tile height");

  conProcessQueue(); // load config
  conProcessArgs!true(args);

  if (args.length < 7) {
    assert(0, "usage: rdmd zgfxcopy.d dstimage <destx desty srcimage srcx srcy>+");
  }
  // dest image
  auto destpic = MemoryImage.fromImageFile(args[1]);

  int argnum = 2;
  while (argnum+4 < args.length) {
    int destx = to!int(args[argnum+0]);
    int desty = to!int(args[argnum+1]);

    // src image
    auto srcpic = MemoryImage.fromImageFile(args[argnum+2]);
    scope(exit) delete srcpic;

    int srcx = to!int(args[argnum+3]);
    int srcy = to!int(args[argnum+4]);

    foreach (immutable dy; 0..optTileHeight) {
      foreach (immutable dx; 0..optTileWidth) {
        auto c = srcpic.getPixel(srcx+dx, srcy+dy);
        if (c.isEmpty) c = Color(255, 0, 255);
        destpic.setPixel(destx+dx, desty+dy, c);
      }
    }

    argnum += 5;
  }

  //writePCX(args[1].setExtension(".new.pcx"), destpic);
  writePng(args[1].setExtension(".new.png"), destpic);
}
