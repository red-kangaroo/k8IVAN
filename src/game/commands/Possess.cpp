COMMAND(Possess) {
  int Dir = game::DirectionQuestion(CONST_S("Choose creature to possess. [press a direction key]"), false);
  if (Dir == DIR_ERROR || !Char->GetArea()->IsValidPos(Char->GetPos() + game::GetMoveVector(Dir))) return false;
  character *ToPossess = Char->GetNearLSquare(Char->GetPos() + game::GetMoveVector(Dir))->GetCharacter();
  if (ToPossess) {
    Char->RemoveFlags(C_PLAYER);
    game::SetPlayer(ToPossess);
  } else {
    ADD_MESSAGE("There's no one to possess, %s!", game::Insult());
  }
  return true; // The old player's turn must end
};
