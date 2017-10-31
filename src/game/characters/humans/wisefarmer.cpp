#ifdef HEADER_PHASE
CHARACTER(wisefarmer, humanoid)
{
protected:
  virtual void GetAICommand();
public:
  virtual v2 GetHeadBitmapPos() const;
  virtual v2 GetRightArmBitmapPos() const;
  virtual v2 GetLeftArmBitmapPos() const { return GetRightArmBitmapPos(); }
};


#else



v2 wisefarmer::GetHeadBitmapPos () const { return v2(96, (4 + (RAND() & 1)) << 4); }



v2 wisefarmer::GetRightArmBitmapPos () const { return v2(64, (RAND() & 1) << 4); }



void wisefarmer::GetAICommand () {
  int Enemies = 0;
  for (int c = 0; c < game::GetTeams(); ++c) {
    if (GetTeam()->GetRelation(game::GetTeam(c)) == HOSTILE) Enemies += game::GetTeam(c)->GetEnabledMembers();
  }
  StandIdleAI();
}
#endif
