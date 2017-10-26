COMMAND(PickUp) {
  if (!Char->GetStackUnder()->GetVisibleItems(Char)) {
    ADD_MESSAGE("There is nothing here to pick up!");
    return false;
  }
  itemvectorvector PileVector;
  Char->GetStackUnder()->Pile(PileVector, Char, CENTER);
  for (int c = 0; c < 4; ++c) {
    stack *Stack = Char->GetLSquareUnder()->GetStackOfAdjacentSquare(c);
    if (Stack) Stack->Pile(PileVector, Char, 3-c);
  }
  if (PileVector.size() == 1) {
    if (PileVector[0][0]->CanBePickedUp()) {
      int Amount = PileVector[0].size();
      if (Amount > 1) Amount = game::ScrollBarQuestion(CONST_S("How many ")+PileVector[0][0]->GetName(PLURAL)+'?', Amount, 1, 0, Amount, 0, WHITE, LIGHT_GRAY, DARK_GRAY);
      if (!Amount) return false;
      if ((!PileVector[0][0]->GetRoom() || PileVector[0][0]->GetRoom()->PickupItem(Char, PileVector[0][0], Amount)) &&
          PileVector[0][0]->CheckPickUpEffect(Char))
      {
        if (PileVector[0][0]->IsCorpse() && ivanconfig::GetConfirmCorpses()) {
          //if (!game::TruthQuestion(CONST_S("Do you really want to pick up the corpse?"))) return false;
          itemvector ToPickup;
          game::DrawEverythingNoBlit();
          Char->GetStackUnder()->DrawContents(ToPickup, Char, CONST_S("Do you really want to pick up the corpse?"), REMEMBER_SELECTED);
          if (ToPickup.empty()) return false;
        }
        for (int c = 0; c < Amount; ++c) PileVector[0][c]->MoveTo(Char->GetStack(), true);
        ADD_MESSAGE("%s picked up.", PileVector[0][0]->GetName(INDEFINITE, Amount).CStr());
        Char->DexterityAction(2);
        return true;
      }
      return false;
    } else {
      ADD_MESSAGE("%s too large to pick up!", PileVector[0].size() == 1 ? "It is" : "They are");
      return false;
    }
  }
  truth Success = false;
  stack::SetSelected(0);
  for (;;) {
    itemvector ToPickup;
    game::DrawEverythingNoBlit();
    Char->GetStackUnder()->DrawContents(ToPickup, Char, CONST_S("What do you want to pick up?"), REMEMBER_SELECTED);
    if (ToPickup.empty()) break;
    if (ToPickup[0]->CanBePickedUp()) {
      if ((!ToPickup[0]->GetRoom() || ToPickup[0]->GetRoom()->PickupItem(Char, ToPickup[0], ToPickup.size())) &&
          ToPickup[0]->CheckPickUpEffect(Char)) {
        for (uInt c = 0; c < ToPickup.size(); ++c) ToPickup[c]->MoveTo(Char->GetStack(), true);
        ADD_MESSAGE("%s picked up.", ToPickup[0]->GetName(INDEFINITE, ToPickup.size()).CStr());
        Success = true;
      }
    } else {
      ADD_MESSAGE("%s too large to pick up!", ToPickup.size() == 1 ? "It is" : "They are");
    }
  }
  if (Success) {
    Char->DexterityAction(2);
    return true;
  }
  return false;
};
