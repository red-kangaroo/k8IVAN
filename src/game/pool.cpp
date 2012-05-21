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


typedef std::set<entity *> EntitySet;

static EntitySet beSet;
static EntitySet hellSet;
static EntitySet notDoSet;
static EntitySet xnewBeSet;


static bool burningHell = false;
static bool doRegister = true;


truth pool::IsBurningHell () {
  return burningHell;
}


void pool::RegisterState (truth doreg) {
  doRegister = doreg;
}


void pool::DeadEntity (entity *) {
}


/* Calls the Be() function of each self-changeable entity during each tick,
 * thus allowing acting characters, spoiling food etc. */
void pool::Be () {
  if (burningHell) return;
  xlogf("Be: START (%u)!\n", beSet.size());
  //
  EntitySet newBe, leftBe;
  //
  for (EntitySet::const_iterator i = beSet.begin(); i != beSet.end(); ++i) {
    if (!entity::IsInDeadSet(*i)) {
      EntitySet::const_iterator it = hellSet.find(*i);
      //
      if (it == hellSet.end()) {
        EntitySet::const_iterator it = notDoSet.find(*i);
        //
        if (it == notDoSet.end()) newBe.insert(*i);
      }
    }
  }
  beSet.clear();
  //
  for (EntitySet::const_iterator i = newBe.begin(); i != newBe.end(); ++i) {
    if (!entity::IsInDeadSet(*i)) {
      EntitySet::const_iterator it = hellSet.find(*i);
      //
      if (it == hellSet.end()) {
        EntitySet::const_iterator it = notDoSet.find(*i);
        //
        if (it == notDoSet.end()) {
          xlogf("Be: %p\n", *i);
          (*i)->Be();
        }
        //
        it = notDoSet.find(*i);
        if (it == notDoSet.end()) leftBe.insert(*i);
      } else {
        xlogf("HellBe: %p\n", *i);
      }
    } else {
      xlogf("NotBe: %p\n", *i);
    }
  }
  //
  for (EntitySet::const_iterator i = leftBe.begin(); i != leftBe.end(); ++i) {
    if (!entity::IsInDeadSet(*i)) {
      EntitySet::const_iterator it = hellSet.find(*i);
      //
      if (it == hellSet.end()) {
        EntitySet::const_iterator it = notDoSet.find(*i);
        //
        if (it == notDoSet.end()) beSet.insert(*i);
      }
    }
  }
  //
  xlogf("Be: DONE!\n");
}


void pool::BurnHell () {
  EntitySet newBe;
  //
  burningHell = true;
  xlogf("BurnHell: START (%u:%u)\n", beSet.size(), hellSet.size());
  //
  for (EntitySet::const_iterator i = hellSet.begin(); i != hellSet.end(); ++i) if (!entity::IsInDeadSet(*i) && entity::IsInLiveSet(*i)) delete (*i);
  hellSet.clear();
  //
  for (EntitySet::const_iterator i = beSet.begin(); i != beSet.end(); ++i) {
    if (!entity::IsInDeadSet(*i)) {
      EntitySet::const_iterator it = notDoSet.find(*i);
      //
      if (it == notDoSet.end()) newBe.insert(*i);
    } else {
      hellSet.insert(*i);
    }
  }
  notDoSet.clear();
  //
  /*
  EntitySet::iterator it = beSet.begin();
  //
  while (it != beSet.end()) {
    // copy the current iterator then increment it
    EntitySet::iterator ct = it++;
    //
    if (entity::IsInDeadSet(*ct)) {
      xlogf("BurnHell: %p\n", *ct);
      beSet.erase(ct);
    }
  }
  */
  beSet.clear();
  for (EntitySet::const_iterator i = newBe.begin(); i != newBe.end(); ++i) beSet.insert(*i);
  for (EntitySet::const_iterator i = xnewBeSet.begin(); i != xnewBeSet.end(); ++i) {
    if (!entity::IsInDeadSet(*i)) {
      EntitySet::const_iterator it = hellSet.find(*i);
      //
      if (it == hellSet.end()) {
        EntitySet::const_iterator it = notDoSet.find(*i);
        //
        if (it == notDoSet.end()) beSet.insert(*i);
      }
    }
  }
  xnewBeSet.clear();
  //
  entity::BurnDeadSet();
  //
  xlogf("BurnHell: DONE\n");
  burningHell = false;
}


void pool::KillBees () {
  xlogf("pool::KillBees()\n");
  beSet.clear();
  xnewBeSet.clear();
  notDoSet.clear();
  BurnHell();
}


void pool::Add (entity *Entity) {
  if (doRegister) {
    EntitySet::const_iterator i = xnewBeSet.find(Entity);
    //
    if (i == xnewBeSet.end()) {
      xlogf("Add: %p\n", Entity);
      xnewBeSet.insert(Entity);
    }
  }
}


void pool::Remove (entity *Entity) {
  if (doRegister) {
    //EntitySet::const_iterator i = beSet.find(Entity);
    //
    /*if (i != beSet.end())*/ {
      xlogf("Remove: %p\n", Entity);
      //beSet.erase(i);
      //
      //EntitySet::const_iterator it = notDoSet.find(*i);
      ////
      //if (it != notDoSet.end()) notDoSet.erase(it);
      //hellSet.insert(Entity);
      notDoSet.insert(Entity);
    }
  }
}


void pool::AddToHell (entity *Entity) {
  if (doRegister) {
    xlogf("AddToHell: %p\n", Entity);
    notDoSet.insert(Entity);
    hellSet.insert(Entity);
  }
}


#undef xlogf
