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


////////////////////////////////////////////////////////////////////////////////
typedef std::set<void *> VoidSet;

static VoidSet mDeadSet; // deleted objects


static truth isInDeadSet (void *e) {
  VoidSet::const_iterator i = mDeadSet.find(e);
  return (i != mDeadSet.end());
}


static void removeFromDeadSet (void *p) {
  VoidSet::const_iterator i = mDeadSet.find(p);
  if (i != mDeadSet.end()) mDeadSet.erase(i);
}


static void addToDeadSet (void *p) {
  mDeadSet.insert(p);
}


truth entity::IsInDeadSet (entity *e) {
  VoidSet::const_iterator i = mDeadSet.find((void *)e);
  return (i != mDeadSet.end());
}


void entity::BurnDeadSet () {
  mDeadSet.clear();
}


void entity::DumpDeadSet () {
  fprintf(stderr, "=== dead set ===\n");
  for (VoidSet::const_iterator i = mDeadSet.begin(); i != mDeadSet.end(); ++i) fprintf(stderr, "%p\n", *i);
}


////////////////////////////////////////////////////////////////////////////////
entity::entity (const entity &Entity) : Emitation(Entity.Emitation), Flags(Entity.Flags) {
  xlogf("entity::entity0: %p\n", this);
  removeFromDeadSet(this);
  if (Flags & HAS_BE) pool::Add(this);
  mOnEvents = Entity.mOnEvents;
}


entity::entity (int Flags) : Emitation(0), Flags(Flags|EXISTS) {
  xlogf("entity::entity1: %p\n", this);
  removeFromDeadSet(this);
  if (Flags & HAS_BE) pool::Add(this);
}


entity::~entity () {
  xlogf("entity::~entity: %p\n", this);
  pool::Remove(this); // it can be in hell, nobody cares
  addToDeadSet(this);
}


/* Calling SendToHell() marks the entity dead,
 * so that it will be removed by hell::Burn() at the end of the tick.
 * In general, you should never delete an entity instead of calling this,
 * because pool::Be() will crash if the entity currently Be()ing is deleted. */
void entity::SendToHell () {
  if (Flags&EXISTS) {
    if (Flags&HAS_BE) {
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
  if (!(Flags&HAS_BE)) {
    pool::Add(this);
    Flags |= HAS_BE;
  }
}


void entity::Disable () {
  if (Flags&HAS_BE) {
    pool::Remove(this);
    Flags ^= HAS_BE;
  }
}


int entity::GetUniqueMemoryMark (entity *e) {
  int *mp = (int *)e;
  //
  if (isInDeadSet(e)) {
    fprintf(stderr, "FATAL: trying to get mark from dead soul!\n");
    abort();
  }
  return mp[-1];
}


void *entity::operator new (size_t size) {
  void *p;
  static int mark = 0;
  //
  if (size == 0) {
    fprintf(stderr, "FATAL: ALLOCATING ENTITY OF ZERO SIZE!\n");
    abort();
  }
  p = calloc(1, size+sizeof(int));
  {
    int *mp = (int *)p;
    //
    *mp = ++mark;
    p = mp+1;
  }
  //if (isInDeadSet(p)) xlogf("***RESURRECTING(%d): %p\n", mark, p); else xlogf("new(%d): %p\n", mark, p);
  return p;
}


void entity::operator delete (void *p) {
  if (p) {
    int *mp = (int *)p;
    //
    if (!isInDeadSet(p)) {
      fprintf(stderr, "FATAL: DEALLOCATING NON-DESTRUCTED ENTITY: %p\n", p);
      abort();
    }
    xlogf("delete(%d): %p\n", mp[-1], p);
    --mp;
    *mp = -1;
    free(mp);
  }
}


#undef xlogf
