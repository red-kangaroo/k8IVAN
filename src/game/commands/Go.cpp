COMMAND(Go) {
  int Dir = game::DirectionQuestion(CONST_S("In what direction do you want to go? [press a direction key]"), false);
  if (Dir == DIR_ERROR) return false;
  go *Go = go::Spawn(Char);
  Go->SetDirection(Dir);
  Go->SetPrevWasTurn(false);
  /*
  int OKDirectionsCounter = 0;
  for (int d = 0; d < Char->GetNeighbourSquares(); ++d) {
    lsquare *Square = Char->GetNeighbourLSquare(d);
    if (Square && Char->CanMoveOn(Square)) ++OKDirectionsCounter;
  }
  Go->SetIsWalkingInOpen(OKDirectionsCounter > 2);
  */
  Char->SetAction(Go);
  Char->EditAP(Char->GetStateAPGain(100)); // gum solution
  Char->GoOn(Go, true);
  return !!Char->GetAction();
};
