COMMAND(Rest) {
  if (Char->StateIsActivated(PANIC) && !Char->IsInTunnelDeadEnd()) {
    ADD_MESSAGE("You are too scared to rest.");
    return false;
  }
  truth Error = false;
  if (Char->GetHP() == Char->GetMaxHP()) {
    ADD_MESSAGE("You HP is already at its maximum.");
    Error = true;
  } else if (!Char->CanHeal()) {
    ADD_MESSAGE("You cannot heal.");
    Error = true;
  }
  if (Error) {
    sLong MinutesToRest = game::NumberQuestion(CONST_S("How many minutes to wait?"), WHITE, true);
    if (MinutesToRest > 0) {
      oterrain *Terrain = Char->GetSquareUnder()->GetOTerrain();
      if (Terrain) Terrain->ShowRestMessage(Char);
      rest *Rest = rest::Spawn(Char);
      Rest->SetMinToStop(game::GetTotalMinutes()+MinutesToRest);
      Rest->SetGoalHP(0);
      Char->SetAction(Rest);
      return true;
    }
    return false;
  }
  sLong HPToRest = game::ScrollBarQuestion(CONST_S("How many hit points you desire?"), Char->GetMaxHP(), 1, 0, Char->GetMaxHP(), 0, WHITE, LIGHT_GRAY, DARK_GRAY);
  if (HPToRest <= Char->GetHP()) {
    if (HPToRest != 0) ADD_MESSAGE("Your HP is already %d.", Char->GetHP());
    return false;
  }
  oterrain *Terrain = Char->GetSquareUnder()->GetOTerrain();
  if (Terrain) Terrain->ShowRestMessage(Char);
  rest *Rest = rest::Spawn(Char);
  Rest->SetMinToStop(0);
  Rest->SetGoalHP(HPToRest);
  Char->SetAction(Rest);
  return true;
};
