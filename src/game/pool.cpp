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
/* Compiled through coreset.cpp */
#include <stdio.h>
#include <set>


//#define xlogf(...)  do { fprintf(stderr, __VA_ARGS__); fflush(stderr); } while (0)
#define xlogf(...)


////////////////////////////////////////////////////////////////////////////////
typedef std::set<entity *> EntitySet;

static EntitySet beSets[2]; // sets of entities which Be() method should be called
static EntitySet dontDo; // entities that was removed on the current step
static EntitySet hellSets[2]; // set of entities that should be burned
static int curBeSet = 0;
static int curHellSet = 0;


static bool burningHell = false; // is we burning lost souls?
static bool doBeing = false; // being process in progress?
static bool doRegister = true; // nothing %-)


////////////////////////////////////////////////////////////////////////////////
static truth isInBeSet (int idx, entity *p) {
  EntitySet::const_iterator it = beSets[idx].find(p);
  return (it != beSets[idx].end());
}


static truth isInHell (entity *p) {
  EntitySet::const_iterator it = hellSets[curHellSet].find(p);
  return (it != hellSets[curHellSet].end());
}


static truth isDontDo (entity *p) {
  EntitySet::const_iterator it = dontDo.find(p);
  return (it != dontDo.end());
}


static void removeFromHell (entity *p) {
  EntitySet::const_iterator it = hellSets[curHellSet].find(p);
  if (it != hellSets[curHellSet].end()) hellSets[curHellSet].erase(p);
}


static void removeFromDontDo (entity *p) {
  EntitySet::const_iterator it = dontDo.find(p);
  if (it != dontDo.end()) dontDo.erase(p);
}


static void removeFromBeSet (int idx, entity *p) {
  EntitySet::const_iterator it = beSets[idx].find(p);
  if (it != beSets[idx].end()) beSets[idx].erase(p);
}


////////////////////////////////////////////////////////////////////////////////
truth pool::IsBurningHell () {
  return burningHell;
}


void pool::RegisterState (truth doreg) {
  //doRegister = doreg;
}


////////////////////////////////////////////////////////////////////////////////
/* Calls the Be() function of each self-changeable entity during each tick,
 * thus allowing acting characters, spoiling food etc. */
void pool::Be () {
  EntitySet newBe, leftBe;
  //
  if (burningHell) { fprintf(stderr, "FATAL: started to being while burning souls!\n"); entity::DumpDeadSet(); abort(); }
  if (doBeing) { fprintf(stderr, "FATAL: started to being while already being!\n"); entity::DumpDeadSet(); abort(); }
  //
  xlogf("Be: START (%u:%u)\n", beSets[curBeSet].size(), beSets[curBeSet^1].size());
  doBeing = true;
  //
  for (EntitySet::const_iterator i = beSets[curBeSet].begin(); i != beSets[curBeSet].end(); ++i) {
    if (!entity::IsInDeadSet(*i) && !isInHell(*i) && !isDontDo(*i)) {
      int mmark = entity::GetUniqueMemoryMark(*i);
      xlogf("Be: %p (%d)\n", *i, mmark);
      (*i)->Be();
      if (entity::GetUniqueMemoryMark(*i) != mmark) {
        fprintf(stderr, "FATAL: entity polymorphed while being!\n");
        entity::DumpDeadSet();
        abort();
      }
    }
  }
  // swap sets
  for (EntitySet::const_iterator i = beSets[curBeSet].begin(); i != beSets[curBeSet].end(); ++i) {
    if (!entity::IsInDeadSet(*i) && !isInHell(*i) && !isDontDo(*i)) beSets[curBeSet^1].insert(*i);
  }
  beSets[curBeSet].clear();
  curBeSet ^= 1;
  dontDo.clear();
  //
  doBeing = false;
  xlogf("Be: DONE (%u:%u)\n", beSets[curBeSet].size(), beSets[curBeSet^1].size());
}


void pool::BurnHell () {
  EntitySet newBe;
  //
  if (burningHell) { fprintf(stderr, "FATAL: started to burning souls while already burning souls!\n"); entity::DumpDeadSet(); abort(); }
  if (doBeing) { fprintf(stderr, "FATAL: started to burning souls while being!\n"); entity::DumpDeadSet(); abort(); }
  //
  burningHell = true;
  xlogf("BurnHell: START (%u)\n", hellSets[curHellSet].size());
  //
  for (EntitySet::const_iterator i = hellSets[curHellSet].begin(); i != hellSets[curHellSet].end(); ++i) {
    if (!entity::IsInDeadSet(*i)) delete (*i);
  }
  hellSets[curHellSet].clear();
  curHellSet ^= 1;
  //
  entity::BurnDeadSet();
  //
  burningHell = false;
  xlogf("BurnHell: DONE\n");
}


void pool::KillBees () {
/*
  xlogf("pool::KillBees()\n");
  beSets.clear();
  xnewBeSet.clear();
  notDoSet.clear();
  BurnHell();
*/
}


void pool::Add (entity *Entity) {
  if (doRegister && Entity) {
    if (entity::IsInDeadSet(Entity)) {
      fprintf(stderr, "FATAL: trying to disguise zombie as living critter!\n");
      entity::DumpDeadSet();
      abort();
    }
    //
    if (burningHell && isInHell(Entity)) {
      fprintf(stderr, "FATAL: trying to resurrect the soul while burning!\n");
      entity::DumpDeadSet();
      abort();
    }
    //
    removeFromHell(Entity);
    curHellSet ^= 1; removeFromHell(Entity); curHellSet ^= 1;
    //
    if (doBeing) {
      // system is living, add to 'future' set
      if (!isInBeSet(curBeSet^1, Entity)) {
        xlogf("Add: %p\n", Entity);
        beSets[curBeSet^1].insert(Entity);
      }
    } else {
      // system is frozen, add to 'current' set
      if (!isInBeSet(curBeSet, Entity)) {
        xlogf("Add: %p\n", Entity);
        beSets[curBeSet].insert(Entity);
        if (isDontDo(Entity)) removeFromDontDo(Entity);
      }
    }
  }
}


void pool::Remove (entity *Entity) {
  if (doRegister && Entity) {
    if (doBeing) {
      // system is living, add to 'dontDo' set
      if (!isDontDo(Entity)) {
        xlogf("Remove: %p\n", Entity);
        dontDo.insert(Entity);
      }
    } else {
      // system is frozen, remove from 'current' set
      if (isInBeSet(curBeSet, Entity)) {
        xlogf("Remove: %p\n", Entity);
        removeFromBeSet(curBeSet, Entity);
      }
    }
  }
}


void pool::AddToHell (entity *Entity) {
  if (doRegister && Entity) {
    Remove(Entity); // burn it with fire!
    if (isInHell(Entity)) {
      xlogf("AddToHell: %p\n", Entity);
      if (burningHell) curHellSet ^= 1;
      hellSets[curHellSet].insert(Entity);
      if (burningHell) curHellSet ^= 1;
    }
  }
}


#undef xlogf
