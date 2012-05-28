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
#ifndef __IVANCOMMON_H__
#define __IVANCOMMON_H__

#include <cstdlib>
#include <exception> // for std::bad_alloc
#include <new>


void *operator new (size_t size) throw (std::bad_alloc);
void operator delete (void *p) throw ();


#endif
