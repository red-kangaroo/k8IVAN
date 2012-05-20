COMMAND(Dump) {
  if (!Char->GetStack()->SortedItems(Char, &item::IsDumpable) && !Char->EquipsSomething(&item::IsDumpable)) {
    ADD_MESSAGE("You have nothing to dump!");
    return false;
  }
  item *Item = Char->SelectFromPossessions(CONST_S("What do you want to dump?"), &item::IsDumpable);
  if (Item) {
    int Dir = game::DirectionQuestion(CONST_S("In what direction do you wish to dump it? [press a direction key or '.']"), false, true);
    if (Dir == DIR_ERROR) return false;
    v2 Pos = Char->GetPos()+game::GetMoveVector(Dir);
    return Item->DumpTo(Char, Pos);
  }
  return false;
};
