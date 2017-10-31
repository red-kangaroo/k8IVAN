#ifdef HEADER_PHASE
ITEM(oillamp, item)
{
 public:
  oillamp();
  oillamp(const oillamp&);
  virtual void Save(outputfile&) const;
  virtual void Load(inputfile&);
  virtual truth GetInhabitedByGenie() const { return InhabitedByGenie; }
  virtual void SetInhabitedByGenie(truth What) { InhabitedByGenie = What; }
  virtual truth Apply(character*);
  virtual truth IsAppliable(ccharacter*) const { return true; }
 protected:
  truth InhabitedByGenie;
};


#else



truth oillamp::Apply(character* Applier)
{
  if(Applier->IsPlayer())
    ADD_MESSAGE("You rub %s.", CHAR_NAME(DEFINITE));

  if(GetInhabitedByGenie())
  {
    genie* Genie = genie::Spawn();
    v2 TryToCreate;
    truth FoundPlace = false;

    if(RAND_N(5))
      Genie->SetTeam(Applier->GetTeam());
    else
      Genie->SetTeam(game::GetTeam(MONSTER_TEAM));

    /* First try to create a genie nearby (10 tries - if all of them fail then stop trying) */

    for(int c = 0; c < 10 && !FoundPlace; ++c)
    {
      TryToCreate = Applier->GetPos() + game::GetMoveVector(RAND() % DIRECTION_COMMAND_KEYS);

      if(GetArea()->IsValidPos(TryToCreate) && Genie->CanMoveOn(GetNearLSquare(TryToCreate)) && Genie->IsFreeForMe(GetNearLSquare(TryToCreate)))
      {
  Genie->PutTo(TryToCreate);
  FoundPlace = true;
  SetInhabitedByGenie(false);
  break;
      }
    }

    if(FoundPlace)
    {
      Genie->GetLSquareUnder()->AddSmoke(gas::Spawn(SMOKE, 1000));

      if(!Genie->IsPet())
  ADD_MESSAGE("You see a puff of smoke, and %s appears. \"For centuries I have been imprisoned in this lamp. But at last you have freed me! As a reward, I will kill you.\"", Genie->CHAR_NAME(INDEFINITE));
      else
      {
  if(Applier->IsPlayer())
  {
    ADD_MESSAGE("You see a puff of smoke, and %s appears. \"For centuries I have been imprisoned in this lamp. But at last you have freed me! I am deeply grateful. You deserve a generous reward. I may serve you for 1001 nights or grant you a wish. It's your choice.\"", Genie->CHAR_NAME(INDEFINITE));

    if(game::TruthQuestion(CONST_S("Do you want to wish?"), YES))
    {
      ADD_MESSAGE("You may wish for an item.");
      game::Wish(Applier,
           "%s appears from nothing and the genie flies happily away!",
           "Two %s appear from nothing and the genie flies happily away!");

      Genie->Remove();
      delete Genie;
      Applier->EditAP(-1000);
      return true;
    }
  }
      }

      meleeweapon* Weapon = meleeweapon::Spawn(TWO_HANDED_SCIMITAR, NO_MATERIALS);
      Weapon->InitMaterials(MAKE_MATERIAL(ARCANITE), MAKE_MATERIAL(ARCANITE), true);
      Genie->SetRightWielded(Weapon);
      ADD_MESSAGE("%s wishes for a two-handed scimitar. Suddenly %s appears from nothing and %s wields it.", Genie->CHAR_NAME(DEFINITE), Genie->GetMainWielded()->CHAR_NAME(INDEFINITE), Genie->CHAR_NAME(DEFINITE));
    }
    else
    {
      if(Applier->IsPlayer())
  ADD_MESSAGE("You feel that it is warmer.");

      delete Genie;
    }
  }
  else
    if(Applier->IsPlayer())
      ADD_MESSAGE("Nothing happens.");

  Applier->EditAP(-1000);
  return true;
}



void oillamp::Save(outputfile& SaveFile) const
{
  item::Save(SaveFile);
  SaveFile << InhabitedByGenie;
}



void oillamp::Load(inputfile& SaveFile)
{
  item::Load(SaveFile);
  SaveFile >> InhabitedByGenie;
}



oillamp::oillamp()
{
  if(!game::IsLoading())
    InhabitedByGenie = RAND_2;
}



oillamp::oillamp(const oillamp& Lamp) : mybase(Lamp), InhabitedByGenie(false)
{
}
#endif
