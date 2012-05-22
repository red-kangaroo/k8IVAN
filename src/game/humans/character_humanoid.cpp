#ifdef HEADER_PHASE
CHARACTER(humanoid, character)
{
public:
  friend class rightarm;
  friend class leftarm;
public:
  humanoid () : CurrentRightSWeaponSkill(0), CurrentLeftSWeaponSkill(0) { }
  humanoid (const humanoid &);
  virtual ~humanoid ();

  virtual truth CanWield () const;
  virtual truth Hit (character *, v2, int, int = 0);
  virtual int GetSize () const;
  head *GetHead () const { return static_cast<head*>(*BodyPartSlot[HEAD_INDEX]); }
  arm *GetRightArm () const { return static_cast<arm*>(*BodyPartSlot[RIGHT_ARM_INDEX]); }
  arm *GetLeftArm () const { return static_cast<arm*>(*BodyPartSlot[LEFT_ARM_INDEX]); }
  groin *GetGroin () const { return static_cast<groin*>(*BodyPartSlot[GROIN_INDEX]); }
  leg *GetRightLeg () const { return static_cast<leg*>(*BodyPartSlot[RIGHT_LEG_INDEX]); }
  leg *GetLeftLeg () const { return static_cast<leg*>(*BodyPartSlot[LEFT_LEG_INDEX]); }
  item *GetHelmet () const { return GetHead() ? GetHead()->GetHelmet() : 0; }
  item *GetAmulet () const { return GetHead() ? GetHead()->GetAmulet() : 0; }
  item *GetCloak () const { return GetHumanoidTorso() ? GetHumanoidTorso()->GetCloak() : 0; }
  item *GetBodyArmor () const { return GetHumanoidTorso() ? GetHumanoidTorso()->GetBodyArmor() : 0; }
  item *GetBelt () const { return GetHumanoidTorso() ? GetHumanoidTorso()->GetBelt() : 0; }
  item *GetRightWielded () const { return GetRightArm() ? GetRightArm()->GetWielded() : 0; }
  item *GetLeftWielded () const { return GetLeftArm() ? GetLeftArm()->GetWielded() : 0; }
  item *GetRightRing () const { return GetRightArm() ? GetRightArm()->GetRing() : 0; }
  item *GetLeftRing () const { return GetLeftArm() ? GetLeftArm()->GetRing() : 0; }
  item *GetRightGauntlet () const { return GetRightArm() ? GetRightArm()->GetGauntlet() : 0; }
  item *GetLeftGauntlet () const { return GetLeftArm() ? GetLeftArm()->GetGauntlet() : 0; }
  item *GetRightBoot () const { return GetRightLeg() ? GetRightLeg()->GetBoot() : 0; }
  item *GetLeftBoot () const { return GetLeftLeg() ? GetLeftLeg()->GetBoot() : 0; }
  void SetHelmet (item *What) { GetHead()->SetHelmet(What); }
  void SetAmulet (item *What) { GetHead()->SetAmulet(What); }
  void SetCloak (item *What) { GetHumanoidTorso()->SetCloak(What); }
  void SetBodyArmor (item *What) { GetHumanoidTorso()->SetBodyArmor(What); }
  void SetBelt (item *What) { GetHumanoidTorso()->SetBelt(What); }
  void SetRightWielded (item *What) { GetRightArm()->SetWielded(What); }
  void SetLeftWielded (item *What) { GetLeftArm()->SetWielded(What); }
  void SetRightRing (item *What) { GetRightArm()->SetRing(What); }
  void SetLeftRing (item *What) { GetLeftArm()->SetRing(What); }
  void SetRightGauntlet (item *What) { GetRightArm()->SetGauntlet(What); }
  void SetLeftGauntlet (item *What) { GetLeftArm()->SetGauntlet(What); }
  void SetRightBoot (item *What) { GetRightLeg()->SetBoot(What); }
  void SetLeftBoot (item *What) { GetLeftLeg()->SetBoot(What); }
  arm *GetMainArm () const;
  arm *GetSecondaryArm () const;
  virtual truth ReceiveDamage(character*, int, int, int = ALL, int = 8, truth = false, truth = false, truth = false, truth = true);
  virtual truth BodyPartIsVital(int) const;
  virtual truth BodyPartCanBeSevered(int) const;
  virtual item* GetMainWielded() const;
  virtual item* GetSecondaryWielded() const;
  virtual cchar* GetEquipmentName(int) const;
  virtual bodypart* GetBodyPartOfEquipment(int) const;
  virtual item* GetEquipment(int) const;
  virtual int GetEquipments() const { return 13; }
  virtual void SwitchToDig(item*, v2);
  virtual int GetUsableLegs() const;
  virtual int GetUsableArms() const;
  virtual truth CheckKick() const;
  virtual int OpenMultiplier() const;
  virtual int CloseMultiplier() const;
  virtual truth CheckThrow() const;
  virtual truth CheckThrowItemOpportunity();
  virtual truth CheckAIZapOpportunity();
  virtual truth CheckOffer() const;
  virtual sorter EquipmentSorter(int) const;
  virtual void SetEquipment(int, item*);
  virtual void DrawSilhouette(truth) const;
  virtual int GetGlobalResistance(int) const;
  virtual truth TryToRiseFromTheDead();
  virtual truth HandleNoBodyPart(int);
  virtual void Kick(lsquare*, int, truth = false);
  virtual double GetTimeToKill(ccharacter*, truth) const;
  virtual int GetAttribute(int, truth = true) const;
  virtual truth EditAttribute(int, int);
  virtual void EditExperience(int, double, double);
  virtual int DrawStats(truth) const;
  virtual void Bite(character*, v2, int, truth = false);
  virtual int GetCarryingStrength() const;
  virtual int GetRandomStepperBodyPart() const;
  virtual int CheckForBlock(character*, item*, double, int, int, int);
  virtual truth AddSpecialSkillInfo(felist&) const;
  virtual truth CheckBalance(double);
  virtual sLong GetMoveAPRequirement(int) const;
  virtual v2 GetEquipmentPanelPos(int) const;
  virtual truth EquipmentEasilyRecognized(int) const;
  sweaponskill* GetCurrentRightSWeaponSkill() const { return CurrentRightSWeaponSkill; }
  void SetCurrentRightSWeaponSkill(sweaponskill* What) { CurrentRightSWeaponSkill = What; }
  sweaponskill* GetCurrentLeftSWeaponSkill() const { return CurrentLeftSWeaponSkill; }
  void SetCurrentLeftSWeaponSkill(sweaponskill* What) { CurrentLeftSWeaponSkill = What; }
  virtual void SWeaponSkillTick();
  virtual void Save(outputfile&) const;
  virtual void Load(inputfile&);
  virtual void SignalEquipmentAdd(int);
  virtual void SignalEquipmentRemoval(int, citem*);
  virtual void DrawBodyParts(blitdata&) const;
  virtual truth CanUseStethoscope(truth) const;
  virtual truth IsUsingArms() const;
  virtual truth IsUsingLegs() const;
  virtual truth IsUsingHead() const;
  virtual void CalculateBattleInfo();
  virtual void CalculateBodyParts();
  virtual void CalculateAllowedWeaponSkillCategories();
  virtual truth HasALeg() const { return HasAUsableLeg(); }
  virtual void AddSpecialEquipmentInfo(festring&, int) const;
  virtual void CreateInitialEquipment(int);
  virtual festring GetBodyPartName(int, truth = false) const;
  virtual void CreateBlockPossibilityVector(blockvector&, double) const;
  virtual item* SevereBodyPart(int, truth = false, stack* = 0);
  virtual int GetSWeaponSkillLevel(citem*) const;
  virtual truth UseMaterialAttributes() const;
  virtual void CalculateDodgeValue();
  virtual truth CheckZap();
  virtual truth IsHumanoid() const { return true; }
  virtual truth CheckTalk();
  virtual truth CheckIfEquipmentIsNotUsable(int) const;
  virtual void AddSpecialStethoscopeInfo(felist&) const;
  virtual item* GetPairEquipment(int) const;
  virtual truth HasHead() const { return truth(GetHead()); }
  virtual cfestring& GetStandVerb() const;
  virtual head* Behead();
  virtual void AddAttributeInfo(festring&) const;
  virtual void AddAttackInfo(felist&) const;
  virtual void AddDefenceInfo(felist&) const;
  virtual void DetachBodyPart();
  virtual int GetRandomApplyBodyPart() const;
  void EnsureCurrentSWeaponSkillIsCorrect(sweaponskill*&, citem*);
  virtual int GetSumOfAttributes() const;
  virtual truth CheckConsume(cfestring&) const;
  virtual truth CanConsume(material*) const;
  virtual truth PreProcessForBone();
  virtual void FinalProcessForBone();
  virtual void StayOn(liquid*);
  virtual head* GetVirtualHead() const { return GetHead(); }
  virtual character* CreateZombie() const;
  virtual void LeprosyHandler();
  virtual void DropRandomNonVitalBodypart();
  virtual void DropBodyPart(int);
  virtual void DuplicateEquipment(character*, feuLong);
  virtual int GetAttributeAverage() const;
  virtual truth CanVomit() const;
  virtual truth CheckApply() const;
  virtual truth CanForceVomit() const { return TorsoIsAlive() && HasAUsableArm(); }
  virtual truth IsTransparent() const;
  virtual void ModifySituationDanger(double&) const;
  virtual int RandomizeTryToUnStickBodyPart(feuLong) const;
  virtual truth AllowUnconsciousness() const;
  virtual truth CanChokeOnWeb(web*) const;
  virtual truth BrainsHurt() const;
  virtual cchar* GetRunDescriptionLine(int) const;
  virtual cchar* GetNormalDeathMessage() const;
  virtual void ApplySpecialAttributeBonuses();
  virtual truth MindWormCanPenetrateSkull(mindworm*) const;
  truth HasSadistWeapon() const;
  virtual truth HasSadistAttackMode() const;
  virtual void SurgicallyDetachBodyPart();
  virtual truth SpecialBiteEffect(character*, v2, int, int, truth);
 protected:
  virtual v2 GetBodyPartBitmapPos(int, truth = false) const;
  virtual col16 GetBodyPartColorB(int, truth = false) const;
  virtual col16 GetBodyPartColorC(int, truth = false) const;
  virtual col16 GetBodyPartColorD(int, truth = false) const;
  virtual int GetBodyPartSparkleFlags(int) const;
  virtual sLong GetBodyPartSize(int, int) const;
  virtual sLong GetBodyPartVolume(int) const;
  virtual bodypart* MakeBodyPart(int) const;
  virtual cfestring& GetDeathMessage() const;
  virtual v2 GetDrawDisplacement(int) const { return ZERO_V2; }
  truth HasAUsableArm() const;
  truth HasAUsableLeg() const;
  truth HasTwoUsableLegs() const;
  truth CanAttackWithAnArm() const;
  truth RightArmIsUsable() const;
  truth LeftArmIsUsable() const;
  truth RightLegIsUsable() const;
  truth LeftLegIsUsable() const;
  std::list<sweaponskill*> SWeaponSkill;
  sweaponskill* CurrentRightSWeaponSkill;
  sweaponskill* CurrentLeftSWeaponSkill;
  static cint DrawOrder[];
};


#else



cint humanoid::DrawOrder[] = { TORSO_INDEX, GROIN_INDEX, RIGHT_LEG_INDEX, LEFT_LEG_INDEX, RIGHT_ARM_INDEX, LEFT_ARM_INDEX, HEAD_INDEX };



truth humanoid::BodyPartIsVital(int I) const { return I == TORSO_INDEX || I == HEAD_INDEX || I == GROIN_INDEX; }



truth humanoid::BodyPartCanBeSevered(int I) const { return I != TORSO_INDEX && I != GROIN_INDEX; }



int humanoid::OpenMultiplier() const { return HasAUsableArm() ? 1 : 3; }



int humanoid::CloseMultiplier() const { return HasAUsableArm() ? 1 : 2; }



int humanoid::GetCarryingStrength() const { return Max(GetAttribute(LEG_STRENGTH), 1) + CarryingBonus; }



void humanoid::CalculateBodyParts() { BodyParts = HUMANOID_BODYPARTS; }



void humanoid::CalculateAllowedWeaponSkillCategories() { AllowedWeaponSkillCategories = WEAPON_SKILL_CATEGORIES; }



void humanoid::Save(outputfile& SaveFile) const
{
  character::Save(SaveFile);
  SaveFile << SWeaponSkill;
}



void humanoid::Load(inputfile& SaveFile)
{
  character::Load(SaveFile);
  SaveFile >> SWeaponSkill;

  if(GetRightWielded())
    for(std::list<sweaponskill*>::iterator i = SWeaponSkill.begin(); i != SWeaponSkill.end(); ++i)
      if((*i)->IsSkillOf(GetRightWielded()))
      {
  SetCurrentRightSWeaponSkill(*i);
  break;
      }

  if(GetLeftWielded())
    for(std::list<sweaponskill*>::iterator i = SWeaponSkill.begin(); i != SWeaponSkill.end(); ++i)
      if((*i)->IsSkillOf(GetLeftWielded()))
      {
  SetCurrentLeftSWeaponSkill(*i);
  break;
      }
}



item* humanoid::GetMainWielded() const
{
  if(GetMainArm())
    if(GetMainArm()->GetWielded())
      return GetMainArm()->GetWielded();
    else
      if(GetSecondaryArm())
  return GetSecondaryArm()->GetWielded();
      else
  return 0;
  else
    if(GetSecondaryArm())
      return GetSecondaryArm()->GetWielded();
    else
      return 0;
}



item* humanoid::GetSecondaryWielded() const
{
  if(GetMainArm() && GetMainArm()->GetWielded() && GetSecondaryArm())
    return GetSecondaryArm()->GetWielded();
  else
    return 0;
}



truth humanoid::Hit(character* Enemy, v2 HitPos, int Direction, int Flags)
{
  if(CheckIfTooScaredToHit(Enemy))
    return false;

  if(IsPlayer())
  {
    if(!(Enemy->IsMasochist() && GetRelation(Enemy) == FRIEND) && GetRelation(Enemy) != HOSTILE && !game::TruthQuestion(CONST_S("This might cause a hostile reaction. Are you sure? [y/N]")))
      return false;
  }
  else if(GetAttribute(WISDOM) >= Enemy->GetAttackWisdomLimit())
    return false;

  if(GetBurdenState() == OVER_LOADED)
  {
    if(IsPlayer())
      ADD_MESSAGE("You cannot fight while carrying so much.");

    return false;
  }

  int c, AttackStyles;

  for(c = 0, AttackStyles = 0; c < 8; ++c)
    if(GetAttackStyle() & (1 << c))
      ++AttackStyles;

  int Chosen = RAND() % AttackStyles;

  for(c = 0, AttackStyles = 0; c < 8; ++c) {
    if(GetAttackStyle() & (1 << c) && AttackStyles++ == Chosen)
    {
      Chosen = 1 << c;
      break;
    }
  }

  if (StateIsActivated(VAMPIRISM) && !(RAND()%2)) Chosen = USE_HEAD;

  switch(Chosen)
  {
   case USE_ARMS:
    if(CanAttackWithAnArm() && (!(Flags & SADIST_HIT) || HasSadistWeapon()))
    {
      msgsystem::EnterBigMessageMode();
      Hostility(Enemy);
      sLong FirstAPCost = 0, SecondAPCost = 0;
      arm* FirstArm, * SecondArm;

      if(RAND() & 1)
      {
  FirstArm = GetRightArm();
  SecondArm = GetLeftArm();
      }
      else
      {
  FirstArm = GetLeftArm();
  SecondArm = GetRightArm();
      }

      int Strength = Max(GetAttribute(ARM_STRENGTH), 1);

      if(FirstArm && FirstArm->GetDamage() && (!(Flags & SADIST_HIT) || FirstArm->HasSadistWeapon()))
      {
  FirstAPCost = FirstArm->GetAPCost();
  FirstArm->Hit(Enemy, HitPos, Direction, Flags);

  if(StateIsActivated(LEPROSY) && !RAND_N(25 * GetAttribute(ENDURANCE)))
    DropBodyPart(FirstArm->GetBodyPartIndex());
      }

      if(!GetAction() && IsEnabled() && Enemy->IsEnabled() && SecondArm && SecondArm->GetDamage() && (!(Flags & SADIST_HIT) || SecondArm->HasSadistWeapon()))
      {
  SecondAPCost = SecondArm->GetAPCost();
  SecondArm->Hit(Enemy, HitPos, Direction, Flags);

  if(StateIsActivated(LEPROSY) && !RAND_N(25 * GetAttribute(ENDURANCE)))
    DropBodyPart(SecondArm->GetBodyPartIndex());
      }

      EditNP(-50);
      EditAP(-Max(FirstAPCost, SecondAPCost));
      EditStamina(-10000 / Strength, false);
      msgsystem::LeaveBigMessageMode();
      return true;
    }
   case USE_LEGS:
    if(HasTwoUsableLegs())
    {
      msgsystem::EnterBigMessageMode();
      Hostility(Enemy);
      Kick(GetNearLSquare(HitPos), Direction, Flags & SADIST_HIT);

      if(StateIsActivated(LEPROSY) && !RAND_N(25 * GetAttribute(ENDURANCE)))
  DropBodyPart(RAND_2 ? RIGHT_LEG_INDEX : LEFT_LEG_INDEX);

      msgsystem::LeaveBigMessageMode();
      return true;
    }
   case USE_HEAD:
    if(GetHead())
    {
      msgsystem::EnterBigMessageMode();
      Hostility(Enemy);
      Bite(Enemy, HitPos, Direction, Flags & SADIST_HIT);
      msgsystem::LeaveBigMessageMode();
      return true;
    }
   default:
    if(IsPlayer())
      ADD_MESSAGE("You are currently quite unable to damage anything.");

    return false;
  }
}



truth humanoid::AddSpecialSkillInfo(felist& List) const
{
  truth Something = false;

  if(CurrentRightSWeaponSkill && CurrentRightSWeaponSkill->GetHits() / 100)
  {
    List.AddEntry(CONST_S(""), LIGHT_GRAY);
    festring Buffer = CONST_S("right accustomization");
    Buffer.Resize(30);
    Buffer << CurrentRightSWeaponSkill->GetLevel();
    Buffer.Resize(40);
    Buffer << CurrentRightSWeaponSkill->GetHits() / 100;
    Buffer.Resize(50);

    if(CurrentRightSWeaponSkill->GetLevel() != 20)
      Buffer << (CurrentRightSWeaponSkill->GetLevelMap(CurrentRightSWeaponSkill->GetLevel() + 1) - CurrentRightSWeaponSkill->GetHits()) / 100;
    else
      Buffer << '-';

    Buffer.Resize(60);
    int Bonus = CurrentRightSWeaponSkill->GetBonus();
    Buffer << '+' << (Bonus - 1000) / 10;

    if(Bonus %= 10)
      Buffer << '.' << Bonus;

    Buffer << '%';
    List.AddEntry(Buffer, WHITE);
    Something = true;
  }

  if(CurrentLeftSWeaponSkill && CurrentLeftSWeaponSkill->GetHits() / 100)
  {
    if(!Something)
      List.AddEntry(CONST_S(""), LIGHT_GRAY);

    festring Buffer = CONST_S("left accustomization");
    Buffer.Resize(30);
    Buffer << CurrentLeftSWeaponSkill->GetLevel();
    Buffer.Resize(40);
    Buffer << CurrentLeftSWeaponSkill->GetHits() / 100;
    Buffer.Resize(50);

    if(CurrentLeftSWeaponSkill->GetLevel() != 20)
      Buffer << (CurrentLeftSWeaponSkill->GetLevelMap(CurrentLeftSWeaponSkill->GetLevel() + 1) - CurrentLeftSWeaponSkill->GetHits()) / 100;
    else
      Buffer << '-';

    Buffer.Resize(60);
    int Bonus = CurrentLeftSWeaponSkill->GetBonus();
    Buffer << '+' << (Bonus - 1000) / 10;

    if(Bonus %= 10)
      Buffer << '.' << Bonus;

    Buffer << '%';
    List.AddEntry(Buffer, WHITE);
    Something = true;
  }

  return Something;
}



truth humanoid::CheckThrowItemOpportunity () {
  if (!HasAUsableArm()) return false;
  return character::CheckThrowItemOpportunity();
}



truth humanoid::CheckAIZapOpportunity () {
  if (!HasAUsableArm() || !CanZap()) return false;
  return character::CheckAIZapOpportunity();
}



int humanoid::GetSize() const
{
  int Size = 0;

  if(GetHead())
    Size += GetHead()->GetSize();

  if(GetTorso())
    Size += GetTorso()->GetSize();

  leg* RightLeg = GetRightLeg();
  leg* LeftLeg = GetLeftLeg();

  if(LeftLeg && RightLeg)
    Size += Max(LeftLeg->GetSize(), RightLeg->GetSize());
  else if(LeftLeg)
    Size += LeftLeg->GetSize();
  else if(RightLeg)
    Size += RightLeg->GetSize();

  return Size;
}



sLong humanoid::GetBodyPartSize(int I, int TotalSize) const
{
  switch(I)
  {
   case HEAD_INDEX: return 20;
   case TORSO_INDEX: return ((TotalSize - 20) << 1) / 5;
   case RIGHT_ARM_INDEX:
   case LEFT_ARM_INDEX: return (TotalSize - 20) * 3 / 5;
   case GROIN_INDEX: return (TotalSize - 20) / 3;
   case RIGHT_LEG_INDEX:
   case LEFT_LEG_INDEX: return (TotalSize - 20) * 3 / 5;
  }

  ABORT("Illegal humanoid bodypart size request!");
  return 0;
}



sLong humanoid::GetBodyPartVolume(int I) const
{
  switch(I)
  {
   case HEAD_INDEX: return 4000;
   case TORSO_INDEX: return (GetTotalVolume() - 4000) * 13 / 30;
   case RIGHT_ARM_INDEX:
   case LEFT_ARM_INDEX: return (GetTotalVolume() - 4000) / 10;
   case GROIN_INDEX: return (GetTotalVolume() - 4000) / 10;
   case RIGHT_LEG_INDEX:
   case LEFT_LEG_INDEX: return ((GetTotalVolume() - 4000) << 1) / 15;
  }

  ABORT("Illegal humanoid bodypart volume request!");
  return 0;
}



bodypart* humanoid::MakeBodyPart(int I) const
{
  switch(I)
  {
   case TORSO_INDEX: return humanoidtorso::Spawn(0, NO_MATERIALS);
   case HEAD_INDEX: return head::Spawn(0, NO_MATERIALS);
   case RIGHT_ARM_INDEX: return rightarm::Spawn(0, NO_MATERIALS);
   case LEFT_ARM_INDEX: return leftarm::Spawn(0, NO_MATERIALS);
   case GROIN_INDEX: return groin::Spawn(0, NO_MATERIALS);
   case RIGHT_LEG_INDEX: return rightleg::Spawn(0, NO_MATERIALS);
   case LEFT_LEG_INDEX: return leftleg::Spawn(0, NO_MATERIALS);
  }

  ABORT("Weird bodypart to make for a humanoid. It must be your fault!");
  return 0;
}



truth humanoid::ReceiveDamage(character* Damager, int Damage, int Type, int TargetFlags, int Direction, truth Divide, truth PenetrateArmor, truth Critical, truth ShowMsg)
{
  int ChooseFrom[MAX_BODYPARTS], BodyParts = 0;

  if(TargetFlags & RIGHT_ARM && GetRightArm())
    ChooseFrom[BodyParts++] = 2;

  if(TargetFlags & LEFT_ARM && GetLeftArm())
    ChooseFrom[BodyParts++] = 3;

  if(TargetFlags & RIGHT_LEG && GetRightLeg())
    ChooseFrom[BodyParts++] = 5;

  if(TargetFlags & LEFT_LEG && GetLeftLeg())
    ChooseFrom[BodyParts++] = 6;

  if(TargetFlags & HEAD && GetHead())
    ChooseFrom[BodyParts++] = 1;

  if(TargetFlags & TORSO && GetTorso())
    ChooseFrom[BodyParts++] = 0;

  if(TargetFlags & GROIN && GetGroin())
    ChooseFrom[BodyParts++] = 4;

  if(!BodyParts)
    return false;

  truth Affected = false;

  if(Divide)
  {
    int c;
    sLong TotalVolume = 0;

    for(c = 0; c < BodyParts; ++c)
      TotalVolume += GetBodyPart(ChooseFrom[c])->GetBodyPartVolume();

    for(c = 0; c < BodyParts; ++c)
      if(ReceiveBodyPartDamage(Damager, sLong(Damage) * GetBodyPart(ChooseFrom[c])->GetBodyPartVolume() / TotalVolume, Type, ChooseFrom[c], Direction, PenetrateArmor, Critical, false))
  Affected = true;
  }
  else
  {
    sLong Possibility[MAX_BODYPARTS], PossibilitySum = 0;
    int Index = 0;

    for(int c = 0; c < BodyParts; ++c)
      PossibilitySum += Possibility[Index++] = GetBodyPart(ChooseFrom[c])->GetBodyPartVolume();

    Index = femath::WeightedRand(Possibility, PossibilitySum);
    Affected = ReceiveBodyPartDamage(Damager, Damage, Type, ChooseFrom[Index], Direction, PenetrateArmor, Critical, false);
  }

  if(!Affected && ShowMsg)
  {
    if(IsPlayer())
      ADD_MESSAGE("You are not hurt.");
    else if(CanBeSeenByPlayer())
      ADD_MESSAGE("%s is not hurt.", GetPersonalPronoun().CStr());
  }

  if(DamageTypeAffectsInventory(Type))
  {
    for(int c = 0; c < GetEquipments(); ++c)
    {
      item* Equipment = GetEquipment(c);

      if(Equipment)
  Equipment->ReceiveDamage(Damager, Damage, Type);
    }

    GetStack()->ReceiveDamage(Damager, Damage, Type);
  }

  return Affected;
}



arm* humanoid::GetMainArm() const
{
  return GetRightArm() ? GetRightArm() : GetLeftArm();
}



arm* humanoid::GetSecondaryArm() const
{
  return GetRightArm() ? GetLeftArm() : 0;
}



cchar* humanoid::GetEquipmentName(int I) const // convert to array
{
  switch(I)
  {
   case HELMET_INDEX: return "helmet";
   case AMULET_INDEX: return "amulet";
   case CLOAK_INDEX: return "cloak";
   case BODY_ARMOR_INDEX: return "body armor";
   case BELT_INDEX: return "belt";
   case RIGHT_WIELDED_INDEX: return "right hand wielded";
   case LEFT_WIELDED_INDEX: return "left hand wielded";
   case RIGHT_RING_INDEX: return "right ring";
   case LEFT_RING_INDEX: return "left ring";
   case RIGHT_GAUNTLET_INDEX: return "right gauntlet";
   case LEFT_GAUNTLET_INDEX: return "left gauntlet";
   case RIGHT_BOOT_INDEX: return "right boot";
   case LEFT_BOOT_INDEX: return "left boot";
  }

  return "forbidden piece of cloth";
}



sorter humanoid::EquipmentSorter(int I) const
{
  switch(I)
  {
   case HELMET_INDEX: return &item::IsHelmet;
   case AMULET_INDEX: return &item::IsAmulet;
   case CLOAK_INDEX: return &item::IsCloak;
   case BODY_ARMOR_INDEX: return &item::IsBodyArmor;
   case BELT_INDEX: return &item::IsBelt;
   case RIGHT_WIELDED_INDEX:
   case LEFT_WIELDED_INDEX: return 0;
   case RIGHT_RING_INDEX:
   case LEFT_RING_INDEX: return &item::IsRing;
   case RIGHT_GAUNTLET_INDEX:
   case LEFT_GAUNTLET_INDEX: return &item::IsGauntlet;
   case RIGHT_BOOT_INDEX:
   case LEFT_BOOT_INDEX: return &item::IsBoot;
  }

  return 0;
}



bodypart* humanoid::GetBodyPartOfEquipment(int I) const
{
  switch(I)
  {
   case HELMET_INDEX:
   case AMULET_INDEX:
    return GetHead();
   case CLOAK_INDEX:
   case BODY_ARMOR_INDEX:
   case BELT_INDEX:
    return GetTorso();
   case RIGHT_WIELDED_INDEX:
   case RIGHT_RING_INDEX:
   case RIGHT_GAUNTLET_INDEX:
    return GetRightArm();
   case LEFT_WIELDED_INDEX:
   case LEFT_RING_INDEX:
   case LEFT_GAUNTLET_INDEX:
    return GetLeftArm();
   case RIGHT_BOOT_INDEX:
    return GetRightLeg();
   case LEFT_BOOT_INDEX:
    return GetLeftLeg();
  }

  return 0;
}



item* humanoid::GetEquipment(int I) const
{
  switch(I)
  {
   case HELMET_INDEX: return GetHelmet();
   case AMULET_INDEX: return GetAmulet();
   case CLOAK_INDEX: return GetCloak();
   case BODY_ARMOR_INDEX: return GetBodyArmor();
   case BELT_INDEX: return GetBelt();
   case RIGHT_WIELDED_INDEX: return GetRightWielded();
   case LEFT_WIELDED_INDEX: return GetLeftWielded();
   case RIGHT_RING_INDEX: return GetRightRing();
   case LEFT_RING_INDEX: return GetLeftRing();
   case RIGHT_GAUNTLET_INDEX: return GetRightGauntlet();
   case LEFT_GAUNTLET_INDEX: return GetLeftGauntlet();
   case RIGHT_BOOT_INDEX: return GetRightBoot();
   case LEFT_BOOT_INDEX: return GetLeftBoot();
  }

  return 0;
}



void humanoid::SetEquipment(int I, item* What)
{
  switch(I)
  {
   case HELMET_INDEX: SetHelmet(What); break;
   case AMULET_INDEX: SetAmulet(What); break;
   case CLOAK_INDEX: SetCloak(What); break;
   case BODY_ARMOR_INDEX: SetBodyArmor(What); break;
   case BELT_INDEX: SetBelt(What); break;
   case RIGHT_WIELDED_INDEX: SetRightWielded(What); break;
   case LEFT_WIELDED_INDEX: SetLeftWielded(What); break;
   case RIGHT_RING_INDEX: SetRightRing(What); break;
   case LEFT_RING_INDEX: SetLeftRing(What); break;
   case RIGHT_GAUNTLET_INDEX: SetRightGauntlet(What); break;
   case LEFT_GAUNTLET_INDEX: SetLeftGauntlet(What); break;
   case RIGHT_BOOT_INDEX: SetRightBoot(What); break;
   case LEFT_BOOT_INDEX: SetLeftBoot(What); break;
  }
}



void humanoid::SwitchToDig(item* DigItem, v2 Square)
{
  dig* Dig = dig::Spawn(this);

  if(GetRightArm())
  {
    item* Item = GetRightArm()->GetWielded();

    if(Item && Item != DigItem)
    {
      Dig->SetRightBackupID(GetRightArm()->GetWielded()->GetID());
      GetRightArm()->GetWielded()->MoveTo(GetStack());
    }
  }

  if(GetLeftArm())
  {
    item* Item = GetLeftArm()->GetWielded();

    if(Item && Item != DigItem)
    {
      Dig->SetLeftBackupID(GetLeftArm()->GetWielded()->GetID());
      GetLeftArm()->GetWielded()->MoveTo(GetStack());
    }
  }

  if(GetMainWielded() != DigItem)
  {
    Dig->SetMoveDigger(true);
    DigItem->RemoveFromSlot();

    if(GetMainArm() && GetMainArm()->IsUsable())
      GetMainArm()->SetWielded(DigItem);
    else
      GetSecondaryArm()->SetWielded(DigItem);
  }
  else
    Dig->SetMoveDigger(false);

  Dig->SetSquareDug(Square);
  SetAction(Dig);
}



truth humanoid::CheckKick() const
{
  if(!CanKick())
  {
    if(IsPlayer())
      ADD_MESSAGE("This race can't kick.");

    return false;
  }

  if(GetUsableLegs() < 2)
  {
    if(IsPlayer())
      ADD_MESSAGE("How are you going to do that with %s?", GetUsableLegs() ? "only one usable leg" : "no usable legs");

    return false;
  }
  else
    return true;
}



int humanoid::GetUsableLegs() const
{
  int Legs = 0;

  if(RightLegIsUsable())
    ++Legs;

  if(LeftLegIsUsable())
    ++Legs;

  return Legs;
}



int humanoid::GetUsableArms() const
{
  int Arms = 0;

  if(RightArmIsUsable())
    ++Arms;

  if(LeftArmIsUsable())
    ++Arms;

  return Arms;
}



truth humanoid::CheckThrow() const
{
  if(!character::CheckThrow())
    return false;

  if(HasAUsableArm())
    return true;
  else
  {
    ADD_MESSAGE("You don't have a usable arm to do that!");
    return false;
  }
}



truth humanoid::CheckOffer() const
{
  if(HasAUsableArm())
    return true;
  else
  {
    ADD_MESSAGE("You need a usable arm to offer.");
    return false;
  }
}



v2 humanoid::GetEquipmentPanelPos(int I) const // convert to array
{
  switch(I)
  {
   case HELMET_INDEX: return v2(34, -22);
   case AMULET_INDEX: return v2(14, -22);
   case CLOAK_INDEX: return v2(-6, -22);
   case BODY_ARMOR_INDEX: return v2(54, -22);
   case BELT_INDEX: return v2(24, 70);
   case RIGHT_WIELDED_INDEX: return v2(-14, 4);
   case LEFT_WIELDED_INDEX: return v2(62, 4);
   case RIGHT_RING_INDEX: return v2(-14, 44);
   case LEFT_RING_INDEX: return v2(62, 44);
   case RIGHT_GAUNTLET_INDEX: return v2(-14, 24);
   case LEFT_GAUNTLET_INDEX: return v2(62, 24);
   case RIGHT_BOOT_INDEX: return v2(4, 70);
   case LEFT_BOOT_INDEX: return v2(44, 70);
  }

  return v2(24, 12);
}



void humanoid::DrawSilhouette(truth AnimationDraw) const
{
  int c;
  blitdata B1 = { DOUBLE_BUFFER,
      { 0, 0 },
      { 0, 0 },
      { TILE_SIZE, TILE_SIZE },
      { ivanconfig::GetContrastLuminance() },
      TRANSPARENT_COLOR,
      ALLOW_ANIMATE };

  v2 Where(RES.X - SILHOUETTE_SIZE.X - 39, 53);
  cint Equipments = GetEquipments();

  if(CanUseEquipment())
    for(c = 0; c < Equipments; ++c)
      if(GetBodyPartOfEquipment(c) && EquipmentIsAllowed(c))
      {
  v2 Pos = Where + GetEquipmentPanelPos(c);

  if(!AnimationDraw)
    DOUBLE_BUFFER->DrawRectangle(Pos + v2(-1, -1), Pos + TILE_V2, DARK_GRAY);

  item* Equipment = GetEquipment(c);

  if(Equipment && (!AnimationDraw || Equipment->IsAnimated()))
  {
    igraph::BlitBackGround(Pos, TILE_V2);
    B1.Dest = Pos;

    if(Equipment->AllowAlphaEverywhere())
      B1.CustomData |= ALLOW_ALPHA;

    Equipment->Draw(B1);
    B1.CustomData &= ~ALLOW_ALPHA;
  }
      }

  if(!AnimationDraw)
  {
    blitdata B2 = { DOUBLE_BUFFER,
        { 0, 0 },
        { Where.X + 8, Where.Y },
        { SILHOUETTE_SIZE.X, SILHOUETTE_SIZE.Y },
        { 0 },
        0,
        0 };

    for(int c = 0; c < BodyParts; ++c)
    {
      bodypart* BodyPart = GetBodyPart(c);

      if(BodyPart)
      {
  int Type = BodyPart->IsUsable() ? SILHOUETTE_NORMAL : SILHOUETTE_INTER_LACED;
  bitmap* Cache = igraph::GetSilhouetteCache(c, BodyPart->GetConditionColorIndex(), Type);
  Cache->NormalMaskedBlit(B2);
  BodyPart->DrawScars(B2);
      }
    }
  }
}



int humanoid::GetGlobalResistance(int Type) const
{
  int Resistance = GetResistance(Type);

  if(GetCloak())
    Resistance += GetCloak()->GetResistance(Type);

  if(!(Type & PHYSICAL_DAMAGE))
  {
    if(GetAmulet())
      Resistance += GetAmulet()->GetResistance(Type);

    if(GetRightRing())
      Resistance += GetRightRing()->GetResistance(Type);

    if(GetLeftRing())
      Resistance += GetLeftRing()->GetResistance(Type);
  }

  return Resistance;
}



truth humanoid::TryToRiseFromTheDead()
{
  int c;

  for(c = 0; c < BodyParts; ++c)
    if(!GetBodyPart(c))
    {
      bodypart* BodyPart = SearchForOriginalBodyPart(c);

      if(BodyPart)
      {
  BodyPart->RemoveFromSlot();
  AttachBodyPart(BodyPart);
  BodyPart->SetHP(1);
      }
    }

  for(c = 0; c < BodyParts; ++c)
  {
    bodypart* BodyPart = GetBodyPart(c);

    if(BodyPartIsVital(c) && !BodyPart)
      if(!HandleNoBodyPart(c))
  return false;

    if(BodyPart)
    {
      BodyPart->ResetSpoiling();

      if(BodyPart->CanRegenerate() || BodyPart->GetHP() < 1)
  BodyPart->SetHP(1);
    }
  }

  ResetStates();
  return true;
}



truth humanoid::HandleNoBodyPart(int I)
{
  switch(I)
  {
   case HEAD_INDEX:
    if(CanBeSeenByPlayer())
      ADD_MESSAGE("The headless body of %s vibrates violently.", CHAR_NAME(DEFINITE));

    return false;
   case GROIN_INDEX:
    if(CanBeSeenByPlayer())
      ADD_MESSAGE("The groinless body of %s vibrates violently.", CHAR_NAME(DEFINITE));

    return false;
   case TORSO_INDEX:
    ABORT("The corpse does not have a torso.");
   default:
    return true;
  }
}



v2 humanoid::GetBodyPartBitmapPos(int I, truth) const
{
  switch(I)
  {
   case TORSO_INDEX: return GetTorsoBitmapPos();
   case HEAD_INDEX: return GetHeadBitmapPos();
   case RIGHT_ARM_INDEX: return GetRightArmBitmapPos();
   case LEFT_ARM_INDEX: return GetLeftArmBitmapPos();
   case GROIN_INDEX: return GetGroinBitmapPos();
   case RIGHT_LEG_INDEX: return GetRightLegBitmapPos();
   case LEFT_LEG_INDEX: return GetLeftLegBitmapPos();
  }

  ABORT("Weird bodypart BitmapPos request for a humanoid!");
  return v2();
}



col16 humanoid::GetBodyPartColorB(int I, truth) const
{
  switch(I)
  {
   case TORSO_INDEX: return GetTorsoMainColor();
   case HEAD_INDEX: return GetCapColor();
   case RIGHT_ARM_INDEX:
   case LEFT_ARM_INDEX: return GetArmMainColor();
   case GROIN_INDEX:
   case RIGHT_LEG_INDEX:
   case LEFT_LEG_INDEX: return GetLegMainColor();
  }

  ABORT("Weird bodypart col B request for a humanoid!");
  return 0;
}



col16 humanoid::GetBodyPartColorC(int I, truth) const
{
  switch(I)
  {
   case TORSO_INDEX: return GetBeltColor();
   case HEAD_INDEX: return GetHairColor();
   case RIGHT_ARM_INDEX:
   case LEFT_ARM_INDEX: return GetGauntletColor();
   case GROIN_INDEX:
   case RIGHT_LEG_INDEX:
   case LEFT_LEG_INDEX: return GetBootColor();
  }

  ABORT("Weird bodypart col C request for a humanoid!");
  return 0;
}



col16 humanoid::GetBodyPartColorD(int I, truth) const
{
  switch(I)
  {
   case TORSO_INDEX: return GetTorsoSpecialColor();
   case HEAD_INDEX: return GetEyeColor();
   case RIGHT_ARM_INDEX:
   case LEFT_ARM_INDEX: return GetArmSpecialColor();
   case GROIN_INDEX:
   case RIGHT_LEG_INDEX:
   case LEFT_LEG_INDEX: return GetLegSpecialColor();
  }

  ABORT("Weird bodypart col D request for a humanoid!");
  return 0;
}



int humanoid::GetBodyPartSparkleFlags(int I) const
{
  truth Sparkling = false;
  int SparkleFlags = GetNaturalSparkleFlags() & SKIN_COLOR ? SPARKLING_A : 0;

  switch(I)
  {
   case TORSO_INDEX: Sparkling = GetNaturalSparkleFlags() & TORSO_MAIN_COLOR; break;
   case HEAD_INDEX: Sparkling = GetNaturalSparkleFlags() & CAP_COLOR; break;
   case RIGHT_ARM_INDEX:
   case LEFT_ARM_INDEX: Sparkling = GetNaturalSparkleFlags() & ARM_MAIN_COLOR; break;
   case GROIN_INDEX:
   case RIGHT_LEG_INDEX:
   case LEFT_LEG_INDEX: Sparkling = GetNaturalSparkleFlags() & LEG_MAIN_COLOR; break;
  }

  SparkleFlags |= Sparkling ? SPARKLING_B : 0;
  Sparkling = false;

  switch(I)
  {
   case TORSO_INDEX: Sparkling = GetNaturalSparkleFlags() & BELT_COLOR; break;
   case HEAD_INDEX: Sparkling = GetNaturalSparkleFlags() & HAIR_COLOR; break;
   case RIGHT_ARM_INDEX:
   case LEFT_ARM_INDEX: Sparkling = GetNaturalSparkleFlags() & GAUNTLET_COLOR; break;
   case GROIN_INDEX:
   case RIGHT_LEG_INDEX:
   case LEFT_LEG_INDEX: Sparkling = GetNaturalSparkleFlags() & BOOT_COLOR; break;
  }

  SparkleFlags |= Sparkling ? SPARKLING_C : 0;
  Sparkling = false;

  switch(I)
  {
   case TORSO_INDEX: Sparkling = GetNaturalSparkleFlags() & TORSO_SPECIAL_COLOR; break;
   case HEAD_INDEX: Sparkling = GetNaturalSparkleFlags() & EYE_COLOR; break;
   case RIGHT_ARM_INDEX:
   case LEFT_ARM_INDEX: Sparkling = GetNaturalSparkleFlags() & ARM_SPECIAL_COLOR; break;
   case GROIN_INDEX:
   case RIGHT_LEG_INDEX:
   case LEFT_LEG_INDEX: Sparkling = GetNaturalSparkleFlags() & LEG_SPECIAL_COLOR; break;
  }

  SparkleFlags |= Sparkling ? SPARKLING_D : 0;
  return SparkleFlags;
}



void humanoid::Bite(character* Enemy, v2 HitPos, int Direction, truth ForceHit)
{
  /* This function ought not to be called without a head */

  EditNP(-50);
  EditAP(-GetHead()->GetBiteAPCost());
  EditExperience(AGILITY, 150, 1 << 9);
  EditStamina(-1000, false);
  Enemy->TakeHit(this, 0, GetHead(), HitPos, GetHead()->GetBiteDamage(), GetHead()->GetBiteToHitValue(), RAND() % 26 - RAND() % 26, BITE_ATTACK, Direction, !(RAND() % GetCriticalModifier()), ForceHit);
}



void humanoid::Kick(lsquare* Square, int Direction, truth ForceHit)
{
  leg* KickLeg = RAND_2 ? GetRightLeg() : GetLeftLeg();
  EditNP(-50);
  EditAP(-KickLeg->GetKickAPCost());
  EditStamina(-10000 / GetAttribute(LEG_STRENGTH), false);

  if(Square->BeKicked(this, 0, KickLeg, KickLeg->GetKickDamage(), KickLeg->GetKickToHitValue(), RAND() % 26 - RAND() % 26, Direction, !(RAND() % GetCriticalModifier()), ForceHit))
  {
    KickLeg->EditExperience(LEG_STRENGTH, 75, 1 << 9);
    KickLeg->EditExperience(AGILITY, 75, 1 << 9);
  }
}



double humanoid::GetTimeToKill(ccharacter* Enemy, truth UseMaxHP) const
{
  double Effectivity = 0;
  int AttackStyles = 0;

  if(IsUsingArms())
  {
    arm* RightArm = GetRightArm();

    if(RightArm)
    {
      double Damage = RightArm->GetDamage();

      if(Damage)
  Effectivity += 1 / (Enemy->GetTimeToDie(this, int(Damage) + 1, RightArm->GetToHitValue(), AttackIsBlockable(GetRightWielded() ? WEAPON_ATTACK : UNARMED_ATTACK), UseMaxHP) * RightArm->GetAPCost());
    }

    arm* LeftArm = GetLeftArm();

    if(LeftArm)
    {
      double Damage = LeftArm->GetDamage();

      if(Damage)
  Effectivity += 1 / (Enemy->GetTimeToDie(this, int(Damage) + 1, LeftArm->GetToHitValue(), AttackIsBlockable(GetLeftWielded() ? WEAPON_ATTACK : UNARMED_ATTACK), UseMaxHP) * LeftArm->GetAPCost());
    }

    ++AttackStyles;
  }

  if(IsUsingLegs())
  {
    leg* RightLeg = GetRightLeg();
    leg* LeftLeg = GetLeftLeg();
    double TimeToDie = Enemy->GetTimeToDie(this, int(RightLeg->GetKickDamage()) + 1, RightLeg->GetKickToHitValue(), AttackIsBlockable(KICK_ATTACK), UseMaxHP) * RightLeg->GetKickAPCost()
           + Enemy->GetTimeToDie(this, int(LeftLeg->GetKickDamage()) + 1, LeftLeg->GetKickToHitValue(), AttackIsBlockable(KICK_ATTACK), UseMaxHP) * LeftLeg->GetKickAPCost();
    Effectivity += 2 / TimeToDie;
    ++AttackStyles;
  }

  if(IsUsingHead())
  {
    head* Head = GetHead();
    Effectivity += 1 / (Enemy->GetTimeToDie(this, int(Head->GetBiteDamage()) + 1, Head->GetBiteToHitValue(), AttackIsBlockable(BITE_ATTACK), UseMaxHP) * Head->GetBiteAPCost());
    ++AttackStyles;
  }

  if(StateIsActivated(HASTE))
    Effectivity *= 2;

  if(StateIsActivated(SLOW))
    Effectivity /= 2;

  return AttackStyles ? AttackStyles / Effectivity : 10000000;
}



int humanoid::GetAttribute(int Identifier, truth AllowBonus) const
{
  if(Identifier < BASE_ATTRIBUTES)
    return character::GetAttribute(Identifier, AllowBonus);
  else
  {
    int Attrib = 0;

    if(Identifier == ARM_STRENGTH || Identifier == DEXTERITY)
    {
      arm* RightArm = GetRightArm();

      if(RightArm)
  Attrib += RightArm->GetAttribute(Identifier, AllowBonus);

      arm* LeftArm = GetLeftArm();

      if(LeftArm)
  Attrib += LeftArm->GetAttribute(Identifier, AllowBonus);
    }
    else if(Identifier == LEG_STRENGTH || Identifier == AGILITY)
    {
      leg* RightLeg = GetRightLeg();

      if(RightLeg)
  Attrib += RightLeg->GetAttribute(Identifier, AllowBonus);

      leg* LeftLeg = GetLeftLeg();

      if(LeftLeg)
  Attrib += LeftLeg->GetAttribute(Identifier, AllowBonus);
    }
    else
    {
      ABORT("Illegal humanoid attribute %d request!", Identifier);
      return 0xEBBA;
    }

    return Attrib >> 1;
  }
}



truth humanoid::EditAttribute(int Identifier, int Value)
{
  if(Identifier < BASE_ATTRIBUTES)
    return character::EditAttribute(Identifier, Value);
  else if(Identifier == ARM_STRENGTH || Identifier == DEXTERITY)
  {
    truth Success = false;

    if(GetRightArm() && GetRightArm()->EditAttribute(Identifier, Value))
      Success = true;

    if(GetLeftArm() && GetLeftArm()->EditAttribute(Identifier, Value))
      Success = true;

    return Success;
  }
  else if(Identifier == LEG_STRENGTH || Identifier == AGILITY)
  {
    truth Success = false;

    if(GetRightLeg() && GetRightLeg()->EditAttribute(Identifier, Value))
      Success = true;

    if(GetLeftLeg() && GetLeftLeg()->EditAttribute(Identifier, Value))
      Success = true;

    return Success;
  }
  else
  {
    ABORT("Illegal humanoid attribute %d edit request!", Identifier);
    return false;
  }
}



void humanoid::EditExperience(int Identifier, double Value, double Speed)
{
  if(!AllowExperience())
    return;

  if(Identifier < BASE_ATTRIBUTES)
    character::EditExperience(Identifier, Value, Speed);
  else if(Identifier == ARM_STRENGTH || Identifier == DEXTERITY)
  {
    if(GetRightArm())
      GetRightArm()->EditExperience(Identifier, Value, Speed);

    if(GetLeftArm())
      GetLeftArm()->EditExperience(Identifier, Value, Speed);
  }
  else if(Identifier == LEG_STRENGTH || Identifier == AGILITY)
  {
    if(GetRightLeg())
      GetRightLeg()->EditExperience(Identifier, Value, Speed);

    if(GetLeftLeg())
      GetLeftLeg()->EditExperience(Identifier, Value, Speed);
  }
  else
    ABORT("Illegal humanoid attribute %d experience edit request!", Identifier);
}



int humanoid::DrawStats(truth AnimationDraw) const
{
  DrawSilhouette(AnimationDraw);

  if(AnimationDraw)
    return 15;

  int PanelPosX = RES.X - 96, PanelPosY = 15;
  PrintAttribute("AStr", ARM_STRENGTH, PanelPosX, PanelPosY++);
  PrintAttribute("LStr", LEG_STRENGTH, PanelPosX, PanelPosY++);
  PrintAttribute("Dex", DEXTERITY, PanelPosX, PanelPosY++);
  PrintAttribute("Agi", AGILITY, PanelPosX, PanelPosY++);
  return PanelPosY;
}



int humanoid::GetRandomStepperBodyPart() const
{
  int Possible = 0, PossibleArray[3];

  if(GetRightLeg())
    PossibleArray[Possible++] = RIGHT_LEG_INDEX;

  if(GetLeftLeg())
    PossibleArray[Possible++] = LEFT_LEG_INDEX;

  if(Possible)
    return PossibleArray[RAND_N(Possible)];

  if(GetRightArm())
    PossibleArray[Possible++] = RIGHT_ARM_INDEX;

  if(GetLeftArm())
    PossibleArray[Possible++] = LEFT_ARM_INDEX;

  if(Possible)
    return PossibleArray[RAND_N(Possible)];

  if(GetHead())
    PossibleArray[Possible++] = HEAD_INDEX;

  if(GetGroin())
    PossibleArray[Possible++] = GROIN_INDEX;

  PossibleArray[Possible++] = TORSO_INDEX;
  return PossibleArray[RAND_N(Possible)];
}



int humanoid::CheckForBlock(character* Enemy, item* Weapon, double ToHitValue, int Damage, int Success, int Type)
{
  if(GetAction())
    return Damage;

  if(GetRightWielded())
    Damage = CheckForBlockWithArm(Enemy, Weapon, GetRightArm(), ToHitValue, Damage, Success, Type);

  if(Damage && GetLeftWielded() && (!Weapon || Weapon->Exists()))
    Damage = CheckForBlockWithArm(Enemy, Weapon, GetLeftArm(), ToHitValue, Damage, Success, Type);

  return Damage;
}



truth humanoid::CanWield() const
{
  return CanUseEquipment(RIGHT_WIELDED_INDEX) || CanUseEquipment(LEFT_WIELDED_INDEX);
}



truth humanoid::CheckBalance(double KickDamage)
{
  return !CanMove()
    || IsStuck()
    || !KickDamage
    || (GetUsableLegs() != 1
  && !IsFlying()
  && KickDamage * 5 < RAND() % GetSize());
}



sLong humanoid::GetMoveAPRequirement(int Difficulty) const
{
  if(IsFlying())
    return (!StateIsActivated(PANIC) ? 10000000 : 8000000) * Difficulty / (APBonus(GetAttribute(AGILITY)) * GetMoveEase());

  switch(GetUsableLegs())
  {
   case 0:
    return (!StateIsActivated(PANIC) ? 20000000 : 16000000) * Difficulty / (APBonus(GetAttribute(AGILITY)) * GetMoveEase());
   case 1:
    return (!StateIsActivated(PANIC) ? 13333333 : 10666667) * Difficulty / (APBonus(GetAttribute(AGILITY)) * GetMoveEase());
   case 2:
    return (!StateIsActivated(PANIC) ? 10000000 : 8000000) * Difficulty / (APBonus(GetAttribute(AGILITY)) * GetMoveEase());
  }

  ABORT("A %d legged humanoid invaded the dungeon!", GetUsableLegs());
  return 0;
}



truth humanoid::EquipmentEasilyRecognized(int I) const
{
  if(GetRelation(PLAYER) != HOSTILE)
    return true;

  switch(I)
  {
   case AMULET_INDEX:
   case RIGHT_RING_INDEX:
   case LEFT_RING_INDEX:
   case BELT_INDEX:
    return false;
  }

  return true;
}



void humanoid::SurgicallyDetachBodyPart () {
  int ToBeDetached;
  //
  switch (game::KeyQuestion(CONST_S("What limb? (l)eft arm, (r)ight arm, (L)eft leg, (R)ight leg, (h)ead?"), KEY_ESC, 5, 'l','r','L','R', 'h')) {
    case 'l': ToBeDetached = LEFT_ARM_INDEX; break;
    case 'r': ToBeDetached = RIGHT_ARM_INDEX; break;
    case 'L': ToBeDetached = LEFT_LEG_INDEX; break;
    case 'R': ToBeDetached = RIGHT_LEG_INDEX; break;
    case 'h': ToBeDetached = HEAD_INDEX; break; // maybe no head, should be up to the player to decide.
    default: return;
  }
  //
  if (GetBodyPart(ToBeDetached)) {
    item *ToDrop = SevereBodyPart(ToBeDetached);
    SendNewDrawRequest();
    if (ToDrop) {
      GetStack()->AddItem(ToDrop);
      ToDrop->DropEquipment();
    }
    ADD_MESSAGE("Bodypart detached!");
  } else {
    ADD_MESSAGE("That bodypart has already been detached.");
  }
  CheckDeath(CONST_S("had one of his vital bodyparts surgically removed"), 0);
}



void humanoid::SignalEquipmentAdd(int EquipmentIndex)
{
  character::SignalEquipmentAdd(EquipmentIndex);

  if(EquipmentIndex == RIGHT_WIELDED_INDEX)
    EnsureCurrentSWeaponSkillIsCorrect(CurrentRightSWeaponSkill, GetRightWielded());
  else if(EquipmentIndex == LEFT_WIELDED_INDEX)
    EnsureCurrentSWeaponSkillIsCorrect(CurrentLeftSWeaponSkill, GetLeftWielded());

  if(!IsInitializing())
    CalculateBattleInfo();
}



void humanoid::SignalEquipmentRemoval(int EquipmentIndex, citem* Item)
{
  character::SignalEquipmentRemoval(EquipmentIndex, Item);

  if(EquipmentIndex == RIGHT_WIELDED_INDEX)
    EnsureCurrentSWeaponSkillIsCorrect(CurrentRightSWeaponSkill, 0);
  else if(EquipmentIndex == LEFT_WIELDED_INDEX)
    EnsureCurrentSWeaponSkillIsCorrect(CurrentLeftSWeaponSkill, 0);

  if(!IsInitializing())
    CalculateBattleInfo();
}



void humanoid::SWeaponSkillTick()
{
  for(std::list<sweaponskill*>::iterator i = SWeaponSkill.begin(); i != SWeaponSkill.end();)
  {
    if((*i)->Tick() && IsPlayer())
    {
      item* Item = SearchForItem(*i);

      if(Item)
  (*i)->AddLevelDownMessage(Item->CHAR_NAME(UNARTICLED));
    }

    if(!(*i)->GetHits() && *i != GetCurrentRightSWeaponSkill() && *i != GetCurrentLeftSWeaponSkill())
    {
      std::list<sweaponskill*>::iterator Dirt = i++;
      SWeaponSkill.erase(Dirt);
    }
    else
      ++i;
  }
}



void humanoid::DrawBodyParts(blitdata& BlitData) const
{
  bitmap* TileBuffer = igraph::GetTileBuffer();
  bitmap* RealBitmap = BlitData.Bitmap;
  blitdata B = { TileBuffer,
     { BlitData.Dest.X, BlitData.Dest.Y },
     { 0, 0 },
     { TILE_SIZE, TILE_SIZE },
     { 0 },
     TRANSPARENT_COLOR,
     BlitData.CustomData };

  RealBitmap->NormalBlit(B);
  TileBuffer->FillPriority(0);
  B.Src.X = B.Src.Y = 0;
  B.Luminance = BlitData.Luminance;

  for(int c = 0; c < BodyParts; ++c)
  {
    bodypart* BodyPart = GetBodyPart(DrawOrder[c]);

    if(BodyPart)
    {
      B.Dest = GetDrawDisplacement(c);
      BodyPart->Draw(B);
    }
  }

  B.Dest.X = B.Dest.Y = 0;
  arm* LeftArm = GetLeftArm();

  if(LeftArm)
    LeftArm->DrawWielded(B);

  arm* RightArm = GetRightArm();

  if(RightArm)
    RightArm->DrawWielded(B);

  TileBuffer->FastBlit(RealBitmap, BlitData.Dest);
}



truth humanoid::CanUseStethoscope(truth PrintReason) const
{
  if(!GetUsableArms())
  {
    if(PrintReason)
      ADD_MESSAGE("You need a usable arm to use a stethoscope.");

    return false;
  }

  if(!GetHead())
  {
    if(PrintReason)
      ADD_MESSAGE("You need a head to use stethoscope.");

    return false;
  }

  return true;
}



truth humanoid::IsUsingArms() const
{
  return GetAttackStyle() & USE_ARMS && CanAttackWithAnArm();
}



truth humanoid::IsUsingLegs() const
{
  return (GetAttackStyle() & USE_LEGS
    || (GetAttackStyle() & USE_ARMS && !CanAttackWithAnArm()))
    && HasTwoUsableLegs();
}



truth humanoid::IsUsingHead() const
{
  return (GetAttackStyle() & USE_HEAD
    || ((GetAttackStyle() & USE_LEGS
         || (GetAttackStyle() & USE_ARMS && !CanAttackWithAnArm()))
        && !HasTwoUsableLegs()))
    && GetHead();
}



void humanoid::CalculateBattleInfo()
{
  CalculateDodgeValue();
  doforbodyparts()(this, &bodypart::CalculateAttackInfo);
}



void humanoid::AddSpecialEquipmentInfo(festring& String, int I) const
{
  if((I == RIGHT_WIELDED_INDEX && GetRightArm()->TwoHandWieldIsActive()) || (I == LEFT_WIELDED_INDEX && GetLeftArm()->TwoHandWieldIsActive()))
    String << " (in both hands)";
}



/* Yes, this is evil. */
#define INSTANTIATE(name) if(DataBase->name.IsValid() && (Item = DataBase->name.Instantiate(SpecialFlags))) Set##name(Item);

void humanoid::CreateInitialEquipment(int SpecialFlags)
{
  character::CreateInitialEquipment(SpecialFlags);
  item* Item;

  INSTANTIATE(Helmet);
  INSTANTIATE(Amulet);
  INSTANTIATE(Cloak);
  INSTANTIATE(BodyArmor);
  INSTANTIATE(Belt);
  INSTANTIATE(RightWielded);
  INSTANTIATE(LeftWielded);
  INSTANTIATE(RightRing);
  INSTANTIATE(LeftRing);
  INSTANTIATE(RightGauntlet);
  INSTANTIATE(LeftGauntlet);
  INSTANTIATE(RightBoot);
  INSTANTIATE(LeftBoot);

  if(CurrentRightSWeaponSkill)
    CurrentRightSWeaponSkill->AddHit(GetRightSWeaponSkillHits() * 100);

  if(CurrentLeftSWeaponSkill)
    CurrentLeftSWeaponSkill->AddHit(GetLeftSWeaponSkillHits() * 100);
}



festring humanoid::GetBodyPartName(int I, truth Articled) const
{
  festring Article;

  if(Articled)
    Article << 'a';

  switch(I)
  {
   case HEAD_INDEX: return Article + "head";
   case TORSO_INDEX: return Article + "torso";
   case RIGHT_ARM_INDEX: return Article + "right arm";
   case LEFT_ARM_INDEX: return Article + "left arm";
   case GROIN_INDEX: return Article + "groin";
   case RIGHT_LEG_INDEX: return Article + "right leg";
   case LEFT_LEG_INDEX: return Article + "left leg";
  }

  ABORT("Illegal humanoid bodypart name request!");
  return "";
}



void humanoid::CreateBlockPossibilityVector(blockvector& Vector, double ToHitValue) const
{
  double RightBlockChance = 0;
  int RightBlockCapability = 0;
  double LeftBlockChance = 0;
  int LeftBlockCapability = 0;
  arm* RightArm = GetRightArm();
  arm* LeftArm = GetLeftArm();

  if(RightArm)
  {
    RightBlockChance = RightArm->GetBlockChance(ToHitValue);
    RightBlockCapability = RightArm->GetBlockCapability();
  }

  if(LeftArm)
  {
    LeftBlockChance = LeftArm->GetBlockChance(ToHitValue);
    LeftBlockCapability = LeftArm->GetBlockCapability();
  }

  /* Double block */

  if(RightBlockCapability + LeftBlockCapability)
    Vector.push_back(std::make_pair(RightBlockChance * LeftBlockChance, RightBlockCapability + LeftBlockCapability));

  /* Right block */

  if(RightBlockCapability)
    Vector.push_back(std::make_pair(RightBlockChance * (1 - LeftBlockChance), RightBlockCapability));

  /* Left block */

  if(LeftBlockCapability)
    Vector.push_back(std::make_pair(LeftBlockChance * (1 - RightBlockChance), LeftBlockCapability));
}



item* humanoid::SevereBodyPart(int BodyPartIndex, truth ForceDisappearance, stack* EquipmentDropStack)
{
  if(BodyPartIndex == RIGHT_ARM_INDEX)
    EnsureCurrentSWeaponSkillIsCorrect(CurrentRightSWeaponSkill, 0);
  else if(BodyPartIndex == LEFT_ARM_INDEX)
    EnsureCurrentSWeaponSkillIsCorrect(CurrentLeftSWeaponSkill, 0);

  return character::SevereBodyPart(BodyPartIndex, ForceDisappearance, EquipmentDropStack);
}



humanoid::humanoid(const humanoid& Humanoid) : mybase(Humanoid), CurrentRightSWeaponSkill(0), CurrentLeftSWeaponSkill(0)
{
  SWeaponSkill.resize(Humanoid.SWeaponSkill.size());
  std::list<sweaponskill*>::iterator i1 = SWeaponSkill.begin();
  std::list<sweaponskill*>::const_iterator i2 = Humanoid.SWeaponSkill.begin();

  for(; i1 != SWeaponSkill.end(); ++i1, ++i2)
    *i1 = new sweaponskill(**i2);
}



cfestring& humanoid::GetDeathMessage() const
{
  static festring HeadlessDeathMsg = CONST_S("@Dd dies without a sound.");
  return GetHead() || character::GetDeathMessage() != "@Dd dies screaming." ? character::GetDeathMessage() : HeadlessDeathMsg;
}



int humanoid::GetSWeaponSkillLevel(citem* Item) const
{
  std::list<sweaponskill*>::const_iterator i;

  for(i = SWeaponSkill.begin(); i != SWeaponSkill.end(); ++i)
    if((*i)->IsSkillOf(Item))
      return (*i)->GetLevel();

  for(idholder* I = Item->GetCloneMotherID(); I; I = I->Next)
    for(i = SWeaponSkill.begin(); i != SWeaponSkill.end(); ++i)
      if((*i)->IsSkillOfCloneMother(Item, I->ID))
  return (*i)->GetLevel();

  return 0;
}



truth humanoid::UseMaterialAttributes() const
{
  return combinebodypartpredicates()(this, &bodypart::UseMaterialAttributes, 0);
}



void humanoid::CalculateDodgeValue()
{
  DodgeValue = 0.05 * GetMoveEase() * GetAttribute(AGILITY) / sqrt(GetSize());

  if(IsFlying())
    DodgeValue *= 2;
  else
  {
    if(!HasAUsableLeg())
      DodgeValue *= 0.50;
    if(!HasTwoUsableLegs())
      DodgeValue *= 0.75;
  }

  if(DodgeValue < 1)
    DodgeValue = 1;
}



truth humanoid::CheckZap()
{
  if(!GetUsableArms())
  {
    ADD_MESSAGE("You need at least one usable arm to zap.");
    return false;
  }
  else
    return character::CheckZap();
}



truth humanoid::CheckTalk()
{
  if(!character::CheckTalk())
    return false;

  if(!GetHead())
  {
    ADD_MESSAGE("You need a head to talk.");
    return false;
  }

  return true;
}



truth humanoid::CheckIfEquipmentIsNotUsable(int I) const
{
  return (I == RIGHT_WIELDED_INDEX && GetRightArm()->CheckIfWeaponTooHeavy("this item"))
       || (I == LEFT_WIELDED_INDEX && GetLeftArm()->CheckIfWeaponTooHeavy("this item"))
       || (I == RIGHT_WIELDED_INDEX && GetLeftWielded() && GetLeftWielded()->IsTwoHanded() && GetLeftArm()->CheckIfWeaponTooHeavy(festring(GetPossessivePronoun() + " other wielded item").CStr()))
       || (I == LEFT_WIELDED_INDEX && GetRightWielded() && GetRightWielded()->IsTwoHanded() && GetRightArm()->CheckIfWeaponTooHeavy(festring(GetPossessivePronoun() + " other wielded item").CStr()));
}



void humanoid::AddSpecialStethoscopeInfo(felist& Info) const
{
  Info.AddEntry(CONST_S("Arm strength: ") + GetAttribute(ARM_STRENGTH), LIGHT_GRAY);
  Info.AddEntry(CONST_S("Leg strength: ") + GetAttribute(LEG_STRENGTH), LIGHT_GRAY);
  Info.AddEntry(CONST_S("Dexterity: ") + GetAttribute(DEXTERITY), LIGHT_GRAY);
  Info.AddEntry(CONST_S("Agility: ") + GetAttribute(AGILITY), LIGHT_GRAY);
}



item* humanoid::GetPairEquipment(int I) const
{
  switch(I)
  {
   case RIGHT_WIELDED_INDEX: return GetLeftWielded();
   case LEFT_WIELDED_INDEX: return GetRightWielded();
   case RIGHT_GAUNTLET_INDEX: return GetLeftGauntlet();
   case LEFT_GAUNTLET_INDEX: return GetRightGauntlet();
   case RIGHT_BOOT_INDEX: return GetLeftBoot();
   case LEFT_BOOT_INDEX: return GetRightBoot();
  }

  return 0;
}



cfestring& humanoid::GetStandVerb() const
{
  if(ForceCustomStandVerb())
    return DataBase->StandVerb;

  static festring HasntFeet = CONST_S("crawling");
  static festring Hovering = CONST_S("hovering");
  static festring Swimming = CONST_S("swimming");

  if(StateIsActivated(LEVITATION))
    return Hovering;

  if(IsSwimming())
    return Swimming;

  return HasAUsableLeg() ? DataBase->StandVerb : HasntFeet;
}



head* humanoid::Behead()
{
  head* Head = GetHead();

  if(Head)
    SevereBodyPart(HEAD_INDEX);

  return Head;
}



int humanoid::GetRandomApplyBodyPart() const
{
  if(RightArmIsUsable())
  {
    if(LeftArmIsUsable())
      return RAND_2 ? RIGHT_ARM_INDEX : LEFT_ARM_INDEX;
    else
      return RIGHT_ARM_INDEX;
  }
  else if(LeftArmIsUsable())
    return LEFT_ARM_INDEX;

  if(GetHead())
    return HEAD_INDEX;

  return TORSO_INDEX;
}



#ifdef WIZARD
void humanoid::AddAttributeInfo(festring& Entry) const
{
  Entry.Resize(45);
  Entry << GetAttribute(ARM_STRENGTH);
  Entry.Resize(48);
  Entry << GetAttribute(LEG_STRENGTH);
  Entry.Resize(51);
  Entry << GetAttribute(DEXTERITY);
  Entry.Resize(54);
  Entry << GetAttribute(AGILITY);
  character::AddAttributeInfo(Entry);
}



void humanoid::AddAttackInfo(felist& List) const
{
  if(GetAttackStyle() & USE_ARMS)
  {
    if(GetRightArm())
      GetRightArm()->AddAttackInfo(List);

    if(GetLeftArm())
      GetLeftArm()->AddAttackInfo(List);
  }

  festring Entry;

  if(IsUsingLegs())
  {
    GetRightLeg()->AddAttackInfo(List);
    GetLeftLeg()->AddAttackInfo(List);
  }

  if(IsUsingHead())
  {
    Entry = CONST_S("   bite attack");
    Entry.Resize(50);
    Entry << GetHead()->GetBiteMinDamage() << '-' << GetHead()->GetBiteMaxDamage();
    Entry.Resize(60);
    Entry << int(GetHead()->GetBiteToHitValue());
    Entry.Resize(70);
    Entry << GetHead()->GetBiteAPCost();
    List.AddEntry(Entry, LIGHT_GRAY);
  }
}



void humanoid::AddDefenceInfo(felist& List) const
{
  character::AddDefenceInfo(List);

  if(GetRightArm())
    GetRightArm()->AddDefenceInfo(List);

  if(GetLeftArm())
    GetLeftArm()->AddDefenceInfo(List);
}



void humanoid::DetachBodyPart()
{
  int ToBeDetached;

  switch(game::KeyQuestion(CONST_S("What limb? (l)eft arm, (r)ight arm, (L)eft leg, (R)ight leg, (h)ead?"), KEY_ESC, 5, 'l','r','L','R', 'h'))
  {
   case 'l':
    ToBeDetached = LEFT_ARM_INDEX;
    break;
   case 'r':
    ToBeDetached = RIGHT_ARM_INDEX;
    break;
   case 'L':
    ToBeDetached = LEFT_LEG_INDEX;
    break;
   case 'R':
    ToBeDetached = RIGHT_LEG_INDEX;
    break;
   case 'h':
    ToBeDetached = HEAD_INDEX;
    break;
   default:
    return;
  }

  if(GetBodyPart(ToBeDetached))
  {
    item* ToDrop = SevereBodyPart(ToBeDetached);
    SendNewDrawRequest();

    if(ToDrop)
    {
      GetStack()->AddItem(ToDrop);
      ToDrop->DropEquipment();
    }

    ADD_MESSAGE("Bodypart detached!");
  }
  else
    ADD_MESSAGE("That bodypart has already been detached.");

  CheckDeath(CONST_S("removed one of his vital bodyparts"), 0);
}

#else

void humanoid::AddAttributeInfo(festring&) const {}
void humanoid::AddAttackInfo(felist&) const {}
void humanoid::AddDefenceInfo(felist&) const {}
void humanoid::DetachBodyPart() {}

#endif


truth humanoid::PreProcessForBone()
{
  for(std::list<sweaponskill*>::iterator i = SWeaponSkill.begin(); i != SWeaponSkill.end(); ++i)
    (*i)->PreProcessForBone();

  return character::PreProcessForBone();
}



void humanoid::FinalProcessForBone()
{
  character::FinalProcessForBone();

  for(std::list<sweaponskill*>::iterator i = SWeaponSkill.begin(); i != SWeaponSkill.end();)
  {
    boneidmap::iterator BI = game::GetBoneItemIDMap().find(-(*i)->GetID());

    if(BI == game::GetBoneItemIDMap().end())
    {
      std::list<sweaponskill*>::iterator Dirt = i++;
      SWeaponSkill.erase(Dirt);
    }
    else
    {
      (*i)->SetID(BI->second);
      ++i;
    }
  }
}



void humanoid::EnsureCurrentSWeaponSkillIsCorrect(sweaponskill*& Skill, citem* Wielded)
{
  if(Wielded)
  {
    if(!Skill || !Skill->IsSkillOf(Wielded))
    {
      if(Skill)
  EnsureCurrentSWeaponSkillIsCorrect(Skill, 0);

      std::list<sweaponskill*>::iterator i;

      for(i = SWeaponSkill.begin(); i != SWeaponSkill.end(); ++i)
  if((*i)->IsSkillOf(Wielded))
  {
    Skill = *i;
    return;
  }

      for(idholder* I = Wielded->GetCloneMotherID(); I; I = I->Next)
  for(i = SWeaponSkill.begin(); i != SWeaponSkill.end(); ++i)
    if((*i)->IsSkillOfCloneMother(Wielded, I->ID))
    {
      Skill = new sweaponskill(**i);
      Skill->SetID(Wielded->GetID());
      SWeaponSkill.push_back(Skill);
      return;
    }

      Skill = new sweaponskill(Wielded);
      SWeaponSkill.push_back(Skill);
    }
  }
  else if(Skill)
  {
    if(!Skill->GetHits() && (CurrentRightSWeaponSkill != Skill || CurrentLeftSWeaponSkill != Skill))
      for(std::list<sweaponskill*>::iterator i = SWeaponSkill.begin(); i != SWeaponSkill.end(); ++i)
  if(*i == Skill)
  {
    delete *i;
    SWeaponSkill.erase(i);
    break;
  }

    Skill = 0;
  }
}



humanoid::~humanoid()
{
  for(std::list<sweaponskill*>::iterator i = SWeaponSkill.begin(); i != SWeaponSkill.end(); ++i)
    delete *i;
}



int humanoid::GetSumOfAttributes() const
{
  return character::GetSumOfAttributes() + GetAttribute(LEG_STRENGTH) + GetAttribute(DEXTERITY) ;
}



truth humanoid::CheckConsume(cfestring& Verb) const
{
  if(!HasHead())
  {
    if(IsPlayer())
      ADD_MESSAGE("You need a head to %s.", Verb.CStr());

    return false;
  }

  return character::CheckConsume(Verb);
}



truth humanoid::CanConsume(material* Material) const
{
  return character::CanConsume(Material) && HasHead();
}



void humanoid::StayOn(liquid* Liquid)
{
  if(IsFlying())
    return;

  truth Standing = false;

  if(GetRightLeg())
  {
    GetRightLeg()->StayOn(Liquid);
    Standing = true;
  }

  if(IsEnabled() && GetLeftLeg())
  {
    GetLeftLeg()->StayOn(Liquid);
    Standing = true;
  }

  if(!Standing)
  {
    bodypart* BodyPart[MAX_BODYPARTS];
    int Index = 0;

    for(int c = 0; c < BodyParts; ++c)
      if(GetBodyPart(c))
  BodyPart[Index++] = GetBodyPart(c);

    BodyPart[RAND() % Index]->StayOn(Liquid);
  }
}



character* humanoid::CreateZombie() const
{
  if(!TorsoIsAlive())
    return 0;

  humanoid* Zombie = zombie::Spawn();
  int c;

  for(c = 0; c < BodyParts; ++c)
  {
    bodypart* BodyPart = GetBodyPart(c);

    if(!BodyPart)
    {
      BodyPart = SearchForOriginalBodyPart(c);

      if(BodyPart)
      {
  BodyPart->RemoveFromSlot();
  BodyPart->SendToHell();
      }
    }

    if(BodyPart)
    {
      bodypart* ZombieBodyPart = Zombie->GetBodyPart(c);

      if(!ZombieBodyPart)
  ZombieBodyPart = Zombie->CreateBodyPart(c);

      material* M = BodyPart->GetMainMaterial()->Duplicate();
      M->SetSpoilCounter(2000 + RAND() % 1000);
      M->SetSkinColor(Zombie->GetSkinColor());
      ZombieBodyPart->ChangeMainMaterial(M);
      ZombieBodyPart->CopyAttributes(BodyPart);
    }
    else if(!Zombie->BodyPartIsVital(c))
    {
      bodypart* ZombieBodyPart = Zombie->GetBodyPart(c);

      if(ZombieBodyPart)
      {
  ZombieBodyPart->RemoveFromSlot();
  ZombieBodyPart->SendToHell();
      }
    }
  }

  for(c = 0; c < Zombie->AllowedWeaponSkillCategories; ++c)
    Zombie->CWeaponSkill[c] = CWeaponSkill[c];

  Zombie->SWeaponSkill.resize(SWeaponSkill.size());
  std::list<sweaponskill*>::iterator i1 = Zombie->SWeaponSkill.begin();
  std::list<sweaponskill*>::const_iterator i2 = SWeaponSkill.begin();

  for(; i2 != SWeaponSkill.end(); ++i1, ++i2)
    *i1 = new sweaponskill(**i2);

  memmove(Zombie->BaseExperience,
   BaseExperience,
   BASE_ATTRIBUTES * sizeof(*BaseExperience));
  Zombie->CalculateAll();
  Zombie->RestoreHP();
  Zombie->RestoreStamina();
  static_cast<zombie*>(Zombie)->SetDescription(GetZombieDescription());
  Zombie->GenerationDanger = GenerationDanger;
  return Zombie;
}



void humanoid::LeprosyHandler()
{
  if(IsImmuneToLeprosy())
  {
    return;
  }

  if(!RAND_N(1000 * GetAttribute(ENDURANCE)))
    DropRandomNonVitalBodypart();

  if(!game::IsInWilderness())
  {
    for(int d = 0; d < GetNeighbourSquares(); ++d)
    {
      lsquare* Square = GetNeighbourLSquare(d);

      if(Square && Square->GetCharacter())
  Square->GetCharacter()->TryToInfectWithLeprosy(this);
    }
  }

  character::LeprosyHandler();
}



void humanoid::DropRandomNonVitalBodypart()
{
  int BodyPartIndexToDrop = GetRandomNonVitalBodyPart();

  if(BodyPartIndexToDrop != NONE_INDEX)
    DropBodyPart(BodyPartIndexToDrop);
}



void humanoid::DropBodyPart(int Index)
{
  if(!GetBodyPart(Index)->IsAlive())
    return;

  festring NameOfDropped = GetBodyPart(Index)->GetBodyPartName();
  item* Dropped = SevereBodyPart(Index);

  if(Dropped)
  {
    GetStack()->AddItem(Dropped);
    Dropped->DropEquipment();

    if(IsPlayer())
    {
      ADD_MESSAGE("You feel very ill. Your %s snaps off.", NameOfDropped.CStr());
      game::AskForEscPress(CONST_S("Bodypart severed!"));
      DeActivateVoluntaryAction();
    }
    else if(CanBeSeenByPlayer())
      ADD_MESSAGE("Suddenly %s's %s snaps off.", CHAR_NAME(DEFINITE), NameOfDropped.CStr());
  }
  else
  {
    if(IsPlayer())
    {
      ADD_MESSAGE("You feel very ill. Your %s disappears.", NameOfDropped.CStr());
      game::AskForEscPress(CONST_S("Bodypart destroyed!"));
      DeActivateVoluntaryAction();
    }
    else if(CanBeSeenByPlayer())
      ADD_MESSAGE("Suddenly %s's %s disappears.", CHAR_NAME(DEFINITE), NameOfDropped.CStr());
  }
}



void humanoid::DuplicateEquipment(character* Receiver, feuLong Flags)
{
  character::DuplicateEquipment(Receiver, Flags);
  EnsureCurrentSWeaponSkillIsCorrect(CurrentRightSWeaponSkill, GetRightWielded());
  EnsureCurrentSWeaponSkillIsCorrect(CurrentLeftSWeaponSkill, GetLeftWielded());
}



int humanoid::GetAttributeAverage() const
{
  return GetSumOfAttributes() / 9;
}



truth humanoid::CanVomit() const
{
  return HasHead() && character::CanVomit();
}



truth humanoid::CheckApply() const
{
  if(!character::CheckApply())
    return false;

  if(!HasAUsableArm())
  {
    ADD_MESSAGE("You need a usable arm to apply.");
    return false;
  }

  return true;
}



truth humanoid::IsTransparent() const
{
  return character::IsTransparent() || !(GetRightLeg() || GetLeftLeg());
}



void humanoid::ModifySituationDanger(double& Danger) const
{
  character::ModifySituationDanger(Danger);

  switch(GetUsableArms())
  {
   case 0: Danger *= 10;
   case 1: Danger *= 2;
  }

  switch(GetUsableLegs())
  {
   case 0: Danger *= 10;
   case 1: Danger *= 2;
  }
}



int humanoid::RandomizeTryToUnStickBodyPart(feuLong PossibleBodyParts) const
{
  int Possible = 0, PossibleArray[3];

  if(RightArmIsUsable() && 1 << RIGHT_ARM_INDEX & PossibleBodyParts)
    PossibleArray[Possible++] = RIGHT_ARM_INDEX;

  if(LeftArmIsUsable() && 1 << LEFT_ARM_INDEX & PossibleBodyParts)
    PossibleArray[Possible++] = LEFT_ARM_INDEX;

  if(Possible)
    return PossibleArray[RAND_N(Possible)];

  if(RightLegIsUsable() && 1 << RIGHT_LEG_INDEX & PossibleBodyParts)
    PossibleArray[Possible++] = RIGHT_LEG_INDEX;

  if(LeftLegIsUsable() && 1 << LEFT_LEG_INDEX & PossibleBodyParts)
    PossibleArray[Possible++] = LEFT_LEG_INDEX;

  if(Possible)
    return PossibleArray[RAND_N(Possible)];

  if(GetHead() && 1 << HEAD_INDEX & PossibleBodyParts)
    return HEAD_INDEX;

  if(GetGroin() && 1 << GROIN_INDEX & PossibleBodyParts)
    PossibleArray[Possible++] = GROIN_INDEX;

  if(1 << TORSO_INDEX & PossibleBodyParts)
    PossibleArray[Possible++] = TORSO_INDEX;

  return Possible ? PossibleArray[RAND_N(Possible)] : NONE_INDEX;
}



truth humanoid::HasAUsableArm() const
{
  arm* R = GetRightArm(), * L = GetLeftArm();
  return (R && R->IsUsable()) || (L && L->IsUsable());
}



truth humanoid::HasAUsableLeg() const
{
  leg* R = GetRightLeg(), * L = GetLeftLeg();
  return (R && R->IsUsable()) || (L && L->IsUsable());
}



truth humanoid::HasTwoUsableLegs() const
{
  leg* R = GetRightLeg(), * L = GetLeftLeg();
  return R && R->IsUsable() && L && L->IsUsable();
}



truth humanoid::CanAttackWithAnArm() const
{
  arm* R = GetRightArm();

  if(R && R->GetDamage())
    return true;

  arm* L = GetLeftArm();
  return L && L->GetDamage();
}



truth humanoid::RightArmIsUsable() const
{
  arm* A = GetRightArm();
  return A && A->IsUsable();
}



truth humanoid::LeftArmIsUsable() const
{
  arm* A = GetLeftArm();
  return A && A->IsUsable();
}



truth humanoid::RightLegIsUsable() const
{
  leg* L = GetRightLeg();
  return L && L->IsUsable();
}



truth humanoid::LeftLegIsUsable() const
{
  leg* L = GetLeftLeg();
  return L && L->IsUsable();
}



truth humanoid::AllowUnconsciousness() const
{
  return (DataBase->AllowUnconsciousness && TorsoIsAlive()
    && BodyPartIsVital(HEAD_INDEX));
}



truth humanoid::CanChokeOnWeb(web* Web) const
{
  return CanChoke() && Web->IsStuckToBodyPart(HEAD_INDEX);
}



truth humanoid::BrainsHurt() const
{
  head* Head = GetHead();
  return !Head || Head->IsBadlyHurt();
}



cchar* humanoid::GetRunDescriptionLine(int I) const
{
  if(!GetRunDescriptionLineOne().IsEmpty())
    return !I ? GetRunDescriptionLineOne().CStr() : GetRunDescriptionLineTwo().CStr();

  if(IsFlying())
    return !I ? "Flying" : "very fast";

  if(IsSwimming())
  {
    if(!GetRightArm() && !GetLeftArm() && !GetRightLeg() && !GetLeftLeg())
      return !I ? "Floating" : "ahead fast";
    else
      return !I ? "Swimming" : "very fast";
  }

  if(!GetRightLeg() && !GetLeftLeg())
    return !I ? "Rolling" : "very fast";

  if(!GetRightLeg() || !GetLeftLeg())
    return !I ? "Hopping" : "very fast";

  return !I ? "Running" : "";
}



cchar* humanoid::GetNormalDeathMessage() const
{
  if(BodyPartIsVital(HEAD_INDEX) && (!GetHead() || GetHead()->GetHP() <= 0))
    return "beheaded @k";
  else if(BodyPartIsVital(GROIN_INDEX) && (!GetGroin() || GetGroin()->GetHP() <= 0))
    return "killed @bkp dirty attack below the belt";
  else
    return "killed @k";
}



void humanoid::ApplySpecialAttributeBonuses()
{
  if(GetHead())
  {
    AttributeBonus[CHARISMA] -= GetHead()->
        CalculateScarAttributePenalty(GetAttribute(CHARISMA, false));
  }
  else
    AttributeBonus[CHARISMA] -= GetAttribute(CHARISMA, false) - 1;
}



truth humanoid::MindWormCanPenetrateSkull(mindworm* Worm) const
{
  if(GetHelmet())
  {
    if(RAND_N(102) > GetHelmet()->GetCoverPercentile())
    {
      return RAND_2;
    }
  }

  return RAND_2;
}



truth humanoid::HasSadistWeapon() const
{
  arm* Right = GetRightArm(), * Left = GetLeftArm();
  return (Right && Right->HasSadistWeapon()) || (Left && Left->HasSadistWeapon());
}



truth humanoid::HasSadistAttackMode() const
{
  return HasSadistWeapon() || IsUsingLegs();
}



truth humanoid::SpecialBiteEffect (character *Char, v2 HitPos, int BodyPartIndex, int Direction, truth BlockedByArmour) {
  if (StateIsActivated(VAMPIRISM)) {
    if (!BlockedByArmour && Char->IsWarm() && !(RAND()%2)) {
      if (Char->IsHumanoid()) Char->BeginTemporaryState(VAMPIRISM, 1000+RAND_N(500)); // Randomly instigate vampirism
      if (Char->IsPlayer() || IsPlayer() || Char->CanBeSeenByPlayer() || CanBeSeenByPlayer()) {
        ADD_MESSAGE("%s drains some precious lifeblood from %s!", CHAR_DESCRIPTION(DEFINITE), Char->CHAR_DESCRIPTION(DEFINITE));
      }
      return Char->ReceiveBodyPartDamage(this, 8+(RAND()%9), DRAIN, BodyPartIndex, Direction);
    }
  }
  return false;
}
#endif
