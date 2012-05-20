COMMAND(Dip) {
  if (!Char->GetStack()->SortedItems(Char, &item::IsDippable) && !Char->EquipsSomething(&item::IsDippable)) {
    ADD_MESSAGE("You have nothing to dip!");
    return false;
  }
  truth HasDipDestination = Char->PossessesItem(&item::IsDipDestination);
  truth DipDestinationNear = false;
  for (int d = 0; d < 9; ++d) {
    lsquare *Square = Char->GetNaturalNeighbourLSquare(d);
    if (Square && Square->IsDipDestination()) DipDestinationNear = true;
  }
  if (!HasDipDestination && !DipDestinationNear) {
    ADD_MESSAGE("There is nothing to dip anything into.");
    return false;
  }
  item *Item = Char->SelectFromPossessions(CONST_S("What do you want to dip?"), &item::IsDippable);
  if (Item) {
    if (!HasDipDestination || (DipDestinationNear && game::TruthQuestion(CONST_S("Do you wish to dip in a nearby square? [y/N]")))) {
      int Dir = game::DirectionQuestion(CONST_S("Where do you want to dip ")+Item->GetName(DEFINITE)+"? [press a direction key or '.']", false, true);
      v2 Pos = Char->GetPos()+game::GetMoveVector(Dir);
      if (Dir == DIR_ERROR || !Char->GetArea()->IsValidPos(Pos) || !Char->GetNearLSquare(Pos)->IsDipDestination()) return false;
      return Char->GetNearLSquare(Pos)->DipInto(Item, Char);
    }
    item *DipTo = Char->SelectFromPossessions(CONST_S("Into what do you wish to dip it?"), &item::IsDipDestination);
    if (DipTo) {
      if (Item == DipTo) {
        ADD_MESSAGE("Very funny...");
        return false;
      }
      //fprintf(stderr, "BSM: %p\n", DipTo->GetSecondaryMaterial());
      Item->DipInto(DipTo->CreateDipLiquid(), Char);
      //fprintf(stderr, "ASM: %p\n", DipTo->GetSecondaryMaterial());
      if (!game::IsInWilderness() && !DipTo->GetSecondaryMaterial()) {
        // container is empty
        if ((DipTo->IsBottle() && ivanconfig::GetAutoDropBottles()) ||
            (DipTo->IsCan() && ivanconfig::GetAutoDropCans())) {
          // drop this item
          if (!Char->GetRoom() || Char->GetRoom()->DropItem(Char, DipTo, 1)) {
            ADD_MESSAGE("%s dropped.", DipTo->GetName(INDEFINITE, 1).CStr());
            DipTo->MoveTo(Char->GetStackUnder());
            Char->DexterityAction(2);
          }
        }
      }
      return true;
    }
  }
  return false;
};
