#ifdef HEADER_PHASE
CHARACTER(slave, playerkind)
{
 public:
  virtual void BeTalkedTo();
  virtual void GetAICommand();
  virtual col16 GetHairColor() const { return humanoid::GetHairColor(); }
  virtual col16 GetEyeColor() const { return humanoid::GetEyeColor(); }
 protected:
  virtual void PostConstruct();
};


#else



void slave::BeTalkedTo()
{
  if(GetRelation(PLAYER) == HOSTILE)
  {
    ADD_MESSAGE("\"Yikes!\"");
    return;
  }

  room* Room = GetHomeRoom();

  if(Room && Room->MasterIsActive())
  {
    character* Master = Room->GetMaster();

    if(PLAYER->GetMoney() >= 50)
    {
      ADD_MESSAGE("%s talks: \"Do you want to buy me? \1Y50\2 gold pieces. I work very hard.\"", CHAR_DESCRIPTION(DEFINITE));

      if(game::TruthQuestion(CONST_S("Do you want to buy him?")))
      {
  PLAYER->SetMoney(PLAYER->GetMoney() - 50);
  Master->SetMoney(Master->GetMoney() + 50);
  ChangeTeam(PLAYER->GetTeam());
  RemoveHomeData();
      }
    }
    else
      ADD_MESSAGE("\"Don't touch me! Master doesn't want people to touch sale items. I'm worth \1YT50\2 gold pieces, you know!\"");

    return;
  }

  if(GetTeam() == PLAYER->GetTeam())
  {
    if((PLAYER->GetMainWielded() && PLAYER->GetMainWielded()->IsWhip()) || (PLAYER->GetSecondaryWielded() && PLAYER->GetSecondaryWielded()->IsWhip()))
      ADD_MESSAGE("\"Don't hit me! I work! I obey! I don't think!\"");
    else
      character::BeTalkedTo();
  }
  else
    ADD_MESSAGE("\"I'm free! Rejoice!\"");
}



void slave::GetAICommand()
{
  SeekLeader(GetLeader());

  if(CheckForEnemies(true, true, true))
    return;

  if(CheckForUsefulItemsOnGround())
    return;

  if(FollowLeader(GetLeader()))
    return;

  if(CheckForDoors())
    return;

  if(!GetHomeRoom() || !GetHomeRoom()->MasterIsActive())
  {
    RemoveHomeData();

    if(MoveRandomly())
      return;
  }
  else if(MoveTowardsHomePos())
    return;

  EditAP(-1000);
}



void slave::PostConstruct()
{
  Talent = TALENT_STRONG;
  Weakness = TALENT_CLEVER;
}
#endif
