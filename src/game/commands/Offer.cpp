COMMAND(Offer) {
  if (!Char->CheckOffer()) return false;
  lsquare *Square = Char->GetLSquareUnder();
  if (Square->GetOLTerrain() && Square->GetOLTerrain()->AcceptsOffers()) {
    if (!Char->GetStack()->GetItems()) {
      ADD_MESSAGE("You have nothing to offer!");
      return false;
    }
    int master = Square->GetDivineMaster();
    if (!master) { ADD_MESSAGE("Somehow you feel that no god will hear you here."); return false; }
    item *Item = Char->GetStack()->DrawContents(Char, CONST_S("What do you want to offer?"));
    if (Item) {
      if (game::GetGod(Square->GetDivineMaster())->ReceiveOffer(Item)) {
        Item->RemoveFromSlot();
        Item->SendToHell();
        Char->DexterityAction(5); /** C **/
        return true;
      }
      return false;
    }
    return false;
  }
  ADD_MESSAGE("There isn't any altar here!");
  return false;
};
