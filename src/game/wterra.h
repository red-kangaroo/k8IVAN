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
#ifndef __WTERRA_H__
#define __WTERRA_H__

#include "ivancommon.h"

#include "cont.h"
#include "worldmap.h"
#include "terra.h"
#include "wsquare.h"
#include "festring.h"


struct blitdata;

class owterrainprototype;
class gwterrainprototype;
class continent;
class gwterrain;


typedef gwterrain *(*gwterrainspawner) (int Config, int SpecialFlags);
typedef owterrain *(*owterrainspawner) (int Config, int SpecialFlags);


// ////////////////////////////////////////////////////////////////////////// //
gwterrain *GWSpawn (int type);


// ////////////////////////////////////////////////////////////////////////// //
class wterrain {
public:
  wterrain () : WSquareUnder(0), AnimationFrames(1) {}
  virtual ~wterrain () {}

  virtual void Save (outputfile &) const;
  virtual void Load (inputfile &);
  v2 GetPos () const { return WSquareUnder->GetPos(); }
  void SetWSquareUnder (wsquare *What) { WSquareUnder = What; }
  worldmap *GetWorldMap () const { return WSquareUnder->GetWorldMap(); }
  void AddName (festring &, int) const;
  festring GetName (int) const;
  truth IsAnimated () const { return AnimationFrames > 1; }
  void SetAnimationFrames (int What) { AnimationFrames = What; }
  virtual festring GetNameStem () const = 0;

protected:
  virtual truth UsesLongArticle () const { return false; }
  virtual v2 GetBitmapPos (int) const = 0;

protected:
  wsquare *WSquareUnder;
  int AnimationFrames;
};


// ////////////////////////////////////////////////////////////////////////// //
struct gwterraindatabase : public databasebase {
  friend class databasecreator<gwterrain>;

public:
  typedef gwterrainprototype prototype;

public:
  void InitDefaults (const prototype *NewProtoType, int NewConfig, cfestring &acfgstrname);
  void PostProcess() {}

public:
  const prototype *ProtoType;

  truth IsAbstract;
  v2 BitmapPos;
  festring NameStem;
  truth UsesLongArticle;
  int Priority;
  int AnimationFrames;
  truth IsFatalToStay;
  festring SurviveMessage;
  festring MonsterSurviveMessage;
  festring DeathMessage;
  festring MonsterDeathVerb;
  festring ScoreEntry;
  int Walkability;

  //HACK: worldmap generation constants
  int MaxTemperature;
  int LatitudeEffect;
  float AltitudeEffect;

  int TemperatureCold;
  int TemperatureMedium;
  int TemperatureWarm;
  int TemperatureHot;
};


class gwterrainprototype {
  friend class databasecreator<gwterrain>;
  friend class gwterrain;

public:
  gwterrainprototype (const gwterrainprototype*, gwterrainspawner, cchar *);
  virtual ~gwterrainprototype () {}

  gwterrain *Spawn (int Config, int SpecialFlags=0) const;
  gwterrain *SpawnAndLoad (inputfile &) const;
  cchar *GetClassID () const { return ClassID; }
  inline cchar *GetTypeID () const { return ClassID; }
  inline truth IsOfType (cchar *tname) const { return (tname ? (strcmp(tname, ClassID) == 0) : false); }
  inline truth IsOfType (cfestring &tname) const { return (tname.Compare(ClassID) == 0); }
  const gwterrainprototype *GetBase () const { return Base; }
  int CreateSpecialConfigurations (gwterraindatabase **, int Configs, int) { return Configs; }
  const gwterraindatabase *ChooseBaseForConfig (gwterraindatabase **TempConfig, int, int) { return *TempConfig; }
  const gwterraindatabase *const *GetConfigData () const { return ConfigData; }
  int GetConfigSize () const { return ConfigSize; }

private:
  int GetIndex () const { return Index; }

private:
  int Index;
  const gwterrainprototype* Base;
  gwterraindatabase** ConfigData;
  gwterraindatabase** ConfigTable[CONFIG_TABLE_SIZE];
  int ConfigSize;
  gwterrainspawner Spawner;
  cchar *ClassID;

public:
  EventHandlerMap mOnEvents;
};


class gwterrain : public wterrain, public gterrain {
  friend class databasecreator<gwterrain>;
  friend class gwterrainprototype;

public:
  typedef gwterrainprototype prototype;
  typedef gwterraindatabase database;

public:
  virtual ~gwterrain () {}

  void Initialize (int NewConfig, int SpecialFlags);

  virtual void Save (outputfile &) const;
  virtual void Load (inputfile &);

  void Draw (blitdata &) const;
  //virtual int GetPriority () const = 0;
  virtual int GetEntryDifficulty () const { return 10; }
  //virtual const prototype *GetProtoType() const = 0;
  void CalculateNeighbourBitmapPoses ();
  //virtual int GetWalkability () const;

  const database *GetDataBase () const { return DataBase; }

  virtual cchar *SurviveMessage () const override { return GetSurviveMessage().CStr(); }
  virtual cchar *MonsterSurviveMessage () const override { return GetMonsterSurviveMessage().CStr(); }
  virtual cchar *DeathMessage () const override { return GetDeathMessage().CStr(); }
  virtual cchar *MonsterDeathVerb () const override { return GetMonsterDeathVerb().CStr(); }
  virtual cchar *ScoreEntry () const override { return GetScoreEntry().CStr(); }
  //virtual truth IsFatalToStay () const override { return GetIsFatalToStay(); }

  DATA_BASE_VALUE(const prototype *, ProtoType);
  DATA_BASE_VALUE(int, Config);
  DATA_BASE_VALUE(festring, NameStem);
  DATA_BASE_TRUTH(UsesLongArticle);
  DATA_BASE_VALUE(int, Priority);
  DATA_BASE_VALUE(int, AnimationFrames);
  DATA_BASE_TRUTH(IsFatalToStay);
  DATA_BASE_VALUE(festring, SurviveMessage);
  DATA_BASE_VALUE(festring, MonsterSurviveMessage);
  DATA_BASE_VALUE(festring, DeathMessage);
  DATA_BASE_VALUE(festring, MonsterDeathVerb);
  DATA_BASE_VALUE(festring, ScoreEntry);
  DATA_BASE_VALUE(int, Walkability);

  //HACK: worldmap generation constants
  DATA_BASE_VALUE(int, MaxTemperature);
  DATA_BASE_VALUE(int, LatitudeEffect);
  DATA_BASE_VALUE(float, AltitudeEffect);

  DATA_BASE_VALUE(int, TemperatureCold);
  DATA_BASE_VALUE(int, TemperatureMedium);
  DATA_BASE_VALUE(int, TemperatureWarm);
  DATA_BASE_VALUE(int, TemperatureHot);

  inline cchar *GetTypeID () const { return GetProtoType()->GetClassID(); }
  inline truth IsOfType (cchar *tname) const { return (tname ? (strcmp(tname, GetProtoType()->GetClassID()) == 0) : false); }
  inline truth IsOfType (cfestring &tname) const { return (tname.Compare(GetProtoType()->GetClassID()) == 0); }

  virtual const prototype* FindProtoType () const { return &ProtoType; }

private:
  int GetType () const { return GetProtoType()->GetIndex(); }

protected:
  virtual v2 GetBitmapPos (int Frame) const {
    //HACK: ocean is animated, and it has exactly 32 animation frames
    //return v2(48 + ((Frame << 3)&~8), 48);
    return v2(DataBase->BitmapPos.X+((Frame<<3)&~8), DataBase->BitmapPos.Y);
  }
  virtual void InstallDataBase (int);

protected:
  static const prototype ProtoType;
  const database* DataBase;

protected:
  std::pair<v2, int> Neighbour[8];
};


// ////////////////////////////////////////////////////////////////////////// //
struct owterraindatabase : public databasebase {
  friend class databasecreator<owterrain>;

public:
  typedef owterrainprototype prototype;
  typedef owterraindatabase database;

public:
  void InitDefaults (const prototype *NewProtoType, int NewConfig, cfestring &acfgstrname);
  void PostProcess() {}

  const prototype *ProtoType;

  truth IsAbstract;
  v2 BitmapPos;
  festring NameStem;
  festring NameSingular;
  truth UsesLongArticle;

  int AttachedDungeon;
  int AttachedArea;
  truth CanBeGenerated;
  int NativeGTerrainType;
  truth RevealEnvironmentInitially;
  truth CanBeOnAnyTerrain;
  int WantContinentWith;
  int Probability; // in percents
  truth CanBeSkipped; // if it is impossible to generate such POI, skip it
  truth PlaceInitially;
};


class owterrainprototype {
  friend class databasecreator<owterrain>;
  friend class owterrain;

public:
  owterrainprototype (const owterrainprototype*, owterrainspawner, cchar *);
  virtual ~owterrainprototype () {}

  owterrain *Spawn (int Config, int SpecialFlags=0) const;
  owterrain *SpawnAndLoad (inputfile &) const;
  cchar *GetClassID () const { return ClassID; }
  inline cchar *GetTypeID () const { return ClassID; }
  inline truth IsOfType (cchar *tname) const { return (tname ? (strcmp(tname, ClassID) == 0) : false); }
  inline truth IsOfType (cfestring &tname) const { return (tname.Compare(ClassID) == 0); }
  const owterrainprototype *GetBase () const { return Base; }
  int CreateSpecialConfigurations (owterraindatabase **, int Configs, int) { return Configs; }
  const owterraindatabase *ChooseBaseForConfig (owterraindatabase **TempConfig, int, int) { return *TempConfig; }
  const owterraindatabase *const *GetConfigData () const { return ConfigData; }
  int GetConfigSize () const { return ConfigSize; }

private:
  int GetIndex () const { return Index; }

private:
  int Index;
  const owterrainprototype* Base;
  owterraindatabase** ConfigData;
  owterraindatabase** ConfigTable[CONFIG_TABLE_SIZE];
  int ConfigSize;
  owterrainspawner Spawner;
  cchar *ClassID;

public:
  EventHandlerMap mOnEvents;
};


class owterrain : public wterrain, public oterrain {
  friend class databasecreator<owterrain>;
  friend class owterrainprototype;

public:
  typedef owterrainprototype prototype;
  typedef owterraindatabase database;

public:
  void Initialize (int NewConfig, int SpecialFlags);

  owterrain *Clone () const;

  v2 GetPosition () const { return mPos; }
  void SetPosition (v2 pos) { mPos = pos; }

  truth IsGenerated () const { return mGenerated; }
  void SetGenerated (truth v) { mGenerated = v; }

  virtual void Save (outputfile &) const;
  virtual void Load (inputfile &);
  void Draw (blitdata &) const;
  virtual int GetAttachedEntry () const;
  virtual truth Enter (truth) const;
  virtual int GetWalkability () const;
  const database *GetDataBase () const { return DataBase; }

  truth IsRevealed () const { return mRevealed; }
  void SetRevealed (truth v) { mRevealed = v; }

  truth IsPlaced () const { return mPlaced; }
  void SetPlaced (truth v) { mPlaced = v; }

  truth IsSuitableContinent (continent *Continent);

  DATA_BASE_VALUE(const prototype *, ProtoType);
  DATA_BASE_VALUE(int, Config);
  DATA_BASE_VALUE(festring, NameStem);
  DATA_BASE_VALUE(festring, NameSingular);
  DATA_BASE_TRUTH(UsesLongArticle);
  DATA_BASE_VALUE(int, AttachedDungeon);
  DATA_BASE_VALUE(int, AttachedArea);
  DATA_BASE_TRUTH(CanBeGenerated);
  DATA_BASE_VALUE(int, NativeGTerrainType);
  DATA_BASE_VALUE(int, WantContinentWith);
  DATA_BASE_TRUTH(RevealEnvironmentInitially);
  DATA_BASE_TRUTH(CanBeOnAnyTerrain);
  DATA_BASE_VALUE(int, Probability);
  DATA_BASE_TRUTH(CanBeSkipped);
  DATA_BASE_TRUTH(PlaceInitially);

  inline cchar *GetTypeID () const { return GetProtoType()->GetClassID(); }
  inline truth IsOfType (cchar *tname) const { return (tname ? (strcmp(tname, GetProtoType()->GetClassID()) == 0) : false); }
  inline truth IsOfType (cfestring &tname) const { return (tname.Compare(GetProtoType()->GetClassID()) == 0); }

  virtual const prototype* FindProtoType () const { return &ProtoType; }

private:
  int GetType () const { return GetProtoType()->GetIndex(); }

protected:
  virtual v2 GetBitmapPos (int) const { return DataBase->BitmapPos; }
  //virtual cfestring& GetNameStem () const;
  virtual void InstallDataBase (int);

protected:
  static const prototype ProtoType;
  const database* DataBase;

  v2 mPos;
  truth mRevealed;
  truth mPlaced;
  truth mGenerated;

public:
  truth MustBeSkipped; // only set and used in `worldmap::Generate()`; prolly should be moved out of here
};


// ////////////////////////////////////////////////////////////////////////// //
/*
#ifdef __FILE_OF_STATIC_WTERRAIN_PROTOTYPE_DEFINITIONS__
#define WTERRAIN_PROTO(name, protobase) \
  template<> const protobase##prototype name##sysbase::ProtoType((protobase##spawner)(&name##sysbase::Spawn), #name);
#else
#define WTERRAIN_PROTO(name, protobase)
#endif


#define WTERRAIN(name, base, protobase)\
  class name;\
  typedef simplesysbase<name, base, protobase##prototype> name##sysbase;\
  WTERRAIN_PROTO(name, protobase)\
  class name : public name##sysbase

#define GWTERRAIN(name, base) WTERRAIN(name, base, gwterrain)
*/


#endif
