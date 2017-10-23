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
class graphics;


struct CursorState {
private:
  int x, y;
  int vis = -666;

private:
  CursorState (int ax, int ay, int avis) { x = ax; y = ay; vis = avis; }
  CursorState (int ax, int ay, int avis, int newx, int newy, truth newvis);
  CursorState (int ax, int ay, int avis, int newx, int newy);
  CursorState (int ax, int ay, int avis, truth newvis);

public:
  ~CursorState ();
  void reset () { vis = -666; }

  friend graphics;
};


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
#ifndef DISABLE_OPENGL
  static void createTexture ();
#endif

  static void gotoXY (int cx, int cy);
  static bool isCursorVisible ();
  static void showCursor ();
  static void hideCursor ();

  // on destroying, `CursorState` will restore the state
  static CursorState getCursorState ();
  static CursorState getCursorState (int newx, int newy, truth newvis);
  static CursorState getCursorState (int newx, int newy);
  static CursorState getCursorState (truth newvis);

private:
  static void (*SwitchModeHandler)();
  static SDL_Surface *Screen;
  static bitmap *DoubleBuffer;
  static v2 Res;
  static int ColorDepth;
  static rawbitmap *DefaultFont;

  static int curx;
  static int cury;
  static int curvisible;

  friend CursorState;
};


#endif
