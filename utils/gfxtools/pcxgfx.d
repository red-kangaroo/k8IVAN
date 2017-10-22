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
module pcxgfx is aliced;

import arsd.color;
import arsd.image;


// ////////////////////////////////////////////////////////////////////////// //
enum TileW = 16;
enum TileH = 16;


// ////////////////////////////////////////////////////////////////////////// //
bool isEmpty() (in auto ref Color c) { pragma(inline, true); return (c.a == 0 || (c.r == 255 && c.g == 0 && c.b == 255)); }

bool sameColor() (in auto ref Color c0, in auto ref Color c1) {
  //pragma(inline, true);
  if (c0.a == 0) return c1.isEmpty;
  if (c1.a == 0) return c0.isEmpty;
  return
    (c0.r>>2 == c1.r>>2) &&
    (c0.g>>2 == c1.g>>2) &&
    (c0.b>>2 == c1.b>>2);
}


// ////////////////////////////////////////////////////////////////////////// //
void xPutPixel (MemoryImage img, int x, int y, Color c) {
  if (c.isEmpty) c = Color(255, 0, 255);
  if (auto pim = cast(IndexedImage)img) {
    assert(!pim.hasAlpha);
    pim.setPixel(x, y, c);
  } else if (auto tim = cast(TrueColorImage)img) {
    tim.setPixel(x, y, c);
  } else {
    assert(0, "wtf?!");
  }
}


void copyTileTo (MemoryImage dimg, MemoryImage simg, int tx, int ty) {
  tx *= TileW;
  ty *= TileH;
  foreach (immutable dy; 0..TileH) {
    foreach (immutable dx; 0..TileW) {
      xPutPixel(dimg, tx+dx, ty+dy, simg.getPixel(tx+dx, ty+dy));
    }
  }
}


// ////////////////////////////////////////////////////////////////////////// //
bool isEmptyTile (MemoryImage i0, int tx, int ty) {
  tx *= TileW;
  ty *= TileH;
  foreach (immutable dy; 0..TileH) {
    foreach (immutable dx; 0..TileW) {
      if (!i0.getPixel(tx+dx, ty+dy).isEmpty) return false;
    }
  }
  return true;
}


bool isSameTile (MemoryImage i0, MemoryImage i1, int tx, int ty) {
  tx *= TileW;
  ty *= TileH;
  foreach (immutable dy; 0..TileH) {
    foreach (immutable dx; 0..TileW) {
      if (!sameColor(i0.getPixel(tx+dx, ty+dy), i1.getPixel(tx+dx, ty+dy))) return false;
    }
  }
  return true;
}


void saveTile (string fname, MemoryImage i0, int tx, int ty) {
  auto img = new TrueColorImage(TileW, TileH);
  tx *= TileW;
  ty *= TileH;
  foreach (immutable dy; 0..TileH) {
    foreach (immutable dx; 0..TileW) {
      auto c = i0.getPixel(tx+dx, ty+dy);
      if (c.isEmpty) c = Color.transparent;
      img.setPixel(dx, dy, c);
    }
  }
  writePng(fname, img);
}
