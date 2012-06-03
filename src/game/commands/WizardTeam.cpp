COMMAND(WizardTeam) {
  int Dir = game::DirectionQuestion(CONST_S("Choose creature to teaming. [press a direction key]"), false);
  if (Dir == DIR_ERROR || !Char->GetArea()->IsValidPos(Char->GetPos()+game::GetMoveVector(Dir))) return false;
  character *ToTeam = Char->GetNearLSquare(Char->GetPos()+game::GetMoveVector(Dir))->GetCharacter();
  if (ToTeam) {
    team *t = 0;
    //
    while (!t) {
      festring Temp = game::DefaultQuestion(CONST_S("Assign to which team?"), game::GetDefaultTeam());
      //
      if (Temp == "none") return false;
      t = game::FindTeam(Temp);
    }
    ToTeam->SetTeam(game::GetTeam(MONSTER_TEAM));
    //ToTeam->SetRelation(ToTeam->GetRelation(PLAYER->GetTeam()))
    ADD_MESSAGE("Team changed.");
  } else {
    ADD_MESSAGE("There's no one to team, %s!", game::Insult());
  }
  return false;
};
