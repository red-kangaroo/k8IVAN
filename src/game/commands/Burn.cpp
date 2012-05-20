COMMAND(Burn) {
  if (!Char->PossessesItem(&item::IsFlaming)) {
    ADD_MESSAGE("You have no flaming items, %s.", game::Insult());
    return false;
  }
  item *Item = Char->SelectFromPossessions(CONST_S("What do you want to use?"), &item::IsFlaming);
  if (Item) {
    int Answer = game::DirectionQuestion(CONST_S("In what direction do you wish to burn? [press a direction key]"), false);
    if (Answer == DIR_ERROR) return false;
    if (Item->Burn(Char, Char->GetPos(), Answer)) {
      //FIXME: AP
      Char->EditExperience(AGILITY, 150, 1<<6);
      Char->EditNP(-10);
      Char->EditAP(-100000/APBonus(Char->GetAttribute(AGILITY)));
      return true;
    }
    return false;
  }
  return false;
};
