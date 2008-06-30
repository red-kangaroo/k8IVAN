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
#define set_new_handler std::set_new_handler

#include "error.h"

/* Shouldn't be initialized here! */

cchar* globalerrorhandler::BugMsg
= "\n\nPlease send bug report to ivan-users@sourceforge.net\n"
"including a brief description of what you did, what version\n"
"you are running and which kind of system you are using.";

void (*globalerrorhandler::OldNewHandler)() = 0;


void globalerrorhandler::Install()
{
  static truth AlreadyInstalled = false;

  if(!AlreadyInstalled)
  {
    AlreadyInstalled = true;
    OldNewHandler = set_new_handler(NewHandler);

    atexit(globalerrorhandler::DeInstall);
  }
}

void globalerrorhandler::DeInstall()
{
  set_new_handler(OldNewHandler);
}

void globalerrorhandler::Abort(cchar* Format, ...)
{
  char Buffer[512];

  va_list AP;
  va_start(AP, Format);
  vsprintf(Buffer, Format, AP);
  va_end(AP);

  strcat(Buffer, BugMsg);

  std::cout << Buffer << std::endl;

  exit(4);
}

void globalerrorhandler::NewHandler()
{
  cchar* Msg = "Fatal Error: Memory depleted.\n"
        "Get more RAM and hard disk space.";
  std::cout << Msg << std::endl;

  exit(1);
}
