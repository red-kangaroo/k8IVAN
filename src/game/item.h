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

#ifndef __ITEM_H__
#define __ITEM_H__

#include "ivancommon.h"

#include <cmath>

#include "object.h"
#include "lsquare.h"
#include "slot.h"
#include "lock.h"
#include "festring.h"


class felist;
class head;
class itemprototype;
template <class type> class databasecreator;

typedef std::vector<item*> itemvector;
typedef std::vector<fluid*> fluidvector;
typedef truth (rawbitmap::*pixelpredicate)(v2) const;
typedef truth (material::*materialpredicate)() const;
typedef truth (item::*sorter)(ccharacter*) const;
typedef item* (*itemspawner)(int, int);
typedef item* (*itemcloner)(citem*);

extern materialpredicate TrueMaterialPredicate;

struct sortdata
{
  sortdata(itemvector& AllItems, ccharacter* Character, truth Recurse, sorter Sorter)
  : AllItems(AllItems),
    Character(Character),
    Recurse(Recurse),
    Sorter(Sorter) { }
  itemvector& AllItems;
  ccharacter* Character;
  truth Recurse;
  sorter Sorter;
};

struct idholder
{
  idholder(feuLong ID) : ID(ID) { }
  idholder* Next;
  feuLong ID;
};

outputfile& operator<<(outputfile&, const idholder*);
inputfile& operator>>(inputfile&, idholder*&);

class itemlock
{
public:
  typedef itemprototype prototype;

public:
  itemlock() : Locked(false) { }
  virtual ~itemlock () { }

public:
  void Save(outputfile&) const;
  void Load(inputfile&);
  virtual truth TryKey(item*, character*);
  virtual int GetVirtualConfig() const = 0;
  virtual void SetVirtualConfig(int, int = 0) = 0;
  virtual const prototype* GetVirtualProtoType() const = 0;
  virtual festring GetVirtualDescription(int) const = 0;
 protected:
  virtual void PostConstruct();
  truth Locked;
};

typedef lockable<item, itemlock> lockableitem;

struct itemdatabase : public databasebase
{
  typedef itemprototype prototype;
  void InitDefaults (const itemprototype *NewProtoType, int NewConfig, cfestring &acfgstrname);
  truth AllowRandomInstantiation() const;
  void PostProcess() { }

  const prototype* ProtoType;
  /* Maintained by configcontainer */
  sLong PartialPossibilitySum;
  sLong PartialCategoryPossibilitySum;
  int Possibility;
  int WeaponCategory;
  truth IsDestroyable;
  truth CanBeWished;
  truth IsMaterialChangeable;
  truth IsPolymorphSpawnable;
  truth IsAutoInitializable;
  truth IsAbstract;
  truth IsPolymorphable;
  truth CanBeGeneratedInContainer;
  truth IsTwoHanded;
  truth CreateDivineConfigurations;
  truth CanBeCloned;
  truth CanBeMirrored;
  truth AffectsArmStrength;
  truth AffectsLegStrength;
  truth AffectsDexterity;
  truth AffectsAgility;
  truth AffectsEndurance;
  truth AffectsPerception;
  truth AffectsIntelligence;
  truth AffectsWisdom;
  truth AffectsWillPower;
  truth AffectsCharisma;
  truth AffectsMana;
  truth PriceIsProportionalToEnchantment;
  truth CanBeUsedBySmith;
  truth AffectsCarryingCapacity;
  truth HandleInPairs;
  truth CanBeEnchanted;
  truth IsQuestItem;
  truth IsGoodWithPlants;
  truth CreateLockConfigurations;
  truth CanBePickedUp;
  truth HasSecondaryMaterial;
  truth AllowEquip;
  truth IsValuable;
  truth HasNormalPictureDirection;
  truth IsKamikazeWeapon;
  truth FlexibilityIsEssential;
  truth CanBeBroken;
  truth CanBePiled;
  sLong Category;
  int EnergyResistance;
  int FireResistance;
  int PoisonResistance;
  int ElectricityResistance;
  int AcidResistance;
  int SoundResistance;
  int StrengthModifier;
  int FormModifier;
  int DefaultSize;
  sLong DefaultMainVolume;
  sLong DefaultSecondaryVolume;
  v2 BitmapPos;
  sLong Price;
  col24 BaseEmitation;
  truth UsesLongArticle;
  festring Adjective;
  truth UsesLongAdjectiveArticle;
  festring NameSingular;
  festring NamePlural;
  festring PostFix;
  int ArticleMode;
  fearray<sLong> MainMaterialConfig;
  fearray<sLong> SecondaryMaterialConfig;
  fearray<sLong> MaterialConfigChances;
  sLong MaterialConfigChanceSum;
  fearray<festring> Alias;
  int OKVisualEffects;
  int ForcedVisualEffects;
  int Roundness;
  sLong GearStates;
  int BeamRange;
  v2 WallBitmapPos;
  festring FlexibleNameSingular;
  int MinCharges;
  int MaxCharges;
  sLong StorageVolume;
  int MaxGeneratedContainedItems;
  int BaseEnchantment;
  int InElasticityPenaltyModifier;
  int DamageDivider;
  col16 BeamColor;
  int BeamEffect;
  int BeamStyle;
  int WearWisdomLimit;
  int AttachedGod;
  int BreakEffectRangeSquare;
  v2 WieldedBitmapPos;
  int CoverPercentile;
  v2 TorsoArmorBitmapPos;
  v2 ArmArmorBitmapPos;
  v2 AthleteArmArmorBitmapPos;
  v2 LegArmorBitmapPos;
  v2 HelmetBitmapPos;
  v2 CloakBitmapPos;
  v2 BeltBitmapPos;
  v2 GauntletBitmapPos;
  v2 BootBitmapPos;
  int ReadDifficulty;
  int EnchantmentMinusChance;
  int EnchantmentPlusChance;
  int TeleportPriority;
  int DamageFlags;
  festring BreakMsg;
  truth IsSadistWeapon;
  truth IsThrowingWeapon;
  sLong ThrowItemTypes;
  truth CanFlame;
  int MagicEffect;
  RandomChance MagicEffectDuration;
  RandomChance MagicEffectChance;
  fearray<festring> MagicMessageCanSee;
  fearray<festring> MagicMessageCannotSee;
  fearray<int> AllowedDungeons;
  fearray<festring> LevelTags;
};

class itemprototype
{
 public:
  friend class databasecreator<item>;

  itemprototype(const itemprototype*, itemspawner, itemcloner, cchar*);
  virtual ~itemprototype () {}

  item* Spawn(int Config = 0, int SpecialFlags = 0) const { return Spawner(Config, SpecialFlags); }
  item* SpawnAndLoad(inputfile&) const;
  item* Clone(citem* Item) const { return Cloner(Item); }
  cchar* GetClassID() const { return ClassID; }
  int GetIndex() const { return Index; }
  inline cchar *GetTypeID () const { return ClassID; }
  inline truth IsOfType (cchar *tname) const { return (tname ? (strcmp(tname, ClassID) == 0) : false); }
  inline truth IsOfType (cfestring &tname) const { return (tname.Compare(ClassID) == 0); }
  const itemprototype* GetBase() const { return Base; }
  int CreateSpecialConfigurations(itemdatabase**, int, int);
  const itemdatabase* ChooseBaseForConfig(itemdatabase**, int, int);
  const itemdatabase*const* GetConfigData() const { return ConfigData; }
  int GetConfigSize() const { return ConfigSize; }
 private:
  int Index;
  const itemprototype* Base;
  itemdatabase** ConfigData;
  itemdatabase** ConfigTable[CONFIG_TABLE_SIZE];
  int ConfigSize;
  itemspawner Spawner;
  itemcloner Cloner;
  cchar* ClassID;

public:
  EventHandlerMap mOnEvents;
};


class item : public object
{
 public:
  friend class databasecreator<item>;
  typedef itemprototype prototype;
  typedef itemdatabase database;

 public:
  item();
  item(citem&);
  virtual ~item();

 public:
  inline truth IsSteppedOn () const { return mIsStepedOn; }
  inline void SetSteppedOn (truth v) { mIsStepedOn = v; }

  virtual double GetWeaponStrength() const;
  virtual truth Open(character*);
  truth Consume(character*, sLong);
  virtual truth CanBeRead(character*) const { return false; }
  virtual truth Read(character*);
  virtual void FinishReading(character*) { }
  virtual truth HitEffect(character*, character*, v2, int, int, truth) { return false; }
  virtual void DipInto(liquid*, character*) { }
  virtual truth DumpTo (character *dumper, v2 dest) { return false; }
  virtual liquid* CreateDipLiquid() { return 0; }
  virtual item* BetterVersion() const { return 0; }
  virtual int GetOfferValue(int) const;
  virtual void Fly(character*, int, int);
  int HitCharacter(character*, character*, int, double, int);
  virtual truth DogWillCatchAndConsume(ccharacter*) const { return false; }
  virtual truth Apply(character*);
  virtual truth Zap(character*, v2, int) { return false; }
  virtual truth Burn(character*, v2, int);
  virtual truth Polymorph(character*, stack*);
  virtual truth CheckPickUpEffect(character*) { return true; }
  virtual void StepOnEffect(character*) { }
  virtual void SignalSquarePositionChange(int);
  virtual truth CanBeEatenByAI(ccharacter*) const;
  virtual void Save(outputfile&) const;
  virtual void Load(inputfile&);
  virtual void ChargeFully(character*) { }
  void SetSize(int Value) { Size = Value; }
  virtual int GetSize() const { return Size; }
  feuLong GetID() const { return ID; }
  virtual void TeleportRandomly();
  virtual int GetStrengthValue() const;
  slot* GetSlot(int I = 0) const { return Slot[I]; }
  void SetMainSlot(slot* What) { Slot[0] = What; }
  void PlaceToSlot(slot* Slot) { Slot->PutInItem(this); }
  virtual void RemoveFromSlot();
  void MoveTo(stack *Stack, truth setPickupTime=false);
  truth IsMainSlot(const slot* What) const { return Slot[0] == What; }
  static cchar* GetItemCategoryName(sLong);
  truth IsEatable(ccharacter*) const;
  truth IsDrinkable(ccharacter*) const;
  virtual truth IsOpenable(ccharacter*) const { return false; }
  virtual truth IsReadable(ccharacter*) const { return false; }
  virtual truth IsDippable(ccharacter*) const { return false; }
  virtual truth IsDipDestination(ccharacter*) const { return false; }
  virtual truth IsDumpable(ccharacter*) const { return false; }
  virtual truth IsAppliable(ccharacter*) const { return false; }
  virtual truth IsZappable(ccharacter*) const { return false; }
  virtual truth IsChargeable(ccharacter*) const { return false; }
  virtual truth IsHelmet(ccharacter*) const { return false; }
  virtual truth IsAmulet(ccharacter*) const { return false; }
  virtual truth IsCloak(ccharacter*) const { return false; }
  virtual truth IsBodyArmor(ccharacter*) const { return false; }
  virtual truth IsRing(ccharacter*) const { return false; }
  virtual truth IsGauntlet(ccharacter*) const { return false; }
  virtual truth IsBelt(ccharacter*) const { return false; }
  virtual truth IsBoot(ccharacter*) const { return false; }
  virtual truth IsShield(ccharacter*) const { return false; }
  virtual truth IsWeapon(ccharacter*) const { return false; }
  virtual truth IsArmor(ccharacter*) const { return false; }
  virtual truth IsEnchantable(ccharacter*) const { return CanBeEnchanted(); }
  virtual truth IsRepairable(ccharacter*) const { return IsBroken() || IsRusted(); }
  virtual truth IsDecosAdShirt(ccharacter*) const { return false; }
  virtual truth IsLuxuryItem(ccharacter*) const { return false; }
  virtual truth MaterialIsChangeable(ccharacter*) const { return true; }
  virtual truth IsBeverage(ccharacter*) const;
  virtual truth CanBeHardened(ccharacter*) const;
  virtual truth HasLock(ccharacter*) const { return false; }
  virtual truth IsOnGround() const;
  virtual truth IsFlaming (ccharacter *) const { return CanFlame(); }
  virtual truth IsLockableContainer () const { return false; }
  int GetResistance(int) const;
  virtual void Be();
  int GetType() const { return GetProtoType()->GetIndex(); }
  inline cchar *GetTypeID () const { return GetProtoType()->GetClassID(); }
  inline truth IsOfType (cchar *tname) const { return (tname ? (strcmp(tname, GetProtoType()->GetClassID()) == 0) : false); }
  inline truth IsOfType (cfestring &tname) const { return (tname.Compare(GetProtoType()->GetClassID()) == 0); }
  virtual truth ReceiveDamage(character*, int, int, int = YOURSELF);
  virtual truth RaiseTheDead(character*) { return false; }
  virtual int GetBodyPartIndex() const { return 0xFF; }
  const database* GetDataBase() const { return DataBase; }
  virtual truth CanOpenLockType(int) const { return false; }
  DATA_BASE_VALUE(const prototype*, ProtoType);
  DATA_BASE_VALUE(int, Config);
  virtual DATA_BASE_TRUTH_WITH_PARAMETER(IsDestroyable, ccharacter*);
  DATA_BASE_TRUTH(IsMaterialChangeable);
  DATA_BASE_VALUE(int, WeaponCategory);
  DATA_BASE_TRUTH(IsAutoInitializable);
  DATA_BASE_VALUE(sLong, Category);
  DATA_BASE_VALUE(int, EnergyResistance);
  DATA_BASE_VALUE(int, FireResistance);
  DATA_BASE_VALUE(int, PoisonResistance);
  DATA_BASE_VALUE(int, ElectricityResistance);
  DATA_BASE_VALUE(int, AcidResistance);
  DATA_BASE_VALUE(int, SoundResistance);
  DATA_BASE_VALUE(int, StrengthModifier);
  virtual DATA_BASE_VALUE(int, FormModifier);
  DATA_BASE_VALUE(int, DefaultSize);
  DATA_BASE_VALUE(sLong, DefaultMainVolume);
  DATA_BASE_VALUE(sLong, DefaultSecondaryVolume);
  virtual DATA_BASE_VALUE_WITH_PARAMETER(v2, BitmapPos, int);
  virtual DATA_BASE_VALUE(sLong, Price);
  virtual DATA_BASE_VALUE(col24, BaseEmitation);
  virtual DATA_BASE_TRUTH(UsesLongArticle);
  virtual DATA_BASE_VALUE(cfestring&, Adjective);
  virtual DATA_BASE_TRUTH(UsesLongAdjectiveArticle);
  virtual DATA_BASE_VALUE(cfestring&, NameSingular);
  virtual DATA_BASE_VALUE(cfestring&, NamePlural);
  virtual DATA_BASE_VALUE(cfestring&, PostFix);
  virtual DATA_BASE_VALUE(int, ArticleMode);
  DATA_BASE_VALUE(const fearray<sLong>&, MainMaterialConfig);
  DATA_BASE_VALUE(const fearray<sLong>&, SecondaryMaterialConfig);
  virtual DATA_BASE_VALUE(const fearray<sLong>&, MaterialConfigChances);
  virtual DATA_BASE_VALUE(sLong, MaterialConfigChanceSum);
  DATA_BASE_TRUTH(IsPolymorphable);
  virtual DATA_BASE_VALUE(int, OKVisualEffects);
  DATA_BASE_TRUTH(CanBeGeneratedInContainer);
  virtual DATA_BASE_VALUE(int, ForcedVisualEffects);
  DATA_BASE_VALUE(int, Roundness);
  DATA_BASE_VALUE(sLong, GearStates);
  DATA_BASE_TRUTH(IsTwoHanded);
  DATA_BASE_TRUTH(CanBeBroken);
  DATA_BASE_VALUE_WITH_PARAMETER(v2, WallBitmapPos, int);
  DATA_BASE_VALUE(cfestring&, FlexibleNameSingular);
  DATA_BASE_TRUTH(CanBePiled);
  DATA_BASE_TRUTH(AffectsArmStrength);
  DATA_BASE_TRUTH(AffectsLegStrength);
  DATA_BASE_TRUTH(AffectsDexterity);
  DATA_BASE_TRUTH(AffectsAgility);
  DATA_BASE_TRUTH(AffectsEndurance);
  DATA_BASE_TRUTH(AffectsPerception);
  DATA_BASE_TRUTH(AffectsIntelligence);
  DATA_BASE_TRUTH(AffectsWisdom);
  DATA_BASE_TRUTH(AffectsWillPower);
  DATA_BASE_TRUTH(AffectsCharisma);
  DATA_BASE_TRUTH(AffectsMana);
  DATA_BASE_TRUTH(AffectsCarryingCapacity);
  DATA_BASE_VALUE(int, BaseEnchantment);
  DATA_BASE_TRUTH(PriceIsProportionalToEnchantment);
  DATA_BASE_VALUE(int, MaxCharges);
  DATA_BASE_VALUE(int, MinCharges);
  DATA_BASE_VALUE(int, InElasticityPenaltyModifier);
  DATA_BASE_VALUE(sLong, StorageVolume);
  DATA_BASE_VALUE(int, MaxGeneratedContainedItems);
  virtual DATA_BASE_TRUTH(CanBeCloned);
  virtual DATA_BASE_TRUTH(CanBeMirrored);
  DATA_BASE_VALUE(int, BeamRange);
  DATA_BASE_TRUTH(CanBeUsedBySmith);
  DATA_BASE_VALUE(int, DamageDivider);
  DATA_BASE_TRUTH(HandleInPairs);
  DATA_BASE_TRUTH(CanBeEnchanted);
  DATA_BASE_VALUE(sLong, BeamColor);
  DATA_BASE_VALUE(int, BeamEffect);
  DATA_BASE_VALUE(int, BeamStyle);
  DATA_BASE_VALUE(int, WearWisdomLimit);
  DATA_BASE_VALUE(int, BreakEffectRangeSquare);
  virtual DATA_BASE_VALUE_WITH_PARAMETER(v2, WieldedBitmapPos, int);
  DATA_BASE_TRUTH(IsQuestItem);
  DATA_BASE_TRUTH(IsGoodWithPlants);
  DATA_BASE_TRUTH(CanBePickedUp);
  DATA_BASE_VALUE(int, CoverPercentile);
  DATA_BASE_VALUE_WITH_PARAMETER(v2, TorsoArmorBitmapPos, int);
  DATA_BASE_VALUE_WITH_PARAMETER(v2, ArmArmorBitmapPos, int);
  DATA_BASE_VALUE_WITH_PARAMETER(v2, AthleteArmArmorBitmapPos, int);
  DATA_BASE_VALUE_WITH_PARAMETER(v2, LegArmorBitmapPos, int);
  DATA_BASE_VALUE_WITH_PARAMETER(v2, HelmetBitmapPos, int);
  DATA_BASE_VALUE_WITH_PARAMETER(v2, CloakBitmapPos, int);
  DATA_BASE_VALUE_WITH_PARAMETER(v2, BeltBitmapPos, int);
  DATA_BASE_VALUE_WITH_PARAMETER(v2, GauntletBitmapPos, int);
  DATA_BASE_VALUE_WITH_PARAMETER(v2, BootBitmapPos, int);
  DATA_BASE_TRUTH(AllowEquip);
  DATA_BASE_VALUE(int, ReadDifficulty);
  DATA_BASE_VALUE(int, EnchantmentMinusChance);
  DATA_BASE_VALUE(int, EnchantmentPlusChance);
  virtual DATA_BASE_VALUE(int, TeleportPriority);
  DATA_BASE_TRUTH(HasNormalPictureDirection);
  DATA_BASE_VALUE(int, DamageFlags);
  DATA_BASE_TRUTH(FlexibilityIsEssential);
  DATA_BASE_VALUE(cfestring&, BreakMsg);
  DATA_BASE_TRUTH(IsSadistWeapon);
  DATA_BASE_TRUTH(IsThrowingWeapon);
  DATA_BASE_VALUE(sLong, ThrowItemTypes);
  DATA_BASE_TRUTH(CanFlame);
  DATA_BASE_VALUE(int, MagicEffect);
  DATA_BASE_VALUE(RandomChance, MagicEffectDuration);
  DATA_BASE_VALUE(RandomChance, MagicEffectChance);
  DATA_BASE_VALUE(fearray<festring>, MagicMessageCanSee);
  DATA_BASE_VALUE(fearray<festring>, MagicMessageCannotSee);
  DATA_BASE_VALUE(const fearray<int> &, AllowedDungeons);
  DATA_BASE_VALUE(const fearray<festring> &, LevelTags);
  truth CanBeSoldInLibrary(character* Librarian) const { return CanBeRead(Librarian); }
  virtual truth TryKey(item*, character*) { return false; }
  sLong GetBlockModifier() const;
  truth IsSimiliarTo(item*) const;
  virtual truth IsPickable(character*) const { return true; }
  truth CanBeSeenByPlayer() const;
  virtual truth CanBeSeenBy(ccharacter*) const;
  festring GetDescription(int) const;
  virtual square* GetSquareUnderEntity(int = 0) const;
  virtual square* GetSquareUnder(int = 0) const;
  virtual lsquare* GetLSquareUnder(int = 0) const;
  level* GetLevel() const { return static_cast<level*>(Slot[0]->GetSquareUnder()->GetArea()); }
  area* GetArea() const { return Slot[0]->GetSquareUnder()->GetArea(); }
  v2 GetPos(int I = 0) const { return Slot[I]->GetSquareUnder()->GetPos(); }
  square* GetNearSquare(v2 Pos) const { return Slot[0]->GetSquareUnder()->GetArea()->GetSquare(Pos); }
  square* GetNearSquare(int x, int y) const { return Slot[0]->GetSquareUnder()->GetArea()->GetSquare(x, y); }
  lsquare* GetNearLSquare(v2 Pos) const { return static_cast<lsquare*>(Slot[0]->GetSquareUnder()->GetArea()->GetSquare(Pos)); }
  lsquare* GetNearLSquare(int x, int y) const { return static_cast<lsquare*>(Slot[0]->GetSquareUnder()->GetArea()->GetSquare(x, y)); }
  virtual void SignalVolumeAndWeightChange();
  virtual void CalculateVolumeAndWeight();
  sLong GetVolume() const { return Volume; }
  sLong GetWeight() const { return Weight; }
  virtual void SignalEmitationIncrease(col24);
  virtual void SignalEmitationDecrease(col24);
  void CalculateAll();
  virtual void DropEquipment(stack* = 0) { }
  virtual truth IsDangerous(ccharacter*) const { return false; }
  virtual truth NeedDangerSymbol() const { return false; }
  void WeaponSkillHit(int);
  virtual void SetTeam(int) { }
  void SpecialGenerationHandler();
  item* Duplicate(feuLong = 0);
  virtual void SetIsActive(truth) { }
  double GetBaseDamage() const;
  int GetBaseMinDamage() const;
  int GetBaseMaxDamage() const;
  int GetBaseToHitValue() const;
  int GetBaseBlockValue() const;
  virtual void AddInventoryEntry(ccharacter*, festring&, int, truth) const;
  sLong GetNutritionValue() const;
  virtual void SignalSpoil(material*);
  virtual truth AllowSpoil() const;
  item* DuplicateToStack(stack*, feuLong = 0);
  virtual truth CanBePiledWith(citem*, ccharacter*) const;
  virtual sLong GetTotalExplosivePower() const { return 0; }
  virtual void Break(character*, int = YOURSELF);
  virtual void SetEnchantment(int) { }
  virtual void EditEnchantment(int) { }
  virtual void SignalEnchantmentChange();
  virtual double GetTHVBonus() const { return 0.; }
  virtual double GetDamageBonus() const { return 0.; }
  virtual void DrawContents(ccharacter*) { }
  virtual int GetEnchantment() const { return 0; }
  sLong GetEnchantedPrice(int) const;
  virtual item* Fix();
  int GetStrengthRequirement() const;
  virtual int GetInElasticityPenalty(int) const { return 0; }
  virtual truth IsFixableBySmith(ccharacter*) const { return false; }
  virtual truth IsFixableByTailor(ccharacter*) const { return false; }
  virtual sLong GetFixPrice() const;
  virtual void DonateSlotTo(item*);
  virtual int GetSpoilLevel() const;
  virtual void SignalSpoilLevelChange(material*);
  void ResetSpoiling();
  virtual void SetItemsInside(const fearray<contentscript<item> >&, int) { }
  virtual int GetCarryingBonus() const { return 0; }
  cchar* GetStrengthValueDescription() const;
  cchar* GetBaseToHitValueDescription() const;
  cchar* GetBaseBlockValueDescription() const;
  virtual truth IsInCorrectSlot(int) const;
  truth IsInCorrectSlot() const;
  int GetEquipmentIndex() const;
  room* GetRoom(int I = 0) const { return GetLSquareUnder(I)->GetRoom(); }
  virtual truth HasBetterVersion() const { return false; }
  virtual void SortAllItems(const sortdata&) const;
  virtual truth AllowAlphaEverywhere() const { return false; }
  virtual int GetAttachedGod() const;
  virtual sLong GetTruePrice() const;
  virtual void Search(ccharacter*, int) { }
  virtual head* Behead() { return 0; }
  virtual truth EffectIsGood() const { return false; }
#ifdef WIZARD
  virtual void AddAttackInfo(felist&) const;
  void AddMiscellaneousInfo(felist&) const;
#endif
  virtual void PreProcessForBone();
  virtual void PostProcessForBone();
  virtual void FinalProcessForBone() { }
  virtual truth SuckSoul(character*, character* = 0) { return false; }
  void SetConfig(int, int = 0);
  god* GetMasterGod() const;
  idholder* GetCloneMotherID() const { return CloneMotherID; }
  virtual void SignalStackAdd(stackslot*, void (stack::*)(item*, truth));
  virtual int GetSquareIndex(v2) const { return 0; }
  virtual void Draw(blitdata&) const;
  v2 GetLargeBitmapPos(v2, int) const;
  void LargeDraw(blitdata&) const;
  virtual truth BunnyWillCatchAndConsume(ccharacter*) const { return false; }
  void DonateIDTo(item*);
  virtual void SignalRustLevelChange();
  virtual void SendNewDrawAndMemorizedUpdateRequest() const;
  virtual void CalculateSquaresUnder() { SquaresUnder = 1; }
  int GetSquaresUnder() const { return SquaresUnder; }
  virtual void CalculateEmitation();
  void FillFluidVector(fluidvector&, int = 0) const;
  virtual void SpillFluid(character*, liquid*, int = 0);
  virtual void TryToRust(sLong);
  void RemoveFluid(fluid*);
  void RemoveAllFluids ();
  void AddFluid(liquid*, festring, int, truth);
  virtual truth IsAnimated() const;
  const rawbitmap* GetRawPicture() const;
  void DrawFluidGearPictures(blitdata&, int = 0) const;
  void DrawFluidBodyArmorPictures(blitdata&, int) const;
  void CheckFluidGearPictures(v2, int, truth);
  void DrawFluids(blitdata&) const;
  virtual void ReceiveAcid(material*, cfestring&, sLong);
  virtual truth ShowFluids() const { return true; }
  void DonateFluidsTo(item*);
  void Destroy(character*, int);
  virtual truth AllowFluidBe() const { return true; }
  virtual truth IsRusted() const;
  virtual void RemoveRust();
  void SetSpoilPercentage(int);
  virtual pixelpredicate GetFluidPixelAllowedPredicate() const;
  void RedistributeFluids();
  virtual material* GetConsumeMaterial(ccharacter*, materialpredicate = TrueMaterialPredicate) const;
  virtual material* RemoveMaterial(material*);
  virtual void Cannibalize();
  void InitMaterials(material*, truth = true);
  void SetMainMaterial(material*, int = 0);
  void ChangeMainMaterial(material*, int = 0);
  virtual void GenerateMaterials();
  virtual void InitMaterials(const materialscript*, const materialscript*, truth);
  int GetSquarePosition() const { return (Flags & SQUARE_POSITION_BITS) >> SQUARE_POSITION_SHIFT; }
  virtual void DestroyBodyPart(stack*) { SendToHell(); }
  int GetLifeExpectancy() const { return LifeExpectancy; }
  virtual void SetLifeExpectancy(int, int);
  int NeedsBe() const { return LifeExpectancy; }
  truth IsVeryCloseToDisappearance() const { return LifeExpectancy && LifeExpectancy < 10; }
  truth IsVeryCloseToSpoiling() const;
  virtual truth IsValuable() const;
  virtual truth Necromancy(character*) { return false; }
  virtual void CalculateEnchantment() { }
  virtual character* GetBodyPartMaster() const { return 0; }
  virtual truth AllowFluids() const { return false; }
  int GetHinderVisibilityBonus(ccharacter*) const;
  virtual DATA_BASE_TRUTH_WITH_PARAMETER(IsKamikazeWeapon, ccharacter*);
  virtual void AddTrapName(festring&, int) const;
  int GetMainMaterialRustLevel() const;
  truth HasID(feuLong What) const { return ID == What; }
  virtual truth Spoils() const;
  int GetMaxSpoilPercentage() const;
  truth HasPrice() const;
  virtual void Disappear();
  festring GetLocationDescription() const;
  truth IsEquipped() const;
  festring GetExtendedDescription() const;
  virtual ccharacter* FindCarrier() const;
  virtual void BlockEffect(character*, character*, item*, int type) { }
  virtual bool WillExplodeSoon() const { return false; }
  virtual const character* GetWearer() const;
  virtual bool SpecialOfferEffect(int) { return false; }
  void Haste();
  void Slow();
  void SendMemorizedUpdateRequest() const;

  festring ProcessMessage (cfestring &xmsg, ccharacter *hitter=0, ccharacter *enemy=0) const;
  void ProcessAndAddMessage (cfestring &xmsg, ccharacter *hitter=0, ccharacter *enemy=0) const;
  void ProcessAndAddRandomMessage (const fearray<festring> &list, ccharacter *hitter=0, ccharacter *enemy=0) const;

  virtual truth IsHeadOfElpuri() const { return false; }
  virtual truth IsPetrussNut() const { return false; }
  virtual truth IsGoldenEagleShirt() const { return false; }
  virtual truth IsMoneyBag () const { return false; }
  virtual truth IsKleinBottle () const { return false; }
  virtual truth IsTheAvatar() const { return false; }
  virtual truth IsExplosive() const { return false; }
  virtual truth IsConsumable() const { return true; }
  virtual truth IsWhip() const { return false; }
  virtual truth IsBroken() const;
  virtual truth IsFood() const;
  virtual truth IsBanana() const { return false; }
  virtual truth IsEncryptedScroll() const { return false; }
  virtual truth IsShadowVeil () const { return false; }
  virtual truth IsLostRubyFlamingSword () const { return false; }
  virtual truth IsMangoSeedling() const { return false; }
  virtual truth IsMondedrPass () const { return false; }
  virtual truth IsRingOfThieves () const { return false; }
  virtual truth IsGorovitsFamilyRelic() const { return false; }
  virtual truth IsBananaPeel() const { return false; }
  virtual truth IsLanternOnWall() const { return false; }
  virtual truth IsABone () const { return false; }
  virtual truth IsASkull () const { return false; }

  //FIXME:k8: are there such functions already? can we use RTTI?
  virtual truth IsCorpse () const { return false; }
  virtual truth IsBodyPart () const { return false; }
  virtual truth IsBottle () const { return false; }
  virtual truth IsCan () const { return false; }
  virtual truth IsScroll () const { return false; }
  virtual truth IsMango () const { return false; }

  virtual truth AllowDetailedDescription () const { return false; }

  virtual truth IsBoneNameSingular () const override { return (GetNameSingular() == "bone"); }

  inline cchar *GetClassID () const { return (FindProtoType() ? FindProtoType()->GetClassID() : ""); }
  virtual const prototype* FindProtoType () const { return &ProtoType; }

protected:
  virtual cchar* GetBreakVerb() const;
  virtual sLong GetMaterialPrice() const;
  void LoadDataBaseStats();
  virtual void PostConstruct() { }
  void Initialize(int, int);
  virtual int GetGraphicsContainerIndex() const;
  virtual truth ShowMaterial() const;
  virtual truth AllowSparkling() const { return !Fluid; }
  virtual truth WeightIsIrrelevant() const { return false; }
  virtual truth AddStateDescription(festring&, truth) const;
  static const prototype ProtoType;
  slot** Slot;
  int Size;
  feuLong ID;
  const database* DataBase;
  sLong Volume;
  sLong Weight;
  idholder* CloneMotherID;
  fluid** Fluid;
  int FluidCount; // same as 'SquaresUnder'?
  int SquaresUnder;
  int LifeExpectancy;
  feuLong ItemFlags;
  truth mIsStepedOn;

public: // why not?
  feuLong pickupTime;
};

#ifdef __FILE_OF_STATIC_ITEM_PROTOTYPE_DEFINITIONS__
#define ITEM_PROTO(name, base)\
template<> const itemprototype\
  name##sysbase::ProtoType(&base::ProtoType,\
         (itemspawner)(&name##sysbase::Spawn),\
         (itemcloner)(&name##sysbase::Clone), #name);
#else
#define ITEM_PROTO(name, base)
#endif

#define ITEM(name, base)\
class name;\
typedef sysbase<name, base, itemprototype> name##sysbase;\
ITEM_PROTO(name, base)\
class name : public name##sysbase

#endif
