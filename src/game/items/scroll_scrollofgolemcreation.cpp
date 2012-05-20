#ifdef HEADER_PHASE
ITEM(scrollofgolemcreation, scroll)
{
 public:
  virtual void FinishReading(character*);
};


#else



void scrollofgolemcreation::FinishReading(character* Reader)
{
  for(;;)
  {
    itemvector Item;

    if(!Reader->SelectFromPossessions(Item, CONST_S("Which item do you wish to use for golem creation?"), NO_MULTI_SELECT, &item::IsDestroyable))
    {
      ADD_MESSAGE("You notice you haven't got anything to change into a golem.");
      return;
    }

    if(!Item.empty())
    {
      material* Main = Item[0]->GetMainMaterial();
      material* Sec = Item[0]->GetSecondaryMaterial();
      truth MainPossible = Main->GetCategoryFlags() & IS_GOLEM_MATERIAL;
      truth SecPossible = Sec && Sec->GetVolume()
        && Sec->GetCategoryFlags() & IS_GOLEM_MATERIAL
        && !Sec->IsSameAs(Main);

      if(!MainPossible && !SecPossible)
      {
  ADD_MESSAGE("You can't use that for golem creation.");
  continue;
      }

      if (MainPossible && SecPossible) {
        if (game::TruthQuestion(CONST_S("Use main material? [Y/n]"), YES)) SecPossible = false; else MainPossible = false;
      }

      int MaterialConfig = MainPossible ? Main->GetConfig() : Sec->GetConfig();
      golem* Golem = golem::Spawn(MaterialConfig);
      sLong Volume = MainPossible ? Sec && Sec->IsSameAs(Main)
        ? Main->GetVolume() + Sec->GetVolume()
        : Main->GetVolume() : Sec->GetVolume();
      Golem->SetItemVolume(Volume);
      v2 Where = GetLevel()->GetNearestFreeSquare(Golem, Reader->GetPos());
      Item[0]->RemoveFromSlot();
      Item[0]->SendToHell();

      if(Where == ERROR_V2)
      {
  ADD_MESSAGE("You cast the spell and %s is sucked into a rainbow-coled magical vortex, but nothing happens.", Item[0]->CHAR_NAME(DEFINITE));
  delete Golem;
      }
      else
      {
  ADD_MESSAGE("You cast the spell and %s is sucked into a rainbow-coled magical vortex.", Item[0]->CHAR_NAME(DEFINITE));
  Golem->SetTeam(Reader->GetTeam());
  Golem->PutTo(Where);

  if(Golem->CanBeSeenByPlayer())
    ADD_MESSAGE("Suddenly %s materializes!", Golem->CHAR_NAME(INDEFINITE));

  Golem->GetLSquareUnder()->DrawParticles(RED);
      }

      break;
    }
    else if(game::TruthQuestion(CONST_S("Really cancel read? [y/N]")))
      return;
  }

  RemoveFromSlot();
  SendToHell();
  Reader->EditExperience(INTELLIGENCE, 300, 1 << 12);
}
#endif
