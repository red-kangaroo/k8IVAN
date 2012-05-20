#ifdef HEADER_PHASE
ITEM(scrollofchangematerial, scroll)
{
 public:
  virtual void FinishReading(character*);
};


#else



void scrollofchangematerial::FinishReading(character* Reader)
{
  for(;;)
  {
    itemvector Item;

    if(!Reader->SelectFromPossessions(Item, CONST_S("What item do you wish to change?"), NO_MULTI_SELECT|SELECT_PAIR, &item::MaterialIsChangeable))
    {
      ADD_MESSAGE("You notice you haven't got anything to alter.");
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

    festring Temp = game::DefaultQuestion(CONST_S("What material do you want to wish for?"),
            game::GetDefaultChangeMaterial());
    material* TempMaterial = protosystem::CreateMaterial(Temp);

    if(!TempMaterial)
    {
      game::DrawEverythingNoBlit();
      continue;
    }

    msgsystem::EnterBigMessageMode();

    if(Item.size() == 1)
      ADD_MESSAGE("Suddenly your %s is consumed in roaring magical flames.", Item[0]->CHAR_NAME(UNARTICLED));
    else
      ADD_MESSAGE("Suddenly your %s are consumed in roaring magical flames.", Item[0]->CHAR_NAME(PLURAL));

    if(TempMaterial->GetIntelligenceRequirement() + 5 > Reader->GetAttribute(INTELLIGENCE) && !game::WizardModeIsActive())
    {
      ADD_MESSAGE("But your mind is not yet strong enough to summon enough %s for the change.", TempMaterial->GetName(false, false).CStr());
      delete TempMaterial;
      msgsystem::LeaveBigMessageMode();
      continue;
    }

    material* MainMaterial = Item[0]->GetMainMaterial();
    material* SecondaryMaterial = Item[0]->GetSecondaryMaterial();

    if(Item.size() == 1)
    {
      if(!MainMaterial->IsSameAs(TempMaterial))
      {
  ADD_MESSAGE("As the fire dies out it looks greatly altered.");

  if(SecondaryMaterial && SecondaryMaterial->IsSameAs(MainMaterial))
    Item[0]->ChangeSecondaryMaterial(TempMaterial->SpawnMore());

  Item[0]->ChangeMainMaterial(TempMaterial);
      }
      else
  ADD_MESSAGE("As the fire dies out it looks unchanged.");
    }
    else
    {
      if(!MainMaterial->IsSameAs(TempMaterial))
      {
  ADD_MESSAGE("As the fire dies out they look greatly altered.");

  if(SecondaryMaterial && SecondaryMaterial->IsSameAs(MainMaterial))
    for(uInt c = 0; c < Item.size(); ++c)
      Item[c]->ChangeSecondaryMaterial(TempMaterial->SpawnMore());

  Item[0]->ChangeMainMaterial(TempMaterial);

  for(uInt c = 1; c < Item.size(); ++c)
    Item[c]->ChangeMainMaterial(TempMaterial->SpawnMore());
      }
      else
  ADD_MESSAGE("As the fire dies out they look unchanged.");
    }

    msgsystem::LeaveBigMessageMode();
    break;
  }

  RemoveFromSlot();
  SendToHell();
  Reader->EditExperience(INTELLIGENCE, 500, 1 << 12);
}
#endif
