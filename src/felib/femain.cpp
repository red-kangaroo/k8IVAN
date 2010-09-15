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
#ifdef WIN32
# include <windows.h>
#endif

#include <iostream>
#include <cstdlib>

#include "SDL.h"
#include "error.h"


extern int Main (int, char **);


int main (int argc, char *argv[]) {
  /*try
  {*/
    return Main(argc, argv);
  /*}
  catch(...)
  {
    cchar* Msg = "Fatal Error: Unknown exception thrown.";
    std::cout << Msg << globalerrorhandler::GetBugMsg() << std::endl;
    exit(3);
  }
  exit(0);*/
}


/*
#ifdef WIN32
int PASCAL WinMain (HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdLine, int nCmdShow) {
  //return Main(argc, argv);
  //FIXME:
  static char *argv[2] = { "ivan.exe", NULL };
  return Main(1, argv);
}
#endif
*/
