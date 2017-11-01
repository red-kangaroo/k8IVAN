#ifdef HEADER_PHASE
ITEM(bodypart, item)
{
 public:
  friend class corpse;
  bodypart() : Master(0) { }
  bodypart(const bodypart&);
  virtual ~bodypart();

  virtual void Save(outputfile&) const;
  virtual void Load(inputfile&);
  virtual int GetGraphicsContainerIndex() const;
  character* GetMaster() const { return Master; }
  humanoid* GetHumanoidMaster() const;
  void SetMaster(character* What) { Master = What; }
  virtual int GetStrengthValue() const;
  int GetMaxHP() const { return MaxHP; }
  void SetHP(int);
  int GetHP() const { return HP; }
  void EditHP(int, int);
  void IncreaseHP();
  virtual int GetTotalResistance(int) const { return 0; }
  virtual truth ReceiveDamage(character*, int, int, int);
  cfestring& GetOwnerDescription() const { return OwnerDescription; }
  void SetOwnerDescription(cfestring& What) { OwnerDescription = What; }
  truth IsUnique() const { return Flags & UNIQUE; }
  void SetIsUnique(truth);
  virtual void DropEquipment(stack* = 0) { }
  virtual void InitSpecialAttributes() { }
  virtual void SignalEquipmentAdd(gearslot*);
  virtual void SignalEquipmentRemoval(gearslot*, citem*);
  virtual void Mutate();
  sLong GetBodyPartVolume() const { return BodyPartVolume; }
  sLong GetCarriedWeight() const { return CarriedWeight; }
  virtual item* GetEquipment(int) const { return 0; }
  virtual int GetEquipments() const { return 0; }
  virtual void CalculateVolumeAndWeight();
  virtual void CalculateEmitation();
  void CalculateMaxHP(feuLong = MAY_CHANGE_HPS|CHECK_USABILITY);
  virtual void SignalVolumeAndWeightChange();
  void FastRestoreHP();
  void RestoreHP();
  virtual void CalculateDamage() { }
  virtual void CalculateToHitValue() { }
  virtual void CalculateAPCost() { }
  void CalculateAttackInfo();
  double GetTimeToDie(int, double, double, truth, truth) const;
  virtual double GetRoughChanceToHit(double, double) const;
  cfestring& GetBodyPartName() const { return GetNameSingular(); }
  void RandomizePosition();
  void ResetPosition() { SpecialFlags &= ~0x7; }
  virtual void SignalSpoil(material*);
  virtual truth CanBePiledWith(citem*, ccharacter*) const;
  truth IsAlive() const;
  void SpillBlood(int);
  void SpillBlood(int, v2);
  virtual void Be();
  int GetConditionColorIndex() const;
  void SetBitmapPos(v2 What) { BitmapPos = What; }
  void SetSpecialFlags(int What) { SpecialFlags = What; }
  void SetWobbleData(int What) { WobbleData = What; }
  void SetMaterialColorB(col16 What) { ColorB = What; }
  void SetMaterialColorC(col16 What) { ColorC = What; }
  void SetMaterialColorD(col16 What) { ColorD = What; }
  virtual void SignalEnchantmentChange();
  virtual void CalculateAttributeBonuses() { }
  virtual void SignalSpoilLevelChange(material*);
  virtual truth CanBeEatenByAI(ccharacter*) const;
  virtual truth DamageArmor(character*, int, int) { return false; }
  truth CanBeSevered(int) const;
  virtual truth EditAllAttributes(int) { return false; }
  virtual void Draw(blitdata&) const;
  void SetSparkleFlags(int);
  virtual int GetSpecialFlags() const;
  virtual truth IsRepairable(ccharacter*) const;
  truth IsWarm() const;
  truth IsWarmBlooded() const;
  truth UseMaterialAttributes() const;
  truth CanRegenerate() const;
  truth CanHaveParasite() const;
  virtual square* GetSquareUnder(int = 0) const;
  virtual lsquare* GetLSquareUnder(int = 0) const;
  virtual item* GetArmorToReceiveFluid(truth) const { return 0; }
  virtual void SpillFluid(character*, liquid*, int = 0);
  void StayOn(liquid*);
  void SetBloodMaterial(int What) { BloodMaterial = What; }
  int GetBloodMaterial() const { return BloodMaterial; }
  liquid* CreateBlood(sLong) const;
  virtual truth UpdateArmorPictures() { return false; }
  virtual void DrawArmor(blitdata&) const { }
  virtual void UpdatePictures();
  item *GetExternalBodyArmor () const;
  item *GetExternalCloak () const;
  item *GetExternalHelmet () const;
  item *GetExternalBelt () const;
  virtual void ReceiveAcid(material*, cfestring&, sLong);
  virtual truth ShowFluids() const { return false; }
  virtual void TryToRust(sLong);
  virtual truth AllowFluidBe() const;
  virtual material* RemoveMaterial(material*);
  virtual void CopyAttributes(const bodypart*) { }
  virtual void DestroyBodyPart(stack*);
  virtual void SetLifeExpectancy(int, int);
  virtual void SpecialEatEffect(character*, int);
  virtual character* GetBodyPartMaster() const { return Master; }
  virtual truth AllowFluids() const { return true; }
  truth IsBadlyHurt() const { return Flags & BADLY_HURT; }
  truth IsStuck() const { return Flags & STUCK; }
  truth IsUsable() const { return !(Flags & (BADLY_HURT|STUCK)); }
  virtual void SignalPossibleUsabilityChange() { UpdateFlags(); }
  void SetIsInfectedByLeprosy(truth);
  virtual int GetSparkleFlags() const;
  virtual truth MaterialIsChangeable(ccharacter*) const;
  virtual void RemoveRust();
  virtual item* Fix();
  virtual sLong GetFixPrice() const;
  virtual truth IsFixableBySmith(ccharacter*) const;
  virtual truth IsFixableByTailor(ccharacter*) const;
  virtual void SignalMaterialChange();
  void SetNormalMaterial(int What) { NormalMaterial = What; }
  virtual truth IsBroken() const { return HP < MaxHP; }
  virtual truth IsDestroyable(ccharacter*) const;
  void DrawScars(cblitdata&) const;
  static truth DamageTypeCanScar(int);
  void GenerateScar(int, int);
  int CalculateScarAttributePenalty(int) const;
  virtual truth IsBodyPart () const { return true; }

protected:
  virtual alpha GetMaxAlpha() const;
  virtual void GenerateMaterials() { }
  virtual void AddPostFix(festring&, int) const;
  virtual truth ShowMaterial() const;
  virtual int GetArticleMode() const;
  virtual col16 GetMaterialColorA(int) const;
  virtual col16 GetMaterialColorB(int) const { return ColorB; }
  virtual col16 GetMaterialColorC(int) const { return ColorC; }
  virtual col16 GetMaterialColorD(int) const { return ColorD; }
  virtual v2 GetBitmapPos(int) const { return BitmapPos; }
  virtual int GetWobbleData() const { return WobbleData; }
  void UpdateArmorPicture(graphicdata&, item*, int, v2 (item::*)(int) const, truth = false) const;
  void DrawEquipment(const graphicdata&, blitdata&) const;
  void UpdateFlags();
  truth MasterIsAnimated() const;
  void SignalAnimationStateChange(truth);
  virtual truth AddAdjective(festring&, truth) const;
  void RemoveDamageIDs(int);
  void AddDamageID(int, int);
  festring OwnerDescription;
  character* Master;
  sLong CarriedWeight;
  sLong BodyPartVolume;
  packv2 BitmapPos;
  packcol16 ColorB;
  packcol16 ColorC;
  packcol16 ColorD;
  uShort SpecialFlags;
  short HP;
  short MaxHP;
  short BloodMaterial;
  short NormalMaterial;
  uChar SpillBloodCounter;
  uChar WobbleData;
  std::vector<scar> Scar;
  std::deque<damageid> DamageID;
};


#else



int bodypart::GetGraphicsContainerIndex() const { return GR_HUMANOID; }
int bodypart::GetArticleMode() const { return IsUnique() ? FORCE_THE : 0; }
truth bodypart::IsAlive() const { return MainMaterial->GetBodyFlags() & IS_ALIVE; }
int bodypart::GetSpecialFlags() const { return SpecialFlags|ST_OTHER_BODYPART; }
col16 bodypart::GetMaterialColorA(int) const { return GetMainMaterial()->GetSkinColor(); }
truth bodypart::IsWarm() const { return MainMaterial->GetBodyFlags() & IS_WARM; }
truth bodypart::IsWarmBlooded() const { return MainMaterial->GetBodyFlags() & IS_WARM_BLOODED; }
truth bodypart::UseMaterialAttributes() const { return MainMaterial->GetBodyFlags() & USE_MATERIAL_ATTRIBUTES || !Master || Master->AlwaysUseMaterialAttributes(); }
truth bodypart::CanRegenerate() const { return MainMaterial->GetBodyFlags() & CAN_REGENERATE; }
truth bodypart::CanHaveParasite() const { return MainMaterial->GetBodyFlags() & CAN_HAVE_PARASITE; }
square* bodypart::GetSquareUnder(int I) const { return Master ? Slot[0]->GetSquareUnder(I) : Slot[I]->GetSquareUnder(); }
lsquare* bodypart::GetLSquareUnder(int I) const { return static_cast<lsquare*>(Master ? Slot[0]->GetSquareUnder(I) : Slot[I]->GetSquareUnder()); }
item* bodypart::GetExternalBodyArmor() const { return GetHumanoidMaster()->GetBodyArmor(); }
item* bodypart::GetExternalCloak() const { return GetHumanoidMaster()->GetCloak(); }
truth bodypart::AllowFluidBe() const { return !Master || !Master->IsPolymorphed(); }
item *bodypart::GetExternalHelmet () const { return GetHumanoidMaster()->GetHelmet(); }
item *bodypart::GetExternalBelt () const { return GetHumanoidMaster()->GetBelt(); }


void bodypart::Save(outputfile& SaveFile) const
{
  item::Save(SaveFile);
  SaveFile << BitmapPos << ColorB << ColorC << ColorD << SpecialFlags << WobbleData << HP;
  SaveFile << OwnerDescription << BloodMaterial << NormalMaterial << Scar << DamageID;
}


void bodypart::Load(inputfile& SaveFile)
{
  item::Load(SaveFile);
  SaveFile >> BitmapPos >> ColorB >> ColorC >> ColorD >> SpecialFlags >> WobbleData >> HP;
  SaveFile >> OwnerDescription >> BloodMaterial >> NormalMaterial >> Scar >> DamageID;
}


int bodypart::GetStrengthValue() const
{
  if(!UseMaterialAttributes())
    return sLong(GetStrengthModifier()) * Master->GetAttribute(ENDURANCE) / 2000;
  else
    return sLong(GetStrengthModifier()) * GetMainMaterial()->GetStrengthValue() / 2000;
}


truth bodypart::ReceiveDamage(character* Damager, int Damage, int Type, int)
{
  if(Master)
  {
    if(Type & POISON && !IsAlive())
      return false;

    int BHP = HP;

    if(HP <= Damage && !CanBeSevered(Type))
      Damage = GetHP() - 1;

    if(!Damage)
      return false;

    EditHP(1, -Damage);

    if(Type & DRAIN && IsAlive())
      for(int c = 0; c < Damage; ++c)
        Damager->HealHitPoint();

    truth WasBadlyHurt = IsBadlyHurt();

    if(HP <= 0)
      return true;

    if(DamageTypeCanScar(Type) && !(RAND_N(25 + 25 * HP / MaxHP)))
      GenerateScar(Damage, Type);

    if (Master->IsPlayer()) {
      if (HP == 1 && BHP > 1) {
        if (IsAlive()) ADD_MESSAGE("Your %s bleeds very badly.", GetBodyPartName().CStr());
        else ADD_MESSAGE("Your %s is in very bad condition.", GetBodyPartName().CStr());
        if (Master->BodyPartIsVital(GetBodyPartIndex())) game::AskForEscPress(CONST_S("Vital bodypart in serious danger!"));
      } else if (IsBadlyHurt() && !WasBadlyHurt) {
        if (IsAlive()) ADD_MESSAGE("Your %s bleeds.", GetBodyPartName().CStr());
        else ADD_MESSAGE("Your %s is in bad condition.", GetBodyPartName().CStr());
        if (Master->BodyPartIsVital(GetBodyPartIndex())) game::AskForEscPress(CONST_S("Vital bodypart in danger!"));
      }
    }
    SignalPossibleUsabilityChange();
  }

  return false;
}


truth bodypart::CanBeSevered(int Type) const
{
  if((HP == MaxHP && HP != 1 && !Master->IsExtraFragile())
     || (Type & (POISON|SOUND) && GetBodyPartIndex() != TORSO_INDEX))
    return false;

  if(!Master->BodyPartIsVital(GetBodyPartIndex()) || Master->IsExtraFragile())
    return true;

  bodypart* Torso = Master->GetTorso();
  return Torso->HP != Torso->MaxHP || Torso->HP == 1;
}


humanoid* bodypart::GetHumanoidMaster() const
{
  return static_cast<humanoid*>(Master);
}


int bodypart::GetSparkleFlags() const
{
  return (GetMainMaterial()->SkinColorIsSparkling() ? SPARKLING_A : 0)
    | (Flags >> BODYPART_SPARKLE_SHIFT & (SPARKLING_B|SPARKLING_C|SPARKLING_D));
}


void bodypart::SignalEquipmentAdd(gearslot* Slot)
{
  if(Master)
    Master->SignalEquipmentAdd(Slot->GetEquipmentIndex());
}


void bodypart::SignalEquipmentRemoval(gearslot* Slot, citem* Item)
{
  if(Master)
    Master->SignalEquipmentRemoval(Slot->GetEquipmentIndex(), Item);
}


void bodypart::Mutate()
{
  GetMainMaterial()->SetVolume(sLong(GetVolume() * (1.5 - (RAND() & 1023) / 1023.)));
}


alpha bodypart::GetMaxAlpha() const
{
  if(Master && Master->StateIsActivated(INVISIBLE))
    return 150;
  else
    return 255;
}


void bodypart::AddPostFix(festring& String, int) const
{
  if(!OwnerDescription.IsEmpty())
    String << ' ' << OwnerDescription;
}


void bodypart::CalculateVolumeAndWeight()
{
  item::CalculateVolumeAndWeight();
  CarriedWeight = 0;
  BodyPartVolume = Volume;

  for(int c = 0; c < GetEquipments(); ++c)
  {
    item* Equipment = GetEquipment(c);

    if(Equipment)
    {
      Volume += Equipment->GetVolume();
      CarriedWeight += Equipment->GetWeight();
    }
  }

  Weight += CarriedWeight;
}


void bodypart::CalculateEmitation()
{
  item::CalculateEmitation();

  for(int c = 0; c < GetEquipments(); ++c)
  {
    item* Equipment = GetEquipment(c);

    if(Equipment)
      game::CombineLights(Emitation, Equipment->GetEmitation());
  }
}


void bodypart::CalculateMaxHP(feuLong Flags)
{
  int HPDelta = MaxHP - HP/*k8, OldMaxHP = MaxHP*/;
  MaxHP = 0;

  if(Master)
  {
    if(!UseMaterialAttributes())
    {
      sLong Endurance = Master->GetAttribute(ENDURANCE);
      double DoubleHP = GetBodyPartVolume() * Endurance * Endurance / 200000;

      for(unsigned int c = 0; c < Scar.size(); ++c)
        DoubleHP *= (100. - Scar[c].Severity * 4) / 100;

      MaxHP = int(DoubleHP);
    }
    else
    {
      sLong SV = GetMainMaterial()->GetStrengthValue();
      MaxHP = (GetBodyPartVolume() * SV >> 4) * SV / 250000;
    }

    if(MaxHP < 1)
      MaxHP = 1;

    if(Flags & MAY_CHANGE_HPS)
    {
      if(MaxHP - HPDelta > 1)
        HP = MaxHP - HPDelta;
      else
        HP = 1;
    }
    else
    {
      //OldMaxHP - MaxHP;
    }

    if(Flags & CHECK_USABILITY)
      SignalPossibleUsabilityChange();
  }
}


void bodypart::SignalVolumeAndWeightChange()
{
  item::SignalVolumeAndWeightChange();

  if(Master && !Master->IsInitializing())
  {
    CalculateMaxHP();
    Master->CalculateHP();
    Master->CalculateMaxHP();
    Master->SignalBodyPartVolumeAndWeightChange();
    square* SquareUnder = GetSquareUnder();

    if(UpdateArmorPictures() && SquareUnder)
      SquareUnder->SendNewDrawRequest();
  }
}


void bodypart::SetHP(int What)
{
  HP = What;

  if(Master)
  {
    Master->CalculateHP();
    SignalPossibleUsabilityChange();
  }
}


void bodypart::EditHP(int SrcID, int What)
{
  HP += What;

  if(What < 0)
    RemoveDamageIDs(-What);
  else
    AddDamageID(SrcID, What);

  if(Master)
  {
    Master->CalculateHP();
    SignalPossibleUsabilityChange();
  }
}


void bodypart::CalculateAttackInfo()
{
  CalculateDamage();
  CalculateToHitValue();
  CalculateAPCost();
}


double bodypart::GetTimeToDie(int Damage, double ToHitValue, double DodgeValue, truth AttackIsBlockable, truth UseMaxHP) const
{
  double Durability;
  int TotalResistance = GetTotalResistance(PHYSICAL_DAMAGE);
  int Damage3 = (Damage << 1) + Damage;
  int Damage5 = (Damage << 2) + Damage;
  int TrueDamage = (19 * (Max((Damage3 >> 2) - TotalResistance, 0)
        +  Max((Damage5 >> 2) + 1 - (TotalResistance >> 1), 0))
        + (Max(((Damage3 + (Damage3 >> 1)) >> 2) - TotalResistance, 0)
           +  Max(((Damage5 + (Damage5 >> 1)) >> 2) + 3 - (TotalResistance >> 1), 0))) / 40;

  int HP = UseMaxHP ? GetMaxHP() : GetHP();

  if(TrueDamage > 0)
  {
    double AverageDamage;

    if(AttackIsBlockable)
    {
      blockvector Block;
      Master->CreateBlockPossibilityVector(Block, ToHitValue);

      if(Block.size())
      {
        double ChanceForNoBlock = 1.0;
        AverageDamage = 0;

        for(uInt c = 0; c < Block.size(); ++c)
        {
          ChanceForNoBlock -= Block[c].first;

          if(TrueDamage - Block[c].second > 0)
            AverageDamage += Block[c].first * (TrueDamage - Block[c].second);
        }

        AverageDamage += ChanceForNoBlock * TrueDamage;
      }
      else
        AverageDamage = TrueDamage;
    }
    else
      AverageDamage = TrueDamage;

    Durability = HP / (AverageDamage * GetRoughChanceToHit(ToHitValue, DodgeValue));

    if(Durability < 1)
      Durability = 1;

    if(Durability > 1000)
      Durability = 1000;
  }
  else
    Durability = 1000;

  return Durability;
}


double bodypart::GetRoughChanceToHit(double ToHitValue, double DodgeValue) const
{
  return GLOBAL_WEAK_BODYPART_HIT_MODIFIER * ToHitValue * GetBodyPartVolume() / ((DodgeValue / ToHitValue + 1) * DodgeValue * Master->GetBodyVolume() * 100);
}


void bodypart::RandomizePosition()
{
  SpecialFlags |= 1 + RAND() % 7;
  UpdatePictures();
}


void bodypart::SignalSpoil(material* Material)
{
  if(Master)
    Master->SignalSpoil();
  else
    item::SignalSpoil(Material);
}


truth bodypart::CanBePiledWith(citem* Item, ccharacter* Viewer) const
{
  return item::CanBePiledWith(Item, Viewer)
    && OwnerDescription == static_cast<const bodypart*>(Item)->OwnerDescription;
}


void bodypart::Be()
{
  if(Master)
  {
    if(HP < MaxHP && ++SpillBloodCounter >= 4)
    {
      if(Master->IsEnabled())
      {
        if(IsBadlyHurt() && !Master->IsPolymorphed() && !(RAND() & 3))
          SpillBlood(1);
      }
      else if(!Master->IsPolymorphed() && !(RAND() & 3))
      {
        SpillBlood(1);
        HP += Max(((MaxHP - HP) >> 2), 1);
      }

      SpillBloodCounter = 0;
    }

    if(Master->AllowSpoil() || !Master->IsEnabled())
      MainMaterial->Be(ItemFlags);

    if (Exists() && LifeExpectancy) {
      if (LifeExpectancy == 1) Master->SignalDisappearance(); else --LifeExpectancy;
    }
  }
  else
  {
    if(HP < MaxHP && ++SpillBloodCounter >= 4)
    {
      if(!(RAND() & 3))
      {
        SpillBlood(1);
        HP += Max(((MaxHP - HP) >> 2), 1);
      }

      SpillBloodCounter = 0;
    }

    item::Be();
  }
}


void bodypart::SpillBlood(int HowMuch, v2 Pos)
{
  if(HowMuch && (!Master || Master->SpillsBlood()) && (IsAlive() || MainMaterial->IsLiquid()) && !game::IsInWilderness())
    GetNearLSquare(Pos)->SpillFluid(0, CreateBlood(sLong(HowMuch * sqrt(BodyPartVolume) / 2)), false, false);
}


void bodypart::SpillBlood(int HowMuch)
{
  if(HowMuch && (!Master || Master->SpillsBlood()) && (IsAlive() || MainMaterial->IsLiquid()) && !game::IsInWilderness())
    for(int c = 0; c < GetSquaresUnder(); ++c)
      if(GetLSquareUnder(c))
  GetLSquareUnder(c)->SpillFluid(0, CreateBlood(sLong(HowMuch * sqrt(BodyPartVolume) / 2)), false, false);
}


void bodypart::SignalEnchantmentChange()
{
  if(Master && !Master->IsInitializing())
  {
    Master->CalculateAttributeBonuses();
    Master->CalculateBattleInfo();
  }
}


void bodypart::SignalSpoilLevelChange(material* Material)
{
  if(Master)
    Master->SignalSpoilLevelChange();
  else
    item::SignalSpoilLevelChange(Material);
}


truth bodypart::CanBeEatenByAI(ccharacter* Who) const
{
  return item::CanBeEatenByAI(Who) && !(Who->IsPet() && PLAYER->HasHadBodyPart(this));
}


int bodypart::GetConditionColorIndex() const
{
  if(HP <= 1 && MaxHP > 1)
    return 0;
  else if((HP << 1) + HP < MaxHP)
    return 1;
  else if((HP << 1) + HP < MaxHP << 1)
    return 2;
  else if(HP < MaxHP)
    return 3;
  else
    return 4;
}


void bodypart::Draw(blitdata& BlitData) const
{
  cint AF = GraphicData.AnimationFrames;
  cint F = !(BlitData.CustomData & ALLOW_ANIMATE) || AF == 1 ? 0 : GET_TICK() & (AF - 1);
  cbitmap* P = GraphicData.Picture[F];

  if(BlitData.CustomData & ALLOW_ALPHA)
    P->AlphaPriorityBlit(BlitData);
  else
    P->MaskedPriorityBlit(BlitData);

  if(Fluid && ShowFluids())
    DrawFluids(BlitData);

  DrawArmor(BlitData);
}


truth bodypart::IsRepairable(ccharacter*) const
{
  return !CanRegenerate() && (GetHP() < GetMaxHP() || IsRusted());
}


void bodypart::SpillFluid(character* Spiller, liquid *Liquid, int SquareIndex) {
  if (!Liquid || Liquid->GetVolume() < 1) {
    //fprintf(stderr, "!!!!!!!!!!! (00)\n");
    //delete Liquid; //k8: this is BUG!
    if (Liquid) Liquid->SendToHell();
    return;
  }
  if (Master) {
    item *Armor = GetArmorToReceiveFluid(false);
    if (Armor) {
      //fprintf(stderr, "bodypart::SpillFluid: Liquid->GetName(0):<%s>\n", Liquid->GetName(false, false).CStr());
      Armor->SpillFluid(Spiller, Liquid);
      //fprintf(stderr, "bodypart::SpillFluid: Liquid->GetName(1):<%s>\n", Liquid->GetName(false, false).CStr());
    } else if (GetMaster()) {
      if (Liquid->GetVolume()) {
        //fprintf(stderr, "bodypart::SpillFluid: Liquid->GetName(2):<%s>\n", Liquid->GetName(false, false).CStr());
        AddFluid(Liquid, "", SquareIndex, false);
        //fprintf(stderr, "bodypart::SpillFluid: Liquid->GetName(3):<%s>\n", Liquid->GetName(false, false).CStr());
      } else {
        //fprintf(stderr, "!!!!!!!!!!! (01)\n");
        //delete Liquid; //k8: this is BUG!
        Liquid->SendToHell();
      }
    }
  } else {
    item::SpillFluid(Spiller, Liquid, SquareIndex);
  }
}


void bodypart::StayOn(liquid* Liquid)
{
  item* Armor = GetArmorToReceiveFluid(true);

  if(Armor)
    Liquid->TouchEffect(Armor, CONST_S(""));
  else if(GetMaster())
    Liquid->TouchEffect(GetMaster(), GetBodyPartIndex());
}


liquid* bodypart::CreateBlood(sLong Volume) const
{
  return liquid::Spawn(GetBloodMaterial(), Volume);
}


void bodypart::UpdateArmorPicture(graphicdata& GData, item* Armor, int SpecialFlags, v2 (item::*Retriever)(int) const, truth BodyArmor) const
{
  if(Armor && Master)
  {
    Armor->UpdatePictures(GData,
        ZERO_V2,
        SpecialFlags|Armor->GetSpecialFlags(),
        GetMaxAlpha(),
        GR_HUMANOID,
        static_cast<bposretriever>(Retriever));
    Armor->CheckFluidGearPictures((Armor->*Retriever)(0), SpecialFlags, BodyArmor);
  }
  else
    GData.Retire();
}


void bodypart::DrawEquipment(const graphicdata& GraphicData, blitdata& BlitData) const
{
  int EAF = GraphicData.AnimationFrames;

  if(EAF)
  {
    int F = !(BlitData.CustomData & ALLOW_ANIMATE) || EAF == 1 ? 0 : GET_TICK() & (EAF - 1);
    GraphicData.Picture[F]->AlphaPriorityBlit(BlitData);
  }
}


truth bodypart::MasterIsAnimated() const
{
  return Master && !Master->IsInitializing() && Master->IsAnimated();
}


void bodypart::UpdatePictures()
{
  truth WasAnimated = MasterIsAnimated();

  item::UpdatePictures();
  UpdateArmorPictures();

  if(!WasAnimated != !MasterIsAnimated())
    SignalAnimationStateChange(WasAnimated);
}


void bodypart::ReceiveAcid (material *Material, cfestring &LocationName, sLong Modifier) {
  if (Master && (MainMaterial->GetInteractionFlags()&CAN_DISSOLVE)) {
    sLong Tries = Modifier/1000;
    Modifier -= Tries*1000; //opt%?
    int Damage = 0;
    for (sLong c = 0; c < Tries; ++c) if (!(RAND() % 100)) ++Damage;
    if (Modifier && !(RAND()%100000/Modifier)) ++Damage;
    if (Damage) {
      feuLong Minute = game::GetTotalMinutes();
      character *Master = this->Master;
      if (Master->GetLastAcidMsgMin() != Minute && (Master->CanBeSeenByPlayer() || Master->IsPlayer())) {
        Master->SetLastAcidMsgMin(Minute);
        cfestring MName = Material->GetName(false, false);
        if (Master->IsPlayer()) {
          cfestring TName = (LocationName.IsEmpty() ? GetBodyPartName() : LocationName);
          ADD_MESSAGE("Acidous %s dissolves your %s.", MName.CStr(), TName.CStr());
        } else {
          ADD_MESSAGE("Acidous %s dissolves %s.", MName.CStr(), Master->CHAR_NAME(DEFINITE));
        }
      }
      Master->ReceiveBodyPartDamage(0, Damage, ACID, GetBodyPartIndex(), YOURSELF, false, false, false);
      feuLong DeathFlags = Material->IsStuckTo(Master) ? IGNORE_TRAPS : 0;
      Master->CheckDeath(CONST_S("dissolved by ")+Material->GetName(), 0, DeathFlags);
    }
  }
}


void bodypart::TryToRust(sLong LiquidModifier)
{
  if(MainMaterial->TryToRust(LiquidModifier << 4))
  {
    cchar* MoreMsg = (MainMaterial->GetRustLevel() == NOT_RUSTED ? "" : " more");

    if(Master)
    {
      if(Master->IsPlayer())
        ADD_MESSAGE("Your %s rusts%s.", CHAR_NAME(UNARTICLED), MoreMsg);
      else if(CanBeSeenByPlayer())
        ADD_MESSAGE("The %s of %s rusts%s.", CHAR_NAME(UNARTICLED), Master->CHAR_NAME(DEFINITE), MoreMsg);
    }
    else if(CanBeSeenByPlayer())
      ADD_MESSAGE("%s rusts%s.", CHAR_NAME(DEFINITE), MoreMsg);

    MainMaterial->SetRustLevel(MainMaterial->GetRustLevel() + 1);
  }
}


material* bodypart::RemoveMaterial(material* Material)
{
  if(Master && GetBodyPartIndex() == TORSO_INDEX)
    return Master->GetMotherEntity()->RemoveMaterial(Material); // gum
  else
    return item::RemoveMaterial(Material);
}


void bodypart::DestroyBodyPart(stack* Stack)
{
  int Lumps = 1 + RAND() % 3;
  sLong LumpVolume = Volume / Lumps >> 2;

  if(LumpVolume >= 10)
    for(int c = 0; c < Lumps; ++c)
    {
      item* Lump = GetMainMaterial()->CreateNaturalForm(LumpVolume + RAND() % LumpVolume);
      Stack->AddItem(Lump);
    }

  SendToHell();
}


void bodypart::SetLifeExpectancy(int Base, int RandPlus)
{
  LifeExpectancy = RandPlus > 1 ? Base + RAND_N(RandPlus) : Base;

  if(!Master)
    Enable();
}


void bodypart::SpecialEatEffect(character* Eater, int Amount)
{
  Amount >>= 6;

  if(Amount && (!Master || Master->SpillsBlood()) && (IsAlive() || MainMaterial->IsLiquid()) && !game::IsInWilderness())
  {
    if(Eater->GetVirtualHead())
      Eater->GetVirtualHead()->SpillFluid(Eater, CreateBlood(Amount));

    Eater->GetTorso()->SpillFluid(Eater, CreateBlood(Amount));
  }
}


void bodypart::UpdateFlags()
{
  if((HP << 1) + HP < MaxHP || (HP == 1 && MaxHP != 1))
    Flags |= BADLY_HURT;
  else
    Flags &= ~BADLY_HURT;

  if(Master->BodyPartIsStuck(GetBodyPartIndex()))
    Flags |= STUCK;
  else
    Flags &= ~STUCK;
}


void bodypart::IncreaseHP()
{
  ++HP;
  RemoveDamageIDs(1);
  SignalPossibleUsabilityChange();
}


void bodypart::FastRestoreHP()
{
  HP = MaxHP;
  DamageID.clear();
  SignalPossibleUsabilityChange();
}


void bodypart::RestoreHP()
{
  HP = MaxHP;
  DamageID.clear();
  SignalPossibleUsabilityChange();
  Master->CalculateHP();
}


void bodypart::SetIsUnique(truth What)
{
  if(What)
    Flags |= UNIQUE;
  else
    Flags &=  ~UNIQUE;
}


void bodypart::SetIsInfectedByLeprosy(truth What)
{
  MainMaterial->SetIsInfectedByLeprosy(What);
}


void bodypart::SetSparkleFlags(int What)
{
  cint S = SPARKLING_B|SPARKLING_C|SPARKLING_D;
  Flags = (Flags & ~(S << BODYPART_SPARKLE_SHIFT)) | ((What & S) << BODYPART_SPARKLE_SHIFT);
}


void bodypart::SignalAnimationStateChange(truth WasAnimated)
{
  if(WasAnimated)
  {
    for(int c = 0; c < GetSquaresUnder(); ++c)
    {
      square* Square = GetSquareUnder(c);

      if(Square)
        Square->DecAnimatedEntities();
    }
  }
  else
  {
    for(int c = 0; c < GetSquaresUnder(); ++c)
    {
      square* Square = GetSquareUnder(c);

      if(Square)
        Square->IncAnimatedEntities();
    }
  }
}


truth bodypart::MaterialIsChangeable(ccharacter*) const
{
  return !Master || !Master->BodyPartIsVital(GetBodyPartIndex()) || UseMaterialAttributes();
}


truth bodypart::AddAdjective(festring& String, truth Articled) const
{
  if(!Master)
  {
    if(Articled)
      String << "a ";

    String << "severed ";
    return true;
  }
  else
    return false;
}


void bodypart::RemoveRust()
{
  item::RemoveRust();
  RestoreHP();
}


sLong bodypart::GetFixPrice() const
{
  return GetMaxHP() - GetHP() + GetMainMaterial()->GetRustLevel() * 25;
}


truth bodypart::IsFixableBySmith(ccharacter*) const
{
  return (GetMainMaterial()->GetCategoryFlags() & IS_METAL
    && (GetHP() < GetMaxHP() || IsRusted()));
}


truth bodypart::IsFixableByTailor(ccharacter*) const
{
  return (GetMainMaterial()->GetCategoryFlags() & CAN_BE_TAILORED
    && GetHP() < GetMaxHP());
}


item* bodypart::Fix()
{
  RestoreHP();
  return this;
}


void bodypart::SignalMaterialChange()
{
  if(Master)
    RestoreHP();
}


truth bodypart::ShowMaterial() const
{
  return MainMaterial->GetConfig() != NormalMaterial;
}


truth bodypart::IsDestroyable(ccharacter*) const
{
  return !Master || !Master->BodyPartIsVital(GetBodyPartIndex());
}


truth bodypart::DamageTypeCanScar(int Type)
{
  return !(Type == POISON || Type == DRAIN);
}


void bodypart::GenerateScar(int Damage, int Type)
{
  Scar.push_back(scar());
  scar& NewScar = Scar.back();
  NewScar.Severity = 1 + RAND_N(1 + 5 * Damage / GetMaxHP());

  if(GetMaster()->IsPlayer())
  {
    int ScarColor = MakeShadeColor(GetMainMaterial()->GetColor());
    NewScar.PanelBitmap = igraph::GenerateScarBitmap(GetBodyPartIndex(),
                 NewScar.Severity,
                 ScarColor);
    ADD_MESSAGE("Your %s is scarred.", CHAR_NAME(UNARTICLED));
  }
  else
    NewScar.PanelBitmap = 0;

  CalculateMaxHP();
  GetMaster()->CalculateMaxHP();
  GetMaster()->CalculateAttributeBonuses();
  CalculateAttackInfo();
}


void bodypart::DrawScars(cblitdata& B) const
{
  for(unsigned int c = 0; c < Scar.size(); ++c)
  {
    if(!Scar[c].PanelBitmap)
    {
      int ScarColor = MakeShadeColor(GetMainMaterial()->GetColor());
      Scar[c].PanelBitmap = igraph::GenerateScarBitmap(GetBodyPartIndex(),
                   Scar[c].Severity,
                   ScarColor);
    }

    Scar[c].PanelBitmap->NormalMaskedBlit(B);
  }
}


int bodypart::CalculateScarAttributePenalty(int Attribute) const
{
  double DoubleAttribute = Attribute;

  for(unsigned int c = 0; c < Scar.size(); ++c)
    DoubleAttribute *= (100. - Scar[c].Severity * 4) / 100;

  return Min(Attribute - int(DoubleAttribute), Attribute - 1);
}


bodypart::~bodypart()
{
  for(unsigned int c = 0; c < Scar.size(); ++c)
    delete Scar[c].PanelBitmap;
}


bodypart::bodypart(const bodypart& B) : mybase(B), OwnerDescription(B.OwnerDescription), Master(B.Master), CarriedWeight(B.CarriedWeight), BodyPartVolume(B.BodyPartVolume), BitmapPos(B.BitmapPos), ColorB(B.ColorB), ColorC(B.ColorC), ColorD(B.ColorD), SpecialFlags(B.SpecialFlags), HP(B.HP), MaxHP(B.MaxHP), BloodMaterial(B.BloodMaterial), NormalMaterial(B.NormalMaterial), SpillBloodCounter(B.SpillBloodCounter), WobbleData(B.WobbleData), Scar(B.Scar)
{
  for(unsigned int c = 0; c < Scar.size(); ++c)
    if(Scar[c].PanelBitmap)
      Scar[c].PanelBitmap = new bitmap(Scar[c].PanelBitmap);
}


void bodypart::RemoveDamageIDs(int Amount)
{
  /*while(Amount)
  {
    damageid& D = DamageID.front();
    int CurrentAmount = D.Amount;

    if(Amount < CurrentAmount)
    {
      D.Amount -= Amount;
      Amount = 0;
    }
    else
    {
      DamageID.pop_front();
      Amount -= CurrentAmount;
    }
  }*/
}


void bodypart::AddDamageID(int SrcID, int Amount)
{
  /*damageid D = { SrcID, Amount };
  DamageID.push_back(D);*/
}

#endif
