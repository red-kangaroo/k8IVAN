COMMAND(Kick) {
  /** No multi-tile support */
  if (!Char->CheckKick()) return false;
  if (Char->GetBurdenState() == OVER_LOADED) {
    ADD_MESSAGE("You try to kick, but you collapse under your load.");
    Char->EditAP(-100000/APBonus(Char->GetAttribute(AGILITY)));
    return true;
  }
  int Dir = game::DirectionQuestion(CONST_S("In what direction do you wish to kick? [press a direction key]"), false);
  if (Dir == DIR_ERROR || !Char->GetArea()->IsValidPos(Char->GetPos()+game::GetMoveVector(Dir))) return false;
  lsquare *Square = Char->GetNearLSquare(Char->GetPos()+game::GetMoveVector(Dir));
  if (!Square->CheckKick(Char)) return false;
  character *Enemy = Square->GetCharacter();
  if (Enemy && !(Enemy->IsMasochist() && Char->GetRelation(Enemy) == FRIEND) && Char->GetRelation(Enemy) != HOSTILE) {
    if (!game::TruthQuestion(CONST_S("This might cause a hostile reaction. Are you sure?"))) return false;
  }

  game::ClearEventData();
  game::mActor = Char;
  game::mSecondActor = Square->GetCharacter();
  if (!game::RunOnCharEvent(Square->GetCharacter(), CONST_S("before_kicked_by"))) { game::ClearEventData(); return false; }
  game::ClearEventData();

  Char->Hostility(Square->GetCharacter());
  Char->Kick(Square, Dir);
  return true;
};
