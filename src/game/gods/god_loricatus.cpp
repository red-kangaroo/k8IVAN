#ifdef HEADER_PHASE
GOD(loricatus, god)
{
 public:
  virtual cchar* GetName() const;
  virtual cchar* GetDescription() const;
  virtual int GetAlignment() const;
  virtual int GetBasicAlignment() const;
  virtual col16 GetColor() const;
  virtual col16 GetEliteColor() const;
  virtual int GetSex() const { return MALE; }
 protected:
  virtual void PrayGoodEffect();
  virtual void PrayBadEffect();
};


#else



cchar* loricatus::GetName() const { return "Loricatus"; }



cchar* loricatus::GetDescription() const { return "god of fire, machines and weaponry"; }



int loricatus::GetAlignment() const { return AN; }



int loricatus::GetBasicAlignment() const { return NEUTRAL; }



col16 loricatus::GetColor() const { return NEUTRAL_BASIC_COLOR; }



col16 loricatus::GetEliteColor() const { return NEUTRAL_ELITE_COLOR; }



void loricatus::PrayGoodEffect()
{
  if (!game::GetLoricatusHammer()) {
    if (GetRelation() == 1000) {
      ADD_MESSAGE("You hear a booming voice: \"Thou have succeeded the arts of blacksmithing, I aid ye well with a divine hammer\" A hammer appears from nothing.");
      PLAYER->GetGiftStack()->AddItem(loricatushammer::Spawn());
      game::SetLoricatusHammer(1);
    }
  }

  item* MainWielded = PLAYER->GetMainWielded();

  if(MainWielded)
  {
    if(MainWielded->IsMaterialChangeable() && MainWielded->GetMainMaterial()->GetAttachedGod() == GetType())
    {
      int Config = MainWielded->GetMainMaterial()->GetHardenedMaterial(MainWielded);

      if(Config)
      {
  int IR = material::GetDataBase(Config)->IntelligenceRequirement - GetRelation() / 50;

  if(IR <= 1 || !RAND_N(IR))
  {
    festring Desc;
    item* SecondaryWielded;

    if(MainWielded->HandleInPairs() && (SecondaryWielded = PLAYER->GetSecondaryWielded()) && SecondaryWielded->CanBePiledWith(MainWielded, PLAYER))
    {
      MainWielded->AddName(Desc, PLURAL);
      Desc << " glow and sparkle like they were";

      if(SecondaryWielded->GetSecondaryMaterial() && SecondaryWielded->GetSecondaryMaterial()->IsSameAs(MainWielded->GetMainMaterial()))
        SecondaryWielded->ChangeSecondaryMaterial(MAKE_MATERIAL(Config));

      SecondaryWielded->ChangeMainMaterial(MAKE_MATERIAL(Config));
    }
    else
    {
      MainWielded->AddName(Desc, UNARTICLED);
      Desc << " glows and sparkles like it was";
    }

    if(MainWielded->GetSecondaryMaterial() && MainWielded->GetSecondaryMaterial()->IsSameAs(MainWielded->GetMainMaterial()))
      MainWielded->ChangeSecondaryMaterial(MAKE_MATERIAL(Config));

    MainWielded->ChangeMainMaterial(MAKE_MATERIAL(Config));
    ADD_MESSAGE("Your %s reforged by invisible hands.", Desc.CStr());
    return;
  }
      }

      if(!(RAND() % 10))
      {
  item* Scroll = scrollofrepair::Spawn();
  ADD_MESSAGE("%s gives you %s.", GetName(), Scroll->CHAR_NAME(INDEFINITE));
  PLAYER->GetGiftStack()->AddItem(Scroll);
  return;
      }
      else
  ADD_MESSAGE("\"Mortal, thou art always my valiant knight!\"");
    }
  }

  for(int c = 0; c < PLAYER->GetEquipments(); ++c)
  {
    item* Equipment = PLAYER->GetEquipment(c);

    if(Equipment && Equipment->IsBroken())
    {
      ADD_MESSAGE("%s fixes your %s.", GetName(), Equipment->CHAR_NAME(UNARTICLED));
      Equipment->Fix();
      return;
    }
  }

  if(PLAYER->GetUsableArms())
    ADD_MESSAGE("You feel a slight tingling in your hands.");
  else
    ADD_MESSAGE("You feel a slight tingle.");
}



void loricatus::PrayBadEffect()
{
  item* MainWielded = PLAYER->GetMainWielded();

  if(MainWielded)
    if(MainWielded->IsMaterialChangeable())
    {
      festring Desc;
      item* SecondaryWielded;

      if(MainWielded->HandleInPairs() && (SecondaryWielded = PLAYER->GetSecondaryWielded()) && SecondaryWielded->CanBePiledWith(MainWielded, PLAYER))
      {
  MainWielded->AddName(Desc, PLURAL);
  Desc << " vibrate and soften";
  SecondaryWielded->ChangeMainMaterial(MAKE_MATERIAL(BANANA_FLESH));
      }
      else
      {
  MainWielded->AddName(Desc, UNARTICLED);
  Desc << " vibrates and softens";
      }

      MainWielded->ChangeMainMaterial(MAKE_MATERIAL(BANANA_FLESH));
      ADD_MESSAGE("Your %s.", Desc.CStr());
    }
    else
      ADD_MESSAGE("%s emits strange light but remain unchanged.", MainWielded->CHAR_NAME(DEFINITE));
  else
  {
    if(PLAYER->GetUsableArms())
      ADD_MESSAGE("You feel a slight tingling in your hands.");
    else
      ADD_MESSAGE("You feel a slight tingle.");
  }
}
#endif
