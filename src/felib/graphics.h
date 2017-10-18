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
#ifndef __FELIB_GRAPHICS_H__
#define __FELIB_GRAPHICS_H__

#include "SDL.h"
#include "v2.h"

#define DOUBLE_BUFFER graphics::GetDoubleBuffer()
#define RES graphics::GetRes()
#define FONT graphics::GetDefaultFont()


class bitmap;
class rawbitmap;
class festring;


class graphics {
  friend class bitmap;
public:
  static void Init ();
  static void DeInit ();
  static void SwitchMode ();
  static void SetMode (cchar *Title, cchar *IconName, v2 NewRes, truth FullScreen, sLong adresmod);
  static void BlitDBToScreen ();
  static v2 GetRes () { return Res; }
  static bitmap *GetDoubleBuffer () { return DoubleBuffer; }
  static void LoadDefaultFont (cfestring &FileName);
  static rawbitmap *GetDefaultFont () { return DefaultFont; }
  static void SetSwitchModeHandler (void (*What)()) { SwitchModeHandler = What; }
  static void ReinitMode (sLong adresmod);

private:
  static void (*SwitchModeHandler)();
  static SDL_Surface *Screen;
  static bitmap *DoubleBuffer;
  static v2 Res;
  static int ColorDepth;
  static rawbitmap *DefaultFont;
};


#endif
