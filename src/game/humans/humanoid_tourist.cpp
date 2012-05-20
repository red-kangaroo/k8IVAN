#ifdef HEADER_PHASE
CHARACTER(tourist, humanoid)
{
 public:
  virtual character* GetLeader() const;
 protected:
  virtual void GetAICommand();
};


#else



character* tourist::GetLeader() const
{
  character* Guide = game::GetTeam(TOURIST_GUIDE_TEAM)->GetLeader();
  return Guide && Guide->GetRelation(this) != HOSTILE ? Guide : GetTeam()->GetLeader();
}



void tourist::GetAICommand()
{
  if(game::IsSumoWrestling() && !(RAND() % 10))
  {
    if(GetConfig() == HUSBAND)
    {
      if(RAND() & 1)
  ADD_MESSAGE("%s shouts: \"Show that skinny wimp what you've got, Huang!\"", CHAR_DESCRIPTION(DEFINITE));
      else
  ADD_MESSAGE("%s screams: \"Go for it, Huang!\"", CHAR_DESCRIPTION(DEFINITE));
    }
    else if(GetConfig() == WIFE)
    {
      if(RAND() & 1)
  ADD_MESSAGE("%s encourages you: \"Knock him out, %s!\"", CHAR_DESCRIPTION(DEFINITE), game::GetPlayerName().CStr());
      else
  ADD_MESSAGE("%s cheers you: \"A handsome guy like you can't lose to that banana ball!\"", CHAR_DESCRIPTION(DEFINITE));
    }
    else if(GetConfig() == CHILD)
    {
      if(RAND() & 1)
  ADD_MESSAGE("%s yells: \"More blood on the ring!!!\"", CHAR_DESCRIPTION(DEFINITE));
      else
  ADD_MESSAGE("%s cries: \"Kill him, Pong!!!\"", CHAR_DESCRIPTION(DEFINITE));
    }
  }

  humanoid::GetAICommand();
}
#endif
