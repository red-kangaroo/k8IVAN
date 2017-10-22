COMMAND(Drop) {
  if (!Char->GetStack()->GetItems()) {
    ADD_MESSAGE("You have nothing to drop!");
    return false;
  }
  truth Success = false;
  stack::SetSelected(0);
  for (;;) {
    itemvector ToDrop;
    game::DrawEverythingNoBlit();
    Char->GetStack()->DrawContents(ToDrop, Char, CONST_S("What do you want to drop?"), REMEMBER_SELECTED);
    if (ToDrop.empty()) break;
    if (game::IsInWilderness()) {
      for (uInt c = 0; c < ToDrop.size(); ++c) {
        if (game::TruthQuestion(CONST_S("Are you sure? You will never see ")+ToDrop[c]->CHAR_NAME(DEFINITE)+" again!")) {
          ADD_MESSAGE("You drop %s.", ToDrop[c]->CHAR_NAME(DEFINITE));
          ToDrop[c]->RemoveFromSlot();
          ToDrop[c]->SendToHell();
        }
      }
    } else if (!Char->GetRoom() || Char->GetRoom()->DropItem(Char, ToDrop[0], ToDrop.size())) {
      ADD_MESSAGE("%s dropped.", ToDrop[0]->GetName(INDEFINITE, ToDrop.size()).CStr());
      for (uInt c = 0; c < ToDrop.size(); ++c) ToDrop[c]->MoveTo(Char->GetStackUnder());
      Success = true;
    }
  }
  if (Success) {
    Char->DexterityAction(2);
    return true;
  }
  return false;
};
