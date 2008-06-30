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

#ifndef __GRAPHICS_H__
#define __GRAPHICS_H__

#include "SDL.h"

#include "v2.h"

#define DOUBLE_BUFFER graphics::GetDoubleBuffer()
#define RES graphics::GetRes()
#define FONT graphics::GetDefaultFont()

class bitmap;
class rawbitmap;
class festring;

class graphics
{
 public:
  friend class bitmap;
  static void Init();
  static void DeInit();
  static void SwitchMode();
  static void SetMode(cchar*, cchar*, v2, truth);
  static void BlitDBToScreen();
  static v2 GetRes() { return Res; }
  static bitmap* GetDoubleBuffer() { return DoubleBuffer; }
  static void LoadDefaultFont(cfestring&);
  static rawbitmap* GetDefaultFont() { return DefaultFont; }
  static void SetSwitchModeHandler(void (*What)())
  { SwitchModeHandler = What; }
 private:
  static void (*SwitchModeHandler)();
  static SDL_Surface* Screen;
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
  static SDL_Surface* TempSurface;
#endif
  static bitmap* DoubleBuffer;
  static v2 Res;
  static int ColorDepth;
  static rawbitmap* DefaultFont;
};

#endif
