COMMAND(ForceVomit) {
  if (Char->CanForceVomit()) {
    int Dir = game::DirectionQuestion(CONST_S("Where do you wish to vomit?  [press a direction key]"), false, true);
    if (Dir != DIR_ERROR) {
      v2 VomitPos = Char->GetPos()+game::GetMoveVector(Dir);
      if (Char->GetArea()->IsValidPos(VomitPos)) {
        ccharacter *Other = Char->GetArea()->GetSquare(VomitPos)->GetCharacter();
        if (Other && Other->GetTeam() != Char->GetTeam() &&
            Other->GetRelation(Char) != HOSTILE &&
            Other->CanBeSeenBy(Char) &&
            !game::TruthQuestion("Do you really want to vomit at "+Other->GetObjectPronoun()+"?")) return false;
        ADD_MESSAGE("%s", Char->GetForceVomitMessage().CStr());
        Char->Vomit(Char->GetPos() + game::GetMoveVector(Dir), 500 + RAND() % 500, false);
        Char->EditAP(-1000);
        return true;
      }
    }
  } else ADD_MESSAGE("You can't vomit.");

  return false;
};
