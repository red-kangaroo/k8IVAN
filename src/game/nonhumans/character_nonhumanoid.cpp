#ifdef HEADER_PHASE
CHARACTER(nonhumanoid, character)
{
 public:
  virtual void Save (outputfile&) const;
  virtual void Load (inputfile&);
  void CalculateUnarmedDamage ();
  void CalculateKickDamage ();
  void CalculateBiteDamage ();
  void CalculateUnarmedToHitValue ();
  void CalculateKickToHitValue ();
  void CalculateBiteToHitValue ();
  void CalculateUnarmedAPCost ();
  void CalculateKickAPCost ();
  void CalculateBiteAPCost ();
  double GetUnarmedDamage () const { return UnarmedDamage; }
  int GetUnarmedMinDamage () const;
  int GetUnarmedMaxDamage () const;
  double GetKickDamage () const { return KickDamage; }
  int GetKickMinDamage () const;
  int GetKickMaxDamage () const;
  double GetBiteDamage () const { return BiteDamage; }
  int GetBiteMinDamage () const;
  int GetBiteMaxDamage () const;
  double GetUnarmedToHitValue () const { return UnarmedToHitValue; }
  double GetKickToHitValue () const { return KickToHitValue; }
  double GetBiteToHitValue () const { return BiteToHitValue; }
  sLong GetUnarmedAPCost () const { return UnarmedAPCost; }
  sLong GetKickAPCost () const { return KickAPCost; }
  sLong GetBiteAPCost () const { return BiteAPCost; }
  virtual void Kick (lsquare*, int, truth = false);
  virtual truth Hit (character*, v2, int, int = 0);
  virtual void UnarmedHit (character*, v2, int, truth = false);
  virtual void InitSpecialAttributes ();
  virtual double GetTimeToKill (ccharacter*, truth) const;
  virtual int GetAttribute (int, truth = true) const;
  virtual truth EditAttribute (int, int);
  virtual void EditExperience (int, double, double);
  virtual int DrawStats (truth) const;
  virtual void Bite (character*, v2, int, truth = false);
  virtual int GetCarryingStrength () const;
  virtual void CalculateBattleInfo ();
  void CalculateUnarmedAttackInfo ();
  void CalculateKickAttackInfo ();
  void CalculateBiteAttackInfo ();
  virtual truth UseMaterialAttributes () const;
  virtual void AddSpecialStethoscopeInfo (felist &) const;
  virtual truth EditAllAttributes (int);
  virtual void AddAttributeInfo (festring &) const;
  virtual void AddAttackInfo (felist &) const;
 protected:
  double StrengthExperience;
  double AgilityExperience;
  double UnarmedDamage;
  double KickDamage;
  double BiteDamage;
  double UnarmedToHitValue;
  double KickToHitValue;
  double BiteToHitValue;
  sLong UnarmedAPCost;
  sLong KickAPCost;
  sLong BiteAPCost;
};


#else


int nonhumanoid::GetUnarmedMinDamage () const { return int(UnarmedDamage*0.75); }
int nonhumanoid::GetUnarmedMaxDamage () const { return int(UnarmedDamage*1.25+1); }
int nonhumanoid::GetKickMinDamage () const { return int(KickDamage*0.75); }
int nonhumanoid::GetKickMaxDamage () const { return int(KickDamage*1.25+1); }
int nonhumanoid::GetBiteMinDamage () const { return int(BiteDamage*0.75); }
int nonhumanoid::GetBiteMaxDamage () const { return int(BiteDamage*1.25+1); }
int nonhumanoid::GetCarryingStrength () const { return (Max(GetAttribute(LEG_STRENGTH), 1)<<1)+CarryingBonus; }
truth nonhumanoid::UseMaterialAttributes () const { return GetTorso()->UseMaterialAttributes(); }


void nonhumanoid::Save (outputfile& SaveFile) const {
  character::Save(SaveFile);
  SaveFile << StrengthExperience << AgilityExperience;
}


void nonhumanoid::Load (inputfile &SaveFile) {
  character::Load(SaveFile);
  SaveFile >> StrengthExperience >> AgilityExperience;
}


void nonhumanoid::CalculateUnarmedDamage () {
  UnarmedDamage = sqrt(5e-12*GetAttribute(ARM_STRENGTH))*GetBaseUnarmedStrength()*GetCWeaponSkill(UNARMED)->GetBonus();
}


void nonhumanoid::CalculateUnarmedToHitValue () {
  UnarmedToHitValue = GetAttribute(DEXTERITY)*sqrt(2.5*GetAttribute(PERCEPTION))*GetCWeaponSkill(UNARMED)->GetBonus()*GetMoveEase()/500000;
}


void nonhumanoid::CalculateUnarmedAPCost () {
  UnarmedAPCost = Max(sLong(10000000000.0/(APBonus(GetAttribute(DEXTERITY))*GetMoveEase()*GetCWeaponSkill(UNARMED)->GetBonus())), 100);
}


void nonhumanoid::CalculateKickDamage () {
  KickDamage = sqrt(5e-12*GetAttribute(LEG_STRENGTH))*GetBaseKickStrength()*GetCWeaponSkill(KICK)->GetBonus();
}


void nonhumanoid::CalculateKickToHitValue () {
  KickToHitValue = GetAttribute(AGILITY)*sqrt(2.5*GetAttribute(PERCEPTION))*GetCWeaponSkill(KICK)->GetBonus()*GetMoveEase()/1000000;
}


void nonhumanoid::CalculateKickAPCost () {
  KickAPCost = Max(sLong(20000000000.0/(APBonus(GetAttribute(AGILITY))*GetMoveEase()*GetCWeaponSkill(KICK)->GetBonus())), 1000);
}


void nonhumanoid::CalculateBiteDamage () {
  BiteDamage = sqrt(5e-12*GetAttribute(ARM_STRENGTH))*GetBaseBiteStrength()*GetCWeaponSkill(BITE)->GetBonus();
}


void nonhumanoid::CalculateBiteToHitValue () {
  BiteToHitValue = GetAttribute(AGILITY)*sqrt(2.5*GetAttribute(PERCEPTION))*GetCWeaponSkill(BITE)->GetBonus()*GetMoveEase()/1000000;
}


void nonhumanoid::CalculateBiteAPCost () {
  BiteAPCost = Max(sLong(10000000000.0/(APBonus(GetAttribute(DEXTERITY))*GetMoveEase()*GetCWeaponSkill(BITE)->GetBonus())), 100);
}


void nonhumanoid::InitSpecialAttributes () {
  StrengthExperience = GetNaturalExperience(ARM_STRENGTH);
  AgilityExperience = GetNaturalExperience(AGILITY);
  LimitRef(StrengthExperience, MIN_EXP, MAX_EXP);
  LimitRef(AgilityExperience, MIN_EXP, MAX_EXP);
}


void nonhumanoid::Bite (character *Enemy, v2 HitPos, int Direction, truth ForceHit) {
  EditNP(-50);
  EditAP(-GetBiteAPCost());
  EditExperience(ARM_STRENGTH, 75, 1<<8);
  EditExperience(AGILITY, 150, 1<<8);
  EditStamina(-10000/GetAttribute(ARM_STRENGTH), false);
  Enemy->TakeHit(this, 0, GetTorso(), HitPos, GetBiteDamage(), GetBiteToHitValue(), RAND()%26-RAND()%26, BITE_ATTACK, Direction, !(RAND()%GetCriticalModifier()), ForceHit);
}


void nonhumanoid::Kick (lsquare *Square, int Direction, truth ForceHit) {
  EditNP(-50);
  EditAP(-GetKickAPCost());
  EditStamina(-10000/GetAttribute(ARM_STRENGTH), false);
  if (Square->BeKicked(this, 0, GetTorso(), GetKickDamage(), GetKickToHitValue(), RAND()%26-RAND()%26, Direction, !(RAND()%GetCriticalModifier()), ForceHit)) {
    EditExperience(LEG_STRENGTH, 150, 1<<8);
    EditExperience(AGILITY, 75, 1<<8);
  }
}


truth nonhumanoid::Hit (character *Enemy, v2 HitPos, int Direction, int Flags) {
  if (CheckIfTooScaredToHit(Enemy)) return false;
  if (IsPlayer()) {
    if (!(Enemy->IsMasochist() && GetRelation(Enemy) == FRIEND) && GetRelation(Enemy) != HOSTILE && !game::TruthQuestion(CONST_S("This might cause a hostile reaction. Are you sure? [y/N]"))) return false;
  } else if (GetAttribute(WISDOM) >= Enemy->GetAttackWisdomLimit()) {
    return false;
  }

  if (GetBurdenState() == OVER_LOADED) {
    if (IsPlayer()) ADD_MESSAGE("You cannot fight while carrying so much.");
    return false;
  }

  /* Behold this Terrible Father of Gum Solutions! */
  int AttackStyle = GetAttackStyle();
  if (AttackStyle&USE_LEGS) {
    room *Room = GetNearLSquare(HitPos)->GetRoom();
    if (Room && !Room->AllowKick(this, GetNearLSquare(HitPos))) AttackStyle &= ~USE_LEGS;
  }
  int c, AttackStyles;
  for (c = 0, AttackStyles = 0; c < 8; ++c) if (AttackStyle&(1<<c)) ++AttackStyles;
  int Chosen = RAND()%AttackStyles;
  for (c = 0, AttackStyles = 0; c < 8; ++c) {
    if (AttackStyle&(1<<c) && AttackStyles++ == Chosen) {
      Chosen = 1<<c;
      break;
    }
  }
  switch (Chosen) {
    case USE_ARMS:
      msgsystem::EnterBigMessageMode();
      Hostility(Enemy);
      UnarmedHit(Enemy, HitPos, Direction, Flags&SADIST_HIT);
      msgsystem::LeaveBigMessageMode();
      return true;
    case USE_LEGS:
      msgsystem::EnterBigMessageMode();
      Hostility(Enemy);
      Kick(GetNearLSquare(HitPos), Direction, Flags&SADIST_HIT);
      msgsystem::LeaveBigMessageMode();
      return true;
    case USE_HEAD:
      msgsystem::EnterBigMessageMode();
      Hostility(Enemy);
      Bite(Enemy, HitPos, Direction, Flags&SADIST_HIT);
      msgsystem::LeaveBigMessageMode();
      return true;
    default:
      ABORT("Strange alien attack style requested!");
      return false;
   }
}


void nonhumanoid::UnarmedHit (character *Enemy, v2 HitPos, int Direction, truth ForceHit) {
  EditNP(-50);
  EditAP(-GetUnarmedAPCost());
  EditStamina(-10000/GetAttribute(ARM_STRENGTH), false);
  switch (Enemy->TakeHit(this, 0, GetTorso(), HitPos, GetUnarmedDamage(), GetUnarmedToHitValue(), RAND()%26-RAND()%26, UNARMED_ATTACK, Direction, !(RAND()%GetCriticalModifier()), ForceHit)) {
    case HAS_HIT:
    case HAS_BLOCKED:
    case HAS_DIED:
    case DID_NO_DAMAGE:
      EditExperience(ARM_STRENGTH, 150, 1<<8);
      //k8bug: forgotten 'break' here?
    case HAS_DODGED:
      EditExperience(DEXTERITY, 75, 1<<8);
  }
}



double nonhumanoid::GetTimeToKill (ccharacter *Enemy, truth UseMaxHP) const {
  double Effectivity = 0;
  int AttackStyles = 0;
  if (IsUsingArms()) {
    Effectivity += 1/(Enemy->GetTimeToDie(this, int(GetUnarmedDamage())+1, GetUnarmedToHitValue(), AttackIsBlockable(UNARMED_ATTACK), UseMaxHP)*GetUnarmedAPCost());
    ++AttackStyles;
  }
  if (IsUsingLegs()) {
    Effectivity += 1/(Enemy->GetTimeToDie(this, int(GetKickDamage())+1, GetKickToHitValue(), AttackIsBlockable(KICK_ATTACK), UseMaxHP)*GetKickAPCost());
    ++AttackStyles;
  }
  if (IsUsingHead()) {
    Effectivity += 1/(Enemy->GetTimeToDie(this, int(GetBiteDamage())+1, GetBiteToHitValue(), AttackIsBlockable(BITE_ATTACK), UseMaxHP)*GetBiteAPCost());
    ++AttackStyles;
  }
  if (StateIsActivated(HASTE)) Effectivity *= 2;
  if (StateIsActivated(SLOW)) Effectivity /= 2;
  return AttackStyles/Effectivity;
}


int nonhumanoid::GetAttribute (int Identifier, truth AllowBonus) const {
  if (Identifier < BASE_ATTRIBUTES) {
    return character::GetAttribute(Identifier, AllowBonus);
  } else if (Identifier == ARM_STRENGTH || Identifier == LEG_STRENGTH) {
    if (!UseMaterialAttributes()) return int(StrengthExperience*EXP_DIVISOR);
    return GetTorso()->GetMainMaterial()->GetStrengthValue();
  } else if (Identifier == DEXTERITY || Identifier == AGILITY) {
    if (!UseMaterialAttributes()) return int(AgilityExperience*EXP_DIVISOR);
    return (GetTorso()->GetMainMaterial()->GetFlexibility()<<2);
  } else {
    ABORT("Illegal nonhumanoid attribute %d request!", Identifier);
    return 0xABBE;
  }
}


truth nonhumanoid::EditAttribute (int Identifier, int Value) {
  if (Identifier < BASE_ATTRIBUTES) return character::EditAttribute(Identifier, Value);
  if (Identifier == ARM_STRENGTH || Identifier == LEG_STRENGTH) return !UseMaterialAttributes() && RawEditAttribute(StrengthExperience, Value);
  if (Identifier == DEXTERITY || Identifier == AGILITY) return !UseMaterialAttributes() && RawEditAttribute(AgilityExperience, Value);
  ABORT("Illegal nonhumanoid attribute %d edit request!", Identifier);
  return false;
}


void nonhumanoid::EditExperience (int Identifier, double Value, double Speed) {
  if (!AllowExperience()) return;
  if (Identifier < BASE_ATTRIBUTES) {
    character::EditExperience(Identifier, Value, Speed);
  } else if (Identifier == ARM_STRENGTH || Identifier == LEG_STRENGTH) {
    if (!UseMaterialAttributes()) {
      int Change = RawEditExperience(StrengthExperience, GetNaturalExperience(ARM_STRENGTH), Value, Speed/2);
      if (Change) {
        cchar *Adj = (Change > 0 ? "stronger" : "weaker");
        if (IsPlayer()) ADD_MESSAGE("Your feel %s!", Adj);
        else if (IsPet() && CanBeSeenByPlayer()) ADD_MESSAGE("Suddenly %s looks %s.", CHAR_NAME(DEFINITE), Adj);
        CalculateBurdenState();
        CalculateBattleInfo();
      }
    }
  } else if (Identifier == DEXTERITY || Identifier == AGILITY) {
    if (!UseMaterialAttributes()) {
      int Change = RawEditExperience(AgilityExperience, GetNaturalExperience(AGILITY), Value, Speed/2);
      if (Change) {
        cchar *Adj = (Change > 0 ? "very agile" : "sluggish");
        if (IsPlayer()) ADD_MESSAGE("Your feel %s!", Adj);
        else if (IsPet() && CanBeSeenByPlayer()) ADD_MESSAGE("Suddenly %s looks %s.", CHAR_NAME(DEFINITE), Adj);
        CalculateBattleInfo();
      }
    }
  } else {
    ABORT("Illegal nonhumanoid attribute %d experience edit request!", Identifier);
  }
}


int nonhumanoid::DrawStats (truth AnimationDraw) const {
  if (AnimationDraw) return 3;
  int PanelPosX = RES.X-96, PanelPosY = 3;
  PrintAttribute("Str", ARM_STRENGTH, PanelPosX, PanelPosY++);
  PrintAttribute("Agi", AGILITY, PanelPosX, PanelPosY++);
  return PanelPosY;
}


void nonhumanoid::CalculateBattleInfo () {
  CalculateDodgeValue();
  CalculateUnarmedAttackInfo();
  CalculateKickAttackInfo();
  CalculateBiteAttackInfo();
}


void nonhumanoid::CalculateUnarmedAttackInfo () {
  CalculateUnarmedDamage();
  CalculateUnarmedToHitValue();
  CalculateUnarmedAPCost();
}


void nonhumanoid::CalculateKickAttackInfo () {
  CalculateKickDamage();
  CalculateKickToHitValue();
  CalculateKickAPCost();
}


void nonhumanoid::CalculateBiteAttackInfo () {
  CalculateBiteDamage();
  CalculateBiteToHitValue();
  CalculateBiteAPCost();
}


void nonhumanoid::AddSpecialStethoscopeInfo (felist &Info) const {
  Info.AddEntry(CONST_S("Strength: ")+GetAttribute(ARM_STRENGTH), LIGHT_GRAY);
  Info.AddEntry(CONST_S("Agility: ")+GetAttribute(AGILITY), LIGHT_GRAY);
}


truth nonhumanoid::EditAllAttributes (int Amount) {
  if (!Amount) return true;
  LimitRef(StrengthExperience += Amount*EXP_MULTIPLIER, MIN_EXP, MAX_EXP);
  LimitRef(AgilityExperience += Amount*EXP_MULTIPLIER, MIN_EXP, MAX_EXP);
  return
    character::EditAllAttributes(Amount) ||
    (Amount < 0 && (StrengthExperience != MIN_EXP || AgilityExperience != MIN_EXP)) ||
    (Amount > 0 && (StrengthExperience != MAX_EXP || AgilityExperience != MAX_EXP));
}



#ifdef WIZARD
void nonhumanoid::AddAttributeInfo (festring &Entry) const {
  Entry.Resize(45);
  Entry << GetAttribute(ARM_STRENGTH);
  Entry.Resize(48);
  Entry << "-  -  " << GetAttribute(AGILITY);
  character::AddAttributeInfo(Entry);
}


void nonhumanoid::AddAttackInfo (felist &List) const {
  festring Entry;
  if (IsUsingArms()) {
    Entry = CONST_S("   unarmed attack");
    Entry.Resize(50);
    Entry << GetUnarmedMinDamage() << '-' << GetUnarmedMaxDamage();
    Entry.Resize(60);
    Entry << int(GetUnarmedToHitValue());
    Entry.Resize(70);
    Entry << GetUnarmedAPCost();
    List.AddEntry(Entry, LIGHT_GRAY);
  }
  if (IsUsingLegs()) {
    Entry = CONST_S("   kick attack");
    Entry.Resize(50);
    Entry << GetKickMinDamage() << '-' << GetKickMaxDamage();
    Entry.Resize(60);
    Entry << int(GetKickToHitValue());
    Entry.Resize(70);
    Entry << GetKickAPCost();
    List.AddEntry(Entry, LIGHT_GRAY);
  }
  if (IsUsingHead()) {
    Entry = CONST_S("   bite attack");
    Entry.Resize(50);
    Entry << GetBiteMinDamage() << '-' << GetBiteMaxDamage();
    Entry.Resize(60);
    Entry << int(GetBiteToHitValue());
    Entry.Resize(70);
    Entry << GetBiteAPCost();
    List.AddEntry(Entry, LIGHT_GRAY);
  }
}

#else

void nonhumanoid::AddAttributeInfo (festring &) const {}
void nonhumanoid::AddAttackInfo (felist &) const {}

#endif


#endif
