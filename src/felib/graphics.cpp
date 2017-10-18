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

#ifndef DISABLE_OPENGL
# include <GL/gl.h>
#endif

#include "SDL.h"

#include "graphics.h"
#include "bitmap.h"
#include "whandler.h"
#include "feerror.h"
#include "rawbit.h"

#if !defined(DISABLE_SOUND) && defined(ENABLE_ALSA)
# include <alsa/asoundlib.h>
// shut the fuck up, alsa!

static void alsa_message_fucker (const char *file, int line, const char *function_, int err, const char *fmt, ...) {}

void fuck_alsa_messages () {
  snd_lib_error_set_handler(&alsa_message_fucker);
}
#else
void fuck_alsa_messages () {}
#endif


void (*graphics::SwitchModeHandler) ();

SDL_Surface *graphics::Screen;

bitmap *graphics::DoubleBuffer;
v2 graphics::Res;
int graphics::ColorDepth;
rawbitmap *graphics::DefaultFont = 0;

static truth dblRes = false;
static truth origDblRes = false;
static uint8_t *bufc32 = 0;

//#define DIVISOR  (1.7)
static float DIVISOR = 1.7;


static int fixVal (int v) { return (dblRes ? (int)(v*DIVISOR) : v); }


#ifndef DISABLE_OPENGL
static GLuint maintid = 0;


static void killTexture () {
  if (maintid != 0) {
    glDeleteTextures(1, &maintid);
    maintid = 0;
  }
}


void graphics::createTexture () {
  glGenTextures(1, &maintid);
  if (maintid == 0) ABORT("Couldn't create OpenGL texture.");

  //GLint gltextbinding;
  //glGetIntegerv(GL_TEXTURE_BINDING_2D, &gltextbinding);
  //scope(exit) glBindTexture(GL_TEXTURE_2D, gltextbinding);

  glBindTexture(GL_TEXTURE_2D, maintid);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  //glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
  //glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, Res.X, Res.Y, 0, /*GL_RGBA*/GL_RGBA, GL_UNSIGNED_BYTE, NULL); // this inits texture
}


#else

struct KIt {
  float frc0, frc1, yfrc0, yfrc1;
  int sofs[4];
};

static KIt *kernel = 0;

#endif


static void buildKernel (int resx, int resy) {
  bufc32 = (uint8_t*)realloc(bufc32, resx*resy*4);
#ifdef DISABLE_OPENGL
  if (fixVal(resx) == resx && fixVal(resy) == resy) return;
  // build kernel
  KIt* res = (KIt*)realloc(kernel, fixVal(resy)*fixVal(resx)*sizeof(KIt));
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
  kernel = res;
#endif
}


void graphics::Init () {
  static truth AlreadyInstalled = false;
  if (!AlreadyInstalled) {
    AlreadyInstalled = true;
    if (SDL_Init(SDL_INIT_VIDEO|SDL_INIT_TIMER|SDL_INIT_NOPARACHUTE)) ABORT("Can't initialize SDL.");
    atexit(graphics::DeInit);
  }
}


void graphics::DeInit () {
#ifndef DISABLE_OPENGL
  killTexture();
#endif
  delete DefaultFont;
  DefaultFont = 0;
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
  feuLong Flags = 0;

  if (FullScreen) {
    SDL_ShowCursor(SDL_DISABLE);
    Flags |= SDL_FULLSCREEN;
    dblRes = false;
  }

#ifndef DISABLE_OPENGL
  Flags |= SDL_OPENGL;
  SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
  SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
  SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
  SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
  //SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 16);
  SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
  killTexture();
#else
  Flags |= SDL_SWSURFACE;
#endif

  Screen = SDL_SetVideoMode(fixVal(NewRes.X), fixVal(NewRes.Y), /*16*/32, Flags);
  if (!Screen) ABORT("Couldn't set video mode.");
  SDL_WM_SetCaption(Title, 0);
  globalwindowhandler::Init();
  DoubleBuffer = new bitmap(NewRes);
  Res = NewRes;
  ColorDepth = 32/*16*/;
  fuck_alsa_messages();
  buildKernel(NewRes.X, NewRes.Y);

#ifndef DISABLE_OPENGL
  createTexture();
#endif
}


#if 0
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
#endif


void graphics::BlitDBToScreen () {
  const packcol16 *SrcPtr = DoubleBuffer->GetImage()[0];
  const int newx = fixVal(Res.X);
  const int newy = fixVal(Res.Y);

  // convert source buffer
  const packcol16 *sptr = SrcPtr;
  uint32_t *dptr = (uint32_t*)bufc32;
  for (int y = 0; y < Res.Y; ++y) {
    for (int x = 0; x < Res.X; ++x) {
      packcol16 c = *sptr++;
      unsigned char b = (c<<3)&0xff;
      unsigned char g = ((c>>5)<<2)&0xff;
      unsigned char r = ((c>>11)<<3)&0xff;
      *dptr++ = b|(g<<8)|(r<<16);
    }
  }

#if DISABLE_OPENGL
  if (SDL_MUSTLOCK(Screen) && SDL_LockSurface(Screen) < 0) ABORT("Can't lock screen");

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
#else
  if (maintid == 0) ABORT("OpenGL not initialized.");

  glMatrixMode(GL_PROJECTION); // for ortho camera
  glLoadIdentity();
  glOrtho(0, newx, newy, 0, -1, 1); // top-to-bottom

  glViewport(0, 0, newx, newy);

  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();

  glBindTexture(GL_TEXTURE_2D, maintid);
  glTexSubImage2D(GL_TEXTURE_2D, 0, 0/*x*/, 0/*y*/, Res.X, Res.Y, /*GL_RGBA*/GL_BGRA, GL_UNSIGNED_BYTE, bufc32);

  glEnable(GL_TEXTURE_2D);
  glDisable(GL_LIGHTING);
  glDisable(GL_DITHER);
  glDisable(GL_BLEND);
  glDisable(GL_DEPTH_TEST);
  glDisable(GL_STENCIL_TEST);
  glDisable(GL_SCISSOR_TEST);
  glDisable(GL_CULL_FACE);
  glDisable(GL_POLYGON_OFFSET_FILL);
  glDisable(GL_ALPHA_TEST);
  glDisable(GL_FOG);
  glDisable(GL_COLOR_LOGIC_OP);
  glDisable(GL_INDEX_LOGIC_OP);
  glDisable(GL_POLYGON_SMOOTH);

  glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
  glBegin(GL_QUADS);
    glTexCoord2f(0.0f, 0.0f); glVertex2i(0, 0); // top-left
    glTexCoord2f(1.0f, 0.0f); glVertex2i(newx, 0); // top-right
    glTexCoord2f(1.0f, 1.0f); glVertex2i(newx, newy); // bottom-right
    glTexCoord2f(0.0f, 1.0f); glVertex2i(0, newy); // bottom-left
  glEnd();

  glBindTexture(GL_TEXTURE_2D, 0);

  SDL_GL_SwapBuffers();
#endif
}


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
#ifndef DISABLE_OPENGL
  Flags |= SDL_OPENGL;
  killTexture();
#endif
  Screen = SDL_SetVideoMode(fixVal(Res.X), fixVal(Res.Y), ColorDepth, Flags);
  if (!Screen) ABORT("Couldn't toggle fullscreen mode.");
  buildKernel(Res.X, Res.Y);
#ifndef DISABLE_OPENGL
  createTexture();
#endif
  BlitDBToScreen();
}


void graphics::ReinitMode (sLong adresmod) {
  if (adresmod < 0) adresmod = 0; else if (adresmod > 10) adresmod = 10;
  dblRes = (adresmod > 0);
  if (adresmod == 10) DIVISOR = 2.0f; else DIVISOR = 1.0f+(adresmod/10.0f);
  origDblRes = dblRes;

  if (Screen->flags&SDL_FULLSCREEN) {
    dblRes = false;
    //Flags = SDL_SWSURFACE|SDL_FULLSCREEN;
    return; // nothing to do here, really
  }
  feuLong Flags = 0;
#ifndef DISABLE_OPENGL
  Flags |= SDL_OPENGL;
  killTexture();
#else
  Flags = SDL_SWSURFACE;
#endif
  Screen = SDL_SetVideoMode(fixVal(Res.X), fixVal(Res.Y), ColorDepth, Flags);
  buildKernel(Res.X, Res.Y);
#ifndef DISABLE_OPENGL
  createTexture();
#endif
  BlitDBToScreen();
}


void graphics::LoadDefaultFont (cfestring &FileName) {
  DefaultFont = new rawbitmap(FileName);
}
