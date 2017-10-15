#ifdef HEADER_PHASE
CHARACTER(spider, nonhumanoid)
{
 protected:
  virtual truth SpecialBiteEffect(character*, v2, int, int, truth, truth Critical, int DoneDamage);
  virtual void GetAICommand();
  virtual bodypart* MakeBodyPart(int) const;
};


#else



bodypart* spider::MakeBodyPart(int) const { return spidertorso::Spawn(0, NO_MATERIALS); }



truth spider::SpecialBiteEffect(character* Char, v2, int, int, truth BlockedByArmour, truth Critical, int DoneDamage)
{
  if(!BlockedByArmour)
  {
    Char->BeginTemporaryState(POISONED, GetConfig() == LARGE ? 80 + RAND_N(40) : 400 + RAND_N(200));
    return true;
  }
  else
    return false;
}



void spider::GetAICommand()
{
  SeekLeader(GetLeader());

  if(FollowLeader(GetLeader()))
    return;

  character* NearestChar = 0;
  sLong NearestDistance = 0x7FFFFFFF;
  v2 Pos = GetPos();
  int Hostiles = 0;

  for(int c = 0; c < game::GetTeams(); ++c)
    if(GetTeam()->GetRelation(game::GetTeam(c)) == HOSTILE)
      for(std::list<character*>::const_iterator i = game::GetTeam(c)->GetMember().begin();
    i != game::GetTeam(c)->GetMember().end(); ++i)
  if((*i)->IsEnabled() && GetAttribute(WISDOM) < (*i)->GetAttackWisdomLimit())
  {
    sLong ThisDistance = Max<sLong>(abs((*i)->GetPos().X - Pos.X), abs((*i)->GetPos().Y - Pos.Y));
    ++Hostiles;

    if((ThisDistance < NearestDistance
        || (ThisDistance == NearestDistance && !(RAND() % 3)))
       && (*i)->CanBeSeenBy(this, false, IsGoingSomeWhere())
       && (!IsGoingSomeWhere() || HasClearRouteTo((*i)->GetPos())))
    {
      NearestChar = *i;
      NearestDistance = ThisDistance;
    }
  }

  if(Hostiles && !RAND_N(Max(80 / Hostiles, 8)))
  {
    web* Web = web::Spawn();
    Web->SetStrength(GetConfig() == LARGE ? 10 : 25);

    if(GetLSquareUnder()->AddTrap(Web))
    {
      if(CanBeSeenByPlayer())
  ADD_MESSAGE("%s spins a web.", CHAR_NAME(DEFINITE));

      EditAP(-1000);
      return;
    }
  }

  if(NearestChar)
  {
    if(NearestChar->IsStuck())
      SetGoingTo(NearestChar->GetPos());
    else
      SetGoingTo((Pos << 1) - NearestChar->GetPos());

    if(MoveTowardsTarget(true))
      return;
  }

  if(MoveRandomly())
    return;

  EditAP(-1000);
}
#endif
