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
#ifndef __FELIB_WHANDLER_H__
#define __FELIB_WHANDLER_H__

#include <vector>
#include "SDL.h"

#include "felibdef.h"


#define GET_KEY   globalwindowhandler::GetKey
#define READ_KEY  globalwindowhandler::ReadKey
#define GET_TICK  globalwindowhandler::GetTick
#define DELAY     globalwindowhandler::Delay


class globalwindowhandler {
public:
  static void KSDLProcessEvents (truth dodelay=false);
  static void KSDLWaitEvent (void);
  static int GetKey (truth EmptyBuffer=true);
  static int ReadKey ();
  static void InstallControlLoop (truth (*What)());
  static void DeInstallControlLoop (truth (*What)());
  static ulong GetTick () { return Tick; }
  static truth ControlLoopsInstalled () { return Controls; }
  static void EnableControlLoops () { ControlLoopsEnabled = true; }
  static void DisableControlLoops () { ControlLoopsEnabled = false; }
  static truth ShiftIsDown ();
  static void Init ();
  static void SetQuitMessageHandler (truth (*What)()) { QuitMessageHandler = What; }
  static ulong UpdateTick() { return Tick = SDL_GetTicks()/40; }

  static void Delay (int ms);

private:
  static void ProcessMessage (SDL_Event *Event);

  static std::vector<int> KeyBuffer;
  static truth (*QuitMessageHandler)();
  static truth (*ControlLoop[MAX_CONTROLS])();
  static int Controls;
  static ulong Tick;
  static truth ControlLoopsEnabled;
};


#endif
