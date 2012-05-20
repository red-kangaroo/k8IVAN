#ifdef HEADER_PHASE
ITEM(scrollofcharging, scroll)
{
 public:
  virtual void FinishReading(character*);
};


#else



void scrollofcharging::FinishReading(character* Reader)
{
  for(;;)
  {
    itemvector Item;

    if(!Reader->SelectFromPossessions(Item, CONST_S("Which item do you wish to charge?"), NO_MULTI_SELECT|SELECT_PAIR, &item::IsChargeable))
    {
      ADD_MESSAGE("You notice you haven't got anything to charge.");
      return;
    }

    if(!Item.empty())
    {
      if(Item[0]->HandleInPairs() && Item.size() == 1)
      {
  ADD_MESSAGE("Only one %s will be charged.", Item[0]->CHAR_NAME(UNARTICLED));

  if(!game::TruthQuestion(CONST_S("Still continue? [y/N]")))
    continue;
      }

      for(uInt c = 0; c < Item.size(); ++c)
  Item[c]->ChargeFully(Reader);

      ADD_MESSAGE("You charge %s and the scroll burns.", Item[0]->CHAR_NAME(DEFINITE|(Item.size() == 1 ? 0 : PLURAL)));
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
