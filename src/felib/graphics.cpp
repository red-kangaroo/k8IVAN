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


void graphics::SetMode (cchar *Title, cchar *IconName, v2 NewRes, truth FullScreen) {
  if (IconName) {
    SDL_Surface *Icon = SDL_LoadBMP(IconName);
    SDL_SetColorKey(Icon, SDL_SRCCOLORKEY, SDL_MapRGB(Icon->format, 255, 255, 255));
    SDL_WM_SetIcon(Icon, NULL);
  }
  uLong Flags = SDL_SWSURFACE;
  if (FullScreen) {
    SDL_ShowCursor(SDL_DISABLE);
    Flags |= SDL_FULLSCREEN;
  }
  Screen = SDL_SetVideoMode(NewRes.X, NewRes.Y, 16, Flags);
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


void graphics::BlitDBToScreen () {
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
  SDL_LockSurface(TempSurface);
  packcol16 *SrcPtr = DoubleBuffer->GetImage()[0];
  packcol16 *DestPtr = static_cast<packcol16 *>(TempSurface->pixels);
  uLong ScreenYMove = (TempSurface->pitch >> 1);
  uLong LineSize = Res.X << 1;
  for (int y = 0; y < Res.Y; ++y, SrcPtr += Res.X, DestPtr += ScreenYMove) memcpy(DestPtr, SrcPtr, LineSize);
  SDL_UnlockSurface(TempSurface);
  SDL_Surface *S = SDL_DisplayFormat(TempSurface);
  SDL_BlitSurface(S, NULL, Screen, NULL);
  SDL_FreeSurface(S);
  SDL_UpdateRect(Screen, 0, 0, Res.X, Res.Y);
#else
  if (SDL_MUSTLOCK(Screen) && SDL_LockSurface(Screen) < 0) ABORT("Can't lock screen");
  packcol16 *SrcPtr = DoubleBuffer->GetImage()[0];
  packcol16 *DestPtr = static_cast<packcol16 *>(Screen->pixels);
  uLong ScreenYMove = (Screen->pitch >> 1);
  uLong LineSize = Res.X << 1;
  for (int y = 0; y < Res.Y; ++y, SrcPtr += Res.X, DestPtr += ScreenYMove) memcpy(DestPtr, SrcPtr, LineSize);
  if (SDL_MUSTLOCK(Screen)) SDL_UnlockSurface(Screen);
  SDL_UpdateRect(Screen, 0, 0, Res.X, Res.Y);
}
#endif


void graphics::SwitchMode () {
  uLong Flags;
  if (Screen->flags & SDL_FULLSCREEN) {
    SDL_ShowCursor(SDL_ENABLE);
    Flags = SDL_SWSURFACE;
  } else {
    SDL_ShowCursor(SDL_DISABLE);
    Flags = SDL_SWSURFACE|SDL_FULLSCREEN;
  }
  if (SwitchModeHandler) SwitchModeHandler();
  Screen = SDL_SetVideoMode(Res.X, Res.Y, ColorDepth, Flags);
  if (!Screen) ABORT("Couldn't toggle fullscreen mode.");
  BlitDBToScreen();
}


void graphics::LoadDefaultFont (cfestring &FileName) {
  DefaultFont = new rawbitmap(FileName);
}
