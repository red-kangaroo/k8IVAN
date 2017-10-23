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
import iv.strex;
import iv.vfs.io;


// ////////////////////////////////////////////////////////////////////////// //
void main (string[] args) {
  foreach (string fname; args[1..$]) {
    if (!fname.endsWithCI(".pcx")) { writeln("SKIP: ", fname); continue; }
    writeln(fname, " -> ", fname.setExtension(".png"));
    auto pic = MemoryImage.fromImageFile(fname);
    writePng(fname.setExtension(".png"), pic);
  }
}
