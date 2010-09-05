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
