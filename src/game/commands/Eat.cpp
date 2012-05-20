COMMAND(Eat) {
  if (!Char->CheckConsume(CONST_S("eat"))) return false;
  lsquare *Square = Char->GetLSquareUnder();
  if (!game::IsInWilderness() && Square->GetOLTerrain() && Square->GetOLTerrain()->HasEatEffect()) {
    if (Square->GetOLTerrain()->Eat(Char)) return true;
  }
  return Consume(Char, "eat", &item::IsEatable);
};
