COMMAND(Zap) {
  if (!Char->CheckZap()) return false;
  if (!Char->PossessesItem(&item::IsZappable)) {
    ADD_MESSAGE("You have nothing to zap with, %s.", game::Insult());
    return false;
  }
  item *Item = Char->SelectFromPossessions(CONST_S("What do you want to zap with?"), &item::IsZappable);
  if (Item) {
    int Answer = game::DirectionQuestion(CONST_S("In what direction do you wish to zap? [press a direction key or '.']"), false, true);
    if (Answer == DIR_ERROR) return false;
    if (Item->Zap(Char, Char->GetPos(), Answer)) {
      Char->EditAP(-100000/APBonus(Char->GetAttribute(PERCEPTION)));
      return true;
    }
    return false;
  }
  return false;
};
