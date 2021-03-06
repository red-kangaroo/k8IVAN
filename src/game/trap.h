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
#ifndef __TRAP_H__
#define __TRAP_H__

#include "ivancommon.h"

#include <set>

#include "entity.h"
#include "festring.h"


class trap;
class lsquare;
class character;
class bitmap;
class outputfile;
class inputfile;
struct blitdata;


typedef trap *(*trapspawner) ();


struct trapdata {
  trapdata (feuLong TrapID, feuLong VictimID, feuLong BodyParts) :
    Next(0), TrapID(TrapID), VictimID(VictimID), BodyParts(BodyParts) {}
  trapdata () : Next(0) {}

  trapdata *Next;
  feuLong TrapID;
  feuLong VictimID;
  feuLong BodyParts;
};


outputfile &operator << (outputfile &, const trapdata *);
inputfile &operator >> (inputfile &, trapdata *&);
outputfile &operator << (outputfile &, const trapdata &);
inputfile &operator >> (inputfile &, trapdata &);


class itemtrapbase {
public:
  itemtrapbase () : Active(false) {}
  virtual ~itemtrapbase () {}

  void Save (outputfile &) const;
  void Load (inputfile &);
  void SetIsActive (truth);
  truth CanBeSeenBy (ccharacter *) const;
  void Search (ccharacter *, int);
  void FinalProcessForBone ();
  void TeleportRandomly ();

  virtual void SendNewDrawAndMemorizedUpdateRequest () const = 0;
  virtual festring GetName (int) const = 0;
  virtual void UpdatePictures () = 0;

protected:
  truth Active;
  int Team;
  std::set<int> DiscoveredByTeam;
};


template <class base> class itemtrap : public base, public itemtrapbase {
public:
  virtual void Save (outputfile &) const;
  virtual void Load (inputfile &);
  virtual truth IsActive () const { return Active; }
  virtual truth IsAppliable (ccharacter *) const { return true; }
  virtual truth NeedDangerSymbol () const { return IsActive(); }
  virtual int GetTeam () const { return Team; }
  virtual void SetTeam (int What) { Team = What; }
  virtual void FinalProcessForBone ();
  virtual void TeleportRandomly ();
  virtual truth CanBeSeenBy (ccharacter *) const;
  virtual void Search (ccharacter *Char, int Perception) { itemtrapbase::Search(Char, Perception); }
  virtual void SetIsActive (truth What) { itemtrapbase::SetIsActive(What); }
  virtual void SendNewDrawAndMemorizedUpdateRequest () const { base::SendNewDrawAndMemorizedUpdateRequest(); }
  virtual festring GetName (int Case) const { return base::GetName(Case); }
  virtual void UpdatePictures () { base::UpdatePictures(); }
};


template <class base> inline void itemtrap<base>::Load (inputfile &SaveFile) {
  base::Load(SaveFile);
  itemtrapbase::Load(SaveFile);
}


template <class base> inline void itemtrap<base>::Save (outputfile &SaveFile) const {
  base::Save(SaveFile);
  itemtrapbase::Save(SaveFile);
}


template <class base> inline void itemtrap<base>::FinalProcessForBone () {
  base::FinalProcessForBone();
  itemtrapbase::FinalProcessForBone();
}


template <class base> inline void itemtrap<base>::TeleportRandomly () {
  itemtrapbase::TeleportRandomly();
  base::TeleportRandomly();
}


template <class base> inline truth itemtrap<base>::CanBeSeenBy (ccharacter *Viewer) const {
  return itemtrapbase::CanBeSeenBy(Viewer) && base::CanBeSeenBy(Viewer);
}


class trapprototype {
public:
  trapprototype (trapspawner truth, cchar *);
  virtual ~trapprototype () {}

  trap *SpawnAndLoad (inputfile &) const;
  cchar *GetClassID () const { return ClassID; }
  int GetIndex () const { return Index; }
  inline cchar *GetTypeID () const { return ClassID; }
  inline truth IsOfType (cchar *tname) const { return (tname ? (strcmp(tname, ClassID) == 0) : false); }
  inline truth IsOfType (cfestring &tname) const { return (tname.Compare(ClassID) == 0); }

private:
  int Index;
  trapspawner Spawner;
  cchar *ClassID;
};


class trap : public entity {
public:
  /* Come To The Dark Side */
  trap *Next;

public:
  typedef trapprototype prototype;
  trap ();
  virtual ~trap ();

  virtual square *GetSquareUnderEntity (int = 0) const;
  void SetLSquareUnder (lsquare *What) { LSquareUnder = What; }
  lsquare *GetLSquareUnder () const { return LSquareUnder; }
  virtual truth IsOnGround () const { return true; }
  virtual void Save (outputfile &) const;
  virtual void Load (inputfile &);
  int GetType () const { return GetProtoType()->GetIndex(); }
  inline cchar *GetTypeID () const { return GetProtoType()->GetClassID(); }
  inline truth IsOfType (cchar *tname) const { return (tname ? (strcmp(tname, GetProtoType()->GetClassID()) == 0) : false); }
  inline truth IsOfType (cfestring &tname) const { return (tname.Compare(GetProtoType()->GetClassID()) == 0); }
  virtual void AddDescription (festring &) const = 0;
  virtual const prototype *GetProtoType () const = 0;
  virtual void StepOnEffect (character *) = 0;
  virtual void Draw (blitdata &) const = 0;
  virtual void ReceiveDamage (character *, int, int, int) { }
  virtual truth IsDangerous (ccharacter *) const { return false; }
  virtual void PreProcessForBone () { }
  virtual void PostProcessForBone () { }
  virtual void Untrap () = 0;

protected:
  lsquare *LSquareUnder;
};


#ifdef __FILE_OF_STATIC_TRAP_PROTOTYPE_DEFINITIONS__
#define TRAP_PROTO(name)\
template<> const trapprototype\
  name##sysbase::ProtoType((trapspawner)(&name##sysbase::Spawn), #name);
#else
#define TRAP_PROTO(name)
#endif


#define TRAP(name, base)\
class name;\
typedef simplesysbase<name, base, trapprototype> name##sysbase;\
TRAP_PROTO(name)\
class name : public name##sysbase


#endif
