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
#ifndef __FELIB_FETIME_H__
#define __FELIB_FETIME_H__

#include <ctime>

#include "festring.h"


class time {
public:
  static time_t GetZeroTime ();
  static time_t TimeAdd (time_t A, time_t B);
  static time_t TimeDifference (time_t A, time_t B);
  // dummies
  static festring VerbalizeAsTimeSpent (time_t);
  static festring VerbalizeAsCalenderTime (time_t);
};


#endif
