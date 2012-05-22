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

#include "game.h"


//#define xlogf(...)  do { fprintf(stderr, __VA_ARGS__); fflush(stderr); } while (0)
#define xlogf(...)


////////////////////////////////////////////////////////////////////////////////
typedef std::set<entity *> EntitySet;

typedef struct EntityListItem {
  struct EntityListItem *next;
  entity *e;
} EntityListItem;


typedef struct EntityList {
  struct EntityListIterator *iterators;
  struct EntityListItem *head;
  struct EntityListItem *tail;
  EntitySet *items;
} EntityList;


typedef struct EntityListIterator {
  struct EntityListIterator *next;
  EntityList *owner;
  EntityListItem *nextitem;
} EntityListIterator;


////////////////////////////////////////////////////////////////////////////////
static void elInitialize (EntityList *el) {
  memset(el, 0, sizeof(*el));
  el->items = new EntitySet();
}


static bool elIsInList (EntityList *el, entity *e) {
  EntitySet::const_iterator it = el->items->find(e);
  //
  return (it != el->items->end());
}


static void elAddItem (EntityList *el, entity *e) {
  EntitySet::const_iterator it = el->items->find(e);
  //
  if (it == el->items->end()) {
    EntityListItem *i = (EntityListItem *)calloc(1, sizeof(EntityListItem));
    //
    el->items->insert(e);
    i->e = e;
    i->next = NULL;
    if (el->tail != NULL) el->tail->next = i; else el->head = i;
    el->tail = i;
  }
}


static void elRemoveItem (EntityList *el, entity *e) {
  EntitySet::iterator it = el->items->find(e);
  //
  if (it != el->items->end()) {
    EntityListItem *p = NULL, *i;
    //
    el->items->erase(it);
    //
    for (i = el->head; i != NULL; p = i, i = i->next) if (i->e == e) break;
    if (i == NULL) { fprintf(stderr, "FATAL: elRemoveItem() desync!\n"); abort(); }
    //
    for (EntityListIterator *eit = el->iterators; eit != NULL; eit = eit->next) {
      if (eit->nextitem == i) eit->nextitem = i->next;
    }
    //
    if (p != NULL) p->next = i->next; else el->head = i->next;
    if (i->next == NULL) el->tail = p;
    free(i);
  }
}


static entity *elInitIterator (EntityList *el, EntityListIterator *it) {
  memset(it, 0, sizeof(*it));
  it->owner = el;
  it->nextitem = (el->head != NULL ? el->head->next : NULL);
  it->next = el->iterators;
  el->iterators = it;
  return (el->head != NULL ? el->head->e : NULL);
}


static void elRemoveIterator (EntityListIterator *it) {
  if (it->owner != NULL) {
    if (it->owner->iterators == it) {
      it->owner->iterators = it->next;
    } else {
      EntityListIterator *p = NULL, *c;
      //
      for (c = it->owner->iterators; c != NULL; p = c, c = c->next) if (c == it) break;
      if (c == NULL || p == NULL) { fprintf(stderr, "FATAL: elRemoveIterator() desync!\n"); abort(); }
      p->next = it->next;
    }
    memset(it, 0, sizeof(*it));
  }
}


static entity *elIteratorNext (EntityListIterator *it) {
  if (it->nextitem != NULL) {
    entity *e = it->nextitem->e;
    //
    it->nextitem = it->nextitem->next;
    return e;
  } else {
    elRemoveIterator(it);
  }
  return NULL;
}


////////////////////////////////////////////////////////////////////////////////
static EntityList beList;
static EntityList hellList;


static bool burningHell = false; // is we burning lost souls?
static bool doBeing = false; // being process in progress?
static bool doRegister = true; // nothing %-)


////////////////////////////////////////////////////////////////////////////////
pool::pool () {
  elInitialize(&beList);
  elInitialize(&hellList);
}


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
  EntityListIterator it;
  entity *e;
  //
  if (burningHell) { fprintf(stderr, "FATAL: started to being while burning souls!\n"); entity::DumpDeadSet(); abort(); }
  if (doBeing) { fprintf(stderr, "FATAL: started to being while already being!\n"); entity::DumpDeadSet(); abort(); }
  //
  xlogf("Be: START\n");
  doBeing = true;
  //
  try {
    for (e = elInitIterator(&beList, &it); e != NULL; e = elIteratorNext(&it)) {
      int mmark = entity::GetUniqueMemoryMark(e);
      //
      xlogf("Be: %p (%d)\n", e, mmark);
      e->Be();
      if (it.owner == NULL) break; // aborted
      if (elIsInList(&beList, e)) {
        if (entity::GetUniqueMemoryMark(e) != mmark) {
          fprintf(stderr, "FATAL: entity polymorphed while being!\n");
          entity::DumpDeadSet();
          abort();
        }
      }
    }
  } catch (...) {
    elRemoveIterator(&it);
    throw;
  }
  elRemoveIterator(&it);
  //
  doBeing = false;
  xlogf("Be: DONE\n");
}


void pool::AbortBe () {
  doBeing = false;
  for (EntityListIterator *eit = beList.iterators; eit != NULL; eit = eit->next) { eit->owner = NULL; eit->nextitem = NULL; }
  beList.iterators = NULL;
}


void pool::BurnHell () {
  EntityListIterator it;
  entity *e;
  //
  if (!game::IsRunning()) AbortBe();
  if (burningHell) { fprintf(stderr, "FATAL: started to burning souls while already burning souls!\n"); entity::DumpDeadSet(); abort(); }
  if (doBeing) { fprintf(stderr, "FATAL: started to burning souls while being!\n"); entity::DumpDeadSet(); abort(); }
  //
  burningHell = true;
  xlogf("BurnHell: START\n");
  //
  try {
    for (e = elInitIterator(&hellList, &it); e != NULL; e = elIteratorNext(&it)) {
      delete e;
      elRemoveItem(&hellList, e);
    }
  } catch (...) {
    elRemoveIterator(&it);
    throw;
  }
  elRemoveIterator(&it);
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


void pool::Add (entity *e) {
  if (doRegister && e) {
    elRemoveItem(&hellList, e);
    if (!elIsInList(&beList, e)) {
      elAddItem(&beList, e);
      xlogf("Add: %p\n", e);
    }
  }
}


void pool::Remove (entity *e) {
  if (doRegister && e) {
    if (elIsInList(&beList, e)) {
      xlogf("Remove: %p\n", e);
      elRemoveItem(&beList, e);
    }
  }
}


void pool::AddToHell (entity *e) {
  if (doRegister && e) {
    Remove(e); // burn it with fire!
    if (!elIsInList(&hellList, e)) {
      xlogf("AddToHell: %p\n", e);
      elAddItem(&hellList, e);
    }
  }
}


void pool::RemoveFromHell (entity *e) {
  if (doRegister && e) {
    if (elIsInList(&hellList, e)) {
      xlogf("RemoveFromHell: %p\n", e);
      elRemoveItem(&hellList, e);
    }
  }
}


#undef xlogf


static pool poolsingleton;
