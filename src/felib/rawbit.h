/*
 *
 *  Iter Vehemens ad Necem (IVAN)
 *  Copyright (C) Timo Kiviluoto
 *  Released under the GNU General
 *  Public License
 *
 *  See LICENSING which should be included
 *  along with this file for more details
 *
 */
#ifndef __FELIB_COLORBIT_H__
#define __FELIB_COLORBIT_H__

#include <stdio.h>

#include <cstdarg>
#include <map>
#include <vector>

#include "v2.h"

class outputfile;
class inputfile;
class bitmap;
class cachedfont;
class festring;


typedef std::map<col16, std::pair<cachedfont *, cachedfont *> > fontcache;


/* actually this is simple 256-color with palette PCX codec */
class rawbitmap {
public:
  rawbitmap (cfestring &FileName);
  rawbitmap (v2);
  ~rawbitmap ();
  void Save (cfestring &FileName);
  void MaskedBlit (bitmap *Bitmap, v2 Src, v2 Dest, v2 Border, packcol16 *Color) const;
  void MaskedBlit (bitmap *Bitmap, packcol16 *Color) const;

  void LIKE_PRINTF(5, 6) Printf (bitmap *Bitmap, v2 Pos, packcol16 Color, cchar *Format, ...) const;
  void LIKE_PRINTF(5, 6) PrintfColored (bitmap *Bitmap, v2 Pos, packcol16 Color, cchar *Format, ...) const;
  void LIKE_PRINTF(5, 6) PrintfUnshaded (bitmap *Bitmap, v2 Pos, packcol16 Color, cchar *Format, ...) const;
  void LIKE_PRINTF(5, 6) PrintfUnshadedColored (bitmap *Bitmap, v2 Pos, packcol16 Color, cchar *Format, ...) const;
  cachedfont *Colorize (cpackcol16 *Color, alpha BaseAlpha=255, cpackalpha *Alpha=0) const;
  bitmap *Colorize (v2 Pos, v2 Border, v2 Move, cpackcol16 *Color, alpha BaseAlpha=255,
    cpackalpha *Alpha=0, cuchar *RustData=0, truth AllowReguralColors=true) const;
  v2 GetSize () const { return Size; }

  void AlterGradient (v2 Pos, v2 Border, int MColor, int Amount, truth Clip);
  void SwapColors (v2 Pos, v2 Border, int Color1, int Color2);
  void Roll (v2 Pos, v2 Border, v2 Move, paletteindex *TempBuffer);

  void CreateFontCache (packcol16 Color);
  static truth IsMaterialColor (int Color) { return Color >= 192; }
  static int GetMaterialColorIndex (int Color) { return (Color-192) >> 4; }
  int GetMaterialColorIndex (int X, int Y) const { return (PaletteBuffer[Y][X]-192) >> 4; }
  truth IsTransparent (v2 Pos) const;
  truth IsMaterialColor1 (v2 Pos) const;
  v2 RandomizeSparklePos (cv2 *ValidityArray, v2 *PossibleBuffer, v2 Pos, v2 Border, int ValidityArraySize, int SparkleFlags) const;
  void CopyPaletteFrom (rawbitmap *Bitmap);
  void PutPixel (v2 Pos, paletteindex Color) { PaletteBuffer[Pos.Y][Pos.X] = Color; }
  paletteindex GetPixel (v2 Pos) const { return PaletteBuffer[Pos.Y][Pos.X]; }
  void Clear ();
  void NormalBlit (rawbitmap *Bitmap, v2 Src, v2 Dest, v2 Border, int Flags=0) const;

  const uChar *getPalette () const { return Palette; }
  const uChar *getBuffer () const { return PaletteBuffer[0]; }

protected:
  static truth strHasCtlCodes (cchar *str);
  void printfColoredVA (bitmap *bmp, v2 pos, packcol16 clr, truth shaded, cchar *fmt, va_list vap) const;
  void printstrColored (bitmap *bmp, v2 pos, packcol16 clr, truth shaded, cchar *str) const;

protected:
  void loadFromFile (FILE *fl);
  void loadPCX (FILE *fl);
#ifdef HAVE_LIBPNG
  void loadPNG (FILE *fl);
#endif

  static festring curfile;
  static unsigned char getb (FILE* fl);

protected:
  v2 Size;
  uChar *Palette;
  paletteindex **PaletteBuffer;
  fontcache FontCache;
};


#endif
