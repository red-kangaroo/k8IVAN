#ifdef HEADER_PHASE
ITEM(wand, item)
{
 public:
  virtual truth Apply(character*);
  virtual void Save(outputfile&) const;
  virtual void Load(inputfile&);
  virtual void ChargeFully(character*) { TimesUsed = 0; }
  virtual truth IsAppliable(ccharacter*) const { return true; }
  virtual truth IsZappable(ccharacter*) const { return true; }
  virtual truth IsChargeable(ccharacter*) const { return true; }
  virtual truth ReceiveDamage(character*, int, int, int);
  virtual truth Zap(character*, v2, int);
  virtual void AddInventoryEntry(ccharacter*, festring&, int, truth) const;
  virtual sLong GetPrice() const;
  virtual truth IsExplosive() const { return true; }
 protected:
  virtual void PostConstruct();
  void BreakEffect(character*, cfestring&);
  feuLong GetSpecialParameters() const;
  int Charges;
  int TimesUsed;
};


#else



sLong wand::GetPrice() const { return Charges > TimesUsed ? item::GetPrice() : 0; }



truth wand::Apply(character* Terrorist)
{
  if(Terrorist->IsPlayer() && !game::TruthQuestion(CONST_S("Are you sure you want to break ") + GetName(DEFINITE) + "?"))
    return false;

  if(Terrorist->IsPlayer())
    ADD_MESSAGE("You bend %s with all your strength.", CHAR_NAME(DEFINITE));
  else if(Terrorist->CanBeSeenByPlayer())
    ADD_MESSAGE("%s bends %s with all %s strength.", Terrorist->CHAR_NAME(DEFINITE), CHAR_NAME(INDEFINITE), Terrorist->CHAR_POSSESSIVE_PRONOUN);

  if(Terrorist->IsPlayer() || Terrorist->CanBeSeenByPlayer())
    ADD_MESSAGE("%s %s.", CHAR_NAME(DEFINITE), GetBreakMsg().CStr());

  BreakEffect(Terrorist, CONST_S("killed by ") + GetName(INDEFINITE) + " broken @bk");
  Terrorist->DexterityAction(5);
  return true;
}



void wand::Save(outputfile& SaveFile) const
{
  item::Save(SaveFile);
  SaveFile << TimesUsed << Charges;
}



void wand::Load(inputfile& SaveFile)
{
  item::Load(SaveFile);
  SaveFile >> TimesUsed >> Charges;
}



truth wand::ReceiveDamage(character* Damager, int Damage, int Type, int)
{
  if(Type & (FIRE|ENERGY|PHYSICAL_DAMAGE) && Damage && (Damage > 125 || !(RAND() % (250 / Damage))))
  {
    festring DeathMsg = CONST_S("killed by an explosion of ");
    AddName(DeathMsg, INDEFINITE);

    if(Damager)
      DeathMsg << " caused @bk";

    if(CanBeSeenByPlayer())
      ADD_MESSAGE("%s %s.", GetExtendedDescription().CStr(), GetBreakMsg().CStr());

    BreakEffect(Damager, DeathMsg);
    return true;
  }

  return false;
}



void wand::PostConstruct()
{
  Charges = GetMinCharges() + RAND() % (GetMaxCharges() - GetMinCharges() + 1);
  TimesUsed = 0;
}



truth wand::Zap(character* Zapper, v2, int Direction)
{
  if(Charges <= TimesUsed)
  {
    ADD_MESSAGE("Nothing happens.");
    return true;
  }

  Zapper->EditExperience(PERCEPTION, 150, 1 << 10);

  beamdata Beam
    (
      Zapper,
      CONST_S("killed by ") + GetName(INDEFINITE) + " zapped @bk",
      Zapper->GetPos(),
      GetBeamColor(),
      GetBeamEffect(),
      Direction,
      GetBeamRange(),
      GetSpecialParameters()
    );

  (GetLevel()->*level::GetBeam(GetBeamStyle()))(Beam);
  ++TimesUsed;
  return true;
}



void wand::AddInventoryEntry (ccharacter*, festring& Entry, int, truth ShowSpecialInfo) const { // never piled
  AddName(Entry, INDEFINITE);
  if (ShowSpecialInfo) {
    Entry << " [\1C" << GetWeight();
    if (TimesUsed == 1) Entry << "g\2, used \1C1\2 time]";
    else if (TimesUsed) Entry << "g\2, used \1C" << TimesUsed << "\2 times]";
    else Entry << "g\2]";
  }
}



void wand::BreakEffect(character* Terrorist, cfestring& DeathMsg)
{
  v2 Pos = GetPos();
  level* Level = GetLevel();
  RemoveFromSlot();
  feuLong StackSize = Level->AddRadiusToSquareStack(Pos, GetBreakEffectRangeSquare());
  lsquare** SquareStack = Level->GetSquareStack();
  feuLong c;

  for(c = 0; c < StackSize; ++c)
    SquareStack[c]->RemoveFlags(IN_SQUARE_STACK);

  fearray<lsquare*> Stack(SquareStack, StackSize);
  (Level->*level::GetBeamEffectVisualizer(GetBeamStyle()))(Stack, GetBeamColor());

  beamdata Beam
    (
      Terrorist,
      DeathMsg,
      YOURSELF,
      GetSpecialParameters()
    );

  for(c = 0; c < Stack.Size; ++c)
    (Stack[c]->*lsquare::GetBeamEffect(GetBeamEffect()))(Beam);

  SendToHell();
}



feuLong wand::GetSpecialParameters() const
{
  switch(GetConfig())
  {
   case WAND_OF_MIRRORING:
    return MIRROR_IMAGE|(1000 << LE_BASE_SHIFT)|(1000 << LE_RAND_SHIFT);
  }

  return 0;
}
#endif
