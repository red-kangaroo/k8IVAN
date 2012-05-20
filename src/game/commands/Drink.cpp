COMMAND(Drink) {
  if (!Char->CheckConsume(CONST_S("drink"))) return false;
  lsquare *Square = Char->GetLSquareUnder();
  if (!game::IsInWilderness() && Square->GetOLTerrain() && Square->GetOLTerrain()->HasDrinkEffect()) {
    if (Square->GetOLTerrain()->Drink(Char)) return true;
  }
  return Consume(Char, "drink", &item::IsDrinkable);
};
