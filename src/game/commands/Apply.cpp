COMMAND(Apply) {
  if (!Char->CanApply()) {
    ADD_MESSAGE("This monster type cannot apply.");
    return false;
  }
  if (!Char->CheckApply()) return false;
  if (!Char->PossessesItem(&item::IsAppliable)) {
    ADD_MESSAGE("You have nothing to apply!");
    return false;
  }
  item *Item = Char->SelectFromPossessions(CONST_S("What do you want to apply?"), &item::IsAppliable);
  return Item && Item->Apply(Char);
};
