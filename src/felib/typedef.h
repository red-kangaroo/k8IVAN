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
#ifndef __FELIB_TYPEDEF_H__
#define __FELIB_TYPEDEF_H__

#include <stdint.h>

#include "pragmas.h"


class bitmap;
class festring;
struct blitdata;
struct v2;

typedef int32_t truth;
typedef uint8_t uChar;
typedef uint16_t uShort;
typedef uint32_t uInt;
typedef uint32_t feuLong; // uint64_t?
typedef int32_t sLong;
typedef int32_t col16;
typedef uShort packcol16;
typedef feuLong col24;
typedef int32_t alpha;
typedef uChar packalpha;
typedef int32_t priority;
typedef uChar packpriority;
typedef uChar paletteindex;

typedef const char cchar;
typedef const int32_t cint;
typedef const int32_t ctruth;
typedef const uint8_t cuchar;
typedef const uint16_t cushort;
typedef const uint32_t cuint;
typedef const uint32_t culong; // uint64_t?
typedef const int32_t cslong;
typedef const int32_t ccol16;
typedef const uShort cpackcol16;
typedef const feuLong ccol24;
typedef const int32_t calpha;
typedef const uChar cpackalpha;
typedef const int32_t cpriority;
typedef const uChar cpackpriority;
typedef const uChar cpaletteindex;

typedef const bitmap cbitmap;
typedef const blitdata cblitdata;
typedef const festring cfestring;
typedef const v2 cv2;


#endif
