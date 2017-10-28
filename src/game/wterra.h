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
class continent;


typedef gwterrain *(*gwterrainspawner) ();
typedef owterrain *(*owterrainspawner) (int Config, int SpecialFlags);


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
class gwterrainprototype {
public:
  gwterrainprototype (gwterrainspawner, cchar *);
  virtual ~gwterrainprototype () {}

  gwterrain *Spawn () const { return Spawner(); }
  gwterrain *SpawnAndLoad (inputfile &) const;
  cchar *GetClassID () const { return ClassID; }
  int GetIndex () const { return Index; }

private:
  int Index;
  gwterrainspawner Spawner;
  cchar *ClassID;

public:
  festring mOnEvents;
};


class gwterrain : public wterrain, public gterrain {
public:
  typedef gwterrainprototype prototype;

  virtual ~gwterrain () {}

  virtual void Save (outputfile &) const;
  void Draw (blitdata &) const;
  virtual int GetPriority () const = 0;
  virtual int GetEntryDifficulty () const { return 10; }
  virtual const prototype *GetProtoType() const = 0;
  int GetType () const { return GetProtoType()->GetIndex(); }
  void CalculateNeighbourBitmapPoses ();
  virtual int GetWalkability () const;

protected:
  std::pair<v2, int> Neighbour[8];
};


// ////////////////////////////////////////////////////////////////////////// //
struct owterraindatabase : public databasebase {
  typedef owterrainprototype prototype;

  void InitDefaults (const owterrainprototype *NewProtoType, int NewConfig, cfestring &acfgstrname);
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
public:
  friend class databasecreator<owterrain>;

  owterrainprototype (const owterrainprototype*, owterrainspawner, cchar *);
  virtual ~owterrainprototype () {}

  owterrain *Spawn (int Config, int SpecialFlags=0) const;
  owterrain *SpawnAndLoad (inputfile &) const;
  cchar *GetClassID () const { return ClassID; }
  int GetIndex () const { return Index; }

  const owterrainprototype *GetBase () const { return Base; }
  int CreateSpecialConfigurations (owterraindatabase **, int Configs, int) { return Configs; }
  const owterraindatabase *ChooseBaseForConfig (owterraindatabase **TempConfig, int, int) { return *TempConfig; }
  const owterraindatabase *const *GetConfigData () const { return ConfigData; }
  int GetConfigSize () const { return ConfigSize; }

  /*
  virtual int GetAttachedDungeon () const { return 0; }
  virtual int GetAttachedArea () const { return 0; }
  virtual int GetNativeGTerrainType () const { return 0; }
  virtual truth RevealEnvironmentInitially () const { return false; }
  virtual truth CanBeOnAnyTerrain () const { return false; }
  virtual int Probability () const { return 100; }
  virtual truth CanBeSkipped () const { return false; }
  */

private:
  int Index;
  const owterrainprototype* Base;
  owterraindatabase** ConfigData;
  owterraindatabase** ConfigTable[CONFIG_TABLE_SIZE];
  int ConfigSize;
  owterrainspawner Spawner;
  cchar *ClassID;

public:
  festring mOnEvents;
};


class owterrain : public wterrain, public oterrain {
public:
  friend class databasecreator<owterrain>;
  friend class owterrainprototype;
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
  int GetType() const { return GetProtoType()->GetIndex(); }
  virtual int GetAttachedEntry () const;
  virtual truth Enter (truth) const;
  virtual int GetWalkability () const;
  const database *GetDataBase () const { return DataBase; }

  truth IsRevealed () const { return mRevealed; }
  void SetRevealed (truth v) { mRevealed = v; }

  truth IsPlaced () const { return mPlaced; }
  void SetPlaced (truth v) { mPlaced = v; }

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

protected:
  virtual v2 GetBitmapPos (int) const { return DataBase->BitmapPos; }
  //virtual cfestring& GetNameStem () const;
  virtual void InstallDataBase (int);
  virtual const prototype* FindProtoType () const { return &ProtoType; }

protected:
  static const prototype ProtoType;
  const database* DataBase;

  v2 mPos;
  truth mRevealed;
  truth mPlaced;
  truth mGenerated;

public:
  virtual truth IsSuitableContinent (continent *);
  /*
  virtual truth WantPetrusContinent () const { return true; } // for now they all should want it
  virtual truth IsAttnam () const { return false; }
  virtual truth IsHidden () const { return false; }
  virtual truth IsRevealed () const { return false; }
  */

public:
  festring mOnEvents;
};


// ////////////////////////////////////////////////////////////////////////// //
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


#endif
