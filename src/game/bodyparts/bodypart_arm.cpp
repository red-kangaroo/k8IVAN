#ifdef HEADER_PHASE
ITEM(arm, bodypart)
{
 public:
  arm() : StrengthBonus(0), DexterityBonus(0) { }
  arm(const arm&);
  virtual ~arm();

  virtual void Save(outputfile&) const;
  virtual void Load(inputfile&);
  virtual int GetTotalResistance(int) const;
  double GetWieldedDamage() const;
  double GetWieldedToHitValue() const;
  void SetWielded(item* What) { WieldedSlot.PutInItem(What); }
  item* GetWielded() const { return *WieldedSlot; }
  void SetGauntlet(item* What) { GauntletSlot.PutInItem(What); }
  item* GetGauntlet() const { return *GauntletSlot; }
  void SetRing(item* What) { RingSlot.PutInItem(What); }
  item* GetRing() const { return *RingSlot; }
  virtual void DropEquipment(stack* = 0);
  double GetUnarmedToHitValue() const;
  double GetUnarmedDamage() const;
  void Hit(character*, v2, int, int = 0);
  int GetAttribute(int, truth = true) const;
  truth EditAttribute(int, int);
  void EditExperience(int, double, double);
  void SetStrength(int What) { StrengthExperience = What * EXP_MULTIPLIER; }
  void SetDexterity(int What) { DexterityExperience = What * EXP_MULTIPLIER; }
  virtual void InitSpecialAttributes();
  virtual void Mutate();
  virtual arm* GetPairArm() const { return 0; }
  sLong GetWieldedAPCost() const;
  sLong GetUnarmedAPCost() const;
  virtual item* GetEquipment(int) const;
  virtual int GetEquipments() const { return 3; }
  int GetBaseUnarmedStrength() const { return BaseUnarmedStrength; }
  void SetBaseUnarmedStrength(sLong What) { BaseUnarmedStrength = What; }
  virtual void CalculateDamage();
  virtual void CalculateToHitValue();
  virtual void CalculateAPCost();
  double GetDamage() const { return Damage; }
  int GetMinDamage() const;
  int GetMaxDamage() const;
  double GetToHitValue() const { return ToHitValue; }
  sLong GetAPCost() const { return APCost; }
  truth PairArmAllowsMelee() const;
  virtual void SignalVolumeAndWeightChange();
  truth TwoHandWieldIsActive() const;
  double GetBlockChance(double) const;
  int GetBlockCapability() const;
  void WieldedSkillHit(int);
  double GetBlockValue() const;
  void ApplyEquipmentAttributeBonuses(item*);
  virtual void CalculateAttributeBonuses();
  int GetWieldedHitStrength() const;
  virtual void SignalEquipmentAdd(gearslot*);
  virtual void SignalEquipmentRemoval(gearslot*, citem*);
  void ApplyDexterityPenalty(item*);
  void ApplyStrengthBonus (item *Item);
  void ApplyDexterityBonus (item *Item);
  virtual truth DamageArmor(character*, int, int);
  truth CheckIfWeaponTooHeavy(cchar*) const;
  virtual truth EditAllAttributes(int);
  void AddAttackInfo(felist&) const;
  void AddDefenceInfo(felist&) const;
  void UpdateWieldedPicture();
  void DrawWielded(blitdata&) const;
  virtual truth IsRightArm() const { return 0; }
  virtual void UpdatePictures();
  virtual double GetTypeDamage(ccharacter*) const;
  virtual item* GetArmorToReceiveFluid(truth) const;
  virtual void CopyAttributes(const bodypart*);
  double GetStrengthExperience() const { return StrengthExperience; }
  double GetDexterityExperience() const { return DexterityExperience; }
  virtual void SignalPossibleUsabilityChange();
  virtual truth IsAnimated() const;
  truth HasSadistWeapon() const;

protected:
  virtual sweaponskill** GetCurrentSWeaponSkill() const { return 0; }
  void UpdateArmArmorPictures(graphicdata&, graphicdata&, int) const;
  int GetCurrentSWeaponSkillBonus() const;
  gearslot WieldedSlot;
  gearslot GauntletSlot;
  gearslot RingSlot;
  double StrengthExperience;
  double DexterityExperience;
  int BaseUnarmedStrength;
  double Damage;
  double ToHitValue;
  sLong APCost;
  int StrengthBonus;
  int DexterityBonus;
  graphicdata WieldedGraphicData;
};


#else



int arm::GetMinDamage() const { return int(Damage * 0.75); }



int arm::GetMaxDamage() const { return int(Damage * 1.25 + 1); }



double arm::GetBlockValue() const { return GetToHitValue() * GetWielded()->GetBlockModifier() / 10000; }



int arm::GetTotalResistance(int Type) const
{
  if(Master)
  {
    int Resistance = GetResistance(Type) + Master->GetGlobalResistance(Type);

    if(GetExternalBodyArmor())
      Resistance += GetExternalBodyArmor()->GetResistance(Type) >> 1;

    if(GetGauntlet())
      Resistance += GetGauntlet()->GetResistance(Type);

    return Resistance;
  }
  else
    return GetResistance(Type);
}



void arm::Save(outputfile& SaveFile) const
{
  bodypart::Save(SaveFile);
  SaveFile << (int)BaseUnarmedStrength;
  SaveFile << StrengthExperience << DexterityExperience;
  SaveFile << WieldedSlot << GauntletSlot << RingSlot;
  SaveFile << WieldedGraphicData;
}



void arm::Load(inputfile& SaveFile)
{
  bodypart::Load(SaveFile);
  SaveFile >> (int&)BaseUnarmedStrength;
  SaveFile >> StrengthExperience >> DexterityExperience;
  SaveFile >> WieldedSlot >> GauntletSlot >> RingSlot;
  SaveFile >> WieldedGraphicData;
}



double arm::GetUnarmedDamage() const
{
  double WeaponStrength = GetBaseUnarmedStrength() * GetBaseUnarmedStrength();
  item* Gauntlet = GetGauntlet();

  if(Gauntlet)
    WeaponStrength += Gauntlet->GetWeaponStrength();

  double Base = sqrt(5e-5 * WeaponStrength);

  if(Gauntlet)
    Base += Gauntlet->GetDamageBonus();

  double Damage = Base * sqrt(1e-7 * GetAttribute(ARM_STRENGTH))
      * GetHumanoidMaster()->GetCWeaponSkill(UNARMED)->GetBonus();

  return Damage;
}



double arm::GetUnarmedToHitValue() const
{
  double BonusMultiplier = 10.;
  item* Gauntlet = GetGauntlet();

  if(Gauntlet)
    BonusMultiplier += Gauntlet->GetTHVBonus();

  return GetAttribute(DEXTERITY)
    * sqrt(2.5 * Master->GetAttribute(PERCEPTION))
    * GetHumanoidMaster()->GetCWeaponSkill(UNARMED)->GetBonus()
    * Master->GetMoveEase()
    * BonusMultiplier / 5000000;
}



sLong arm::GetUnarmedAPCost() const
{
  return sLong(10000000000. / (APBonus(GetAttribute(DEXTERITY)) * Master->GetMoveEase() * Master->GetCWeaponSkill(UNARMED)->GetBonus()));
}



void arm::CalculateDamage()
{
  if(!Master)
    return;

  if(!IsUsable())
    Damage = 0;
  else if(GetWielded())
    Damage = GetWieldedDamage();
  else if(PairArmAllowsMelee())
    Damage = GetUnarmedDamage();
  else
    Damage = 0;
}



void arm::CalculateToHitValue()
{
  if(!Master)
    return;

  if(GetWielded())
    ToHitValue = GetWieldedToHitValue();
  else if(PairArmAllowsMelee())
    ToHitValue = GetUnarmedToHitValue();
  else
    ToHitValue = 0;
}



void arm::CalculateAPCost()
{
  if(!Master)
    return;

  if(GetWielded())
    APCost = GetWieldedAPCost();
  else if(PairArmAllowsMelee())
    APCost = GetUnarmedAPCost();
  else return;

  if(APCost < 100)
    APCost = 100;
}



truth arm::PairArmAllowsMelee() const
{
  const arm* PairArm = GetPairArm();
  return !PairArm || !PairArm->IsUsable() || !PairArm->GetWielded()
    || PairArm->GetWielded()->IsShield(Master);
}



double arm::GetWieldedDamage() const
{
  citem* Wielded = GetWielded();

  if(Wielded->IsShield(Master))
    return 0;

  int HitStrength = GetAttribute(ARM_STRENGTH);
  int Requirement = Wielded->GetStrengthRequirement();

  if(TwoHandWieldIsActive())
  {
    HitStrength += GetPairArm()->GetAttribute(ARM_STRENGTH);
    Requirement >>= 2;
  }

  if(HitStrength > Requirement)
  {
    /* I have no idea whether this works. It needs to be checked */

    return Wielded->GetBaseDamage() * sqrt(1e-13 * HitStrength)
  * GetCurrentSWeaponSkillBonus()
  * GetHumanoidMaster()->GetCWeaponSkill(Wielded->GetWeaponCategory())->GetBonus();
  }
  else
    return 0;
}



double arm::GetWieldedToHitValue() const
{
  int HitStrength = GetWieldedHitStrength();

  if(HitStrength <= 0)
    return 0;

  citem* Wielded = GetWielded();

  double Base = 2e-11
    * Min(HitStrength, 10)
    * GetHumanoidMaster()->GetCWeaponSkill(Wielded->GetWeaponCategory())->GetBonus()
    * GetCurrentSWeaponSkillBonus()
    * Master->GetMoveEase()
    * (10000. / (1000 + Wielded->GetWeight()) + Wielded->GetTHVBonus());
  double ThisToHit = GetAttribute(DEXTERITY) * sqrt(2.5 * Master->GetAttribute(PERCEPTION));
  const arm* PairArm = GetPairArm();

  if(PairArm && PairArm->IsUsable())
  {
    citem* PairWielded = PairArm->GetWielded();

    if(!PairWielded)
    {
      if(Wielded->IsTwoHanded() && !Wielded->IsShield(Master))
  return Base * (ThisToHit + PairArm->GetAttribute(DEXTERITY)
           * sqrt(2.5 * Master->GetAttribute(PERCEPTION))) / 2;
    }
    else if(!Wielded->IsShield(Master) && !PairWielded->IsShield(Master))
      return Base * ThisToHit / (1.0 + (500.0 + PairWielded->GetWeight())
         / (1000.0 + (Wielded->GetWeight() << 1)));
  }

  return Base * ThisToHit;
}



sLong arm::GetWieldedAPCost() const
{
  citem* Wielded = GetWielded();

  if(Wielded->IsShield(Master))
    return 0;

  int HitStrength = GetWieldedHitStrength();

  if(HitStrength <= 0)
    return 0;

  return sLong(1 / (1e-14 * APBonus(GetAttribute(DEXTERITY)) * Master->GetMoveEase() * GetHumanoidMaster()->GetCWeaponSkill(Wielded->GetWeaponCategory())->GetBonus() * (GetCurrentSWeaponSkillBonus() * Min(HitStrength, 10))));
}



void arm::DropEquipment(stack* Stack)
{
  if(Stack)
  {
    if(GetWielded())
      GetWielded()->MoveTo(Stack);

    if(GetGauntlet())
      GetGauntlet()->MoveTo(Stack);

    if(GetRing())
      GetRing()->MoveTo(Stack);
  }
  else
  {
    if(GetWielded())
      GetSlot()->AddFriendItem(GetWielded());

    if(GetGauntlet())
      GetSlot()->AddFriendItem(GetGauntlet());

    if(GetRing())
      GetSlot()->AddFriendItem(GetRing());
  }
}



arm::~arm()
{
  delete GetWielded();
  delete GetGauntlet();
  delete GetRing();
}



void arm::Hit(character* Enemy, v2 HitPos, int Direction, int Flags)
{
  sLong StrExp = 50, DexExp = 50;
  truth THW = false;
  item* Wielded = GetWielded();

  if(Wielded)
  {
    sLong Weight = Wielded->GetWeight();
    StrExp = Limit(15 * Weight / 200, 75, 300);
    DexExp = Weight ? Limit(75000 / Weight, 75, 300) : 300;
    THW = TwoHandWieldIsActive();
  }

  switch(Enemy->TakeHit(Master, Wielded ? Wielded : GetGauntlet(), this, HitPos, GetTypeDamage(Enemy), GetToHitValue(), RAND() % 26 - RAND() % 26, Wielded ? WEAPON_ATTACK : UNARMED_ATTACK, Direction, !(RAND() % Master->GetCriticalModifier()), Flags & SADIST_HIT))
  {
   case HAS_HIT:
   case HAS_BLOCKED:
   case HAS_DIED:
   case DID_NO_DAMAGE:
    EditExperience(ARM_STRENGTH, StrExp, 1 << 9);

    if(THW && GetPairArm())
      GetPairArm()->EditExperience(ARM_STRENGTH, StrExp, 1 << 9);

   case HAS_DODGED:
    EditExperience(DEXTERITY, DexExp, 1 << 9);

    if(THW && GetPairArm())
      GetPairArm()->EditExperience(DEXTERITY, DexExp, 1 << 9);
  }
}



int arm::GetAttribute(int Identifier, truth AllowBonus) const
{
  if(Identifier == ARM_STRENGTH)
  {
    int Base = !UseMaterialAttributes()
         ? int(StrengthExperience * EXP_DIVISOR)
         : GetMainMaterial()->GetStrengthValue();

    if(AllowBonus)
      Base += StrengthBonus;

    return Max(!IsBadlyHurt() || !AllowBonus ? Base : Base / 3, 1);
  }
  else if(Identifier == DEXTERITY)
  {
    int Base = !UseMaterialAttributes()
         ? int(DexterityExperience * EXP_DIVISOR)
         : GetMainMaterial()->GetFlexibility() << 2;

    if(AllowBonus)
      Base += DexterityBonus;

    return Max(IsUsable() || !AllowBonus ? Base : Base / 3, 1);
  }
  else
  {
    ABORT("Illegal arm attribute %d request!", Identifier);
    return 0xACCA;
  }
}



truth arm::EditAttribute(int Identifier, int Value)
{
  if(!Master)
    return false;

  if(Identifier == ARM_STRENGTH)
  {
    if(!UseMaterialAttributes()
       && Master->RawEditAttribute(StrengthExperience, Value))
    {
      Master->CalculateBattleInfo();

      if(Master->IsPlayerKind())
  UpdatePictures();

      return true;
    }
  }
  else if(Identifier == DEXTERITY)
    if(!UseMaterialAttributes()
       && Master->RawEditAttribute(DexterityExperience, Value))
    {
      Master->CalculateBattleInfo();
      return true;
    }

  return false;
}



void arm::EditExperience(int Identifier, double Value, double Speed)
{
  if(!Master)
    return;

  if(Identifier == ARM_STRENGTH)
  {
    if(!UseMaterialAttributes())
    {
      int Change = Master->RawEditExperience(StrengthExperience,
               Master->GetNaturalExperience(ARM_STRENGTH),
               Value, Speed);

      if(Change)
      {
  cchar* Adj = Change > 0 ? "stronger" : "weaker";

  if(Master->IsPlayer())
    ADD_MESSAGE("Your %s feels %s!", GetBodyPartName().CStr(), Adj);
  else if(Master->IsPet() && Master->CanBeSeenByPlayer())
    ADD_MESSAGE("Suddenly %s looks %s.", Master->CHAR_NAME(DEFINITE), Adj);

  Master->CalculateBattleInfo();

  if(Master->IsPlayerKind())
    UpdatePictures();
      }
    }
  }
  else if(Identifier == DEXTERITY)
  {
    if(!UseMaterialAttributes())
    {
      int Change = Master->RawEditExperience(DexterityExperience,
               Master->GetNaturalExperience(DEXTERITY),
               Value, Speed);

      if(Change)
      {
  cchar* Adj = Change > 0 ? "quite dextrous" : "clumsy";

  if(Master->IsPlayer())
    ADD_MESSAGE("Your %s feels %s!", GetBodyPartName().CStr(), Adj);
  else if(Master->IsPet() && Master->CanBeSeenByPlayer())
    ADD_MESSAGE("Suddenly %s looks %s.", Master->CHAR_NAME(DEFINITE), Adj);

  Master->CalculateBattleInfo();
      }
    }
  }
  else
    ABORT("Illegal arm attribute %d experience edit request!", Identifier);
}



void arm::InitSpecialAttributes()
{
  if(!Master->IsHuman() || Master->IsInitializing())
  {
    StrengthExperience = Master->GetNaturalExperience(ARM_STRENGTH);
    DexterityExperience = Master->GetNaturalExperience(DEXTERITY);
  }
  else
  {
    StrengthExperience = game::GetAveragePlayerArmStrengthExperience();
    DexterityExperience = game::GetAveragePlayerDexterityExperience();
  }

  LimitRef(StrengthExperience, MIN_EXP, MAX_EXP);
  LimitRef(DexterityExperience, MIN_EXP, MAX_EXP);
  BaseUnarmedStrength = Master->GetBaseUnarmedStrength();
}



void arm::Mutate()
{
  bodypart::Mutate();
  StrengthExperience = StrengthExperience * (1.5 - (RAND() & 1023) / 1023.);
  DexterityExperience = DexterityExperience * (1.5 - (RAND() & 1023) / 1023.);
  LimitRef(StrengthExperience, MIN_EXP, MAX_EXP);
  LimitRef(DexterityExperience, MIN_EXP, MAX_EXP);
}



item* arm::GetEquipment(int I) const
{
  switch(I)
  {
   case 0: return GetWielded();
   case 1: return GetGauntlet();
   case 2: return GetRing();
  }

  return 0;
}



void arm::SignalVolumeAndWeightChange()
{
  bodypart::SignalVolumeAndWeightChange();

  if(Master && !Master->IsInitializing())
  {
    GetHumanoidMaster()->EnsureCurrentSWeaponSkillIsCorrect(*GetCurrentSWeaponSkill(), GetWielded());
    CalculateAttributeBonuses();
    CalculateAttackInfo();
    UpdateWieldedPicture();

    if(GetSquareUnder())
      GetSquareUnder()->SendNewDrawRequest();
  }
}



truth arm::TwoHandWieldIsActive() const
{
  citem* Wielded = GetWielded();

  if(Wielded->IsTwoHanded() && !Wielded->IsShield(Master))
  {
    arm* PairArm = GetPairArm();
    return PairArm && PairArm->IsUsable() && !PairArm->GetWielded();
  }
  else
    return false;
}



double arm::GetBlockChance(double EnemyToHitValue) const
{
  citem* Wielded = GetWielded();
  return Wielded ? Min(1.0 / (1 + EnemyToHitValue / (GetToHitValue() * Wielded->GetBlockModifier()) * 10000), 1.0) : 0;
}



int arm::GetBlockCapability() const
{
  citem* Wielded = GetWielded();

  if(!Wielded)
    return 0;

  int HitStrength = GetWieldedHitStrength();

  if(HitStrength <= 0)
    return 0;

  return Min(HitStrength, 10) * Wielded->GetStrengthValue() * GetHumanoidMaster()->GetCWeaponSkill(Wielded->GetWeaponCategory())->GetBonus() * (*GetCurrentSWeaponSkill())->GetBonus() / 10000000;
}



void arm::WieldedSkillHit(int Hits)
{
  item* Wielded = GetWielded();

  if(Master->GetCWeaponSkill(Wielded->GetWeaponCategory())->AddHit(Hits))
  {
    CalculateAttackInfo();

    if(Master->IsPlayer())
    {
      int Category = Wielded->GetWeaponCategory();
      GetHumanoidMaster()->GetCWeaponSkill(Category)->AddLevelUpMessage(Category);
    }
  }

  if((*GetCurrentSWeaponSkill())->AddHit(Hits))
  {
    CalculateAttackInfo();

    if(Master->IsPlayer())
      (*GetCurrentSWeaponSkill())->AddLevelUpMessage(Wielded->CHAR_NAME(UNARTICLED));
  }
}



arm::arm(const arm& Arm) : mybase(Arm), StrengthExperience(Arm.StrengthExperience), DexterityExperience(Arm.DexterityExperience), BaseUnarmedStrength(Arm.BaseUnarmedStrength)
{
}



void arm::SignalEquipmentAdd(gearslot* Slot)
{
  int EquipmentIndex = Slot->GetEquipmentIndex();

  if(Master && !Master->IsInitializing())
  {
    item* Equipment = Slot->GetItem();

    if(Equipment->IsInCorrectSlot(EquipmentIndex))
      ApplyEquipmentAttributeBonuses(Equipment);

    if(EquipmentIndex == RIGHT_GAUNTLET_INDEX || EquipmentIndex == LEFT_GAUNTLET_INDEX)
      ApplyDexterityPenalty(Equipment);

    if(EquipmentIndex == RIGHT_WIELDED_INDEX || EquipmentIndex == LEFT_WIELDED_INDEX)
    {
      UpdateWieldedPicture();
      GetSquareUnder()->SendNewDrawRequest();
    }
  }

  if(Master)
    Master->SignalEquipmentAdd(EquipmentIndex);
}



void arm::SignalEquipmentRemoval(gearslot* Slot, citem* Item)
{
  int EquipmentIndex = Slot->GetEquipmentIndex();

  if(Master && !Master->IsInitializing())
    if(EquipmentIndex == RIGHT_WIELDED_INDEX || EquipmentIndex == LEFT_WIELDED_INDEX)
    {
      UpdateWieldedPicture();
      square* Square = GetSquareUnder();

      if(Square)
  Square->SendNewDrawRequest();
    }

  if(Master)
    Master->SignalEquipmentRemoval(EquipmentIndex, Item);
}



void arm::ApplyEquipmentAttributeBonuses(item* Item)
{
  if(Item->AffectsArmStrength())
    StrengthBonus += Item->GetEnchantment();

  if(Item->AffectsDexterity())
    DexterityBonus += Item->GetEnchantment();
}



void arm::CalculateAttributeBonuses()
{
  StrengthBonus = DexterityBonus = 0;

  for(int c = 0; c < GetEquipments(); ++c)
  {
    item* Equipment = GetEquipment(c);

    if(Equipment && Equipment->IsInCorrectSlot())
      ApplyEquipmentAttributeBonuses(Equipment);
  }

  ApplyDexterityPenalty(GetGauntlet());

  if(Master)
  {
    ApplyDexterityPenalty(GetExternalCloak());
    ApplyDexterityPenalty(GetExternalBodyArmor());
    /* */
    ApplyStrengthBonus(GetExternalHelmet());
    ApplyStrengthBonus(GetExternalCloak());
    ApplyStrengthBonus(GetExternalBodyArmor());
    ApplyStrengthBonus(GetExternalBelt());
    ApplyDexterityBonus(GetExternalHelmet());
    ApplyDexterityBonus(GetExternalCloak());
    ApplyDexterityBonus(GetExternalBodyArmor());
    ApplyDexterityBonus(GetExternalBelt());
  }

  if(!UseMaterialAttributes())
  {
    StrengthBonus -= CalculateScarAttributePenalty(GetAttribute(ARM_STRENGTH, false));
    DexterityBonus -= CalculateScarAttributePenalty(GetAttribute(DEXTERITY, false)) ;
  }
}



int arm::GetWieldedHitStrength() const
{
  int HitStrength = GetAttribute(ARM_STRENGTH);
  int Requirement = GetWielded()->GetStrengthRequirement();

  if(TwoHandWieldIsActive())
  {
    HitStrength += GetPairArm()->GetAttribute(ARM_STRENGTH);
    Requirement >>= 2;
  }

  return HitStrength - Requirement;
}



void arm::ApplyDexterityPenalty(item* Item)
{
  if(Item)
    DexterityBonus -= Item->GetInElasticityPenalty(GetAttribute(DEXTERITY, false));
}



truth arm::DamageArmor(character* Damager, int Damage, int Type)
{
  sLong AV[3] = { 0, 0, 0 }, AVSum = 0;
  item* Armor[3];

  if((Armor[0] = GetGauntlet()))
    AVSum += AV[0] = Max(Armor[0]->GetStrengthValue(), 1);

  if((Armor[1] = GetExternalBodyArmor()))
    AVSum += AV[1] = Max(Armor[1]->GetStrengthValue() >> 1, 1);

  if((Armor[2] = GetExternalCloak()))
    AVSum += AV[2] = Max(Armor[2]->GetStrengthValue(), 1);

  return AVSum ? Armor[femath::WeightedRand(AV, AVSum)]->ReceiveDamage(Damager, Damage, Type) : false;
}



truth arm::CheckIfWeaponTooHeavy(cchar* WeaponDescription) const
{
  if(!IsUsable())
  {
    ADD_MESSAGE("%s %s is not usable.", Master->CHAR_POSSESSIVE_PRONOUN, GetBodyPartName().CStr());
    return !game::TruthQuestion(CONST_S("Continue anyway? [y/N]"));
  }

  int HitStrength = GetAttribute(ARM_STRENGTH);
  int Requirement = GetWielded()->GetStrengthRequirement();

  if(TwoHandWieldIsActive())
  {
    HitStrength += GetPairArm()->GetAttribute(ARM_STRENGTH);
    Requirement >>= 2;

    if(HitStrength - Requirement < 10)
    {
      if(HitStrength <= Requirement)
  ADD_MESSAGE("%s cannot use %s. Wielding it with two hands requires %d strength.", Master->CHAR_DESCRIPTION(DEFINITE), WeaponDescription, (Requirement >> 1) + 1);
      else if(HitStrength - Requirement < 4)
  ADD_MESSAGE("Using %s even with two hands is extremely difficult for %s.", WeaponDescription, Master->CHAR_DESCRIPTION(DEFINITE));
      else if(HitStrength - Requirement < 7)
  ADD_MESSAGE("%s %s much trouble using %s even with two hands.", Master->CHAR_DESCRIPTION(DEFINITE), Master->IsPlayer() ? "have" : "has", WeaponDescription);
      else
  ADD_MESSAGE("It is somewhat difficult for %s to use %s even with two hands.", Master->CHAR_DESCRIPTION(DEFINITE), WeaponDescription);

      return !game::TruthQuestion(CONST_S("Continue anyway? [y/N]"));
    }
  }
  else
  {
    if(HitStrength - Requirement < 10)
    {
      festring OtherHandInfo;
      cchar* HandInfo = "";

      if(GetWielded()->IsTwoHanded())
      {
  if(GetPairArm() && !GetPairArm()->IsUsable())
    OtherHandInfo = Master->GetPossessivePronoun() + " other arm is unusable. ";

  HandInfo = " with one hand";
      }

      if(HitStrength <= Requirement)
  ADD_MESSAGE("%s%s cannot use %s. Wielding it%s requires %d strength.", OtherHandInfo.CStr(), Master->GetDescription(DEFINITE).CapitalizeCopy().CStr(), WeaponDescription, HandInfo, Requirement + 1);
      else if(HitStrength - Requirement < 4)
  ADD_MESSAGE("%sUsing %s%s is extremely difficult for %s.", OtherHandInfo.CStr(), WeaponDescription, HandInfo, Master->CHAR_DESCRIPTION(DEFINITE));
      else if(HitStrength - Requirement < 7)
  ADD_MESSAGE("%s%s %s much trouble using %s%s.", OtherHandInfo.CStr(), Master->GetDescription(DEFINITE).CapitalizeCopy().CStr(), Master->IsPlayer() ? "have" : "has", WeaponDescription, HandInfo);
      else
  ADD_MESSAGE("%sIt is somewhat difficult for %s to use %s%s.", OtherHandInfo.CStr(), Master->CHAR_DESCRIPTION(DEFINITE), WeaponDescription, HandInfo);

      return !game::TruthQuestion(CONST_S("Continue anyway? [y/N]"));
    }
  }

  return false;
}



truth arm::EditAllAttributes(int Amount)
{
  LimitRef(StrengthExperience += Amount * EXP_MULTIPLIER, MIN_EXP, MAX_EXP);
  LimitRef(DexterityExperience += Amount * EXP_MULTIPLIER, MIN_EXP, MAX_EXP);
  return (Amount < 0
    && (StrengthExperience != MIN_EXP || DexterityExperience != MIN_EXP))
    || (Amount > 0
  && (StrengthExperience != MAX_EXP || DexterityExperience != MAX_EXP));
}



#ifdef WIZARD
void arm::AddAttackInfo(felist& List) const
{
  if(GetDamage())
  {
    festring Entry = CONST_S("   ");

    if(GetWielded())
    {
      GetWielded()->AddName(Entry, UNARTICLED);

      if(TwoHandWieldIsActive())
  Entry << " (b)";
    }
    else
      Entry << "melee attack";

    Entry.Resize(50);
    Entry << GetMinDamage() << '-' << GetMaxDamage();
    Entry.Resize(60);
    Entry << int(GetToHitValue());
    Entry.Resize(70);
    Entry << GetAPCost();
    List.AddEntry(Entry, LIGHT_GRAY);
  }
}



void arm::AddDefenceInfo(felist& List) const
{
  if(GetWielded())
  {
    festring Entry = CONST_S("   ");
    GetWielded()->AddName(Entry, UNARTICLED);
    Entry.Resize(50);
    Entry << int(GetBlockValue());
    Entry.Resize(70);
    Entry << GetBlockCapability();
    List.AddEntry(Entry, LIGHT_GRAY);
  }
}

#else

void arm::AddAttackInfo(felist&) const { }
void arm::AddDefenceInfo(felist&) const { }
#endif


void arm::UpdateWieldedPicture()
{
  if(!Master || !Master->PictureUpdatesAreForbidden())
  {
    truth WasAnimated = MasterIsAnimated();
    item* Wielded = GetWielded();

    if(Wielded && Master)
    {
      int SpecialFlags = (IsRightArm() ? 0 : MIRROR)|ST_WIELDED|(Wielded->GetSpecialFlags()&~0x3F);
      Wielded->UpdatePictures(WieldedGraphicData,
            Master->GetWieldedPosition(),
            SpecialFlags,
            GetMaxAlpha(),
            GR_HUMANOID,
            static_cast<bposretriever>(&item::GetWieldedBitmapPos));

      if(ShowFluids())
  Wielded->CheckFluidGearPictures(Wielded->GetWieldedBitmapPos(0), SpecialFlags, false);
    }
    else
      WieldedGraphicData.Retire();

    if(!WasAnimated != !MasterIsAnimated())
      SignalAnimationStateChange(WasAnimated);
  }
}



void arm::DrawWielded(blitdata& BlitData) const
{
  DrawEquipment(WieldedGraphicData, BlitData);

  if(ShowFluids() && GetWielded())
    GetWielded()->DrawFluidGearPictures(BlitData, IsRightArm() ? 0 : MIRROR);
}



void arm::UpdatePictures()
{
  bodypart::UpdatePictures();
  UpdateWieldedPicture();
}



double arm::GetTypeDamage(ccharacter* Enemy) const
{
  if(!GetWielded() || !GetWielded()->IsGoodWithPlants() || !Enemy->IsPlant())
    return Damage;
  else
    return Damage * 1.5;
}



item* arm::GetArmorToReceiveFluid(truth) const
{
  item* Cloak = GetExternalCloak();

  if(Cloak && !(RAND() % 3))
    return Cloak;

  item* Wielded = GetWielded();

  if(Wielded && !(RAND() % 3))
    return Wielded;

  item* Gauntlet = GetGauntlet();

  if(Gauntlet && RAND() & 1)
    return Gauntlet;

  item* BodyArmor = GetExternalBodyArmor();
  return BodyArmor ? BodyArmor : 0;
}



void arm::UpdateArmArmorPictures(graphicdata& ArmArmorGraphicData, graphicdata& GauntletGraphicData, int SpecialFlags) const
{
  if(!Master || !Master->PictureUpdatesAreForbidden())
  {
    UpdateArmorPicture(ArmArmorGraphicData,
           Master ? GetExternalBodyArmor() : 0,
           SpecialFlags,
           GetAttribute(ARM_STRENGTH, false) >= 20 ? &item::GetAthleteArmArmorBitmapPos : &item::GetArmArmorBitmapPos,
           true);
    UpdateArmorPicture(GauntletGraphicData,
           GetGauntlet(),
           SpecialFlags,
           &item::GetGauntletBitmapPos);
  }
}



void arm::CopyAttributes(const bodypart* BodyPart)
{
  const arm* Arm = static_cast<const arm*>(BodyPart);
  StrengthExperience = Arm->StrengthExperience;
  DexterityExperience = Arm->DexterityExperience;
}



void arm::SignalPossibleUsabilityChange()
{
  feuLong OldFlags = Flags;
  UpdateFlags();

  if(Flags != OldFlags && !Master->IsInitializing())
    Master->CalculateBattleInfo();
}



truth arm::IsAnimated() const
{
  return WieldedGraphicData.AnimationFrames > 1;
}



int arm::GetCurrentSWeaponSkillBonus() const
{
  return (*GetCurrentSWeaponSkill()
      ? (*GetCurrentSWeaponSkill())->GetBonus() : 1);
}



truth arm::HasSadistWeapon() const
{
  item* Wielded = GetWielded();
  return Wielded && Wielded->IsSadistWeapon();
}



void arm::ApplyStrengthBonus (item *Item) {
  if (Item && Item->AffectsArmStrength()) StrengthBonus += Item->GetEnchantment()/2;
}



void arm::ApplyDexterityBonus (item *Item) {
  if (Item && Item->AffectsDexterity()) DexterityBonus += Item->GetEnchantment()/2;
}
#endif
