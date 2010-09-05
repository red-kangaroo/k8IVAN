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
#include <cstdarg>
#include <cstring>
#include <cstdlib>
#include <cstdio>

#include <iostream>

#include <new>
#define set_new_handler  std::set_new_handler

#include "error.h"


/* Shouldn't be initialized here! */
cchar *globalerrorhandler::BugMsg =
  "\n\nPlease don't send your bug reports, i don't give a shit.\n";

void (*globalerrorhandler::OldNewHandler) () = 0;


void globalerrorhandler::Install () {
  static truth AlreadyInstalled = false;
  if (!AlreadyInstalled) {
    AlreadyInstalled = true;
    OldNewHandler = set_new_handler(NewHandler);
    atexit(globalerrorhandler::DeInstall);
  }
}


void globalerrorhandler::DeInstall () {
  set_new_handler(OldNewHandler);
}


void globalerrorhandler::Abort (cchar *Format, ...) {
  char Buffer[512];
  va_list AP;
  va_start(AP, Format);
  vsnprintf(Buffer, sizeof(Buffer)-1, Format, AP);
  va_end(AP);
  strcat(Buffer, BugMsg);
  std::cout << Buffer << std::endl;
  exit(4);
}


void globalerrorhandler::NewHandler () {
  const cchar *Msg = "Fatal Error: Memory depleted.\nGet more RAM and hard disk space.";
  std::cout << Msg << std::endl;
  exit(1);
}
