#ifdef HEADER_PHASE
CHARACTER(femaleslave, humanoid)
{
 public:
  virtual void BeTalkedTo();
 protected:
  virtual void GetAICommand() { StandIdleAI(); }
};


#else



void femaleslave::BeTalkedTo()
{
  static sLong Said;

  if(GetConfig() != NEW_ATTNAM || GetRelation(PLAYER) == HOSTILE)
    humanoid::BeTalkedTo();
  else if(!game::TweraifIsFree())
    ProcessAndAddMessage(GetFriendlyReplies()[RandomizeReply(Said, 4)]);
  else
    ProcessAndAddMessage(GetFriendlyReplies()[4 + RandomizeReply(Said, 3)]);
}
#endif
