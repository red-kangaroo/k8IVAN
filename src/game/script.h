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
#ifndef __SCRIPT_H__
#define __SCRIPT_H__

#include <list>
#include <map>

#include "rect.h"
#include "femath.h"
#include "festring.h"


#define SCRIPT_MEMBER(type, name)\
public:\
  const type* Get##name() const { return name##Holder.Member; }\
protected:\
  scriptmember< type > name##Holder

#define SCRIPT_MEMBER_WITH_BASE(type, name)\
public:\
  const type* Get##name() const { return GetMemberOf(name##Holder, Base, &scripttype::Get##name); }\
protected:\
  scriptmember< type > name##Holder

#define FAST_SCRIPT_MEMBER(type, name)\
public:\
  type Get##name() const { return name##Holder.Member; }\
protected:\
  fastscriptmember< type > name##Holder

#define SCRIPT_TRUTH(name)\
public:\
  ctruth* name() const { return name##Holder.Member; }\
protected:\
  scriptmember<truth> name##Holder

#define SCRIPT_TRUTH_WITH_BASE(name)\
public:\
  ctruth* name() const { return GetMemberOf(name##Holder, Base, &scripttype::name); }\
protected:\
  scriptmember<truth> name##Holder

#define FAST_SCRIPT_TRUTH(name)\
public:\
  truth name() const { return name##Holder.Member; }\
protected:\
  fastscriptmember<truth> name##Holder


class glterrain;
class olterrain;
class character;
class item;
class material;
class scriptwithbase;
class outputfile;
class inputfile;


struct scriptmemberbase {
  virtual ~scriptmemberbase () {}

  virtual void ReadFrom (inputfile &) = 0;
  virtual void Save (outputfile &) const = 0;
  virtual void Load (inputfile &) = 0;
  virtual void Replace (scriptmemberbase &) = 0;

  festring SrcFile;
  int SrcLine;
};


template <class type> struct scriptmember : public scriptmemberbase {
  scriptmember () : SrcFile(""), SrcLine(0), Member(0) {}
  scriptmember (const scriptmember& Data) : scriptmemberbase (Data), Member (Data.Member ? new type (*Data.Member) : 0) {}
  virtual ~scriptmember () { delete Member; }

  scriptmember &operator = (const scriptmember &);
  virtual void ReadFrom (inputfile &);
  virtual void Replace (scriptmemberbase &);
  virtual void Save (outputfile &) const;
  virtual void Load (inputfile &);

  festring SrcFile;
  int SrcLine;
  type *Member;
};


template <class type, class scripttype> inline const type *GetMemberOf (const scriptmember<type> &Data,
  const scriptwithbase *Base, const type *(scripttype::*MemberRetriever)() const)
{
  return Data.Member ? Data.Member : Base ?  (static_cast<const scripttype*> (Base)->*MemberRetriever) () : 0;
}


template <class type> inline scriptmember<type> &scriptmember<type>::operator = (const scriptmember<type> &Data) {
  if (Member) {
    if (Data.Member) {
      *Member = *Data.Member;
    } else {
      delete Member;
      Member = 0;
    }
    SrcFile = Data.SrcFile;
    SrcLine = Data.SrcLine;
  } else if (Data.Member) {
    Member = new type (*Data.Member);
    SrcFile = "";
    SrcLine = 0;
  }
  return *this;
}


template <class type> struct fastscriptmember : public scriptmemberbase {
  fastscriptmember () {}
  fastscriptmember (type Member) : Member (Member) {}

  virtual void ReadFrom (inputfile &);
  virtual void Replace (scriptmemberbase &);
  virtual void Save (outputfile &) const;
  virtual void Load (inputfile &);

  type Member;
};


class script {
public:
  typedef std::map<cchar *, scriptmemberbase script::*, charcomparer> datamap;
  virtual ~script () {}

  virtual void ReadFrom (inputfile &) = 0;
  virtual void Save (outputfile &SaveFile) const;
  virtual void Load (inputfile &SaveFile);

  inline cfestring &GetSrcFile () const { return SrcFile; }
  inline int GetSrcLine () const { return SrcLine; }

protected:
  truth ReadMember (inputfile&, cfestring &);
  virtual scriptmemberbase *GetDataFromMap (const datamap&, cchar *);
  virtual scriptmemberbase *GetData (cchar *String) { return GetDataFromMap (GetDataMap (), String); }
  virtual const datamap& GetDataMap () const = 0;
  virtual void SaveDataMap (const datamap &, outputfile &) const;
  virtual void LoadDataMap (const datamap &, inputfile &);

protected:
  festring SrcFile;
  int SrcLine;
};


static inline void ReadData (script &Type, inputfile &SaveFile) { Type.ReadFrom(SaveFile); }
inline outputfile &operator << (outputfile &SaveFile, const script &Script) { Script.Save (SaveFile); return SaveFile; }
inline inputfile &operator >> (inputfile &SaveFile, script &Script) { Script.Load (SaveFile); return SaveFile; }


class scriptwithbase : public script {
public:
  scriptwithbase () : script(), Base (0) {}
  virtual ~scriptwithbase () {}

  const scriptwithbase *GetBase () const { return Base; }
  virtual void SetBase (const scriptwithbase *What) { Base = What; }

protected:
  const scriptwithbase *Base;
};


class posscript : public script {
public:
  typedef posscript scripttype;

public:
  virtual ~posscript () {}

  virtual void ReadFrom (inputfile &);
  truth GetRandom () const { return Random; }
  static void InitDataMap ();
  virtual void Save (outputfile &) const;
  virtual void Load (inputfile &);

protected:
  virtual const datamap &GetDataMap () const { return DataMap; }

protected:
  static datamap DataMap;
  SCRIPT_MEMBER(rect, Borders);
  FAST_SCRIPT_MEMBER(packv2, Vector);
  FAST_SCRIPT_MEMBER(uChar, Flags);
  truth Random;
};


class materialscript : public script {
public:
  typedef materialscript scripttype;

public:
  virtual ~materialscript () {}

  virtual void ReadFrom (inputfile &);
  int GetConfig () const { return Config; }
  void SetConfig (int What) { Config = What; }
  material *Instantiate () const;
  virtual void Save (outputfile &) const;
  virtual void Load (inputfile &);
  static void InitDataMap ();

protected:
  virtual const datamap& GetDataMap () const { return DataMap; }

protected:
  static datamap DataMap;
  SCRIPT_MEMBER(interval, Volume);
  int Config;
};


class basecontentscript : public script {
public:
  typedef basecontentscript scripttype;

public:
  basecontentscript ();
  virtual ~basecontentscript () {}

  virtual void ReadFrom (inputfile &);
  int GetContentType () const { return ContentType; }
  truth IsValid () const { return ContentType || Random; }
  virtual void Save (outputfile &) const;
  virtual void Load (inputfile &);
  static void InitDataMap ();

protected:
  virtual const datamap &GetDataMap () const { return DataMap; }
  virtual scriptmemberbase *GetData (cchar *);
  virtual int SearchCodeName (cfestring &) const = 0;
  virtual cchar *GetClassID () const = 0;

protected:
  static datamap DataMap;
  SCRIPT_MEMBER(materialscript, MainMaterial);
  SCRIPT_MEMBER(materialscript, SecondaryMaterial);
  uShort ContentType;
  truth Random;
  uShort Config;
  FAST_SCRIPT_MEMBER(uChar, Parameters);
  festring mCode;
};


inline truth IsValidScript (const basecontentscript *S) { return S->IsValid (); }


template <class type> class contentscripttemplate : public basecontentscript {
protected:
  type *BasicInstantiate (int) const;
  virtual int SearchCodeName (cfestring &) const;
};


template <class type> class contentscript;

template <> class contentscript<item> : public contentscripttemplate<item> {
public:
  typedef contentscript<item> scripttype;

public:
  contentscript<item> ();
  item *InstantiateBasedOnMaterial (int, int = 0) const;
  item *Instantiate (int = 0) const;
  static void InitDataMap ();

protected:
  virtual const datamap& GetDataMap () const { return DataMap; }
  virtual cchar *GetClassID () const;

protected:
  static datamap DataMap;
  SCRIPT_MEMBER(fearray<contentscript<item> >, ItemsInside);
  SCRIPT_MEMBER(interval, Times);
  SCRIPT_MEMBER(interval, LifeExpectancy);
  FAST_SCRIPT_MEMBER(feuLong, Category);
  FAST_SCRIPT_MEMBER(sLong, MinPrice);
  FAST_SCRIPT_MEMBER(sLong, MaxPrice);
  FAST_SCRIPT_MEMBER(uChar, Team);
  FAST_SCRIPT_MEMBER(uChar, SquarePosition);
  FAST_SCRIPT_MEMBER(uChar, Chance);
  FAST_SCRIPT_MEMBER(uChar, ConfigFlags);
  FAST_SCRIPT_MEMBER(uChar, SpoilPercentage);
  FAST_SCRIPT_MEMBER(char, Enchantment);
  FAST_SCRIPT_TRUTH(IsActive);
};


truth IsValidScript (const fearray<contentscript<item> > *);


template <> class contentscript<character> : public contentscripttemplate<character> {
public:
  typedef contentscript<character> scripttype;

public:
  contentscript<character> ();
  character *Instantiate (int = 0) const;
  static void InitDataMap ();

protected:
  virtual const datamap& GetDataMap () const { return DataMap; }
  virtual cchar *GetClassID () const;

protected:
  static datamap DataMap;
  SCRIPT_MEMBER(fearray<contentscript<item> >, Inventory);
  SCRIPT_MEMBER(fearray<packv2>, WayPoint);
  SCRIPT_MEMBER(fearray<int>, AllowedDungeons);
  FAST_SCRIPT_MEMBER(uChar, Team);
  FAST_SCRIPT_MEMBER(uChar, Flags);
};


template <> class contentscript<glterrain> : public contentscripttemplate<glterrain> {
public:
  typedef contentscript<glterrain> scripttype;

public:
  glterrain *Instantiate (int SpecialFlags = 0) const { return contentscripttemplate<glterrain>::BasicInstantiate (SpecialFlags); }
  static void InitDataMap ();

protected:
  virtual const datamap& GetDataMap () const { return DataMap; }
  static datamap DataMap;
  virtual cchar *GetClassID () const;

protected:
  SCRIPT_TRUTH(IsInside);
};


template <> class contentscript<olterrain> : public contentscripttemplate<olterrain> {
public:
  typedef contentscript<olterrain> scripttype;

public:
  contentscript<olterrain> ();
  olterrain *Instantiate (int = 0) const;
  static void InitDataMap ();

protected:
  virtual const datamap& GetDataMap () const { return DataMap; }
  static datamap DataMap;
  virtual cchar *GetClassID () const;

protected:
  SCRIPT_MEMBER(fearray<contentscript<item> >, ItemsInside);
  SCRIPT_MEMBER(festring, Text);
  FAST_SCRIPT_MEMBER(uChar, VisualEffects);
  FAST_SCRIPT_MEMBER(uChar, AttachedArea);
  FAST_SCRIPT_MEMBER(uChar, AttachedEntry);
};


class squarescript : public script {
public:
  typedef squarescript scripttype;

public:
  squarescript ();
  virtual ~squarescript () {}

  virtual void ReadFrom (inputfile &);
  static void InitDataMap ();

protected:
  virtual const datamap &GetDataMap () const { return DataMap; }

protected:
  static datamap DataMap;
  SCRIPT_MEMBER(posscript, Position);
  SCRIPT_MEMBER(contentscript<character>, Character);
  SCRIPT_MEMBER(fearray<contentscript<item> >, Items);
  SCRIPT_MEMBER(contentscript<glterrain>, GTerrain);
  SCRIPT_MEMBER(contentscript<olterrain>, OTerrain);
  SCRIPT_MEMBER(interval, Times);
  FAST_SCRIPT_MEMBER(uChar, EntryIndex);
  FAST_SCRIPT_TRUTH(AttachRequired);
};


template <class type, class contenttype = contentscript<type> > class contentmap : public script {
public:
  typedef contentmap scripttype;

public:
  contentmap ();
  virtual ~contentmap ();
  virtual void ReadFrom (inputfile &);
  const contenttype *GetContentScript (int X, int Y) const { return ContentMap[X][Y].second; }
  virtual void Save (outputfile &) const;
  virtual void Load (inputfile &);
  static void InitDataMap ();

protected:
  virtual const datamap &GetDataMap () const { return DataMap; }

protected:
  static datamap DataMap;
  std::pair<int, contenttype*> **ContentMap;
  std::map<int, contenttype> SymbolMap;
  SCRIPT_MEMBER(v2, Size);
  SCRIPT_MEMBER(v2, Pos);
};


typedef contentmap<item, fearray<contentscript<item> > > itemcontentmap;
typedef contentmap<character> charactercontentmap;
typedef contentmap<glterrain> glterraincontentmap;
typedef contentmap<olterrain> olterraincontentmap;


class roomscript : public scriptwithbase {
public:
  typedef roomscript scripttype;

public:
  virtual ~roomscript () {}

  void ReadFrom (inputfile &);
  const std::list<squarescript> &GetSquare () const;
  virtual void Save (outputfile &) const;
  virtual void Load (inputfile &);
  static void InitDataMap ();

protected:
  virtual const datamap& GetDataMap () const { return DataMap; }

protected:
  static datamap DataMap;
  std::list<squarescript> Square;
  SCRIPT_MEMBER_WITH_BASE(charactercontentmap, CharacterMap);
  SCRIPT_MEMBER_WITH_BASE(itemcontentmap, ItemMap);
  SCRIPT_MEMBER_WITH_BASE(glterraincontentmap, GTerrainMap);
  SCRIPT_MEMBER_WITH_BASE(olterraincontentmap, OTerrainMap);
  SCRIPT_MEMBER_WITH_BASE(squarescript, WallSquare);
  SCRIPT_MEMBER_WITH_BASE(squarescript, FloorSquare);
  SCRIPT_MEMBER_WITH_BASE(squarescript, DoorSquare);
  SCRIPT_MEMBER_WITH_BASE(region, Size);
  SCRIPT_MEMBER_WITH_BASE(region, Pos);
  SCRIPT_TRUTH_WITH_BASE(AltarPossible);
  SCRIPT_TRUTH_WITH_BASE(GenerateDoor);
  SCRIPT_TRUTH_WITH_BASE(GenerateTunnel);
  SCRIPT_MEMBER_WITH_BASE(int, DivineMaster);
  SCRIPT_TRUTH_WITH_BASE(GenerateLanterns);
  SCRIPT_MEMBER_WITH_BASE(int, Type);
  SCRIPT_TRUTH_WITH_BASE(GenerateFountains);
  SCRIPT_TRUTH_WITH_BASE(AllowLockedDoors);
  SCRIPT_TRUTH_WITH_BASE(AllowBoobyTrappedDoors);
  SCRIPT_MEMBER_WITH_BASE(int, Shape);
  SCRIPT_TRUTH_WITH_BASE(IsInside);
  SCRIPT_TRUTH_WITH_BASE(GenerateWindows);
  SCRIPT_TRUTH_WITH_BASE(UseFillSquareWalls);
  SCRIPT_MEMBER_WITH_BASE(feuLong, Flags);
  SCRIPT_TRUTH_WITH_BASE(GenerateWards);
  SCRIPT_MEMBER_WITH_BASE(fearray<int>, AllowedDivineMasters);
};


class levelscript : public scriptwithbase {
public:
  typedef levelscript scripttype;

public:
  virtual ~levelscript () {}

  void ReadFrom (inputfile &);
  const std::list<squarescript>& GetSquare () const;
  const std::list<roomscript>& GetRoom () const;
  void Combine (levelscript &);
  virtual void Save (outputfile &) const;
  virtual void Load (inputfile &);
  virtual void SetBase (const scriptwithbase *);
  static void InitDataMap ();

protected:
  virtual const datamap &GetDataMap () const { return DataMap; }

protected:
  static datamap DataMap;
  std::list<squarescript> Square;
  std::list<roomscript> Room;
  SCRIPT_MEMBER_WITH_BASE(roomscript, RoomDefault);
  SCRIPT_MEMBER_WITH_BASE(squarescript, FillSquare);
  SCRIPT_MEMBER_WITH_BASE(squarescript, TunnelSquare);
  SCRIPT_MEMBER_WITH_BASE(festring, LevelMessage);
  SCRIPT_MEMBER_WITH_BASE(v2, Size);
  SCRIPT_MEMBER_WITH_BASE(interval, Items);
  SCRIPT_MEMBER_WITH_BASE(interval, Rooms);
  SCRIPT_TRUTH_WITH_BASE(GenerateMonsters);
  SCRIPT_TRUTH_WITH_BASE(IsOnGround);
  SCRIPT_MEMBER_WITH_BASE(int, TeamDefault);
  SCRIPT_MEMBER_WITH_BASE(festring, Description);
  SCRIPT_MEMBER_WITH_BASE(int, LOSModifier);
  SCRIPT_TRUTH_WITH_BASE(IgnoreDefaultSpecialSquares);
  SCRIPT_MEMBER_WITH_BASE(int, DifficultyBase);
  SCRIPT_MEMBER_WITH_BASE(int, DifficultyDelta);
  SCRIPT_MEMBER_WITH_BASE(int, MonsterAmountBase);
  SCRIPT_MEMBER_WITH_BASE(int, MonsterAmountDelta);
  SCRIPT_MEMBER_WITH_BASE(int, MonsterGenerationIntervalBase);
  SCRIPT_MEMBER_WITH_BASE(int, MonsterGenerationIntervalDelta);
  SCRIPT_TRUTH_WITH_BASE(AutoReveal);
  SCRIPT_MEMBER_WITH_BASE(festring, ShortDescription);
  SCRIPT_TRUTH_WITH_BASE(CanGenerateBone);
  SCRIPT_MEMBER_WITH_BASE(int, ItemMinPriceBase);
  SCRIPT_MEMBER_WITH_BASE(int, ItemMinPriceDelta);
  SCRIPT_MEMBER_WITH_BASE(int, Type);
  SCRIPT_MEMBER_WITH_BASE(int, EnchantmentMinusChanceBase);
  SCRIPT_MEMBER_WITH_BASE(int, EnchantmentMinusChanceDelta);
  SCRIPT_MEMBER_WITH_BASE(int, EnchantmentPlusChanceBase);
  SCRIPT_MEMBER_WITH_BASE(int, EnchantmentPlusChanceDelta);
  SCRIPT_MEMBER_WITH_BASE(int, BackGroundType);
  SCRIPT_TRUTH_WITH_BASE(IsCatacomb);
  SCRIPT_MEMBER_WITH_BASE(festring, EnterImage);
  SCRIPT_MEMBER_WITH_BASE(v2, EnterTextDisplacement);
};


class dungeonscript : public script {
public:
  typedef dungeonscript scripttype;

public:
  dungeonscript ();
  virtual ~dungeonscript ();

  virtual void ReadFrom (inputfile &);
  const std::map<int, levelscript>& GetLevel () const;
  void RandomizeLevels ();
  virtual void Save (outputfile &) const;
  virtual void Load (inputfile &);
  virtual const datamap& GetDataMap () const { return DataMap; }
  static void InitDataMap ();

protected:
  static datamap DataMap;
  std::map<int, levelscript> Level;
  std::list<std::pair<interval, levelscript> > RandomLevel;
  SCRIPT_MEMBER(levelscript, LevelDefault);
  SCRIPT_MEMBER(int, Levels);
  SCRIPT_MEMBER(festring, Description);
  SCRIPT_MEMBER(festring, ShortDescription);
};


class teamscript : public script {
public:
  typedef teamscript scripttype;

public:
  virtual ~teamscript () {}

  virtual void ReadFrom (inputfile &);
  const std::vector<std::pair<int, int> >& GetRelation () const;
  virtual void Save (outputfile &) const;
  virtual void Load (inputfile &);
  static void InitDataMap ();

protected:
  virtual const datamap &GetDataMap () const { return DataMap; }

protected:
  static datamap DataMap;
  std::vector<std::pair<int, int> > Relation;
  SCRIPT_MEMBER(int, KillEvilness);
};


class gamescript : public script {
public:
  typedef gamescript scripttype;

public:
  virtual ~gamescript () {}

  virtual void ReadFrom (inputfile &);
  const std::list<std::pair<int, teamscript> > &GetTeam () const;
  const std::map<int, dungeonscript> &GetDungeon () const;
  void RandomizeLevels ();
  virtual void Save (outputfile &) const;
  virtual void Load (inputfile &);
  static void InitDataMap ();
  const int *GetDungeons () const { mDungeons = (int)Dungeon.size(); return &mDungeons; }
  const int *GetTeams () const { mTeams = (int)Team.size(); return &mTeams; }

protected:
  virtual const datamap &GetDataMap () const { return DataMap; }

protected:
  static datamap DataMap;
  std::list<std::pair<int, teamscript> > Team;
  std::map<int, dungeonscript> Dungeon;
  mutable int mDungeons;
  mutable int mTeams;
  //SCRIPT_MEMBER(int, Dungeons);
  //SCRIPT_MEMBER(int, Teams);
};


outputfile &operator << (outputfile &, const gamescript *);
inputfile &operator >> (inputfile &, gamescript *&);


class scriptsystem {
public:
  static void Initialize ();
};


#endif
