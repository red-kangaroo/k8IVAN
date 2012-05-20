#ifdef HEADER_PHASE
CHARACTER(guard, humanoid)
{
 public:
  guard() : NextWayPoint(0) { }
  virtual void Save(outputfile&) const;
  virtual void Load(inputfile&);
  virtual void GetAICommand();
  virtual void SetWayPoints(const fearray<packv2>&);
  virtual truth MoveTowardsHomePos();
  virtual void BeTalkedTo();
 protected:
  std::vector<v2> WayPoints;
  uInt NextWayPoint;
};


#else



void guard::SetWayPoints(const fearray<packv2>& What) { ArrayToVector(What, WayPoints); }



void guard::Save(outputfile& SaveFile) const
{
  humanoid::Save(SaveFile);
  SaveFile << WayPoints << NextWayPoint;
}



void guard::Load(inputfile& SaveFile)
{
  humanoid::Load(SaveFile);
  SaveFile >> WayPoints >> NextWayPoint;
}



void guard::GetAICommand()
{
  if(GetConfig() == MASTER && HP << 1 < MaxHP && (GetPos() - v2(30, 17)).GetLengthSquare() > 9)
  {
    if(CanBeSeenByPlayer())
      ADD_MESSAGE("%s disappears.", CHAR_NAME(DEFINITE));

    GetLevel()->GetLSquare(30, 16)->KickAnyoneStandingHereAway();
    Move(v2(30, 16), true);
    EditAP(-1000);
    return;
  }

  if(WayPoints.size() && !IsGoingSomeWhere())
  {
    if(GetPos() == WayPoints[NextWayPoint]) {
      if(NextWayPoint < WayPoints.size() - 1) ++NextWayPoint; else NextWayPoint = 0;
    }

    GoingTo = WayPoints[NextWayPoint];
  }

  SeekLeader(GetLeader());

  if(CheckForEnemies(true, true, true))
    return;

  if(CheckForUsefulItemsOnGround())
    return;

  if(FollowLeader(GetLeader()))
    return;

  if(CheckForDoors())
    return;

  if(MoveTowardsHomePos())
    return;

  if(CheckSadism())
    return;

  if(CheckForBeverage())
    return;

  EditAP(-1000);
}



truth guard::MoveTowardsHomePos()
{
  if(GetConfig() == MASTER && GetPos() != v2(30, 16))
  {
    if(CanBeSeenByPlayer())
      ADD_MESSAGE("%s disappears.", CHAR_NAME(DEFINITE));

    GetLevel()->GetLSquare(30, 16)->KickAnyoneStandingHereAway();
    Move(v2(30, 16), true);

    if(CanBeSeenByPlayer())
      ADD_MESSAGE("%s appears.", CHAR_NAME(DEFINITE));

    EditAP(-1000);
    return true;
  }
  else
    return humanoid::MoveTowardsHomePos();
}



void guard::BeTalkedTo()
{
  itemvector Item;

  if(!PLAYER->SelectFromPossessions(Item, CONST_S("Do you have something to give me?"), 0, &item::IsBeverage)
     || Item.empty())
    humanoid::BeTalkedTo();

  for(unsigned int c = 0; c < Item.size(); ++c)
  {
    Item[c]->RemoveFromSlot();
    GetStack()->AddItem(Item[c]);
  }
}
#endif
