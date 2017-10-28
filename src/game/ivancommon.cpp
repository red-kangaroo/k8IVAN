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
#include <cstdio>
#include <cstring>

#include <cstdlib>
#include <exception> // for std::bad_alloc
#include <new>

/*#include "ivancommon.h"*/


void *operator new (std::size_t size) throw (std::bad_alloc) {
  if (size > 0x1fffffff) abort();
  void *p = malloc(size+64);
  if (!p) throw std::bad_alloc(); // ANSI/ISO compliant behavior
  memset(p, 0, size+64);
  //fprintf(stderr, "GLOBAL NEW!\n");
  return p;
}


void operator delete (void *p) throw () {
  if (p) {
    //fprintf(stderr, "GLOBAL DELETE!\n");
    free(p);
  }
}
