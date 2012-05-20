COMMAND(SummonMonster) {
  character *Summoned = 0;
  //
  while (!Summoned) {
    festring Temp = game::DefaultQuestion(CONST_S("Summon which monster?"), game::GetDefaultSummonMonster());
    //
    if (Temp == "none") return false;
    Summoned = protosystem::CreateMonster(Temp);
  }
  Summoned->SetTeam(game::GetTeam(MONSTER_TEAM));
  Summoned->PutNear(Char->GetPos());
  return false;
};
