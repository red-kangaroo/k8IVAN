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

#ifndef __GOD_H__
#define __GOD_H__

#include "ivancommon.h"

#include "ivandef.h"


class outputfile;
class inputfile;
class god;
class liquid;
class team;
struct materialdatabase;


typedef god* (*godspawner)();


class godprototype {
public:
  godprototype (godspawner, cchar *);
  virtual ~godprototype () {}

  god *Spawn () const { return Spawner(); }
  god *SpawnAndLoad (inputfile &) const;
  cchar *GetClassID() const { return ClassID; }
  int GetIndex() const { return Index; }
  inline cchar *GetTypeID () const { return ClassID; }
  inline truth IsOfType (cchar *tname) const { return (tname ? (strcmp(tname, ClassID) == 0) : false); }
  inline truth IsOfType (cfestring &tname) const { return (tname.Compare(ClassID) == 0); }

private:
  int Index;
  godspawner Spawner;
  cchar *ClassID;
};


class god {
protected:
  virtual void PrayGoodEffect () = 0;
  virtual void PrayBadEffect () = 0;

protected:
  int Relation, LastPray;
  sLong Timer;
  truth Known;

public:
  typedef godprototype prototype;

public:
  god ();
  virtual ~god () {}

public:
  virtual void Pray ();
  virtual cchar *GetName () const = 0;
  virtual cchar *GetDescription () const = 0;
  cchar *GetPersonalPronoun () const;
  cchar *GetObjectPronoun () const;
  virtual int GetAlignment () const = 0;
  festring GetCompleteDescription () const;
  void ApplyDivineTick ();
  void AdjustRelation (god *, int, truth);
  void AdjustRelation (int);
  void AdjustTimer (sLong);
  void Save (outputfile &) const;
  void Load (inputfile &);
  void SetRelation (int Value) { Relation = Value; }
  void SetTimer (sLong Value) { Timer = Value; }
  truth ReceiveOffer (item *);
  virtual int GetBasicAlignment () const;
  int GetRelation () const { return Relation; }
  void PrintRelation () const;
  void SetIsKnown (truth What) { Known = What; }
  truth IsKnown () const { return Known; }
  void PlayerKickedAltar () { AdjustRelation(-100); }
  void PlayerKickedFriendsAltar () { AdjustRelation(-50); }
  virtual truth PlayerVomitedOnAltar (liquid *);
  character *CreateAngel (team *, int = 0);
  virtual col16 GetColor () const = 0;
  virtual col16 GetEliteColor () const = 0;
  virtual const prototype *GetProtoType () const = 0;
  int GetType () const { return GetProtoType()->GetIndex(); }
  inline cchar *GetTypeID () const { return GetProtoType()->GetClassID(); }
  inline truth IsOfType (cchar *tname) const { return (tname ? (strcmp(tname, GetProtoType()->GetClassID()) == 0) : false); }
  inline truth IsOfType (cfestring &tname) const { return (tname.Compare(GetProtoType()->GetClassID()) == 0); }
  virtual truth ForceGiveBodyPart () const { return false; }
  virtual truth HealRegeneratingBodyParts () const { return false; }
  virtual truth LikesMaterial (const materialdatabase *, ccharacter *) const;
  truth TryToAttachBodyPart (character *);
  truth TryToHardenBodyPart (character *);
  virtual truth MutatesBodyParts () const { return false; }
  virtual int GetSex () const = 0;
  void SignalRandomAltarGeneration (const std::vector<v2> &);
  virtual truth LikesVomit () const { return false; }
};


#ifdef __FILE_OF_STATIC_GOD_PROTOTYPE_DEFINITIONS__
#define GOD_PROTO(name)\
template<> const godprototype\
  name##sysbase::ProtoType((godspawner)(&name##sysbase::Spawn), #name);
#else
#define GOD_PROTO(name)
#endif

#define GOD(name, base)\
class name;\
typedef simplesysbase<name, base, godprototype> name##sysbase;\
GOD_PROTO(name)\
class name : public name##sysbase


#endif
