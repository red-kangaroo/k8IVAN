#ifdef HEADER_PHASE
CHARACTER(assassin, humanoid)
{
public:
  virtual void BeTalkedTo();
};


#else



void assassin::BeTalkedTo () {
  if (GetRelation(PLAYER) == HOSTILE) {
    if (PLAYER->GetMoney() >= 1500) {
      ADD_MESSAGE("%s talks: \"If you shell out 1500 gold pieces I'll join your side\"", CHAR_DESCRIPTION(DEFINITE));
      if (game::TruthQuestion(CONST_S("Do you want to bribe him? [y/N]"))) {
        PLAYER->SetMoney(PLAYER->GetMoney()-1500);
        ChangeTeam(PLAYER->GetTeam());
        RemoveHomeData();
      }
    } else {
      ADD_MESSAGE("\"Trying to reason me with diplomancy won't work on me.\"");
    }
  }
}
#endif
