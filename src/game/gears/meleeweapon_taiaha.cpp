#ifdef HEADER_PHASE
ITEM(taiaha, meleeweapon)
{
public:
  taiaha () : Charges(0), TimesUsed(0) {}

  //virtual truth HitEffect(character*, character*, v2, int, int, truth);
  //virtual void BlockEffect(character*, character*, item*, int Type);
  virtual truth AllowAlphaEverywhere () const { return true; }
  virtual void Save (outputfile &) const;
  virtual void Load (inputfile &);
  virtual void ChargeFully (character *) { TimesUsed = 0; }
  virtual truth IsAppliable(ccharacter *) const { return false; }
  virtual truth IsZappable (ccharacter *) const { return true; }
  virtual truth IsChargeable (ccharacter *) const { return true; }
  virtual truth ReceiveDamage (character *, int, int, int);
  virtual truth Zap (character *, v2, int);
  virtual void AddInventoryEntry (ccharacter *, festring &, int, truth) const; //this?
  virtual truth IsExplosive () const { return true; }

protected:
  virtual int GetClassAnimationFrames () const;
  virtual col16 GetOutlineColor (int) const;
  virtual alpha GetOutlineAlpha (int) const;
  virtual void PostConstruct ();
  void BreakEffect (character *, cfestring &);
  //feuLong GetSpecialParameters () const;

protected:
  int Charges;
  int TimesUsed;
};


#else


int taiaha::GetClassAnimationFrames () const { return !IsBroken() ? 128 : 1; }


void taiaha::Save (outputfile &SaveFile) const {
  meleeweapon::Save(SaveFile);
  SaveFile << TimesUsed << Charges;
}


void taiaha::Load (inputfile &SaveFile) {
  meleeweapon::Load(SaveFile);
  SaveFile >> TimesUsed >> Charges;
}


truth taiaha::Zap (character *Zapper, v2, int Direction) {
  if (Charges <= TimesUsed) {
    ADD_MESSAGE("Nothing happens.");
    return true;
  }
  //
  Zapper->EditExperience(PERCEPTION, 150, 1 << 10);
  int TaiahaBeamEffect = RAND()&3;
  //
  // Just hard-code this
  beamdata Beam (
    Zapper,
    CONST_S("killed by ") + GetName(INDEFINITE) + " zapped @bk",
    Zapper->GetPos(),
    GREEN, //was GetBeamColor()
    TaiahaBeamEffect ? ((RAND() & 2) ? BEAM_FIRE_BALL : BEAM_STRIKE ) : BEAM_LIGHTNING, //was GetBeamEffect()
    Direction,
    15, // 10 is the lowest beamrange out of the three
    0 //was GetSpecialParameters()
  );
  (GetLevel()->*level::GetBeam(!TaiahaBeamEffect))(Beam); // BeamStyle = !TaiahaBeamEffect;
  ++TimesUsed;
  return true;
}


// never piled
void taiaha::AddInventoryEntry (ccharacter *Viewer, festring& Entry, int, truth ShowSpecialInfo) const {
  AddName(Entry, INDEFINITE);
  if (ShowSpecialInfo) {
    Entry << " [\1W" << GetWeight() << "g\2, DAM \1Y" << GetBaseMinDamage() << "\2-\1Y" << GetBaseMaxDamage() << "\2";
    Entry << ", " << GetBaseToHitValueDescription();
    if (!IsBroken() && !IsWhip()) Entry << ", " << GetStrengthValueDescription();
    //
    int CWeaponSkillLevel = Viewer->GetCWeaponSkillLevel(this);
    int SWeaponSkillLevel = Viewer->GetSWeaponSkillLevel(this);
    //
    if (CWeaponSkillLevel || SWeaponSkillLevel) Entry << ", skill \1W" << CWeaponSkillLevel << '/' << SWeaponSkillLevel << "\2";
    if (TimesUsed == 1) Entry << ", zapped \1W1\2 time]";
    else if (TimesUsed) Entry << ", zapped \1W" << TimesUsed << "\2 times]";
    else Entry << "]";
  }
}


void taiaha::BreakEffect (character *Terrorist, cfestring &DeathMsg) {
  v2 Pos = GetPos();
  level *Level = GetLevel();
  //
  RemoveFromSlot();
  feuLong StackSize = Level->AddRadiusToSquareStack(Pos, 2); //hardcode, default is 2 for most wands, but zero for fireballs
  lsquare **SquareStack = Level->GetSquareStack();
  //
  for (unsigned int c = 0; c < StackSize; ++c) SquareStack[c]->RemoveFlags(IN_SQUARE_STACK);
  //
  fearray<lsquare *> Stack(SquareStack, StackSize);
  (Level->*level::GetBeamEffectVisualizer(PARTICLE_BEAM))(Stack, YELLOW); //beamstyle
  //
  beamdata Beam (
    Terrorist,
    DeathMsg,
    YOURSELF,
    0 //was GetSpecialParameters()
  );
  for (unsigned int c = 0; c < Stack.Size; ++c) (Stack[c]->*lsquare::GetBeamEffect(BEAM_FIRE_BALL))(Beam); // beam effect
  SendToHell(); //removes the taiaha from existence
}


truth taiaha::ReceiveDamage (character *Damager, int Damage, int Type, int) {
  if (Type & (FIRE|ENERGY|PHYSICAL_DAMAGE) && Damage && (Damage > 125 || !(RAND()%(250/Damage)))) {
    festring DeathMsg = CONST_S("killed by an explosion of ");
    //
    AddName(DeathMsg, INDEFINITE);
    if (Damager) DeathMsg << " caused @bk";
    if (CanBeSeenByPlayer()) ADD_MESSAGE("%s %s.", GetExtendedDescription().CStr(), GetBreakMsg().CStr());
    BreakEffect(Damager, DeathMsg);
    return true;
  }
  return false;
}


void taiaha::PostConstruct () {
  Charges = GetMinCharges()+RAND()%(GetMaxCharges()-GetMinCharges()+1);
  TimesUsed = 0;
  meleeweapon::PostConstruct();
}


alpha taiaha::GetOutlineAlpha (int Frame) const {
  if (!IsBroken()) {
    Frame &= 31;
    return Frame*(31-Frame)>>1;
  }
  return 255;
}


col16 taiaha::GetOutlineColor (int Frame) const {
  if (!IsBroken()) {
    switch ((Frame&127)>>5) {
     case 0: return BLUE;
     case 1: return GREEN;
     case 2: return RED;
     case 3: return YELLOW;
    }
  }
  return TRANSPARENT_COLOR;
}


#endif
