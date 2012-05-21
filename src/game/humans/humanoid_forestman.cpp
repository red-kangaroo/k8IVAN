#ifdef HEADER_PHASE
CHARACTER(forestman, humanoid)
{
public:
  virtual character *GetLeader() const;

protected:
  virtual void GetAICommand ();
};


#else


character *forestman::GetLeader () const {
  character *Guide = game::GetTeam(FORESTMAN_TEAM)->GetLeader();
  return Guide && Guide->GetRelation(this) != HOSTILE ? Guide : GetTeam()->GetLeader();
}


void forestman::GetAICommand () {
  humanoid::GetAICommand();
}


#endif
