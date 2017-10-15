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
#include <math.h>

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

//#define DIVISOR  (1.7)
float DIVISOR = 1.7;


static int fixVal (int v) { return (dblRes ? (int)(v*DIVISOR) : v); }

struct KIt {
  float frc0, frc1, yfrc0, yfrc1;
  int sofs[4];
};


KIt *kernel = 0;
unsigned char *bufc32 = 0;

static void freeKernel () {
  if (kernel) { free(kernel); kernel = 0; }
}


static void buildKernel (int resx, int resy) {
  KIt* res = (KIt*)malloc(fixVal(resy)*fixVal(resx)*sizeof(KIt));
  for (int y = 0; y < fixVal(resy); ++y) {
    for (int x = 0; x < fixVal(resx); ++x) {
      KIt* i = res+y*fixVal(resx)+x;
      float fx = x/DIVISOR;
      float fy = y/DIVISOR;
      float frc1 = fx-(int)fx;
      float frc0 = 1.0-frc1;
      float yfrc1 = fy-(int)fy;
      float yfrc0 = 1.0-yfrc1;
      i->frc0 = frc0;
      i->frc1 = frc1;
      i->yfrc0 = yfrc0;
      i->yfrc1 = yfrc1;
      //r[0] = (r[0]*frc0+r[1]*frc1)*yfrc0+(r[2]*frc0+r[3]*frc1)*yfrc1;
      for (int dy = 0; dy < 2; ++dy) {
        for (int dx = 0; dx < 2; ++dx) {
          int sx = (int)(fx+dx);
          int sy = (int)(fy+dy);
          if (sx >= 0 && sy >= 0 && sx < resx && sy < resy) {
            i->sofs[dy*2+dx] = sy*(resx*4)+sx*4;
          } else {
            i->sofs[dy*2+dx] = 0;
          }
        }
      }
    }
  }
  freeKernel();
  kernel = res;
}



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


void graphics::SetMode (cchar *Title, cchar *IconName, v2 NewRes, truth FullScreen, sLong adresmod) {
  if (IconName) {
    SDL_Surface *Icon = SDL_LoadBMP(IconName);
    SDL_SetColorKey(Icon, SDL_SRCCOLORKEY, SDL_MapRGB(Icon->format, 255, 255, 255));
    SDL_WM_SetIcon(Icon, NULL);
  }
  if (adresmod < 0) adresmod = 0; else if (adresmod > 10) adresmod = 10;
  dblRes = (adresmod > 0);
  if (adresmod == 10) DIVISOR = 2.0f; else DIVISOR = 1.0f+(adresmod/10.0f);
  origDblRes = dblRes;
  feuLong Flags = SDL_SWSURFACE;
  if (FullScreen) {
    SDL_ShowCursor(SDL_DISABLE);
    Flags |= SDL_FULLSCREEN;
    dblRes = false;
  }
  Screen = SDL_SetVideoMode(fixVal(NewRes.X), fixVal(NewRes.Y), /*16*/32, Flags);
  if (!Screen) ABORT("Couldn't set video mode.");
  if (dblRes) buildKernel(NewRes.X, NewRes.Y); else freeKernel();
  bufc32 = (unsigned char*)realloc(bufc32, fixVal(NewRes.X)*fixVal(NewRes.Y)*4);
  SDL_WM_SetCaption(Title, 0);
  globalwindowhandler::Init();
  DoubleBuffer = new bitmap(NewRes);
  Res = NewRes;
  ColorDepth = 32/*16*/;
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
  ABORT("Not Yet");
 #if 0
  Uint32 rmask, gmask, bmask;
  /*
  rmask = 0xF800;
  gmask = 0x7E0;
  bmask = 0x1F;
  TempSurface = SDL_CreateRGBSurface(SDL_SWSURFACE, Res.X, Res.Y, 16, rmask, gmask, bmask, 0);
  */
  rmask = 0xFF0000;
  gmask = 0xFF00;
  bmask = 0xFF;
  TempSurface = SDL_CreateRGBSurface(SDL_SWSURFACE, Res.X, Res.Y, /*16*/32, rmask, gmask, bmask, 0);
  if (!TempSurface) ABORT("CreateRGBSurface failed: %s\n", SDL_GetError());
 #endif
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
  ABORT("Not Yet");
 #if 0
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
 #endif
#else
  if (SDL_MUSTLOCK(Screen) && SDL_LockSurface(Screen) < 0) ABORT("Can't lock screen");
  packcol16 *SrcPtr = DoubleBuffer->GetImage()[0];
  //packcol16 *DestPtr = static_cast<packcol16 *>(Screen->pixels);
  //feuLong ScreenYMove = (Screen->pitch >> 1);
  //feuLong LineSize = Res.X << 1;

  int newx = fixVal(Res.X);
  int newy = fixVal(Res.Y);

  // convert source buffer
  packcol16 *sptr = SrcPtr;
  uint32_t *dptr = (uint32_t*)bufc32;
  for (int y = 0; y < newy; ++y) {
    for (int x = 0; x < newx; ++x) {
      packcol16 c = *sptr++;
      unsigned char b = (c<<3)&0xff;
      unsigned char g = ((c>>5)<<2)&0xff;
      unsigned char r = ((c>>11)<<3)&0xff;
      *dptr++ = b|(g<<8)|(r<<16);
    }
  }

  unsigned char *DestPtr = static_cast<unsigned char*>(Screen->pixels);
  feuLong ScreenYMove = Screen->pitch;

  if (dblRes) {
    if (DIVISOR != 2.0f) {
      // copy converted buffer used kernel
      unsigned char *curs = bufc32;
      KIt *i = kernel;
      for (int y = 0; y < newy; ++y) {
        uint32_t *dp = (uint32_t*)DestPtr;
        for (int x = 0; x < newx; ++x) {
          uint32_t dclr = 0;
          for (int n = 0; n < 3; ++n) {
            int c = lrintf(curs[i->sofs[0]+n]*i->frc0+curs[i->sofs[1]+n]*i->frc1)*i->yfrc0+(curs[i->sofs[2]+n]*i->frc0+curs[i->sofs[3]+n]*i->frc1)*i->yfrc1;
            //if (c < 0) c = 0; else if (c > 255) c = 255;
            //dclr |= (c<<(n*8));
            dclr |= ((c&0xff)|(255-((-(int)(c < 256))>>24)))<<(n*8); //K8 WARNING! ABSOLUTELY NON-PORTABLE AND CAUSES UB!
          }
          *dp++ = dclr;
          ++i;
        }
        DestPtr += ScreenYMove;
      }
    } else {
      for (int y = 0; y < Res.Y; ++y, DestPtr += ScreenYMove) {
        uint32_t *s = (uint32_t*)(bufc32+y*(Res.X*4));
        uint32_t *d = (uint32_t*)(DestPtr);
        for (int x = 0; x < Res.X; ++x) { *d++ = *s; *d++ = *s++; }
        DestPtr += ScreenYMove;
        memcpy(DestPtr, bufc32+y*(Res.X*4), Res.X*4*2);
      }
    }
  } else {
    for (int y = 0; y < Res.Y; ++y, DestPtr += ScreenYMove) {
      uint32_t *s = (uint32_t*)(bufc32+y*(Res.X*4));
      memcpy(DestPtr, s, Res.X*4);
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
  if (dblRes) buildKernel(Res.X, Res.Y); else freeKernel();
  bufc32 = (unsigned char*)realloc(bufc32, fixVal(Res.X)*fixVal(Res.Y)*4);
  BlitDBToScreen();
}


void graphics::ReinitMode (sLong adresmod) {
  if (adresmod < 0) adresmod = 0; else if (adresmod > 10) adresmod = 10;
  dblRes = (adresmod > 0);
  if (adresmod == 10) DIVISOR = 2.0f; else DIVISOR = 1.0f+(adresmod/10.0f);
  origDblRes = dblRes;

  feuLong Flags;
  if (Screen->flags&SDL_FULLSCREEN) {
    dblRes = false;
    //Flags = SDL_SWSURFACE|SDL_FULLSCREEN;
    return; // nothing to do here, really
  }
  Flags = SDL_SWSURFACE;

  Screen = SDL_SetVideoMode(fixVal(Res.X), fixVal(Res.Y), ColorDepth, Flags);
  if (dblRes) buildKernel(Res.X, Res.Y); else freeKernel();
  bufc32 = (unsigned char*)realloc(bufc32, fixVal(Res.X)*fixVal(Res.Y)*4);
  BlitDBToScreen();
}


void graphics::LoadDefaultFont (cfestring &FileName) {
  DefaultFont = new rawbitmap(FileName);
}
