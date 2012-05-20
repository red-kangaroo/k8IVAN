#ifdef HEADER_PHASE
CHARACTER(mysticfrog, frog)
{
 public:
  virtual void GetAICommand();
 protected:
  virtual int GetBodyPartWobbleData(int) const;
  virtual bodypart* MakeBodyPart(int) const;
  int GetSpellAPCost() const { return 1500; }
};


#else



int mysticfrog::GetBodyPartWobbleData(int) const { return WOBBLE_HORIZONTALLY|(1 << WOBBLE_SPEED_SHIFT)|(3 << WOBBLE_FREQ_SHIFT); }



bodypart* mysticfrog::MakeBodyPart(int) const { return mysticfrogtorso::Spawn(0, NO_MATERIALS); }



void mysticfrog::GetAICommand()
{
  SeekLeader(GetLeader());

  if(FollowLeader(GetLeader()))
    return;

  character* NearestEnemy = 0;
  sLong NearestEnemyDistance = 0x7FFFFFFF;
  character* RandomFriend = 0;
  charactervector Friend;
  v2 Pos = GetPos();
  truth Enemies = false;

  for(int c = 0; c < game::GetTeams(); ++c)
  {
    if(GetTeam()->GetRelation(game::GetTeam(c)) == HOSTILE)
    {
      for(std::list<character*>::const_iterator i = game::GetTeam(c)->GetMember().begin(); i != game::GetTeam(c)->GetMember().end(); ++i)
  if((*i)->IsEnabled())
  {
    Enemies = true;
    sLong ThisDistance = Max<sLong>(abs((*i)->GetPos().X - Pos.X), abs((*i)->GetPos().Y - Pos.Y));

    if((ThisDistance < NearestEnemyDistance || (ThisDistance == NearestEnemyDistance && !(RAND() % 3))) && (*i)->CanBeSeenBy(this))
    {
      NearestEnemy = *i;
      NearestEnemyDistance = ThisDistance;
    }
  }
    }
    else if(GetTeam()->GetRelation(game::GetTeam(c)) == FRIEND)
    {
      for(std::list<character*>::const_iterator i = game::GetTeam(c)->GetMember().begin(); i != game::GetTeam(c)->GetMember().end(); ++i)
  if((*i)->IsEnabled() && (*i)->CanBeSeenBy(this))
    Friend.push_back(*i);
    }
  }

  if(NearestEnemy && NearestEnemy->GetPos().IsAdjacent(Pos))
  {
    if(NearestEnemy->IsSmall()
       && GetAttribute(WISDOM) < NearestEnemy->GetAttackWisdomLimit()
       && !(RAND() % 5)
       && Hit(NearestEnemy, NearestEnemy->GetPos(), game::GetDirectionForVector(NearestEnemy->GetPos() - GetPos())))
      return;
    else if(!(RAND() & 3))
    {
      if(CanBeSeenByPlayer())
  ADD_MESSAGE("%s invokes a spell and disappears.", CHAR_NAME(DEFINITE));

      TeleportRandomly(true);
      EditAP(-GetSpellAPCost());
      return;
    }
  }

  if(NearestEnemy && (NearestEnemyDistance < 10 || StateIsActivated(PANIC)) && RAND() & 3)
  {
    SetGoingTo((Pos << 1) - NearestEnemy->GetPos());

    if(MoveTowardsTarget(true))
      return;
  }

  if(Friend.size() && !(RAND() & 3))
  {
    RandomFriend = Friend[RAND() % Friend.size()];
    NearestEnemy = 0;
  }

  if(GetRelation(PLAYER) == HOSTILE && PLAYER->CanBeSeenBy(this) && !RAND_4)
    NearestEnemy = PLAYER;

  beamdata Beam
    (
      this,
      CONST_S("killed by the spells of ") + GetName(INDEFINITE),
      YOURSELF,
      0
    );

  if(NearestEnemy)
  {
    lsquare* Square = NearestEnemy->GetLSquareUnder();
    EditAP(-GetSpellAPCost());

    if(CanBeSeenByPlayer())
      ADD_MESSAGE("%s invokes a spell!", CHAR_NAME(DEFINITE));

    switch(RAND() % 20)
    {
     case 0:
     case 1:
     case 2:
     case 3:
     case 4:
     case 5: Square->DrawParticles(RED); if(NearestEnemy->TeleportRandomItem(GetConfig() == DARK)) break;
     case 6:
     case 7:
     case 8:
     case 9:
     case 10: Square->DrawParticles(RED); Square->Teleport(Beam); break;
     case 11:
     case 12:
     case 13:
     case 14: Square->DrawParticles(RED); Square->Slow(Beam); break;
     case 15: Square->DrawParticles(RED); Square->LowerEnchantment(Beam); break;
     default: Square->DrawLightning(v2(8, 8), WHITE, YOURSELF); Square->Lightning(Beam); break;
    }

    if(CanBeSeenByPlayer())
      NearestEnemy->DeActivateVoluntaryAction(CONST_S("The spell of ") + GetName(DEFINITE) + CONST_S(" interrupts you."));
    else
      NearestEnemy->DeActivateVoluntaryAction(CONST_S("The spell interrupts you."));

    return;
  }

  if(RandomFriend && Enemies)
  {
    lsquare* Square = RandomFriend->GetLSquareUnder();
    EditAP(-GetSpellAPCost());
    Square->DrawParticles(RED);

    if(RAND() & 1)
      Square->Invisibility(Beam);
    else
      Square->Haste(Beam);

    return;
  }

  StandIdleAI();
}
#endif
