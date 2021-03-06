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
#ifndef __MATERIA_H__
#define __MATERIA_H__

#include "ivancommon.h"

#include "script.h"
#include "ivandef.h"
#include "festring.h"
#include "pool.h"

#define MAKE_MATERIAL material::MakeMaterial

class entity;
class bodypart;
class materialprototype;
template <class type> class databasecreator;

typedef material* (*materialspawner)(int, sLong, truth);
typedef material* (*materialcloner)(cmaterial*);

struct materialdatabase : public databasebase
{
  typedef materialprototype prototype;
  void InitDefaults (const materialprototype *NewProtoType, int NewConfig, cfestring &acfgstrname);
  void PostProcess() { }
  const prototype* ProtoType;
  feuLong CategoryFlags;
  feuLong BodyFlags;
  feuLong InteractionFlags;
  int StrengthValue;
  int ConsumeType;
  int Density;
  int Color;
  int RainColor;
  sLong PriceModifier;
  col24 Emitation;
  int NutritionValue;
  festring NameStem;
  festring AdjectiveStem;
  int Effect;
  int ConsumeEndMessage;
  int HitMessage;
  sLong ExplosivePower;
  alpha Alpha;
  int Flexibility;
  int SpoilModifier;
  int EffectStrength;
  int DigProductMaterial;
  int ConsumeWisdomLimit;
  int AttachedGod;
  festring BreatheMessage;
  int StepInWisdomLimit;
  int RustModifier;
  int Acidicity;
  contentscript<item> NaturalForm;
  int HardenedMaterial;
  int SoftenedMaterial;
  int IntelligenceRequirement;
  int Stickiness;
  truth DisablesPanicWhenConsumed;
  truth BlockESP;
};

class materialprototype
{
 public:
  friend class databasecreator<material>;

  materialprototype(const materialprototype*, materialspawner, materialcloner, cchar*);
  virtual ~materialprototype () {}

  material* Spawn(int Config, sLong Volume = 0) const { return Spawner(Config, Volume, false); }
  material* SpawnAndLoad(inputfile&) const;
  material* Clone(cmaterial* Material) const { return Cloner(Material); }
  cchar* GetClassID() const { return ClassID; }
  inline cchar *GetTypeID () const { return ClassID; }
  inline truth IsOfType (cchar *tname) const { return (tname ? (strcmp(tname, ClassID) == 0) : false); }
  inline truth IsOfType (cfestring &tname) const { return (tname.Compare(ClassID) == 0); }
  int GetIndex() const { return Index; }
  const materialprototype* GetBase() const { return Base; }
  int CreateSpecialConfigurations(materialdatabase**, int Configs, int) { return Configs; }
  const materialdatabase* ChooseBaseForConfig(materialdatabase** TempConfig, int, int) { return *TempConfig; }
  const materialdatabase*const* GetConfigData() const { return ConfigData; }
  int GetConfigSize() const { return ConfigSize; }
 private:
  int Index;
  const materialprototype* Base;
  materialdatabase** ConfigData;
  materialdatabase** ConfigTable[CONFIG_TABLE_SIZE];
  int ConfigSize;
  materialspawner Spawner;
  materialcloner Cloner;
  cchar* ClassID;

public:
  EventHandlerMap mOnEvents;
};

class material
{
 public:
  friend class databasecreator<material>;
  typedef materialprototype prototype;
  typedef materialdatabase database;

 public:
  material(int NewConfig, sLong InitVolume = 0, truth Load = false) : MotherEntity(0) { pool::RegisterMaterial(this); Initialize(NewConfig, InitVolume, Load); }
  material() : MotherEntity(0) { pool::RegisterMaterial(this); }
  virtual ~material () { pool::UnregisterMaterial(this); }

  void AddName(festring&, truth = false, truth = true) const;
  festring GetName(truth = false, truth = true) const;
  material* TakeDipVolumeAway();
  material* TakeAllVolumeAway();
  virtual void Save(outputfile&) const;
  virtual void Load(inputfile&);
  truth Effect(character*, int, sLong);
  virtual material* EatEffect(character*, sLong);
  truth HitEffect(character*, bodypart*);
  virtual col16 GetSkinColor() const { return GetColor(); }
  virtual void SetSkinColor(int) { }
  sLong GetRawPrice() const;
  truth CanBeDug(material* ShovelMaterial) const;
  virtual truth HasBe() const { return false; }
  virtual void Be(feuLong) { }
  int GetType() const { return GetProtoType()->GetIndex(); }
  inline cchar *GetTypeID () const { return GetProtoType()->GetClassID(); }
  inline truth IsOfType (cchar *tname) const { return (tname ? (strcmp(tname, GetProtoType()->GetClassID()) == 0) : false); }
  inline truth IsOfType (cfestring &tname) const { return (tname.Compare(GetProtoType()->GetClassID()) == 0); }
  virtual void AddConsumeEndMessage(character*) const;
  DATA_BASE_VALUE(const prototype*, ProtoType);
  DATA_BASE_VALUE(int, Config);
  DATA_BASE_VALUE(feuLong, CommonFlags);
  DATA_BASE_VALUE(feuLong, NameFlags);
  DATA_BASE_VALUE(feuLong, CategoryFlags);
  DATA_BASE_VALUE(feuLong, BodyFlags);
  DATA_BASE_VALUE(feuLong, InteractionFlags);
  virtual DATA_BASE_VALUE(int, StrengthValue);
  DATA_BASE_VALUE(int, ConsumeType);
  DATA_BASE_VALUE(int, Density);
  DATA_BASE_VALUE(int, Color);
  DATA_BASE_VALUE(int, RainColor);
  DATA_BASE_VALUE(sLong, PriceModifier);
  DATA_BASE_VALUE(col24, Emitation);
  DATA_BASE_VALUE(int, NutritionValue);
  DATA_BASE_VALUE(cfestring&, NameStem);
  DATA_BASE_VALUE(cfestring&, AdjectiveStem);
  DATA_BASE_VALUE(int, Effect);
  DATA_BASE_VALUE(int, ConsumeEndMessage);
  DATA_BASE_VALUE(int, HitMessage);
  DATA_BASE_VALUE(sLong, ExplosivePower);
  DATA_BASE_VALUE(alpha, Alpha);
  DATA_BASE_VALUE(int, Flexibility);
  DATA_BASE_VALUE(int, SpoilModifier);
  DATA_BASE_VALUE(int, EffectStrength);
  DATA_BASE_VALUE(int, DigProductMaterial);
  DATA_BASE_VALUE(int, ConsumeWisdomLimit);
  DATA_BASE_VALUE(int, AttachedGod);
  DATA_BASE_VALUE(int, RustModifier);
  DATA_BASE_VALUE(int, Acidicity);
  DATA_BASE_VALUE(const contentscript<item>&, NaturalForm);
  DATA_BASE_VALUE(int, IntelligenceRequirement);
  DATA_BASE_VALUE(int, Stickiness);
  const database* GetDataBase() const { return DataBase; }
  material* SpawnMore() const { return GetProtoType()->Spawn(GetConfig(), Volume); }
  material* SpawnMore(sLong Volume) const { return GetProtoType()->Spawn(GetConfig(), Volume); }
  sLong GetTotalExplosivePower() const;
  static material* MakeMaterial(int, sLong = 0);
  virtual truth IsFlesh() const { return false; }
  virtual truth IsLiquid() const { return false; }
  virtual cchar* GetConsumeVerb() const;
  entity* GetMotherEntity() const { return MotherEntity; }
  void SetMotherEntity(entity* What) { MotherEntity = What; }
  truth IsSameAs(cmaterial* What) const { return What->GetConfig() == GetConfig(); }
  truth IsTransparent() const { return GetAlpha() != 255; }
  virtual sLong GetTotalNutritionValue() const;
  virtual truth IsVeryCloseToSpoiling() const { return false; }
  virtual void AddWetness(sLong) { }
  virtual int GetSpoilLevel() const { return 0; }
  virtual void ResetSpoiling() { }
  truth CanBeEatenByAI(ccharacter*) const;
  virtual void SetSpoilCounter(int) { }
  DATA_BASE_VALUE(cfestring&, BreatheMessage);
  truth BreatheEffect(character*);
  virtual truth SkinColorIsSparkling() const { return IsSparkling(); }
  virtual void SetSkinColorIsSparkling(truth) { }
  DATA_BASE_VALUE(int, StepInWisdomLimit);
  virtual void SetRustLevel(int) { }
  virtual int GetRustLevel() const { return NOT_RUSTED; }
  virtual int GetRustData() const { return NOT_RUSTED; }
  virtual truth TryToRust(sLong, sLong = 0) { return false; }
  static const database* GetDataBase(int);
  virtual truth CanSpoil() const { return false; }
  truth IsSolid() const { return !IsLiquid(); }
  /* A dummy materialpredicate */
  truth True() const { return true; }
  void FinishConsuming(character*);
  sLong GetVolume() const { return Volume; }
  sLong GetWeight() const { return Volume ? sLong(double(Volume) * GetDensity() / 1000) : 0; }
  void EditVolume(sLong What) { SetVolume(Volume + What); }
  void SetVolume(sLong);
  void SetVolumeNoSignals(sLong What) { Volume = What; }
  virtual truth IsPowder() const { return false; }
  static item* CreateNaturalForm(int, sLong);
  item* CreateNaturalForm(sLong) const;
  virtual truth IsInfectedByLeprosy() const { return false; }
  virtual void SetIsInfectedByLeprosy(truth) { }
  virtual truth AddRustLevelDescription(festring&, truth) const { return false; }
  int GetHardenedMaterial(citem*) const;
  int GetSoftenedMaterial(citem*) const;
  int GetHardenModifier(citem*) const;
  virtual int GetSpoilPercentage() const { return 0; }
  virtual truth Spoils() const { return false; }
  virtual truth IsExplosive() const;
  virtual truth IsSparkling() const;
  material* Duplicate() const { return DataBase->ProtoType->Clone(this); }
  truth IsStuckTo(ccharacter*) const;
  DATA_BASE_TRUTH(DisablesPanicWhenConsumed);
  DATA_BASE_TRUTH(BlockESP);

  inline cchar *GetClassID () const { return (FindProtoType() ? FindProtoType()->GetClassID() : ""); }
  virtual const prototype* FindProtoType() const { return &ProtoType; }

  void SendToHell () { pool::MaterToHell(this); }

 protected:
  virtual void PostConstruct() { }
  void Initialize(int, sLong, truth);
  static const prototype ProtoType;
  const database* DataBase;
  entity* MotherEntity;
  sLong Volume;
};


template <class type, class base>
class materialsysbase : public base
{
 public:
  typedef materialsysbase<type, base> mybase;
  static type* Spawn(int Config = 0, sLong Volume = 0, truth Load = false)
  {
    type* M = new type;
    M->Initialize(Config, Volume, Load);
    return M;
  }
  static material* Clone(const type* T) { return new type(*T); }

  virtual ~materialsysbase () {}

  inline cchar *GetClassID () const { return (FindProtoType() ? FindProtoType()->GetClassID() : ""); }
  virtual const materialprototype* FindProtoType() const { return &ProtoType; }
  static const materialprototype ProtoType;
};

#ifdef __FILE_OF_STATIC_MATERIAL_PROTOTYPE_DEFINITIONS__
#define MATERIAL_PROTO(name, base)\
template<> const materialprototype\
  name##sysbase::ProtoType(&base::ProtoType,\
         (materialspawner)(&name##sysbase::Spawn),\
         (materialcloner)(&name##sysbase::Clone), #name);
#else
#define MATERIAL_PROTO(name, base)
#endif

#define MATERIAL(name, base)\
class name;\
typedef materialsysbase<name, base> name##sysbase;\
MATERIAL_PROTO(name, base)\
class name : public name##sysbase

#endif
