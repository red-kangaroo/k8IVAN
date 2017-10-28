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

#include "feerror.h"
#include "fesave.h"


/* Shouldn't be initialized here! */
cchar *globalerrorhandler::BugMsg = "\n\nPlease don't send your bug reports, i cannot into coding!\n";

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
  char Buffer[2048];
  char *bufptr = Buffer;
  int bufsz = (int)sizeof(Buffer)-1;
  for (;;) {
    va_list ap;
    int n;
    char *np;
    va_start(ap, Format);
    n = vsnprintf(bufptr, bufsz, Format, ap);
    va_end(ap);
    if (n > -1 && n < bufsz) break;
    if (n < -1) n = bufsz+4096;
    np = (char *)realloc((bufptr == Buffer ? NULL : bufptr), n+1);
    if (np == NULL) exit(4); //FIXME
  }
  fprintf(stderr, "%s%s", bufptr, BugMsg);
  {
    festring mydir = inputfile::GetMyDir();
    mydir << "/" << "crash.log";
    FILE *fo = fopen(mydir.CStr(), "a");
    //
    if (fo) {
      fprintf(fo, "============================================================\n%s\n", bufptr);
      fclose(fo);
    }
  }
  //*(int *)(0) = 42;
  exit(4);
}


void globalerrorhandler::NewHandler () {
  const cchar *Msg = "Fatal Error: Memory depleted.\nGet more RAM and hard disk space.";
  std::cout << Msg << std::endl;
  exit(1);
}
