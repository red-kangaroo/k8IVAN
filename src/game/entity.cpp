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
#include <stdio.h>
#include <set>
/* Compiled through coreset.cpp */


//#define xlogf(...)  do { fprintf(stderr, __VA_ARGS__); fflush(stderr); } while (0)
#define xlogf(...)


truth entity::TryToUnStick (character *, v2) { return false; }

entity::entity (const entity &Entity) : Emitation(Entity.Emitation), Flags(Entity.Flags), mInPool(false), mInHell(false) {
  //xlogf("entity::entity0: %p\n", this);
  if (Flags & HAS_BE) pool::Add(this);
  mOnEvents = Entity.mOnEvents;
}


entity::entity (int Flags) : Emitation(0), Flags(Flags|EXISTS), mInPool(false), mInHell(false) {
  //xlogf("entity::entity1: %p\n", this);
  if (Flags & HAS_BE) pool::Add(this);
}


entity::~entity () {
  //xlogf("entity dies!\n");
  /*if (Flags & HAS_BE)*/
  pool::Remove(this);
  //xlogf("entity::~entity: %p\n", this);
}


/* Calling SendToHell() marks the entity dead,
 * so that it will be removed by hell::Burn() at the end of the tick.
 * In general, you should never delete an entity instead of calling this,
 * because pool::Be() will crash if the entity currently Be()ing is deleted. */
void entity::SendToHell () {
  if (Flags & EXISTS) {
    if (Flags & HAS_BE) {
      pool::Remove(this);
      Flags ^= HAS_BE;
    }
    pool::AddToHell(this);
    Flags ^= EXISTS;
  }
}


/* These functions tell the poolsystem whether the Be() function of the entity
 * ought to be called during each tick, thus allowing it to alter itself independently.
 * If the entity is stabile, use Disable(), since it speeds up the game. */
void entity::Enable () {
  if (!(Flags & HAS_BE)) {
    pool::Add(this);
    Flags |= HAS_BE;
  }
}


void entity::Disable () {
  if (Flags & HAS_BE) {
    pool::Remove(this);
    Flags ^= HAS_BE;
  }
}


typedef std::set<void *> VoidSet;

static VoidSet mDeadSet;
static VoidSet mLiveSet;


truth entity::IsInDeadSet (entity *e) {
  /*
  VoidSet::const_iterator i = mDeadSet.find((void *)e);
  return (i != mDeadSet.end());
  */
  VoidSet::const_iterator i = mLiveSet.find((void *)e);
  return (i == mLiveSet.end());
}


truth entity::IsInLiveSet (entity *e) {
  /*
  VoidSet::const_iterator i = mDeadSet.find((void *)e);
  return (i != mDeadSet.end());
  */
  VoidSet::const_iterator i = mLiveSet.find((void *)e);
  return (i != mLiveSet.end());
}


void entity::BurnDeadSet () {
  for (VoidSet::const_iterator i = mDeadSet.begin(); i != mDeadSet.end(); ++i) {
    VoidSet::const_iterator it = mLiveSet.find(*i);
    //
    if (it == mLiveSet.end()) {
      xlogf("setfree: %p\n", *i);
      free(*i);
    }
  }
  mDeadSet.clear();
}


void *entity::operator new (size_t size) {
  void *p;
  //
  if (size == 0) {
    fprintf(stderr, "FATAL: ALLOCATING ENTITY OF ZERO SIZE!\n");
    abort();
  }
  p = calloc(1, size);
  mLiveSet.insert(p);
  xlogf("new(%u): %p\n", size, p);
  return p;
}


void entity::operator delete (void *p) {
  if (p) {
    VoidSet::iterator it = mLiveSet.find(p);
    //
    if (it == mLiveSet.end()) {
      fprintf(stderr, "FATAL: DEALLOCATING UNKNOWN ENTITY: %p\n", p);
      abort();
    } else {
      mLiveSet.erase(it);
    }
    //
    mDeadSet.insert(p);
    if (!pool::IsBurningHell()) {
      xlogf("free0: %p\n", p);
      //pool::Remove(static_cast<entity *>(p));
      pool::AddToHell(static_cast<entity *>(p));
    } else {
      xlogf("free1: %p\n", p);
    }
  }
}


#undef xlogf
