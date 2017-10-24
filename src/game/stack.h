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
#ifndef __STACK_H__
#define __STACK_H__

#include "ivancommon.h"

#include <vector>

#include "lsquare.h"
#include "slot.h"


class felist;
class entity;

typedef std::vector<item *> itemvector;
typedef std::vector<itemvector> itemvectorvector;


/* Stack contains an arbitrary number of items in a linked list, which can
   be browsed using stackiterators like this:

   for (stackiterator i = Stack->GetBottom(); i.HasItem(); ++i) {
     item *Item = *i;
     i->ItemMemberFunction();
   }

   or using a temporary vector:

   itemvector ItemVector;
   Stack->FillItemVector(ItemVector);

   for (int c = 0; c < ItemVector.size(); ++c) {
     item *Item = ItemVector[c];
     ItemVector[c]->ItemMemberFunction();
   }

   The former is faster and should be used if items can't be removed nor
   added during the loop (at worst, this could cause a crash), otherwise
   the latter is necessary.

   Items are added to stack with Stack->AddItem(Item) and removed by
   Item->RemoveFromSlot(). A number of Stack->DrawContents() functions
   is provided as an easy item-selecting GUI.
 */
class stackiterator {
public:
  stackiterator (stackslot *Slot) : Slot(Slot) {}
  stackiterator &operator ++ () { Slot = Slot->Next; return *this; }
  stackiterator &operator -- () { Slot = Slot->Last; return *this; }
  truth HasItem () const { return truth(Slot); }
  item *operator -> () const { return Slot->Item; }
  item *operator * () const { return Slot->Item; }
  const stackslot &GetSlot () const { return *Slot; }

private:
  stackslot *Slot;
};


class stack {
public:
  stack (square *, entity *, feuLong = 0);
  virtual ~stack ();

  void Load (inputfile &);
  void Draw (ccharacter *, blitdata &, int) const;
  void AddItem (item *, truth = true);
  void RemoveItem (stackslot *);
  item *GetItem (int) const;
  void GetVisibleItemsV (ccharacter *Viewer, std::vector<item *> &vi);
  truth HasSomethingFunny (ccharacter *Viewer, truth seeCorpses=false, truth seeUnstepped=false);
  void SetSteppedOn (truth v);
  stackiterator GetBottom () const { return stackiterator(Bottom); }
  stackiterator GetTop () const { return stackiterator(Top); }
  int GetItems () const { return Items; }
  int GetSideItems (int) const;
  int GetVisibleItems (ccharacter *) const;
  int GetNativeVisibleItems (ccharacter *) const;
  int GetVisibleSideItems (ccharacter *, int) const;
  void SetMotherSquare (square *What) { MotherSquare = What; }
  item *DrawContents (ccharacter *, cfestring &, int = 0, sorter = 0, item *hiitem=0) const;
  int DrawContents (itemvector &, ccharacter *, cfestring &, int = 0, sorter = 0, item *hiitem=0) const;
  int DrawContents (itemvector &, stack *, ccharacter *, cfestring &, cfestring &, cfestring &,
    cfestring &, col16, int, sorter = 0, item *hiitem=0) const;
  v2 GetPos () const;
  void Clean (truth = false);
  void Save (outputfile &) const;
  int SearchItem (item *) const;
  square *GetSquareUnder () const;
  lsquare *GetLSquareUnder () const { return static_cast<lsquare*>(GetSquareUnder()); }
  truth SortedItems (ccharacter *, sorter) const;
  int SortedItemsCount (ccharacter *, sorter) const;
  void BeKicked (character *, int, int);
  void Polymorph (character *);
  void CheckForStepOnEffect (character *);
  lsquare *GetLSquareTrulyUnder (int) const;
  void ReceiveDamage (character *, int, int, int = YOURSELF);
  void TeleportRandomly (uInt = 0xFFFF);
  void FillItemVector (itemvector &) const;
  truth IsOnGround () const;
  truth RaiseTheDead (character *);
  truth TryKey (item *, character *);
  truth Open (character *);
  void SignalVolumeAndWeightChange ();
  void CalculateVolumeAndWeight ();
  sLong GetVolume () const { return Volume; }
  sLong GetWeight () const { return Weight; }
  sLong GetWeight (ccharacter *, int) const;
  entity *GetMotherEntity () const { return MotherEntity; }
  void SetMotherEntity (entity *What) { MotherEntity = What; }
  area *GetArea () const { return GetSquareUnder()->GetArea(); }
  lsquare *GetNearLSquare (v2 Pos) const { return GetLSquareUnder()->GetLevel()->GetLSquare(Pos); }
  col24 GetEmitation () const { return Emitation; }
  void SignalEmitationIncrease (int, col24);
  void SignalEmitationDecrease (int, col24);
  void CalculateEmitation ();
  col24 GetSideEmitation (int);
  truth CanBeSeenBy (ccharacter *, int) const;
  truth IsDangerous (ccharacter *) const;
  truth Duplicate (int, feuLong = 0);
  void MoveItemsTo (stack *);
  void MoveItemsTo (slot *);
  item *GetBottomItem (ccharacter *, truth) const;
  item *GetBottomVisibleItem (ccharacter *) const;
  item *GetBottomSideItem (ccharacter *, int, truth) const;
  void Pile (itemvectorvector &, ccharacter *, int, sorter = 0) const;
  sLong GetTruePrice () const;
  static int GetSelected () { return Selected; }
  static void SetSelected (int What) { Selected = What; }
  truth TakeSomethingFrom (character *, cfestring&);
  truth PutSomethingIn (character *, cfestring &, sLong, feuLong);
  truth IsVisible () const { return !(Flags & HIDDEN); }
  int GetSpoiledItems () const;
  void SortAllItems (const sortdata &) const;
  void Search (ccharacter *, int);
  truth NeedDangerSymbol (ccharacter *) const;
  void PreProcessForBone ();
  void PostProcessForBone ();
  void FinalProcessForBone ();
  void AddElement (item *, truth = false);
  void SpillFluid (character *, liquid *, sLong);
  void AddItems (const itemvector &);
  void MoveItemsTo (itemvector &, int);
  void Freeze () { Flags |= FREEZED; }
  void UnFreeze () { Flags &= ~FREEZED; }
  void DropSideItems ();
  truth DetectMaterial (cmaterial *) const;
  void SetLifeExpectancy (int, int);
  truth Necromancy (character *);
  void CalculateEnchantments ();
  ccharacter *FindCarrier () const;
  void Haste ();
  void Slow ();

private:
  void RemoveElement (stackslot *);
  void AddContentsToList (felist &, ccharacter *, cfestring &, int, int, sorter, item *hiitem=0) const;
  int SearchChosen (itemvector &, ccharacter *, int, int, int, int, sorter = 0) const;
  static truth AllowDamage (int, int);

private:
  static int Selected;
  stackslot *Bottom;
  stackslot *Top;
  square *MotherSquare;
  entity *MotherEntity;
  sLong Volume;
  sLong Weight;
  col24 Emitation : 24;
  feuLong Flags : 8;
  int Items;
};


#endif
