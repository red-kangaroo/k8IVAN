#ifdef HEADER_PHASE
ITEM(scrollofenchantarmor, scroll)
{
 public:
  virtual void FinishReading(character*);
};


#else



void scrollofenchantarmor::FinishReading(character* Reader)
{
  for(;;)
  {
    itemvector Item;

    if(!Reader->SelectFromPossessions(Item, CONST_S("Choose an armor to enchant:"), NO_MULTI_SELECT|SELECT_PAIR, &item::IsArmor))
    {
      ADD_MESSAGE("You notice you haven't got anything to enchant.");
      return;
    }

    if(!Item.empty())
    {
      if(!Item[0]->CanBeEnchanted())
      {
  ADD_MESSAGE("You cast the spell, but the magic is not powerful enough to affect %s!", Item[0]->CHAR_NAME(DEFINITE|(Item.size() == 1 ? 0 : PLURAL)));
  break;
      }

      if(Item[0]->HandleInPairs() && Item.size() == 1)
      {
  ADD_MESSAGE("Only one %s will be enchanted.", Item[0]->CHAR_NAME(UNARTICLED));

  if(!game::TruthQuestion(CONST_S("Still continue? [y/N]")))
    continue;
      }

      if(Item[0]->GetEnchantment() >= 5 && RAND_GOOD(Item[0]->GetEnchantment() - 3))
      {
  if(Item.size() == 1)
    ADD_MESSAGE("Magic energies swirl around %s, but they fail to enchant it further!", Item[0]->CHAR_NAME(DEFINITE));
  else
    ADD_MESSAGE("Magic energies swirl around %s, but they fail to enchant them further!", Item[0]->CHAR_NAME(DEFINITE|PLURAL));

  break;
      }

      if(Item.size() == 1)
  ADD_MESSAGE("Your %s glows briefly blue. It feels very warm now.", Item[0]->CHAR_NAME(UNARTICLED));
      else
  ADD_MESSAGE("Your %s glow briefly blue. They feel very warm now.", Item[0]->CHAR_NAME(PLURAL));

      for(uInt c = 0; c < Item.size(); ++c)
  Item[c]->EditEnchantment(1);

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
