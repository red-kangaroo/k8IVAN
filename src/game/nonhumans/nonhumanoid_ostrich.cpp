#ifdef HEADER_PHASE
CHARACTER(ostrich, nonhumanoid)
{
 public:
  ostrich() : HasDroppedBananas(false) { }
  virtual void Save(outputfile&) const;
  virtual void Load(inputfile&);
 protected:
  virtual truth HandleCharacterBlockingTheWay(character*, v2, int);
  virtual void GetAICommand();
  truth HasDroppedBananas;
};


#else



void ostrich::GetAICommand()
{
  if(game::TweraifIsFree())
  {
    nonhumanoid::GetAICommand();
    return;
  }

  if(CheckForEnemies(false, false, true, true))
    return;

  if(!IsEnabled())
    return;

  if(GetPos() == v2(45, 45))
    HasDroppedBananas = true;

  itemvector ItemVector;
  GetStackUnder()->FillItemVector(ItemVector);
  int BananasPicked = 0;

  for(uInt c = 0; c < ItemVector.size(); ++c)
    if(ItemVector[c]->IsBanana() && ItemVector[c]->CanBeSeenBy(this)
       && ItemVector[c]->IsPickable(this)
       && !MakesBurdened(GetCarriedWeight() + ItemVector[c]->GetWeight()))
    {
      ItemVector[c]->MoveTo(GetStack());
      ++BananasPicked;
    }

  if(BananasPicked)
  {
    if(CanBeSeenByPlayer())
      ADD_MESSAGE("%s picks up %s.", CHAR_NAME(DEFINITE), BananasPicked == 1 ? "the banana" : "some bananas");

    return;
  }

  if(!HasDroppedBananas)
  {
    SetGoingTo(v2(45, 45));

    if(MoveTowardsTarget(true))
      return;
  }
  else if(GetPos().Y == 54)
  {
    if(CanBeSeenByPlayer())
      ADD_MESSAGE("%s leaves the town.", CHAR_NAME(DEFINITE));

    itemvector ItemVector;
    GetStack()->FillItemVector(ItemVector);

    for(uInt c = 0; c < ItemVector.size(); ++c)
    {
      ItemVector[c]->RemoveFromSlot();
      ItemVector[c]->SendToHell();
    }

    v2 Where = GetLevel()->GetNearestFreeSquare(this, v2(45, 0));

    if(Where == ERROR_V2)
      Where = GetLevel()->GetRandomSquare(this, NOT_IN_ROOM); // this is odd but at least it doesn't crash

    Move(Where, true);
    RestoreHP();
    RestoreStamina();
    ResetStates();
    TemporaryState = 0;
    GainIntrinsic(LEVITATION);

    if(CanBeSeenByPlayer())
      ADD_MESSAGE("%s enters the town.", CHAR_NAME(INDEFINITE));

    HasDroppedBananas = false;
  }
  else
  {
    SetGoingTo(v2(45, 54));

    if(MoveTowardsTarget(true))
      return;
  }

  EditAP(-1000);
}



void ostrich::Save(outputfile& SaveFile) const
{
  nonhumanoid::Save(SaveFile);
  SaveFile << HasDroppedBananas;
}



void ostrich::Load(inputfile& SaveFile)
{
  nonhumanoid::Load(SaveFile);
  SaveFile >> HasDroppedBananas;
}



truth ostrich::HandleCharacterBlockingTheWay(character* Char, v2 Pos, int Dir)
{
  return Char->GetPos() == v2(45, 45) && (Displace(Char, true) || Hit(Char, Pos, Dir));
}
#endif
