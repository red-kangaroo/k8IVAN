COMMAND(Throw) {
  if (!Char->CheckThrow()) return false;
  if (!Char->GetStack()->GetItems()) {
    ADD_MESSAGE("You have nothing to throw!");
    return false;
  }
  item *Item = Char->GetStack()->DrawContents(Char, CONST_S("What do you want to throw?"));
  if (Item) {
    int Answer = game::DirectionQuestion(CONST_S("In what direction do you wish to throw?  [press a direction key]"), false);
    if (Answer == DIR_ERROR) return false;
    Char->ThrowItem(Answer, Item);
    Char->EditExperience(ARM_STRENGTH, 75, 1 << 8);
    Char->EditExperience(DEXTERITY, 75, 1 << 8);
    Char->EditExperience(PERCEPTION, 75, 1 << 8);
    Char->EditNP(-50);
    Char->DexterityAction(5);
    return true;
  }
  return false;
};
