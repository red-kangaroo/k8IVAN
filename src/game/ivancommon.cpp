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
#include "ivancommon.h"

#include <cstdio>
#include <cstring>


void *operator new (size_t size) throw (std::bad_alloc) {
  void *p = malloc(size+1);
  //
  if (!p) throw std::bad_alloc(); // ANSI/ISO compliant behavior
  memset(p, 0, size+1);
  //fprintf(stderr, "GLOBAL NEW!\n");
  return p;
}


void operator delete (void *p) throw () {
  if (p) {
    //fprintf(stderr, "GLOBAL DELETE!\n");
    free(p);
  }
}
