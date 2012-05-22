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
/* Compiled through coreset.cpp */


//#define xlogf(...)  do { fprintf(stderr, __VA_ARGS__); fflush(stderr); } while (0)
#define xlogf(...)


////////////////////////////////////////////////////////////////////////////////
void entity::DumpDeadSet () {
/*
  fprintf(stderr, "=== dead set ===\n");
  for (VoidSet::const_iterator i = mDeadSet.begin(); i != mDeadSet.end(); ++i) fprintf(stderr, "%p\n", *i);
*/
}


////////////////////////////////////////////////////////////////////////////////
entity::entity (const entity &Entity) : Emitation(Entity.Emitation), Flags(Entity.Flags) {
  xlogf("entity::entity0: %p\n", this);
  pool::RemoveFromHell(this);
  if (Flags & HAS_BE) {
    pool::Add(this);
  } else {
    // just in case
    pool::Remove(this);
  }
  mOnEvents = Entity.mOnEvents;
}


entity::entity (int Flags) : Emitation(0), Flags(Flags|EXISTS) {
  xlogf("entity::entity1: %p\n", this);
  pool::RemoveFromHell(this);
  if (Flags & HAS_BE) {
    pool::Add(this);
  } else {
    // just in case
    pool::Remove(this);
  }
}


entity::~entity () {
  xlogf("entity::~entity: %p\n", this);
  pool::Remove(this);
  pool::RemoveFromHell(this);
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
  if (e) {
    int *mp = (int *)e;
    //
    return mp[-1];
  }
  return -1;
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
  return p;
}


void entity::operator delete (void *p) {
  if (p) {
    int *mp = (int *)p;
    //
    xlogf("delete(%d): %p\n", mp[-1], p);
    --mp;
    *mp = -1;
    free(mp);
  }
}


#undef xlogf
