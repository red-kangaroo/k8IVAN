#ifdef HEADER_PHASE
CHARACTER(sumowrestler, humanoid)
{
 public:
  virtual void BeTalkedTo();
  virtual truth CheckIfSatiated() { return GetNP() > BLOATED_LEVEL; }
  virtual truth IsSumoWrestler() const { return true; }
  virtual truth EquipmentIsAllowed(int) const;
 protected:
  virtual void GetAICommand();
};


#else



truth sumowrestler::EquipmentIsAllowed(int I) const { return I == BELT_INDEX; }



void sumowrestler::GetAICommand()
{
  EditNP(-25);

  SeekLeader(GetLeader());

  if(CheckForEnemies(true, true, true))
    return;

  if(CheckForUsefulItemsOnGround())
    return;

  if(CheckForFood(4))
    return;

  if(FollowLeader(GetLeader()))
    return;

  if(CheckForDoors())
    return;

  if(MoveTowardsHomePos())
    return;

  EditAP(-1000);
}



void sumowrestler::BeTalkedTo()
{
  static sLong Said;

  if(GetRelation(PLAYER) == HOSTILE)
    ProcessAndAddMessage(GetHostileReplies()[RandomizeReply(Said, GetHostileReplies().Size)]);
  else if(!game::TweraifIsFree())
    ProcessAndAddMessage(GetFriendlyReplies()[RandomizeReply(Said, 6)]);
  else
    ProcessAndAddMessage(GetFriendlyReplies()[6 + RandomizeReply(Said, 3)]);
}
#endif
