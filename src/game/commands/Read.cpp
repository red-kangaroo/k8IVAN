COMMAND(Read) {
  if (!Char->CanRead() && !game::GetSeeWholeMapCheatMode()) {
    ADD_MESSAGE("You can't read.");
    return false;
  }
  if (!Char->GetStack()->SortedItems(Char, &item::IsReadable)) {
    ADD_MESSAGE("You have nothing to read!");
    return false;
  }
  if (Char->GetLSquareUnder()->IsDark() && !game::GetSeeWholeMapCheatMode()) {
    ADD_MESSAGE("It is too dark to read.");
    return false;
  }
  item *Item = Char->GetStack()->DrawContents(Char, CONST_S("What do you want to read?"), SELECT_MOST_RECENT, &item::IsReadable);
  if (Item) {
    if (ivanconfig::GetConfirmScrollReading() && Item->IsScroll()) {
      if (!game::TruthQuestion(festring("Do you really want to read ")+Item->CHAR_NAME(DEFINITE)+"?")) return false;
    }
    return Char->ReadItem(Item);
  }
  return false;
};
