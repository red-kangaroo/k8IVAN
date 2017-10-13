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
#include "SDL.h"

#include "graphics.h"
#include "bitmap.h"
#include "whandler.h"
#include "error.h"
#include "rawbit.h"


void (*graphics::SwitchModeHandler) ();

SDL_Surface *graphics::Screen;
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
SDL_Surface *graphics::TempSurface;
#endif

bitmap *graphics::DoubleBuffer;
v2 graphics::Res;
int graphics::ColorDepth;
rawbitmap *graphics::DefaultFont = 0;

truth dblRes = false;
truth origDblRes = false;
truth weirdDRes = false;

#define DIVISOR  (1.7)

static int fixVal (int v) { return (dblRes ? (weirdDRes ? (int)(v*DIVISOR) : v*2) : v); }


void graphics::Init () {
  static truth AlreadyInstalled = false;
  if (!AlreadyInstalled) {
    AlreadyInstalled = true;
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_NOPARACHUTE)) ABORT("Can't initialize SDL.");
    atexit(graphics::DeInit);
  }
}


void graphics::DeInit () {
  delete DefaultFont;
  DefaultFont = 0;
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
  SDL_FreeSurface(TempSurface);
#endif
  SDL_Quit();
}


void graphics::SetMode (cchar *Title, cchar *IconName, v2 NewRes, truth FullScreen, truth DoubleRes, truth WeirdDRes) {
  if (IconName) {
    SDL_Surface *Icon = SDL_LoadBMP(IconName);
    SDL_SetColorKey(Icon, SDL_SRCCOLORKEY, SDL_MapRGB(Icon->format, 255, 255, 255));
    SDL_WM_SetIcon(Icon, NULL);
  }
  origDblRes = DoubleRes;
  dblRes = DoubleRes;
  weirdDRes = WeirdDRes;
  feuLong Flags = SDL_SWSURFACE;
  if (FullScreen) {
    SDL_ShowCursor(SDL_DISABLE);
    Flags |= SDL_FULLSCREEN;
    dblRes = false;
  }
  Screen = SDL_SetVideoMode(fixVal(NewRes.X), fixVal(NewRes.Y), 16, Flags);
  if (!Screen) ABORT("Couldn't set video mode.");
  SDL_WM_SetCaption(Title, 0);
  globalwindowhandler::Init();
  DoubleBuffer = new bitmap(NewRes);
  Res = NewRes;
  ColorDepth = 16;
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
  Uint32 rmask, gmask, bmask;
  rmask = 0xF800;
  gmask = 0x7E0;
  bmask = 0x1F;
  TempSurface = SDL_CreateRGBSurface(SDL_SWSURFACE, Res.X, Res.Y, 16, rmask, gmask, bmask, 0);
  if (!TempSurface) ABORT("CreateRGBSurface failed: %s\n", SDL_GetError());
#endif
}


static inline void toRGB (int* r, int* g, int* b, packcol16 c) {
  if (b) *b = (c<<3)&0xff;
  if (g) *g = ((c>>5)<<2)&0xff;
  if (r) *r = ((c>>11)<<3)&0xff;
}


static inline packcol16 fromRGB (int r, int g, int b) {
  if (r < 0) r = 0; else if (r > 255) r = 255;
  if (g < 0) g = 0; else if (g > 255) g = 255;
  if (b < 0) b = 0; else if (b > 255) b = 255;
  packcol16 c = 0;
  c |= (b>>3);
  c |= (g>>2)<<5;
  c |= (r>>3)<<11;
  return c;
}


void graphics::BlitDBToScreen () {
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
  SDL_LockSurface(TempSurface);
  packcol16 *SrcPtr = DoubleBuffer->GetImage()[0];
  packcol16 *DestPtr = static_cast<packcol16 *>(TempSurface->pixels);
  feuLong ScreenYMove = (TempSurface->pitch >> 1);
  feuLong LineSize = Res.X << 1;
  if (dblRes) {
    if (weirdDRes) {
      for (int y = 0; y < fixVal(Res.Y); ++y) {
        for (int x = 0; x < fixVal(Res.X); ++x) {
          // getpixel
          int sx = (Res.X-1)*x/(fixVal(Res.X)-1);
          int sy = (Res.Y-1)*y/(fixVal(Res.Y)-1);
          packcol16 *src = SrcPtr+sy*Res.X+sx;
          packcol16 *dst = DestPtr+y*ScreenYMove+x;
          *dst = *src;
        }
      }
    } else {
      for (int y = 0; y < Res.Y; ++y, SrcPtr += Res.X, DestPtr += ScreenYMove) {
        packcol16 *s = SrcPtr;
        packcol16 *d = DestPtr;
        for (int x = 0; x < Res.X; ++x) { *d++ = *s; *d++ = *s++; }
        s = SrcPtr;
        DestPtr += ScreenYMove;
        d = DestPtr;
        for (int x = 0; x < Res.X; ++x) { *d++ = *s; *d++ = *s++; }
      }
    }
  } else {
    for (int y = 0; y < Res.Y; ++y, SrcPtr += Res.X, DestPtr += ScreenYMove) memmove(DestPtr, SrcPtr, LineSize);
  }
  SDL_UnlockSurface(TempSurface);
  SDL_Surface *S = SDL_DisplayFormat(TempSurface);
  SDL_BlitSurface(S, NULL, Screen, NULL);
  SDL_FreeSurface(S);
  SDL_UpdateRect(Screen, 0, 0, Res.X, Res.Y);
#else
  if (SDL_MUSTLOCK(Screen) && SDL_LockSurface(Screen) < 0) ABORT("Can't lock screen");
  packcol16 *SrcPtr = DoubleBuffer->GetImage()[0];
  packcol16 *DestPtr = static_cast<packcol16 *>(Screen->pixels);
  feuLong ScreenYMove = (Screen->pitch >> 1);
  feuLong LineSize = Res.X << 1;
  if (dblRes) {
    if (weirdDRes) {
      for (int y = 0; y < fixVal(Res.Y); ++y) {
        for (int x = 0; x < fixVal(Res.X); ++x) {
          // getpixel
          /*
          int sx = (Res.X-1)*x/(fixVal(Res.X)-1);
          int sy = (Res.Y-1)*y/(fixVal(Res.Y)-1);
          packcol16 *src = SrcPtr+sy*Res.X+sx;
          packcol16 *dst = DestPtr+y*ScreenYMove+x;
          *dst = *src;
          */
          double fx = x/DIVISOR;
          double fy = y/DIVISOR;
          int r[4], g[4], b[4];
          for (int dy = 0; dy < 2; ++dy) {
            for (int dx = 0; dx < 2; ++dx) {
              int sx = (int)(fx+dx);
              int sy = (int)(fy+dy);
              if (sx >= 0 && sy >= 0 && sx < Res.X && sy < Res.Y) {
                toRGB(&r[dy*2+dx], &g[dy*2+dx], &b[dy*2+dx], SrcPtr[sy*Res.X+sx]);
              } else {
                r[dy*2+dx] = g[dy*2+dx] = b[dy*2+dx] = 0;
              }
            }
          }
          double frc1 = fx-(int)fx;
          double frc0 = 1.0-frc1;
          for (int dy = 0; dy < 2; ++dy) {
            r[dy*2+0] = (int)(r[dy*2+0]*frc0+r[dy*2+1]*frc1);
            g[dy*2+0] = (int)(g[dy*2+0]*frc0+g[dy*2+1]*frc1);
            b[dy*2+0] = (int)(b[dy*2+0]*frc0+b[dy*2+1]*frc1);
          }
          frc1 = fy-(int)fy;
          frc0 = 1.0-frc1;
          r[0] = (int)(r[0]*frc0+r[2]*frc1);
          g[0] = (int)(g[0]*frc0+g[2]*frc1);
          b[0] = (int)(b[0]*frc0+b[2]*frc1);
          DestPtr[y*ScreenYMove+x] = fromRGB(r[0], g[0], b[0]);
        }
      }
    } else {
      for (int y = 0; y < Res.Y; ++y, SrcPtr += Res.X, DestPtr += ScreenYMove) {
        packcol16 *s = SrcPtr;
        packcol16 *d = DestPtr;
        for (int x = 0; x < Res.X; ++x) { *d++ = *s; *d++ = *s++; }
        s = SrcPtr;
        DestPtr += ScreenYMove;
        d = DestPtr;
        for (int x = 0; x < Res.X; ++x) { *d++ = *s; *d++ = *s++; }
      }
    }
  } else {
    for (int y = 0; y < Res.Y; ++y, SrcPtr += Res.X, DestPtr += ScreenYMove) {
      memmove(DestPtr, SrcPtr, LineSize);
    }
  }
  if (SDL_MUSTLOCK(Screen)) SDL_UnlockSurface(Screen);
  SDL_UpdateRect(Screen, 0, 0, fixVal(Res.X), fixVal(Res.Y));
}
#endif


void graphics::SwitchMode () {
  feuLong Flags;
  if (Screen->flags & SDL_FULLSCREEN) {
    SDL_ShowCursor(SDL_ENABLE);
    Flags = SDL_SWSURFACE;
    dblRes = origDblRes;
  } else {
    SDL_ShowCursor(SDL_DISABLE);
    Flags = SDL_SWSURFACE|SDL_FULLSCREEN;
    dblRes = false;
  }
  if (SwitchModeHandler) SwitchModeHandler();
  Screen = SDL_SetVideoMode(fixVal(Res.X), fixVal(Res.Y), ColorDepth, Flags);
  if (!Screen) ABORT("Couldn't toggle fullscreen mode.");
  BlitDBToScreen();
}


void graphics::LoadDefaultFont (cfestring &FileName) {
  DefaultFont = new rawbitmap(FileName);
}
