#ifdef HEADER_PHASE
CHARACTER(bananagrower, humanoid)
{
 public:
  virtual void Save(outputfile&) const;
  virtual void Load(inputfile&);
  virtual void BeTalkedTo();
  virtual festring& ProcessMessage(festring&) const;
  virtual truth IsBananaGrower() const { return true; }
  festring GetProfession() const { return Profession; }
 protected:
  virtual truth HandleCharacterBlockingTheWay(character*, v2, int);
  virtual void PostConstruct();
  virtual void GetAICommand();
  void RandomizeProfession();
  festring Profession;
  truth HasDroppedBananas;
  truth FeedingSumo;
};


#else



void bananagrower::BeTalkedTo()
{
  static sLong Said;

  if(GetRelation(PLAYER) == HOSTILE)
    ProcessAndAddMessage(GetHostileReplies()[RandomizeReply(Said, GetHostileReplies().Size)]);
  else if(!game::TweraifIsFree())
  {
    if(GetRelation(PLAYER) != HOSTILE
       && Profession.Find("president", 0) != festring::NPos && !(RAND() % 7))
      ADD_MESSAGE("\"I'm glad Petrus spared my life even though I was the president.\"");

    ProcessAndAddMessage(GetFriendlyReplies()[RandomizeReply(Said, 6)]);
  }
  else
    ProcessAndAddMessage(GetFriendlyReplies()[6 + RandomizeReply(Said, 3)]);
}



void bananagrower::RandomizeProfession()
{
  switch(RAND_N(12))
  {
   case 0:
    Profession = CONST_S("the president of Tweraif");
    break;
   case 1:
    Profession = CONST_S("a diplomat");
    break;
   case 2:
    Profession = CONST_S("a teacher");
    break;
   case 3:
    Profession = CONST_S("a philosopher");
    break;
   case 4:
    Profession = CONST_S("a journalist");
    break;
   case 5:
    Profession = CONST_S("an alchemist");
    break;
   case 6:
    Profession = CONST_S("a renown mathematician");
    break;
   case 7:
    Profession = CONST_S("a priest of Silva");
    break;
   case 8:
   case 9:
   case 10:
   case 11:
    Profession = CONST_S("a professor of ");
    AddRandomScienceName(Profession);
    break;
  }
}



void bananagrower::PostConstruct()
{
  Stamina = MaxStamina / 5;
  RandomizeProfession();
  HasDroppedBananas = FeedingSumo = false;
}



void bananagrower::Save(outputfile& SaveFile) const
{
  humanoid::Save(SaveFile);
  SaveFile << Profession << HasDroppedBananas << FeedingSumo;
}



void bananagrower::Load(inputfile& SaveFile)
{
  humanoid::Load(SaveFile);
  SaveFile >> Profession >> HasDroppedBananas >> FeedingSumo;
}



void bananagrower::GetAICommand()
{
  if(game::TweraifIsFree())
  {
    humanoid::GetAICommand();
    return;
  }

  if(CheckForEnemies(false, false, true, true))
    return;

  if(!IsEnabled())
    return;

  cv2 BananaTarget = FeedingSumo ? SUMO_ROOM_POS + v2(1, 2) : v2(45, 45);

  if(GetPos() == BananaTarget)
  {
    itemvector ItemVector;
    GetStack()->FillItemVector(ItemVector);
    int BananasDropped = 0;
    uInt c;

    for(c = 0; c < ItemVector.size(); ++c)
      if(ItemVector[c]->IsBanana())
      {
  ItemVector[c]->MoveTo(GetStackUnder());
  ++BananasDropped;
      }

    if(BananasDropped)
    {
      if(CanBeSeenByPlayer())
  ADD_MESSAGE("%s drops %s.", CHAR_NAME(DEFINITE), BananasDropped == 1 ? "a banana" : "some bananas");

      return;
    }

    ItemVector.clear();
    GetStackUnder()->FillItemVector(ItemVector);
    int PeelsPickedUp = 0;

    for(c = 0; c < ItemVector.size(); ++c)
      if(ItemVector[c]->IsBananaPeel())
      {
  ItemVector[c]->MoveTo(GetStack());
  ++PeelsPickedUp;
      }

    if(PeelsPickedUp)
    {
      if(CanBeSeenByPlayer())
  ADD_MESSAGE("%s picks up %s.", CHAR_NAME(DEFINITE), PeelsPickedUp == 1 ? "a banana peel" : "some banana peels");

      return;
    }

    HasDroppedBananas = true;
  }

  if(!HasDroppedBananas)
  {
    SetGoingTo(BananaTarget);

    if(MoveTowardsTarget(true))
      return;
  }
  else if(GetPos().X == 54)
  {
    if(CanBeSeenByPlayer())
      ADD_MESSAGE("%s leaves the town to gather more bananas.", CHAR_NAME(DEFINITE));

    GetStack()->Clean();
    character* Sumo = game::GetSumo();
    FeedingSumo = Sumo && Sumo->GetNP() < (SATIATED_LEVEL + BLOATED_LEVEL) >> 1 && !(RAND() % 15);
    int Bananas = FeedingSumo ? 3 : 10;

    for(int c = 0; c < Bananas; ++c)
      GetStack()->AddItem(banana::Spawn());

    v2 Where = GetLevel()->GetNearestFreeSquare(this, v2(0, 45));

    if(Where == ERROR_V2)
      Where = GetLevel()->GetRandomSquare(this, NOT_IN_ROOM); // this is odd but at least it doesn't crash

    Move(Where, true);
    RandomizeProfession();
    RestoreBodyParts();
    RestoreHP();
    Stamina = MaxStamina / 5;
    ResetStates();
    TemporaryState = 0;

    if(CanBeSeenByPlayer())
      ADD_MESSAGE("%s enters the town.", CHAR_NAME(INDEFINITE));

    HasDroppedBananas = false;
  }
  else
  {
    SetGoingTo(v2(54, 45));

    if(MoveTowardsTarget(true))
      return;
  }

  EditAP(-1000);
}



truth bananagrower::HandleCharacterBlockingTheWay(character* Char, v2 Pos, int Dir)
{
  return Char->GetPos() == v2(45, 45) && (Displace(Char, true) || Hit(Char, Pos, Dir));
}



festring& bananagrower::ProcessMessage(festring& Msg) const
{
  character::ProcessMessage(Msg);
  SEARCH_N_REPLACE(Msg, "@pd", GetProfession());
  SEARCH_N_REPLACE(Msg, "@Pd", GetProfession().CapitalizeCopy());
  return Msg;
}
#endif
