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

#ifndef __POOL_H__
#define __POOL_H__

#include "ivancommon.h"


class entity;


class pool {
public:
  pool ();

  static void Add (entity *);
  static void Remove (entity *);
  static void AddToHell (entity *);
  static void RemoveFromHell (entity *);
  static void BurnHell ();
  static void Be ();
  static truth IsBurningHell ();
  static void KillBees ();
  static void AbortBe ();
  //
  static void RegisterState (truth doreg);
  //
  static void RemoveEverything (); // don't delete!
};


#endif
