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
module pcxwriter is aliced;

import arsd.color;
import iv.vfs;


void writePCX (const(char)[] fname, MemoryImage pcx) {
  if (pcx is null) throw new Exception("cannot write null pcx image");
  if (pcx.width == 0 || pcx.height == 0) throw new Exception("cannot write empty pcx image");
  if (pcx.width > 32000 || pcx.height > 32000) throw new Exception("cannot write huge pcx image");
  if (auto pcx8 = cast(IndexedImage)pcx) {
    if (pcx8.hasAlpha) throw new Exception("cannot write pcx image with alpha");
    writePCX(VFile(fname, "w"), pcx);
    return;
  }
  throw new Exception("cannot write non-indexed pcx image");
}


void writePCX (VFile fl, MemoryImage pcx) {
  if (pcx is null) throw new Exception("cannot write null pcx image");
  if (pcx.width == 0 || pcx.height == 0) throw new Exception("cannot write empty pcx image");
  if (pcx.width > 32000 || pcx.height > 32000) throw new Exception("cannot write huge pcx image");
  if (auto pcx8 = cast(IndexedImage)pcx) {
    if (pcx8.hasAlpha) throw new Exception("cannot write pcx image with alpha");
    // write header
    fl.writeNum!ubyte(0x0a); // manufacturer
    fl.writeNum!ubyte(0x05); // version
    fl.writeNum!ubyte(0x01); // RLE-packed
    fl.writeNum!ubyte(0x08); // bpp
    // dimensions
    fl.writeNum!ushort(0);
    fl.writeNum!ushort(0);
    fl.writeNum!ushort(cast(ushort)(pcx8.width-1));
    fl.writeNum!ushort(cast(ushort)(pcx8.height-1));
    // dpi
    fl.writeNum!ushort(300);
    fl.writeNum!ushort(300);
    // 16-color palette
    foreach (immutable _; 0..16*3) fl.writeNum!ubyte(0);
    fl.writeNum!ubyte(0); // reserved
    fl.writeNum!ubyte(1); // colorplanes
    fl.writeNum!ushort(cast(ushort)pcx8.width); // bytes per line
    fl.writeNum!ushort(cast(ushort)1); // palette type
    // fill rest of the header with zeroes
    foreach (immutable _; 0..58) fl.writeNum!ubyte(0);

    int lineCount = 0;
    ubyte lineByte = 0;

    void putByte (ubyte b) {
      if (lineCount == 0) {
        lineByte = b;
        lineCount = 1;
        return;
      }
      assert(lineCount > 0 && lineCount <= 0x3f);
      if (lineCount == 0x3f || lineByte != b) {
        // flush RLE accumulator
        if (lineCount > 1 || lineByte >= 0xc0) {
          fl.writeNum!ubyte(cast(ubyte)(lineCount|0xc0));
        }
        fl.writeNum!ubyte(lineByte);
        lineCount = 1;
      } else {
        ++lineCount;
      }
      lineByte = b;
    }

    void flushLine () {
      assert(lineCount >= 0 && lineCount <= 0x3f);
      if (lineCount > 0) {
        // flush RLE accumulator
        if (lineCount > 1 || lineByte >= 0xc0) {
          fl.writeNum!ubyte(cast(ubyte)(lineCount|0xc0));
        }
        fl.writeNum!ubyte(lineByte);
        lineCount = 0;
      }
    }

    // write lines
    foreach (immutable y; 0..pcx8.height) {
      foreach (immutable x; 0..pcx8.width) {
        putByte(pcx8.data[y*pcx8.width+x]);
      }
      flushLine();
    }

    // write palette
    fl.writeNum!ubyte(12); // palette signature
    foreach (immutable idx; 0..256) {
      Color c = Color.black;
      if (idx < pcx8.palette.length) c = pcx8.palette[idx];
      fl.writeNum!ubyte(c.r);
      fl.writeNum!ubyte(c.g);
      fl.writeNum!ubyte(c.b);
    }

    // done
    return;
  }
  throw new Exception("cannot write non-indexed pcx image");
}
