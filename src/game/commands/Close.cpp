COMMAND(Close) {
  if (Char->CanOpen()) {
    int Dir = game::DirectionQuestion(CONST_S("What do you wish to close? [press a direction key]"), false);
    if (Dir != DIR_ERROR && Char->GetArea()->IsValidPos(Char->GetPos()+game::GetMoveVector(Dir)))
      return Char->GetNearLSquare(Char->GetPos()+game::GetMoveVector(Dir))->Close(Char);
  } else {
    ADD_MESSAGE("This monster type cannot close anything.");
  }
  return false;
};
