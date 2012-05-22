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
#ifndef __ENTITY_H__
#define __ENTITY_H__

#include <list>

#include "ivandef.h"
#include "festring.h"


#define EXISTS        1
#define HAS_BE        2
#define ENTITY_FLAGS  3


class square;
struct v2;


class entity {
  friend class pool;

public:
  static void *operator new (size_t size);
  static void operator delete (void *p);

public:
  static truth IsInDeadSet (entity *e);
  static void BurnDeadSet ();
  static int GetUniqueMemoryMark (entity *e);
  static void DumpDeadSet ();

public:
  entity (int);
  entity (const entity &);
  virtual ~entity ();

  virtual void Be () {}
  virtual square *GetSquareUnderEntity (int = 0) const = 0;
  virtual void SignalVolumeAndWeightChange () {}
  col24 GetEmitation () const { return Emitation; }
  virtual void SignalEmitationIncrease (col24) {}
  virtual void SignalEmitationDecrease (col24) {}
  virtual truth ContentsCanBeSeenBy (ccharacter *) const { return false; }
  virtual truth AllowSpoil () const { return false; }
  virtual void SignalSpoil (material *) {}
  virtual void SignalSpoilLevelChange (material *) {}
  virtual truth IsOnGround () const = 0;
  virtual truth AllowContentEmitation () const { return true; }
  virtual void SignalRustLevelChange () {}
  virtual material *RemoveMaterial (material*) { return 0; }
  virtual character *TryNecromancy (character*) { return 0; }
  virtual void SignalDisappearance () {}
  virtual void SpecialEatEffect (character *, int) {}
  virtual feuLong GetTrapID () const { return 0; }
  virtual feuLong GetVictimID () const { return 0; }
  virtual void AddTrapName (festring &, int) const {}
  virtual void UnStick () {}
  virtual void UnStick (int) {}
  virtual truth TryToUnStick (character *, v2) { return false; }
  virtual int GetTrapType () const { return 0; }
  virtual truth IsStuckTo (ccharacter *) const { return false; }
  virtual ccharacter *FindCarrier () const { return 0; }

  truth Exists () const { return Flags & EXISTS; }
  void SendToHell ();
  truth IsEnabled () const { return Flags & HAS_BE; }
  void Enable ();
  void Disable ();
  void AddFlags (feuLong What) { Flags |= What; }
  void RemoveFlags (feuLong What) { Flags &= ~What; }

protected:
  col24 Emitation;
  feuLong Flags;

private:
  entity (); // no such constructor

public:
  festring mOnEvents;
};


#endif
