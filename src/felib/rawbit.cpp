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
#include <cstdarg>

#ifdef HAVE_LIBPNG
# include <png.h>
#endif

#include "allocate.h"
#include "rawbit.h"
#include "bitmap.h"
#include "fesave.h"
#include "femath.h"


// ////////////////////////////////////////////////////////////////////////// //
festring rawbitmap::curfile;


// ////////////////////////////////////////////////////////////////////////// //
unsigned char rawbitmap::getb (FILE* fl) {
  unsigned char c = 0;
  if (!fl) ABORT("Cannot read from nothing!");
  if (fread(&c, 1, 1, fl) != 1) ABORT("Error reading file '%s'!", curfile.CStr());
  return c;
}


// ////////////////////////////////////////////////////////////////////////// //
void rawbitmap::loadPCX (FILE *fl) {
  if (fseek(fl, 0, SEEK_SET) == -1) ABORT("Error reading file '%s'!", curfile.CStr());
  // simple checks
  if (getb(fl) != 10) ABORT("Invalid bitmap format: %s!", curfile.CStr());
  int ver = getb(fl);
  switch (ver) {
    case 4: case 5: break;
    default: ABORT("Invalid bitmap version: %s!", curfile.CStr());
  }
  truth packed = true;
  switch (getb(fl)) {
    case 0: packed = false; break;
    case 1: break;
    default: ABORT("Invalid bitmap encoding: %s!", curfile.CStr());
  }
  if (getb(fl) != 8) ABORT("Invalid bitmap BPP: %s!", curfile.CStr());
  // dimensions
  // x0
  int x0 = getb(fl);
  x0 |= getb(fl)<<8;
  // y0
  int y0 = getb(fl);
  y0 |= getb(fl)<<8;
  // x1
  int x1 = getb(fl);
  x1 |= getb(fl)<<8;
  // y1
  int y1 = getb(fl);
  y1 |= getb(fl)<<8;
  // calculate dimensions
  int wdt = (x1-x0)+1;
  int hgt = (y1-y0)+1;
  // check dimensions
  if (wdt < 1 || wdt > 32700) ABORT("Invalid bitmap width: %s!", curfile.CStr());
  if (hgt < 1 || hgt > 32700) ABORT("Invalid bitmap height: %s!", curfile.CStr());
  // skip dpi
  for (int skipcount = 4; skipcount > 0; --skipcount) getb(fl);
  // skip 16-color palette and reserved
  for (int skipcount = 16*3+1; skipcount > 0; --skipcount) getb(fl);
  // check colorplanes
  if (getb(fl) != 1) ABORT("Invalid bitmap colorplanes: %s!", curfile.CStr());
  // bytes per line
  int bpl = getb(fl);
  bpl |= getb(fl)<<8;
  if (bpl < wdt) ABORT("Invalid bitmap bytes per line: %s!", curfile.CStr());
  int hdrread = 4+4*2+2*2+16*3+1+1+2;
  // check palette type
  if (ver == 4) {
    if (getb(fl) != 1) ABORT("Invalid bitmap palette type: %s!", curfile.CStr());
    if (getb(fl) != 0) ABORT("Invalid bitmap palette type: %s!", curfile.CStr());
    hdrread += 2;
  }
  // skip rest of the header
  while (hdrread < 128) { getb(fl); ++hdrread; }
  // load bitmap
  Size.X = wdt;
  Size.Y = hgt;
  Alloc2D(PaletteBuffer, Size.Y, Size.X);
  for (int y = 0; y < hgt; ++y) {
    int cnt = 0, b = 0;
    for (int x = 0; x < bpl; ++x) {
      if (cnt == 0) {
        b = (unsigned char)getb(fl);
        if (packed && b >= 0xc0) {
          cnt = b&0x3f;
          if (cnt == 0) {
            delete [] PaletteBuffer;
            ABORT("Invalid compressed bitmap: %s!", curfile.CStr());
          }
          b = (unsigned char)getb(fl);
        } else {
          cnt = 1;
        }
      }
      if (x < wdt) {
        paletteindex *Buffer = PaletteBuffer[0];
        Buffer += (y*wdt)+x;
        *Buffer = (paletteindex)b;
      }
      --cnt;
    }
  }
  // load palette
  if (fseek(fl, -769, SEEK_END) == -1) ABORT("Error reading file '%s'!", curfile.CStr());
  // check signature
  if (getb(fl) != 12) ABORT("Invalid bitmap palette: %s!", curfile.CStr());
  Palette = new uChar[768];
  if (fread(reinterpret_cast<char *>(Palette), 1, 768, fl) != 768) ABORT("Cannot read bitmap palette: %s!", curfile.CStr());
}


// ////////////////////////////////////////////////////////////////////////// //
#ifdef HAVE_LIBPNG
void rawbitmap::loadPNG (FILE *fl) {
  if (fseek(fl, 0, SEEK_SET) == -1) ABORT("Error reading file '%s'!", curfile.CStr());

  png_structp PNGStruct = png_create_read_struct(PNG_LIBPNG_VER_STRING, 0, 0, 0);
  if (!PNGStruct) ABORT("Couldn't read PNG file %s!", curfile.CStr());

  png_infop PNGInfo = png_create_info_struct(PNGStruct);
  if (!PNGInfo) ABORT("Couldn't read PNG file %s!", curfile.CStr());

  if (setjmp(png_jmpbuf(PNGStruct)) != 0) ABORT("Couldn't read PNG file %s!", curfile.CStr());

  png_init_io(PNGStruct, fl);
  png_read_info(PNGStruct, PNGInfo);

  Size.X = png_get_image_width(PNGStruct, PNGInfo);
  Size.Y = png_get_image_height(PNGStruct, PNGInfo);

  if (Size.X < 1 || Size.X > 32700) ABORT("Invalid bitmap width: %s!", curfile.CStr());
  if (Size.Y < 1 || Size.Y > 32700) ABORT("Invalid bitmap height: %s!", curfile.CStr());

  if (png_get_bit_depth(PNGStruct, PNGInfo) != 8) ABORT("%s has wrong bit depth %d, should be 8!", curfile.CStr(), int(png_get_bit_depth(PNGStruct, PNGInfo)));

  png_colorp PNGPalette;
  int PaletteSize;

  if (!png_get_PLTE(PNGStruct, PNGInfo, &PNGPalette, &PaletteSize)) ABORT("%s is not in indexed color mode!", curfile.CStr());

  if (PaletteSize != 256) ABORT("%s has wrong palette size %d, should be 256!", curfile.CStr(), PaletteSize);

  Palette = new uChar[768];

  for (int i = 0; i < PaletteSize; ++i) {
    Palette[i*3+0] = PNGPalette[i].red;
    Palette[i*3+1] = PNGPalette[i].green;
    Palette[i*3+2] = PNGPalette[i].blue;
  }

  Alloc2D(PaletteBuffer, Size.Y, Size.X);
  png_read_image(PNGStruct, PaletteBuffer);

  /*
  paletteindex *Buffer = PaletteBuffer[0];
  paletteindex *End = &PaletteBuffer[Size.Y - 1][Size.X];
  for (; Buffer != End; ++Buffer) *Buffer = 255-(*Buffer);
  */

  png_destroy_read_struct(&PNGStruct, &PNGInfo, nullptr);
}
#endif


// ////////////////////////////////////////////////////////////////////////// //
void rawbitmap::loadFromFile (FILE *fl) {
  if (getb(fl) == 0x0a) { loadPCX(fl); return; }
#ifdef HAVE_LIBPNG
  loadPNG(fl);
#else
  ABORT("Invalid image file: '%s'!", curfile.CStr());
#endif
}


// ////////////////////////////////////////////////////////////////////////// //
rawbitmap::rawbitmap (cfestring &FileName) {
  curfile = FileName;
  FILE *fl = fopen(FileName.CStr(), "rb");
  if (!fl) {
    // try to find .png
    //fprintf(stderr, "OLD: <%s>\n", curfile.CStr());
    if (curfile.GetSize() > 4 && curfile.endsWithCI(".pcx")) {
      curfile.Erase(curfile.GetSize()-4, 4);
      curfile += ".png";
      //fprintf(stderr, "NEW: <%s>\n", curfile.CStr());
      fl = fopen(curfile.CStr(), "rb");
    } else if (curfile.GetSize() > 4 && curfile.endsWithCI(".png")) {
      curfile.Erase(curfile.GetSize()-4, 4);
      curfile += ".pcx";
      //fprintf(stderr, "NEW: <%s>\n", curfile.CStr());
      fl = fopen(curfile.CStr(), "rb");
    }
    if (!fl) ABORT("Cannot open file '%s'!", FileName.CStr());
  }
  loadFromFile(fl);
  fclose(fl);
  curfile.Empty();
}


rawbitmap::rawbitmap (v2 Size) : Size(Size) {
  Palette = new uChar[768];
  Alloc2D(PaletteBuffer, Size.Y, Size.X);
}


rawbitmap::~rawbitmap () {
  delete [] Palette;
  delete [] PaletteBuffer;
  for (fontcache::iterator i = FontCache.begin(); i != FontCache.end(); ++i) {
    delete i->second.first;
    delete i->second.second;
  }
}


/* a lousy bitmap saver that uses the pcx format */
void rawbitmap::Save (cfestring &FileName) {
  char PCXHeader[128];
  memset(PCXHeader, 0, 128);
  PCXHeader[0] = 0x0a;
  PCXHeader[1] = 0x05; // version
  PCXHeader[2] = 1; // RLE-packed
  PCXHeader[3] = 8; // bpp
  PCXHeader[65] = 0x01;
  // bytes in line
  PCXHeader[66] = Size.X&0xFF;
  PCXHeader[67] = (Size.X>>8)&0xFF;
  PCXHeader[0x08] = (Size.X-1)&0xFF;
  PCXHeader[0x09] = ((Size.X-1)>>8)&0xFF;
  PCXHeader[0x0A] = (Size.Y-1)&0xFF;
  PCXHeader[0x0B] = ((Size.Y-1)>>8)&0xFF;

  FILE *fo = fopen(FileName.CStr(), "wb");
  //SaveFile.Write(PCXHeader, 128);
  fwrite(PCXHeader, 128, 1, fo);

  paletteindex* Buffer = PaletteBuffer[0];
  paletteindex* End = &PaletteBuffer[Size.Y-1][Size.X];

  int curx = 0;
  int count = 0, accb = 0;

  while (Buffer != End) {
    paletteindex b = *Buffer++;
    if (count == 0) {
      count = 1;
      accb = b;
    } else if (count == 0x3f || b != accb) {
      // flush
      if (count > 1 || accb >= 0xc0) fputc((unsigned char)(count|0xc0), fo);
      fputc((unsigned char)accb, fo);
      count = 1;
      accb = b;
    } else {
      ++count;
    }
    if (++curx == Size.X) {
      // flush
      if (count > 0) {
        if (count > 1 || accb >= 0xc0) fputc((unsigned char)(count|0xc0), fo);
        fputc((unsigned char)accb, fo);
        count = 0;
      }
      curx = 0; // next line
    }
  }
  // flush
  if (count > 0) {
    if (count > 1 || accb >= 0xc0) fputc((unsigned char)(count|0xc0), fo);
    fputc((unsigned char)accb, fo);
  }

  //SaveFile.Write(reinterpret_cast<char*>(Palette), 768);
  fputc(12, fo); // palette signature
  fwrite(reinterpret_cast<char*>(Palette), 768, 1, fo);
  fclose(fo);
}


void rawbitmap::MaskedBlit (bitmap *Bitmap, packcol16 *Color) const {
  MaskedBlit(Bitmap, ZERO_V2, ZERO_V2, Size, Color);
}


void rawbitmap::MaskedBlit (bitmap *Bitmap, v2 Src, v2 Dest, v2 Border, packcol16 *Color) const {
  if (!femath::Clip(Src.X, Src.Y, Dest.X, Dest.Y, Border.X, Border.Y, Size.X, Size.Y, Bitmap->GetSize().X, Bitmap->GetSize().Y)) return;
  paletteindex *Buffer = &PaletteBuffer[Src.Y][Src.X];
  packcol16 *DestBuffer = &Bitmap->GetImage()[Dest.Y][Dest.X];
  int BitmapXSize = Bitmap->GetSize().X;
  uChar *Palette = this->Palette; // eliminate the efficiency cost of dereferencing
  for (int y = 0; y < Border.Y; ++y) {
    for (int x = 0; x < Border.X; ++x) {
      int PaletteElement = Buffer[x];
      if (PaletteElement >= 192) {
        int ThisColor = Color[(PaletteElement-192)>>4];
        int Index = PaletteElement&15;
        int Red = (ThisColor >> 8 & 0xF8)*Index;
        int Green = (ThisColor >> 3 & 0xFC)*Index;
        int Blue = (ThisColor << 3 & 0xF8)*Index;
        if (Red > 0x7FF) Red = 0x7FF;
        if (Green > 0x7FF) Green = 0x7FF;
        if (Blue > 0x7FF) Blue = 0x7FF;
        DestBuffer[x] = (Red << 5 & 0xF800) | (Green & 0x7E0) | (Blue >> 6 & 0x1F);
      } else {
        int PaletteIndex = PaletteElement+(PaletteElement << 1);
        int ThisColor =
          ((Palette[PaletteIndex] & 0xFFF8) << 8) |
          ((Palette[PaletteIndex+1] & 0xFFFC) << 3) |
          (Palette[PaletteIndex+2] >> 3);
        if (ThisColor != TRANSPARENT_COLOR) DestBuffer[x] = ThisColor;
      }
    }
    DestBuffer += BitmapXSize;
    Buffer += Size.X;
  }
}


cachedfont *rawbitmap::Colorize (cpackcol16 *Color, alpha BaseAlpha, cpackalpha *Alpha) const {
  cachedfont *Bitmap = new cachedfont(Size);
  paletteindex *Buffer = PaletteBuffer[0];
  packcol16 *DestBuffer = Bitmap->GetImage()[0];
  uChar *Palette = this->Palette; // eliminate the efficiency cost of dereferencing
  packalpha *AlphaMap;
  truth UseAlpha;
  if (BaseAlpha != 255 || (Alpha && (Alpha[0] != 255 || Alpha[1] != 255 || Alpha[2] != 255 || Alpha[3] != 255))) {
    Bitmap->CreateAlphaMap(BaseAlpha);
    AlphaMap = Bitmap->GetAlphaMap()[0];
    UseAlpha = true;
  } else {
    AlphaMap = 0;
    UseAlpha = false;
  }
  int BitmapXSize = Bitmap->GetSize().X;
  for (int y = 0; y < Size.Y; ++y) {
    for (int x = 0; x < Size.X; ++x) {
      int PaletteElement = Buffer[x];
      if (PaletteElement >= 192) {
        int ColorIndex = (PaletteElement-192)>>4;
        int ThisColor = Color[ColorIndex];
        if (ThisColor != TRANSPARENT_COLOR) {
          int Index = PaletteElement & 15;
          int Red = (ThisColor >> 8 & 0xF8)*Index;
          int Green = (ThisColor >> 3 & 0xFC)*Index;
          int Blue = (ThisColor << 3 & 0xF8)*Index;
          if(Red > 0x7FF) Red = 0x7FF;
          if(Green > 0x7FF) Green = 0x7FF;
          if(Blue > 0x7FF) Blue = 0x7FF;
          DestBuffer[x] = (Red << 5 & 0xF800) | (Green & 0x7E0) | (Blue >> 6 & 0x1F);
          if (UseAlpha) AlphaMap[x] = Alpha[ColorIndex];
        } else {
          DestBuffer[x] = TRANSPARENT_COLOR;
        }
      } else {
        int PaletteIndex = PaletteElement+(PaletteElement<<1);
        DestBuffer[x] =
          ((Palette[PaletteIndex] & 0xFFF8) << 8) |
          ((Palette[PaletteIndex + 1] & 0xFFFC) << 3) |
          (Palette[PaletteIndex + 2] >> 3);
      }
    }
    DestBuffer += BitmapXSize;
    AlphaMap += BitmapXSize;
    Buffer += Size.X;
  }
  return Bitmap;
}


bitmap *rawbitmap::Colorize (v2 Pos, v2 Border, v2 Move, cpackcol16 *Color, alpha BaseAlpha,
  cpackalpha *Alpha, cuchar *RustData, truth AllowReguralColors) const
{
  bitmap *Bitmap = new bitmap(Border);
  v2 TargetPos(0, 0);
  if (Move.X || Move.Y) {
    Bitmap->ClearToColor(TRANSPARENT_COLOR);
    if (Move.X < 0) {
      Pos.X -= Move.X;
      Border.X += Move.X;
    } else if (Move.X > 0) {
      TargetPos.X = Move.X;
      Border.X -= Move.X;
    }
    if (Move.Y < 0) {
      Pos.Y -= Move.Y;
      Border.Y += Move.Y;
    } else if (Move.Y > 0) {
      TargetPos.Y = Move.Y;
      Border.Y -= Move.Y;
    }
  }
  paletteindex *Buffer = &PaletteBuffer[Pos.Y][Pos.X];
  packcol16 *DestBuffer = &Bitmap->GetImage()[TargetPos.Y][TargetPos.X];
  int BitmapXSize = Bitmap->GetSize().X;
  uChar *Palette = this->Palette; // eliminate the efficiency cost of dereferencing
  packalpha *AlphaMap;
  truth UseAlpha;
  if (BaseAlpha != 255 || (Alpha && (Alpha[0] != 255 || Alpha[1] != 255 || Alpha[2] != 255 || Alpha[3] != 255))) {
    Bitmap->CreateAlphaMap(BaseAlpha);
    AlphaMap = &Bitmap->GetAlphaMap()[TargetPos.Y][TargetPos.X];
    UseAlpha = true;
  } else {
    AlphaMap = 0;
    UseAlpha = false;
  }
  truth Rusted = RustData && (RustData[0] || RustData[1] || RustData[2] || RustData[3]);
  feuLong RustSeed[4];
  if (Rusted) {
    RustSeed[0] = (RustData[0]&0xFC)>>2;
    RustSeed[1] = (RustData[1]&0xFC)>>2;
    RustSeed[2] = (RustData[2]&0xFC)>>2;
    RustSeed[3] = (RustData[3]&0xFC)>>2;
  }
  for (int y = 0; y < Border.Y; ++y) {
    for (int x = 0; x < Border.X; ++x) {
      int PaletteElement = Buffer[x];
      if (PaletteElement >= 192) {
        int ColorIndex = (PaletteElement-192)>>4;
        int ThisColor = Color[ColorIndex];
        if (ThisColor != TRANSPARENT_COLOR) {
          int Index = PaletteElement & 15;
          int Red = (ThisColor >> 8 & 0xF8)*Index;
          int Green = (ThisColor >> 3 & 0xFC)*Index;
          int Blue = (ThisColor << 3 & 0xF8)*Index;
          if (Rusted && RustData[ColorIndex] &&
              (RustData[ColorIndex] & 3UL) > (RustSeed[ColorIndex] = RustSeed[ColorIndex] * 1103515245 + 12345) >> 30) {
            Green = ((Green << 1) + Green) >> 2;
            Blue >>= 1;
          }
          if (Red > 0x7FF) Red = 0x7FF;
          if (Green > 0x7FF) Green = 0x7FF;
          if (Blue > 0x7FF) Blue = 0x7FF;
          DestBuffer[x] = (Red << 5 & 0xF800) | (Green & 0x7E0) | (Blue >> 6 & 0x1F);
          if (UseAlpha) AlphaMap[x] = Alpha[ColorIndex];
        } else {
          DestBuffer[x] = TRANSPARENT_COLOR;
        }
      } else if (AllowReguralColors) {
        int PaletteIndex = PaletteElement+(PaletteElement<<1);
        DestBuffer[x] =
          ((Palette[PaletteIndex] & 0xFFF8) << 8) |
          ((Palette[PaletteIndex + 1] & 0xFFFC) << 3) |
          (Palette[PaletteIndex + 2] >> 3);
      } else {
        DestBuffer[x] = TRANSPARENT_COLOR;
      }
    }
    DestBuffer += BitmapXSize;
    AlphaMap += BitmapXSize;
    Buffer += Size.X;
  }
  return Bitmap;
}


truth rawbitmap::strHasCtlCodes (cchar *str) {
  while (*str) {
    if (*str == '\1' || *str == '\2') return true;
    ++str;
  }
  return false;
}


void rawbitmap::printstrColored (bitmap *bmp, v2 pos, packcol16 clr, truth shaded, cchar *str) const {
  if (!str || *str == 0) return;
  // has control codes?
  if (!strHasCtlCodes(str)) {
    // no, try cached font
    fontcache::const_iterator Iterator = FontCache.find(clr);
    if (Iterator != FontCache.end()) {
      // found cached font
      const cachedfont *Font = (shaded ? Iterator->second.first : Iterator->second.second);
      blitdata B = {
        bmp,
        { 0, 0 },
        { pos.X, pos.Y },
        { 9, 9 },
        { 0 },
        TRANSPARENT_COLOR,
        0
      };
      while (*str) {
        unsigned char ch = (unsigned char)*str++;
        if (ch < ' ' || ch >= 127) ch = ' '; //FIXME: ignore?
        B.Src.X = ((ch-0x20)&0xF) << 4;
        B.Src.Y = (ch-0x20)&0xF0;
        Font->PrintCharacter(B);
        B.Dest.X += 8;
      }
      return;
    }
  }
  // has control codes, or no cached font
  packcol16 curcol = clr;
  packcol16 clrshade = MakeShadeColor(clr);
  while (*str) {
    // new color
    if (*str == '\1') {
      ++str;
      if (str[0] == 0) break;
      switch (*str++) {
        case 'R': curcol = RED; break;
        case 'G': curcol = GREEN; break;
        case 'B': curcol = BLUE; break;
        case 'Y': curcol = YELLOW; break;
        case 'W': curcol = (clr != WHITE ? WHITE : LIGHT_GRAY); break;
        case 'C': curcol = CYAN; break;
        case 'P': curcol = PINK; break;
        case 'D': curcol = DARK_GRAY; break;
        case 'S': curcol = SEL_BLUE; break;
      }
      continue;
    }
    // reset color
    if (*str == '\2') { curcol = clr; ++str; continue; }
    // other control chars?
    if ((unsigned char)*str < ' ' || (unsigned char)*str >= 127) { ++str; continue; }
    // normal char
    v2 F(((*str-0x20)&0xF)<<4, (*str-0x20)&0xF0);
    ++str;
    if (shaded) MaskedBlit(bmp, F, v2(pos.X+1, pos.Y+1), v2(8, 8), &clrshade);
    MaskedBlit(bmp, F, pos, v2(8, 8), &curcol);
    pos.X += 8;
  }
}


void rawbitmap::printfColoredVA (bitmap *bmp, v2 pos, packcol16 clr, truth shaded, cchar *fmt, va_list vap) const {
  char buffer[2048];
  char *bufptr = buffer;
  int bufsz = (int)sizeof(buffer)-1;

  for (;;) {
    va_list ap;
    int n;
    char *np;
    va_copy(ap, vap);
    n = vsnprintf(bufptr, bufsz, fmt, ap);
    va_end(ap);
    if (n > -1 && n < bufsz) break;
    if (n < -1) n = bufsz+4096;
    np = (char *)realloc((bufptr == buffer ? NULL : bufptr), n+1);
    if (np == NULL) return; //FIXME
  }

  printstrColored(bmp, pos, clr, shaded, bufptr);

  if (bufptr != buffer) free(bufptr);
}


void rawbitmap::Printf (bitmap *Bitmap, v2 Pos, packcol16 Color, cchar *Format, ...) const {
  va_list ap;
  va_start(ap, Format);
  printfColoredVA(Bitmap, Pos, Color, true, Format, ap);
  va_end(ap);
}


void rawbitmap::PrintfColored (bitmap *Bitmap, v2 Pos, packcol16 Color, cchar *Format, ...) const {
  va_list ap;
  va_start(ap, Format);
  printfColoredVA(Bitmap, Pos, Color, true, Format, ap);
  va_end(ap);
}


void rawbitmap::PrintfUnshaded (bitmap *Bitmap, v2 Pos, packcol16 Color, cchar *Format, ...) const {
  va_list ap;
  va_start(ap, Format);
  printfColoredVA(Bitmap, Pos, Color, false, Format, ap);
  va_end(ap);
}


void rawbitmap::PrintfUnshadedColored (bitmap *Bitmap, v2 Pos, packcol16 Color, cchar *Format, ...) const {
  va_list ap;
  va_start(ap, Format);
  printfColoredVA(Bitmap, Pos, Color, false, Format, ap);
  va_end(ap);
}


void rawbitmap::AlterGradient (v2 Pos, v2 Border, int MColor, int Amount, truth Clip) {
  int ColorMin = 192+(MColor<<4);
  int ColorMax = 207+(MColor<<4);
  if (Clip) {
    for (int x = Pos.X; x < Pos.X + Border.X; ++x) {
      for (int y = Pos.Y; y < Pos.Y + Border.Y; ++y) {
        int Pixel = PaletteBuffer[y][x];
        if (Pixel >= ColorMin && Pixel <= ColorMax) {
          int NewPixel = Pixel+Amount;
          if (NewPixel < ColorMin) NewPixel = ColorMin;
          if (NewPixel > ColorMax) NewPixel = ColorMax;
          PaletteBuffer[y][x] = NewPixel;
        }
      }
    }
  } else {
    int x;
    for (x = Pos.X; x < Pos.X + Border.X; ++x) {
      for (int y = Pos.Y; y < Pos.Y + Border.Y; ++y) {
        int Pixel = PaletteBuffer[y][x];
        if (Pixel >= ColorMin && Pixel <= ColorMax) {
          int NewPixel = Pixel+Amount;
          if (NewPixel < ColorMin) return;
          if (NewPixel > ColorMax) return;
        }
      }
    }
    for (x = Pos.X; x < Pos.X + Border.X; ++x) {
      for (int y = Pos.Y; y < Pos.Y + Border.Y; ++y) {
        int Pixel = PaletteBuffer[y][x];
        if (Pixel >= ColorMin && Pixel <= ColorMax) PaletteBuffer[y][x] = Pixel+Amount;
      }
    }
  }
}


void rawbitmap::SwapColors (v2 Pos, v2 Border, int Color1, int Color2) {
  if (Color1 > 3 || Color2 > 3) ABORT("Illgal col swap!");
  for (int x = Pos.X; x < Pos.X + Border.X; ++x) {
    for (int y = Pos.Y; y < Pos.Y + Border.Y; ++y) {
      paletteindex &Pixel = PaletteBuffer[y][x];
      if (Pixel >= 192+(Color1<<4) && Pixel <= 207+(Color1<<4)) Pixel += (Color2-Color1)<<4;
      else if (Pixel >= 192+(Color2<<4) && Pixel <= 207+(Color2<<4)) Pixel += (Color1-Color2)<<4;
    }
  }
}


/* TempBuffer must be an array of Border.X * Border.Y paletteindices */
void rawbitmap::Roll (v2 Pos, v2 Border, v2 Move, paletteindex *TempBuffer) {
  int x, y;
  for (x = Pos.X; x < Pos.X + Border.X; ++x) {
    for (y = Pos.Y; y < Pos.Y + Border.Y; ++y) {
      int XPos = x+Move.X, YPos = y+Move.Y;
      if (XPos < Pos.X) XPos += Border.X;
      if (YPos < Pos.Y) YPos += Border.Y;
      if (XPos >= Pos.X+Border.X) XPos -= Border.X;
      if (YPos >= Pos.Y+Border.Y) YPos -= Border.Y;
      TempBuffer[(YPos-Pos.Y)*Border.X+XPos-Pos.X] = PaletteBuffer[y][x];
    }
  }
  for (x = Pos.X; x < Pos.X + Border.X; ++x)
    for (y = Pos.Y; y < Pos.Y + Border.Y; ++y)
      PaletteBuffer[y][x] = TempBuffer[(y-Pos.Y)*Border.X+x-Pos.X];
}


void rawbitmap::CreateFontCache (packcol16 Color) {
  if (FontCache.find(Color) != FontCache.end()) return;
  packcol16 ShadeColor = MakeShadeColor(Color);
  cachedfont *Font = new cachedfont(Size, TRANSPARENT_COLOR);
  MaskedBlit(Font, ZERO_V2, v2(1, 1), v2(Size.X-1, Size.Y-1), &ShadeColor);
  cachedfont *UnshadedFont = Colorize(&Color);
  blitdata B = {
    Font,
    { 0, 0 },
    { 0, 0 },
    { Size.X, Size.Y },
    { 0 },
    TRANSPARENT_COLOR,
    0
  };
  UnshadedFont->NormalMaskedBlit(B);
  Font->CreateMaskMap();
  UnshadedFont->CreateMaskMap();
  FontCache[Color] = std::make_pair(Font, UnshadedFont);
}


/* returns ERROR_V2 if fails find Pos else returns pos */
v2 rawbitmap::RandomizeSparklePos (cv2 *ValidityArray, v2 *PossibleBuffer, v2 Pos, v2 Border,
  int ValidityArraySize, int SparkleFlags) const
{
  if (!SparkleFlags) return ERROR_V2;
  /* Don't use { } to initialize, or GCC optimizations will produce code that crashes! */
  v2 *BadPossible[4];
  BadPossible[0] = PossibleBuffer;
  BadPossible[1] = BadPossible[0]+((Border.X+Border.Y)<<1)-4;
  BadPossible[2] = BadPossible[1]+((Border.X+Border.Y)<<1)-12;
  BadPossible[3] = BadPossible[2]+((Border.X+Border.Y)<<1)-20;
  v2 *PreferredPossible = BadPossible[3]+((Border.X+Border.Y)<<1)-28;
  int Preferred = 0;
  int Bad[4] = { 0, 0, 0, 0 };
  int XMax = Pos.X+Border.X;
  int YMax = Pos.Y+Border.Y;
  for (int c = 0; c < ValidityArraySize; ++c) {
    v2 V = ValidityArray[c]+Pos;
    int Entry = PaletteBuffer[V.Y][V.X];
    if (IsMaterialColor(Entry) && 1 << GetMaterialColorIndex(Entry) & SparkleFlags) {
      int MinDist = 0x7FFF;
      if (V.X < Pos.X+4) MinDist = Min(V.X-Pos.X, MinDist);
      if (V.X >= XMax-4) MinDist = Min(XMax-V.X-1, MinDist);
      if (V.Y < Pos.Y+4) MinDist = Min(V.Y-Pos.Y, MinDist);
      if (V.Y >= YMax-4) MinDist = Min(YMax-V.Y-1, MinDist);
      if (MinDist >= 4) PreferredPossible[Preferred++] = V;
      else BadPossible[MinDist][Bad[MinDist]++] = V;
    }
  }
  v2 Return;
  if (Preferred) Return = PreferredPossible[RAND()%Preferred]-Pos;
  else if (Bad[3]) Return = BadPossible[3][RAND()%Bad[3]]-Pos;
  else if (Bad[2]) Return = BadPossible[2][RAND()%Bad[2]]-Pos;
  else if (Bad[1]) Return = BadPossible[1][RAND()%Bad[1]]-Pos;
  else if (Bad[0]) Return = BadPossible[0][RAND()%Bad[0]]-Pos;
  else Return = ERROR_V2;
  return Return;
}


truth rawbitmap::IsTransparent (v2 Pos) const {
  return PaletteBuffer[Pos.Y][Pos.X] == TRANSPARENT_PALETTE_INDEX;
}


truth rawbitmap::IsMaterialColor1 (v2 Pos) const {
  int P = PaletteBuffer[Pos.Y][Pos.X];
  return P >= 192 && P < 208;
}


void rawbitmap::CopyPaletteFrom (rawbitmap *Bitmap) {
  memmove(Palette, Bitmap->Palette, 768);
}


void rawbitmap::Clear () {
  memset(PaletteBuffer[0], TRANSPARENT_PALETTE_INDEX, Size.X*Size.Y*sizeof(paletteindex));
}


void rawbitmap::NormalBlit (rawbitmap *Bitmap, v2 Src, v2 Dest, v2 Border, int Flags) const {
  paletteindex **SrcBuffer = PaletteBuffer;
  paletteindex **DestBuffer = Bitmap->PaletteBuffer;
  int TrueDestXMove;
  paletteindex *DestBase;
  switch (Flags & 7) {
    case NONE:
      if (Size.X == Bitmap->Size.X && Size.Y == Bitmap->Size.Y) {
        memmove(DestBuffer[0], SrcBuffer[0], Size.X*Size.Y*sizeof(paletteindex));
      } else {
        cint Bytes = Border.X*sizeof(paletteindex);
        for (int y = 0; y < Border.Y; ++y) memmove(&DestBuffer[Dest.Y+y][Dest.X], &SrcBuffer[Src.Y+y][Src.X], Bytes);
      }
      break;
    case MIRROR:
      Dest.X += Border.X-1;
      for (int y = 0; y < Border.Y; ++y) {
        cpaletteindex *SrcPtr = &SrcBuffer[Src.Y+y][Src.X];
        cpaletteindex *EndPtr = SrcPtr+Border.X;
        paletteindex *DestPtr = &DestBuffer[Dest.Y+y][Dest.X];
        for (; SrcPtr != EndPtr; ++SrcPtr, --DestPtr) *DestPtr = *SrcPtr;
      }
      break;
    case FLIP: {
      Dest.Y += Border.Y-1;
      cint Bytes = Border.X*sizeof(paletteindex);
      for (int y = 0; y < Border.Y; ++y) memmove(&DestBuffer[Dest.Y-y][Dest.X], &SrcBuffer[Src.Y+y][Src.X], Bytes);
      } break;
    case (MIRROR | FLIP):
      Dest.X += Border.X-1;
      Dest.Y += Border.Y-1;
      for (int y = 0; y < Border.Y; ++y) {
        cpaletteindex *SrcPtr = &SrcBuffer[Src.Y+y][Src.X];
        cpaletteindex *EndPtr = SrcPtr+Border.X;
        paletteindex *DestPtr = &DestBuffer[Dest.Y-y][Dest.X];
        for (; SrcPtr != EndPtr; ++SrcPtr, --DestPtr) *DestPtr = *SrcPtr;
      }
      break;
    case ROTATE:
      Dest.X += Border.X-1;
      TrueDestXMove = Bitmap->Size.X;
      DestBase = &DestBuffer[Dest.Y][Dest.X];
      for (int y = 0; y < Border.Y; ++y) {
        cpaletteindex *SrcPtr = &SrcBuffer[Src.Y+y][Src.X];
        cpaletteindex *EndPtr = SrcPtr+Border.X;
        paletteindex *DestPtr = DestBase-y;
        for (; SrcPtr != EndPtr; ++SrcPtr, DestPtr += TrueDestXMove) *DestPtr = *SrcPtr;
      }
      break;
    case (MIRROR | ROTATE):
      TrueDestXMove = Bitmap->Size.X;
      DestBase = &DestBuffer[Dest.Y][Dest.X];
      for (int y = 0; y < Border.Y; ++y) {
        cpaletteindex *SrcPtr = &SrcBuffer[Src.Y+y][Src.X];
        cpaletteindex *EndPtr = SrcPtr+Border.X;
        paletteindex *DestPtr = DestBase+y;
        for (; SrcPtr != EndPtr; ++SrcPtr, DestPtr += TrueDestXMove) *DestPtr = *SrcPtr;
      }
      break;
    case (FLIP | ROTATE):
      Dest.X += Border.X-1;
      Dest.Y += Border.Y-1;
      TrueDestXMove = Bitmap->Size.X;
      DestBase = &DestBuffer[Dest.Y][Dest.X];
      for (int y = 0; y < Border.Y; ++y) {
        cpaletteindex *SrcPtr = &SrcBuffer[Src.Y+y][Src.X];
        cpaletteindex *EndPtr = SrcPtr+Border.X;
        paletteindex *DestPtr = DestBase-y;
        for (; SrcPtr != EndPtr; ++SrcPtr, DestPtr -= TrueDestXMove) *DestPtr = *SrcPtr;
      }
      break;
   case (MIRROR | FLIP | ROTATE):
      Dest.Y += Border.Y-1;
      TrueDestXMove = Bitmap->Size.X;
      DestBase = &DestBuffer[Dest.Y][Dest.X];
      for (int y = 0; y < Border.Y; ++y) {
        cpaletteindex *SrcPtr = &SrcBuffer[Src.Y+y][Src.X];
        cpaletteindex *EndPtr = SrcPtr+Border.X;
        paletteindex *DestPtr = DestBase+y;
        for (; SrcPtr != EndPtr; ++SrcPtr, DestPtr -= TrueDestXMove) *DestPtr = *SrcPtr;
      }
      break;
  }
}
