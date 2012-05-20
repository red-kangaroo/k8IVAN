#ifdef HEADER_PHASE
ITEM(scrollofhardenmaterial, scroll)
{
 public:
  virtual void FinishReading(character*);
};


#else



void scrollofhardenmaterial::FinishReading(character* Reader)
{
  for(;;)
  {
    itemvector Item;

    if(!Reader->SelectFromPossessions(Item, CONST_S("What item do you wish to harden?"), NO_MULTI_SELECT|SELECT_PAIR, &item::CanBeHardened))
    {
      ADD_MESSAGE("You notice you haven't got anything to harden.");
      return;
    }

    if (Item.empty()) {
      if(game::TruthQuestion(CONST_S("Really cancel read? [y/N]"))) return;
      continue;
    }

    if(!Item[0]->IsMaterialChangeable())
    {
      ADD_MESSAGE("You cast the spell, but the magic is not powerful enough to affect %s!", Item[0]->CHAR_NAME(DEFINITE|(Item.size() == 1 ? 0 : PLURAL)));
      break;
    }

    if(Item[0]->HandleInPairs() && Item.size() == 1)
    {
      ADD_MESSAGE("Only one %s will be altered.", Item[0]->CHAR_NAME(UNARTICLED));

      if(!game::TruthQuestion(CONST_S("Still continue? [y/N]")))
  continue;
    }

    msgsystem::EnterBigMessageMode();

    if(Item.size() == 1)
      ADD_MESSAGE("Suddenly your %s is consumed in roaring magical flames.", Item[0]->CHAR_NAME(UNARTICLED));
    else
      ADD_MESSAGE("Suddenly your %s are consumed in roaring magical flames.", Item[0]->CHAR_NAME(PLURAL));

    int Config = Item[0]->GetMainMaterial()->GetHardenedMaterial(Item[0]);

    if(!Config)
    {
      /* Should not be possible */

      if(Item.size() == 1)
  ADD_MESSAGE("But it is already as hard as it can get.");
      else
  ADD_MESSAGE("But they are already as hard as they can get.");

      msgsystem::LeaveBigMessageMode();
      break;
    }

    material* TempMaterial = MAKE_MATERIAL(Config);
    int Intelligence = Reader->GetAttribute(INTELLIGENCE);

    if(TempMaterial->GetIntelligenceRequirement() > Intelligence && !game::WizardModeIsActive())
    {
      delete TempMaterial;
      ADD_MESSAGE("But your mind is not yet strong enough to harden %s.", Item.size() == 1 ? "it" : "them");
      msgsystem::LeaveBigMessageMode();
      continue;
    }

    for(int NewConfig = TempMaterial->GetHardenedMaterial(Item[0]), c = 1;
  NewConfig;
  NewConfig = TempMaterial->GetHardenedMaterial(Item[0]), ++c)
    {
      material* NewMaterial = MAKE_MATERIAL(NewConfig);

      if(NewMaterial->GetIntelligenceRequirement()
   <= Intelligence - c * 5)
      {
  delete TempMaterial;
  TempMaterial = NewMaterial;
      }
      else
  break;
    }

    material* MainMaterial = Item[0]->GetMainMaterial();
    material* SecondaryMaterial = Item[0]->GetSecondaryMaterial();

    if(Item.size() == 1)
    {
      ADD_MESSAGE("As the fire dies out it looks much harder.");

      if(SecondaryMaterial && SecondaryMaterial->IsSameAs(MainMaterial))
  Item[0]->ChangeSecondaryMaterial(TempMaterial->SpawnMore());

      Item[0]->ChangeMainMaterial(TempMaterial);
    }
    else
    {
      ADD_MESSAGE("As the fire dies out they look much harder.");

      if(SecondaryMaterial && SecondaryMaterial->IsSameAs(MainMaterial))
  for(uInt c = 0; c < Item.size(); ++c)
    Item[c]->ChangeSecondaryMaterial(TempMaterial->SpawnMore());

      Item[0]->ChangeMainMaterial(TempMaterial);

      for(uInt c = 1; c < Item.size(); ++c)
  Item[c]->ChangeMainMaterial(TempMaterial->SpawnMore());
    }

    msgsystem::LeaveBigMessageMode();
    break;
  }

  RemoveFromSlot();
  SendToHell();
  Reader->EditExperience(INTELLIGENCE, 300, 1 << 12);
}
#endif
