COMMAND(Burn) {
  item *Item = 0;
  //
  if (!Char->PossessesItem(&item::IsFlaming)) {
    ADD_MESSAGE("You have no flaming items, %s.", game::Insult());
    return false;
  }
  if (Char->MoreThanOnePossessesItem(&item::IsFlaming)) {
    Item = Char->SelectFromPossessions(CONST_S("What do you want to use?"), &item::IsFlaming);
  } else {
    Item = Char->FirstPossessesItem(&item::IsFlaming);
  }
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
