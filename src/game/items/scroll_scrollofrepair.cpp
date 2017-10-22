#ifdef HEADER_PHASE
ITEM(scrollofrepair, scroll)
{
 public:
  virtual void FinishReading(character*);
};


#else



void scrollofrepair::FinishReading (character *Reader) {
  for (;;) {
    itemvector Item;
    if (!Reader->SelectFromPossessions(Item, CONST_S("Which item do you wish to repair?"), NO_MULTI_SELECT|SELECT_PAIR, &item::IsRepairable))
    {
      ADD_MESSAGE("You notice you haven't got anything to repair.");
      return;
    }
    if (!Item.empty()) {
      if (Item[0]->HandleInPairs() && Item.size() == 1) {
        ADD_MESSAGE("Only one %s will be repaired.", Item[0]->CHAR_NAME(UNARTICLED));
        if (!game::TruthQuestion(CONST_S("Still continue?"))) continue;
      }
      if (Item.size() == 1)
        ADD_MESSAGE("As you read the scroll, %s glows green and %s.", Item[0]->CHAR_NAME(DEFINITE), Item[0]->IsBroken() ? "fixes itself" : "its rust vanishes");
      else
        ADD_MESSAGE("As you read the scroll, %s glow green and %s.", Item[0]->CHAR_NAME(PLURAL), Item[0]->IsBroken() ? "fix themselves" : "their rust vanishes");
      for (uInt c = 0; c < Item.size(); ++c) {
        Item[c]->RemoveRust();
        Item[c]->Fix();
      }
      break;
    } else {
      if (game::TruthQuestion(CONST_S("Really cancel read?"))) return;
    }
  }
  RemoveFromSlot();
  SendToHell();
  Reader->EditExperience(INTELLIGENCE, 300, 1 << 12);
}
#endif
