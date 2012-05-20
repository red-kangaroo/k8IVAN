COMMAND(Open) {
  if (Char->CanOpen()) {
    int Key;
    truth OpenableItems = Char->GetStack()->SortedItems(Char, &item::IsOpenable);
    //
    if (OpenableItems) {
      Key = game::AskForKeyPress(CONST_S("What do you wish to open? [press a direction key, space or i]"));
    } else {
      Key = game::AskForKeyPress(CONST_S("What do you wish to open? [press a direction key or space]"));
    }
    //
    if (OpenableItems && (Key == 'i' || Key == 'I')) {
      item *Item = Char->GetStack()->DrawContents(Char, CONST_S("What do you want to open?"), 0, &item::IsOpenable);
      return Item && Item->Open(Char);
    }
    v2 DirVect = game::GetDirectionVectorForKey(Key);
    if (DirVect != ERROR_V2 && Char->GetArea()->IsValidPos(Char->GetPos()+DirVect)) return Char->GetNearLSquare(Char->GetPos()+DirVect)->Open(Char);
  } else {
    ADD_MESSAGE("This monster type cannot open anything.");
  }
  return false;
};
