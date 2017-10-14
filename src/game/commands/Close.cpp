COMMAND(Close) {
  if (Char->CanOpen()) {
    int openableSquares = 0;
    v2 lastOpenableDir = v2(0, 0);

    for (int dy = -1; dy < 2; ++dy) {
      for (int dx = -1; dx < 2; ++dx) {
        v2 dirVect = v2(dx, dy);
        if (dirVect != ERROR_V2 && Char->GetArea()->IsValidPos(Char->GetPos()+dirVect)) {
          olterrain *Terrain = Char->GetNearLSquare(Char->GetPos()+dirVect)->GetOLTerrain();
          if (Terrain && Terrain->CanBeClosed()) {
            lastOpenableDir = dirVect;
            ++openableSquares;
          }
        }
      }
    }

    if (!openableSquares) {
      ADD_MESSAGE("There is nothing to close around.");
      return false;
    }

    if (openableSquares > 1) {
      int Dir = game::DirectionQuestion(CONST_S("What do you wish to close? [press a direction key]"), false);
      if (Dir != DIR_ERROR && Char->GetArea()->IsValidPos(Char->GetPos()+game::GetMoveVector(Dir))) {
        return Char->GetNearLSquare(Char->GetPos()+game::GetMoveVector(Dir))->Close(Char);
      }
    } else {
      if (Char->GetArea()->IsValidPos(Char->GetPos()+lastOpenableDir)) {
        return Char->GetNearLSquare(Char->GetPos()+lastOpenableDir)->Close(Char);
      }
    }
  } else {
    ADD_MESSAGE("This monster type cannot close anything.");
  }
  return false;
};
