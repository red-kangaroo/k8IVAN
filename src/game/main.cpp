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
#include <iostream>

#include "game.h"
#include "database.h"
#include "feio.h"
#include "igraph.h"
#include "iconf.h"
#include "whandler.h"
#include "hscore.h"
#include "graphics.h"
#include "script.h"
#include "message.h"
#include "proto.h"


int Main (int argc, char **argv) {
  if (argc > 1 && (!strcmp(argv[1], "--version") || !strcmp(argv[1], "-v"))) {
    std::cout << "Iter Vehemens ad Necem version " << IVAN_VERSION << std::endl;
    return 0;
  }

  femath::SetSeed(time(0));
  game::InitGlobalValueMap();
  scriptsystem::Initialize();
  databasesystem::Initialize();
  game::InitLuxTable();
  ivanconfig::Initialize();
  igraph::Init();
  game::CreateBusyAnimationCache();
  globalwindowhandler::SetQuitMessageHandler(game::HandleQuitMessage);
  msgsystem::Init();
  protosystem::Initialize();
  igraph::LoadMenu();

  for(;;) {
    int Select = iosystem::Menu(
      igraph::GetMenuGraphic(),
      v2(RES.X / 2, RES.Y / 2 - 20),
      CONST_S("\r"),
      CONST_S("Start Game\rContinue Game\rConfiguration\rHighscores\rQuit\r"),
      LIGHT_GRAY,
      CONST_S("Released under the GNU\rGeneral Public License\rMore info: see COPYING\r"),
      CONST_S("IVAN v" IVAN_VERSION "\r"),
      true);
    switch (Select) {
      case -1: return 0; // esc
      case 0:
        if (game::Init()) {
          igraph::UnLoadMenu();
          game::Run();
          game::DeInit();
          igraph::LoadMenu();
        }
        break;
      case 1: {
        festring LoadName = iosystem::ContinueMenu(WHITE, LIGHT_GRAY, game::GetSaveDir());
        if (LoadName.GetSize()) {
          LoadName.Resize(LoadName.GetSize()-4);
          if (game::Init(LoadName)) {
            igraph::UnLoadMenu();
            game::Run();
            game::DeInit();
            igraph::LoadMenu();
          }
        }
        break; }
      case 2:
        ivanconfig::Show();
        break;
      case 3: {
        highscore HScore;
        HScore.Draw();
        break; }
      case 4: return 0;
    }
  }
}


#ifdef WIN32
# include <windows.h>
int PASCAL WinMain (HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdLine, int nCmdShow) {
  //return Main(argc, argv);
  //FIXME:
  static char *argv[2] = { "ivan.exe", NULL };
  return Main(1, argv);
}
#endif
