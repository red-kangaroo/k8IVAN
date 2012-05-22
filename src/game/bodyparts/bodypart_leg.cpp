#ifdef HEADER_PHASE
ITEM(leg, bodypart)
{
 public:
  leg() : StrengthBonus(0), AgilityBonus(0) { }
  leg(const leg&);
  virtual ~leg();

  virtual void Save(outputfile&) const;
  virtual void Load(inputfile&);
  virtual int GetTotalResistance(int) const;
  void SetBoot(item* What) { BootSlot.PutInItem(What); }
  item* GetBoot() const { return *BootSlot; }
  virtual void DropEquipment(stack* = 0);
  double GetKickToHitValue() const { return KickToHitValue; }
  double GetKickDamage() const { return KickDamage; }
  int GetKickMinDamage() const;
  int GetKickMaxDamage() const;
  int GetAttribute(int, truth = true) const;
  truth EditAttribute(int, int);
  void EditExperience(int, double, double);
  virtual void InitSpecialAttributes();
  virtual void Mutate();
  sLong GetKickAPCost() const { return KickAPCost; }
  virtual item* GetEquipment(int) const;
  virtual int GetEquipments() const { return 1; }
  sLong GetBaseKickStrength() const { return BaseKickStrength; }
  void SetBaseKickStrength(sLong What) { BaseKickStrength = What; }
  virtual void CalculateDamage();
  virtual void CalculateToHitValue();
  virtual void CalculateAPCost();
  void ApplyEquipmentAttributeBonuses(item*);
  virtual void CalculateAttributeBonuses();
  virtual void SignalEquipmentAdd(gearslot*);
  void ApplyAgilityPenalty(item*);
  void ApplyStrengthBonus (item *Item);
  void ApplyAgilityBonus (item *Item);
  virtual void SignalVolumeAndWeightChange();
  virtual truth DamageArmor(character*, int, int);
  virtual truth EditAllAttributes(int);
  void AddAttackInfo(felist&) const;
  virtual item* GetArmorToReceiveFluid(truth) const;
  virtual void CopyAttributes(const bodypart*);
  double GetStrengthExperience() const { return StrengthExperience; }
  double GetAgilityExperience() const { return AgilityExperience; }
  virtual void SignalPossibleUsabilityChange();

protected:
  void UpdateLegArmorPictures(graphicdata&, graphicdata&, int) const;
  gearslot BootSlot;
  double StrengthExperience;
  double AgilityExperience;
  int BaseKickStrength;
  double KickDamage;
  double KickToHitValue;
  sLong KickAPCost;
  int StrengthBonus;
  int AgilityBonus;
};


#else



int leg::GetKickMinDamage() const { return int(KickDamage * 0.75); }



int leg::GetKickMaxDamage() const { return int(KickDamage * 1.25 + 1); }



int leg::GetTotalResistance(int Type) const
{
  if(Master)
  {
    int Resistance = GetResistance(Type) + Master->GetGlobalResistance(Type);

    if(GetExternalBodyArmor())
      Resistance += GetExternalBodyArmor()->GetResistance(Type) >> 1;

    if(GetBoot())
      Resistance += GetBoot()->GetResistance(Type);

    return Resistance;
  }
  else
    return GetResistance(Type);
}



void leg::Save(outputfile& SaveFile) const
{
  bodypart::Save(SaveFile);
  SaveFile << BaseKickStrength << StrengthExperience << AgilityExperience;
  SaveFile << BootSlot;
}



void leg::Load(inputfile& SaveFile)
{
  bodypart::Load(SaveFile);
  SaveFile >> BaseKickStrength >> StrengthExperience >> AgilityExperience;
  SaveFile >> BootSlot;
}



void leg::CalculateDamage()
{
  if(!Master)
    return;

  double WeaponStrength = GetBaseKickStrength() * GetBaseKickStrength();
  item* Boot = GetBoot();

  if(Boot)
    WeaponStrength += Boot->GetWeaponStrength();

  double Base = sqrt(5e-5 * WeaponStrength);

  if(Boot)
    Base += Boot->GetDamageBonus();

  KickDamage = Base * sqrt(1e-7 * GetAttribute(LEG_STRENGTH))
         * GetHumanoidMaster()->GetCWeaponSkill(KICK)->GetBonus();
}



void leg::CalculateToHitValue()
{
  if(!Master)
    return;

  double BonusMultiplier = 10.;
  item* Boot = GetBoot();

  if(Boot)
    BonusMultiplier += Boot->GetTHVBonus();

  KickToHitValue = GetAttribute(AGILITY)
       * sqrt(2.5 * Master->GetAttribute(PERCEPTION))
       * GetHumanoidMaster()->GetCWeaponSkill(KICK)->GetBonus()
       * Master->GetMoveEase()
       * BonusMultiplier / 10000000;
}



void leg::CalculateAPCost()
{
  if(!Master)
    return;

  KickAPCost = Max(sLong(20000000000. / (APBonus(GetAttribute(AGILITY)) * Master->GetMoveEase() * Master->GetCWeaponSkill(KICK)->GetBonus())), 100);
}



void leg::DropEquipment(stack* Stack)
{
  if(Stack)
  {
    if(GetBoot())
      GetBoot()->MoveTo(Stack);
  }
  else
  {
    if(GetBoot())
      GetSlot()->AddFriendItem(GetBoot());
  }
}



leg::~leg()
{
  delete GetBoot();
}



int leg::GetAttribute(int Identifier, truth AllowBonus) const
{
  if(Identifier == LEG_STRENGTH)
  {
    int Base = !UseMaterialAttributes()
         ? int(StrengthExperience * EXP_DIVISOR)
         : GetMainMaterial()->GetStrengthValue();

    if(AllowBonus)
      Base += StrengthBonus;

    return Max(!IsBadlyHurt() || !AllowBonus ? Base : Base / 3, 1);
  }
  else if(Identifier == AGILITY)
  {
    int Base = !UseMaterialAttributes()
         ? int(AgilityExperience * EXP_DIVISOR)
         : GetMainMaterial()->GetFlexibility() << 2;

    if(AllowBonus)
      Base += AgilityBonus;

    return Max(IsUsable() || !AllowBonus ? Base : Base / 3, 1);
  }
  else
  {
    ABORT("Illegal leg attribute %d request!", Identifier);
    return 0xECCE;
  }
}



truth leg::EditAttribute(int Identifier, int Value)
{
  if(!Master)
    return false;

  if(Identifier == LEG_STRENGTH)
  {
    if(!UseMaterialAttributes()
       && Master->RawEditAttribute(StrengthExperience, Value))
    {
      Master->CalculateBurdenState();
      Master->CalculateBattleInfo();
      return true;
    }
  }
  else if(Identifier == AGILITY)
    if(!UseMaterialAttributes()
       && Master->RawEditAttribute(AgilityExperience, Value))
    {
      Master->CalculateBattleInfo();
      return true;
    }

  return false;
}



void leg::EditExperience(int Identifier, double Value, double Speed)
{
  if(!Master)
    return;

  if(Identifier == LEG_STRENGTH)
  {
    if(!UseMaterialAttributes())
    {
      int Change = Master->RawEditExperience(StrengthExperience,
               Master->GetNaturalExperience(LEG_STRENGTH),
               Value, Speed);

      if(Change)
      {
  cchar* Adj = Change > 0 ? "stronger" : "weaker";

  if(Master->IsPlayer())
    ADD_MESSAGE("Your %s feels %s!", GetBodyPartName().CStr(), Adj);
  else if(Master->IsPet() && Master->CanBeSeenByPlayer())
    ADD_MESSAGE("Suddenly %s looks %s.", Master->CHAR_NAME(DEFINITE), Adj);

  Master->CalculateBurdenState();
  Master->CalculateBattleInfo();
      }
    }
  }
  else if(Identifier == AGILITY)
  {
    if(!UseMaterialAttributes())
    {
      int Change = Master->RawEditExperience(AgilityExperience,
               Master->GetNaturalExperience(AGILITY),
               Value, Speed);

      if(Change)
      {
  cchar* Adj = Change > 0 ? "very agile" : "slower";

  if(Master->IsPlayer())
    ADD_MESSAGE("Your %s feels %s!", GetBodyPartName().CStr(), Adj);
  else if(Master->IsPet() && Master->CanBeSeenByPlayer())
    ADD_MESSAGE("Suddenly %s looks %s.", Master->CHAR_NAME(DEFINITE), Adj);

  Master->CalculateBattleInfo();
      }
    }
  }
  else
    ABORT("Illegal leg attribute %d experience edit request!", Identifier);
}



void leg::InitSpecialAttributes()
{
  if(!Master->IsHuman() || Master->IsInitializing())
  {
    StrengthExperience = Master->GetNaturalExperience(LEG_STRENGTH);
    AgilityExperience = Master->GetNaturalExperience(AGILITY);
  }
  else
  {
    StrengthExperience = game::GetAveragePlayerLegStrengthExperience();
    AgilityExperience = game::GetAveragePlayerAgilityExperience();
  }

  LimitRef(StrengthExperience, MIN_EXP, MAX_EXP);
  LimitRef(AgilityExperience, MIN_EXP, MAX_EXP);
  BaseKickStrength = Master->GetBaseKickStrength();
}



void leg::Mutate()
{
  bodypart::Mutate();
  StrengthExperience = StrengthExperience * (1.5 - (RAND() & 1023) / 1023.);
  AgilityExperience = AgilityExperience * (1.5 - (RAND() & 1023) / 1023.);
  LimitRef(StrengthExperience, MIN_EXP, MAX_EXP);
  LimitRef(AgilityExperience, MIN_EXP, MAX_EXP);
}



item* leg::GetEquipment(int I) const
{
  return !I ? GetBoot() : 0;
}



void leg::SignalVolumeAndWeightChange()
{
  bodypart::SignalVolumeAndWeightChange();

  if(Master && !Master->IsInitializing())
  {
    CalculateAttributeBonuses();
    CalculateAttackInfo();
  }
}



leg::leg(const leg& Leg) : mybase(Leg), StrengthExperience(Leg.StrengthExperience), AgilityExperience(Leg.AgilityExperience), BaseKickStrength(Leg.BaseKickStrength)
{
}



void leg::SignalEquipmentAdd(gearslot* Slot)
{
  int EquipmentIndex = Slot->GetEquipmentIndex();

  if(Master && !Master->IsInitializing())
  {
    item* Equipment = Slot->GetItem();

    if(Equipment->IsInCorrectSlot(EquipmentIndex))
      ApplyEquipmentAttributeBonuses(Equipment);

    if(EquipmentIndex == RIGHT_BOOT_INDEX || EquipmentIndex == LEFT_BOOT_INDEX)
      ApplyAgilityPenalty(Equipment);
  }

  if(Master)
    Master->SignalEquipmentAdd(EquipmentIndex);
}



void leg::ApplyEquipmentAttributeBonuses(item* Item)
{
  if(Item->AffectsLegStrength())
  {
    StrengthBonus += Item->GetEnchantment();

    if(Master)
      Master->CalculateBurdenState();
  }

  if(Item->AffectsAgility())
    AgilityBonus += Item->GetEnchantment();
}



void leg::CalculateAttributeBonuses()
{
  StrengthBonus = AgilityBonus = 0;

  for(int c = 0; c < GetEquipments(); ++c)
  {
    item* Equipment = GetEquipment(c);

    if(Equipment && Equipment->IsInCorrectSlot())
      ApplyEquipmentAttributeBonuses(Equipment);
  }

  ApplyAgilityPenalty(GetBoot());

  if(Master)
  {
    ApplyAgilityPenalty(GetExternalCloak());
    ApplyAgilityPenalty(GetExternalBodyArmor());
    /* */
    ApplyStrengthBonus(GetExternalHelmet());
    ApplyStrengthBonus(GetExternalCloak());
    ApplyStrengthBonus(GetExternalBodyArmor());
    ApplyStrengthBonus(GetExternalBelt());
    ApplyAgilityBonus(GetExternalHelmet());
    ApplyAgilityBonus(GetExternalCloak());
    ApplyAgilityBonus(GetExternalBodyArmor());
    ApplyAgilityBonus(GetExternalBelt());
  }

  if(!UseMaterialAttributes())
  {
    StrengthBonus -= CalculateScarAttributePenalty(GetAttribute(LEG_STRENGTH, false));
    AgilityBonus -= CalculateScarAttributePenalty(GetAttribute(AGILITY, false)) ;
  }
}



void leg::ApplyAgilityPenalty(item* Item)
{
  if(Item)
    AgilityBonus -= Item->GetInElasticityPenalty(GetAttribute(AGILITY, false));
}



truth leg::DamageArmor(character* Damager, int Damage, int Type)
{
  sLong AV[3] = { 0, 0, 0 }, AVSum = 0;
  item* Armor[3];

  if((Armor[0] = GetBoot()))
    AVSum += AV[0] = Max(Armor[0]->GetStrengthValue(), 1);

  if((Armor[1] = GetExternalBodyArmor()))
    AVSum += AV[1] = Max(Armor[1]->GetStrengthValue() >> 1, 1);

  if((Armor[2] = GetExternalCloak()))
    AVSum += AV[2] = Max(Armor[2]->GetStrengthValue(), 1);

  return AVSum ? Armor[femath::WeightedRand(AV, AVSum)]->ReceiveDamage(Damager, Damage, Type) : false;
}



truth leg::EditAllAttributes(int Amount)
{
  LimitRef(StrengthExperience += Amount * EXP_MULTIPLIER, MIN_EXP, MAX_EXP);
  LimitRef(AgilityExperience += Amount * EXP_MULTIPLIER, MIN_EXP, MAX_EXP);
  return (Amount < 0
    && (StrengthExperience != MIN_EXP || AgilityExperience != MIN_EXP))
    || (Amount > 0
  && (StrengthExperience != MAX_EXP || AgilityExperience != MAX_EXP));
}



void leg::AddAttackInfo(felist& List) const
{
  festring Entry = CONST_S("   kick attack");
  Entry.Resize(50);
  Entry << GetKickMinDamage() << '-' << GetKickMaxDamage();
  Entry.Resize(60);
  Entry << int(GetKickToHitValue());
  Entry.Resize(70);
  Entry << GetKickAPCost();
  List.AddEntry(Entry, LIGHT_GRAY);
}



item* leg::GetArmorToReceiveFluid(truth SteppedOn) const
{
  if(SteppedOn)
  {
    item* Boot = GetBoot();
    return Boot ? Boot : 0;
  }
  else
  {
    item* Cloak = GetExternalCloak();

    if(Cloak && !(RAND() % 3))
      return Cloak;

    item* Boot = GetBoot();

    if(Boot && RAND() & 1)
      return Boot;

    item* BodyArmor = GetExternalBodyArmor();
    return BodyArmor ? BodyArmor : 0;
  }
}



void leg::UpdateLegArmorPictures(graphicdata& LegArmorGraphicData, graphicdata& BootGraphicData, int SpecialFlags) const
{
  if(!Master || !Master->PictureUpdatesAreForbidden())
  {
    UpdateArmorPicture(LegArmorGraphicData,
           Master ? GetExternalBodyArmor() : 0,
           SpecialFlags,
           &item::GetLegArmorBitmapPos,
           true);
    UpdateArmorPicture(BootGraphicData,
           GetBoot(),
           SpecialFlags,
           &item::GetBootBitmapPos);
  }
}



void leg::CopyAttributes(const bodypart* BodyPart)
{
  const leg* Leg = static_cast<const leg*>(BodyPart);
  StrengthExperience = Leg->StrengthExperience;
  AgilityExperience = Leg->AgilityExperience;
}



void leg::SignalPossibleUsabilityChange()
{
  feuLong OldFlags = Flags;
  UpdateFlags();

  if(Flags != OldFlags && !Master->IsInitializing())
    Master->CalculateBattleInfo();
}



void leg::ApplyStrengthBonus (item *Item) {
  if (Item && Item->AffectsLegStrength()) StrengthBonus += Item->GetEnchantment()/2;
}



void leg::ApplyAgilityBonus (item *Item) {
  if (Item && Item->AffectsAgility()) AgilityBonus += Item->GetEnchantment()/2;
}
#endif
