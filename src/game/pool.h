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


class entity;


class pool {
public:
  static void Add (entity *);
  static void Remove (entity *);
  static void AddToHell (entity *);
  static void BurnHell ();
  static void Be ();
  static truth IsBurningHell ();
  static void KillBees ();
  //
  static void RegisterState (truth doreg);
};


#endif
