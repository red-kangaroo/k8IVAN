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
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include <algorithm>

#include "whandler.h"
#include "graphics.h"
#include "feerror.h"
#include "bitmap.h"
#include "festring.h"
#include "fesave.h"


truth (*globalwindowhandler::ControlLoop[MAX_CONTROLS])();
int globalwindowhandler::Controls = 0;
feuLong globalwindowhandler::Tick;
truth globalwindowhandler::ControlLoopsEnabled = true;
Uint32 globalwindowhandler::nextGameTick = 0;

truth globalwindowhandler::lastShift = false;
truth globalwindowhandler::lastCtrl = false;
truth globalwindowhandler::lastAlt = false;

std::vector<int> globalwindowhandler::KeyBuffer;
truth (*globalwindowhandler::QuitMessageHandler)() = 0;


void globalwindowhandler::InstallControlLoop (truth (*What)()) {
  if (Controls == MAX_CONTROLS) ABORT("Animation control frenzy!");
  ControlLoop[Controls++] = What;
}


void globalwindowhandler::DeInstallControlLoop (truth (*What)()) {
  int c;
  for (c = 0; c < Controls; ++c) if (ControlLoop[c] == What) break;
  if (c != Controls) {
    --Controls;
    for (; c < Controls; ++c) ControlLoop[c] = ControlLoop[c+1];
  }
}


void globalwindowhandler::Init () {
  SDL_EnableUNICODE(1);
  SDL_EnableKeyRepeat(500, 30);
}


static bool WaitEventTimeout (int timeout) {
  if (timeout <= 0) return (SDL_PollEvent(nullptr) != 0);
  Uint32 endtick = SDL_GetTicks()+(Uint32)timeout;
  for (;;) {
    Uint32 curtick = SDL_GetTicks();
    if (SDL_PollEvent(nullptr)) return true;
    if (curtick >= endtick) return false;
    Uint32 left = endtick-curtick;
    if (left > 20) left = 20;
    SDL_Delay(left);
  }
}


void globalwindowhandler::KSDLProcessEvents (truth dodelay) {
  SDL_Event Event;

  memset(&Event, 0, sizeof(Event)); /* some systems needs this fix */
  while (SDL_PollEvent(&Event)) {
    ProcessMessage(&Event);
    memset(&Event, 0, sizeof(Event)); /* some systems needs this fix */
  }

  if (dodelay) {
    Uint32 curtick = SDL_GetTicks();
    Uint32 timeout = (curtick >= nextGameTick ? 0 : nextGameTick-curtick);
    if (timeout > 0) {
      if (WaitEventTimeout(timeout)) {
        memset(&Event, 0, sizeof(Event)); /* some systems needs this fix */
        while (SDL_PollEvent(&Event)) {
          ProcessMessage(&Event);
          memset(&Event, 0, sizeof(Event)); /* some systems needs this fix */
        }
      }
    }
    curtick = SDL_GetTicks();
    while (nextGameTick <= curtick) nextGameTick += 40;
  }
}


//FIXME
void globalwindowhandler::Delay (int ms) {
  SDL_Event event;
  if (ms < 0) ms = 0;
  Uint32 endtick = SDL_GetTicks()+(Uint32)ms;
  for (;;) {
    Uint32 curtick = SDL_GetTicks();
    Uint32 timeout = (curtick >= endtick ? 0 : endtick-curtick);
    if (WaitEventTimeout(timeout)) {
      memset(&event, 0, sizeof(event)); /* some systems needs this fix */
      while (SDL_PeepEvents(&event, 1, SDL_PEEKEVENT, SDL_ALLEVENTS)) {
        switch (event.type) {
          case SDL_VIDEOEXPOSE:
            memset(&event, 0, sizeof(event)); /* some systems needs this fix */
            SDL_PollEvent(&event); // remove event
            ProcessMessage(&event);
            break;
          case SDL_QUIT:
          case SDL_KEYDOWN:
            return;
          default:
            memset(&event, 0, sizeof(event)); /* some systems needs this fix */
            SDL_PollEvent(&event);
            break;
        }
      }
    }
    if (timeout == 0) break;
  }
}


void globalwindowhandler::KSDLWaitEvent () {
  SDL_Event Event;
  memset(&Event, 0, sizeof(Event)); /* some systems needs this fix */
  SDL_WaitEvent(&Event);
  ProcessMessage(&Event);
}


int globalwindowhandler::GetKey (truth EmptyBuffer) {
  static feuLong LastTick = 0;
  // Flush the buffer
  if (EmptyBuffer) {
    KSDLProcessEvents();
    KeyBuffer.clear();
  }
  Uint32 nexttm = SDL_GetTicks()+100;
  for (;;) {
    if (!KeyBuffer.empty()) {
      int Key = KeyBuffer[0];
      KeyBuffer.erase(KeyBuffer.begin());
      if (Key > 0xE000) return Key-0xE000;
      if (Key && Key < 0x81) return Key; // if it's an ASCII symbol
    } else {
      if (SDL_GetAppState()&SDL_APPACTIVE) {
        KSDLProcessEvents(true);
        if (Controls && ControlLoopsEnabled) {
          UpdateTick();
          if (LastTick != Tick) {
            LastTick = Tick;
            truth Draw = graphics::isCursorVisible();
            if (!Draw) { for (int c = 0; c < Controls; ++c) if (ControlLoop[c]()) Draw = true; }
            if (Draw) graphics::BlitDBToScreen();
          }
        } else if (graphics::isCursorVisible()) {
          Uint32 curtick = SDL_GetTicks();
          if (curtick >= nexttm) {
            graphics::BlitDBToScreen();
            while (nexttm <= curtick) nexttm += 100;
          }
        }
      } else {
        KSDLWaitEvent();
      }
    }
  }
  // doesn't return here
}


int globalwindowhandler::ReadKey () {
  if (SDL_GetAppState()&SDL_APPACTIVE) KSDLProcessEvents(); else KSDLWaitEvent();
  return (KeyBuffer.size() ? GetKey(false) : 0);
}


void globalwindowhandler::ProcessMessage (SDL_Event *Event) {
  int KeyPressed;
  switch (Event->active.type) {
    case SDL_VIDEOEXPOSE:
      graphics::BlitDBToScreen();
      break;
    case SDL_QUIT:
      if (!QuitMessageHandler || QuitMessageHandler()) exit(0);
      return;
    case SDL_KEYDOWN:
      lastAlt = ((Event->key.keysym.mod&KMOD_ALT) != 0);
      lastCtrl = ((Event->key.keysym.mod&KMOD_CTRL) != 0);
      lastShift = ((Event->key.keysym.mod&KMOD_SHIFT) != 0);
      switch (Event->key.keysym.sym) {
        case SDLK_RETURN: case SDLK_KP_ENTER:
          if (Event->key.keysym.mod & KMOD_ALT) {
            graphics::SwitchMode();
            return;
          }
          KeyPressed = KEY_ENTER; //Event->key.keysym.unicode;
          break;
        case SDLK_DOWN: case SDLK_KP2:
          KeyPressed = KEY_DOWN+0xE000;
          break;
        case SDLK_UP: case SDLK_KP8:
          KeyPressed = KEY_UP+0xE000;
          break;
        case SDLK_RIGHT: case SDLK_KP6:
          KeyPressed = KEY_RIGHT+0xE000;
          break;
        case SDLK_LEFT: case SDLK_KP4:
          KeyPressed = KEY_LEFT+0xE000;
          break;
        case SDLK_HOME: case SDLK_KP7:
          KeyPressed = KEY_HOME+0xE000;
          break;
        case SDLK_END: case SDLK_KP1:
          KeyPressed = KEY_END+0xE000;
          break;
        case SDLK_PAGEUP: case SDLK_KP9:
          KeyPressed = KEY_PAGE_UP+0xE000;
          break;
        case SDLK_PAGEDOWN: case SDLK_KP3:
          KeyPressed = KEY_PAGE_DOWN+0xE000;
          break;
        case SDLK_KP5:
          KeyPressed = '.';
          break;
        case SDLK_KP0:
          KeyPressed = KEY_INS+0xE000;
          break;
        case SDLK_KP_PERIOD:
          KeyPressed = KEY_DEL+0xE000;
          break;
        case SDLK_KP_MINUS:
          KeyPressed = KEY_ESC;
          break;
        case SDLK_KP_PLUS:
          KeyPressed = KEY_PLUS+0xE000;
          break;
        case SDLK_SYSREQ: case SDLK_PRINT: {
          /*DOUBLE_BUFFER->Save(festring(getenv("HOME")) + "/Scrshot.bmp");*/
          //festring dir = inputfile::GetMyDir()+"/SShots";
          festring dir;
#ifdef LOCAL_SAVES
          dir << inputfile::GetMyDir() << "/screenshots";
#else
          dir << getenv("HOME") << "/.ivan-screenshots";
#endif
          mkdir(dir.CStr(), 0755);
          for (int f = 0; f < 1000; f++) {
            char buf[16];
            sprintf(buf, "%03d", f);
            festring fn = dir+"/scrshot"+buf;
#if defined(HAVE_IMLIB2) || defined(HAVE_LIBPNG)
            fn << ".png";
#else
            fn << ".bmp";
#endif
            FILE *fl = fopen(fn.CStr(), "r");
            if (fl) { fclose(fl); continue; }
            fprintf(stderr, "screenshot: %s\n", fn.CStr());
#if defined(HAVE_IMLIB2) || defined(HAVE_LIBPNG)
            DOUBLE_BUFFER->SavePNG(fn);
#else
            DOUBLE_BUFFER->SaveBMP(fn);
#endif
            break;
          }
          return; }
        /*
        case SDLK_e:
          if ((Event->key.keysym.mod & KMOD_ALT) &&
              ((Event->key.keysym.mod & KMOD_LCTRL) || (Event->key.keysym.mod & KMOD_RCTRL))) {
            KeyPressed = 127;
            break;
          }
        */
        default:
          KeyPressed = Event->key.keysym.unicode;
          if (!KeyPressed) return;
      }
      if (std::find(KeyBuffer.begin(), KeyBuffer.end(), KeyPressed) == KeyBuffer.end()) KeyBuffer.push_back(KeyPressed);
      break;
  }
}


// returns true if shift is being pressed
// else false
truth globalwindowhandler::ShiftIsDown () {
  return false;
}
