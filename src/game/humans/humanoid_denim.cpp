#ifdef HEADER_PHASE
CHARACTER(denim, humanoid)
{
protected:
  virtual void GetAICommand();
};


#else



void denim::GetAICommand () {
  int Enemies = 0;
  for (int c = 0; c < game::GetTeams(); ++c) {
    if (GetTeam()->GetRelation(game::GetTeam(c)) == HOSTILE) Enemies += game::GetTeam(c)->GetEnabledMembers();
  }
  StandIdleAI();
}
#endif
