#ifdef HEADER_PHASE
CHARACTER(necromancer, humanoid)
{
 public:
  virtual truth TryToRaiseZombie();
  virtual void RaiseSkeleton();
 protected:
  virtual void GetAICommand();
  int GetSpellAPCost() const;
};


#else



void necromancer::GetAICommand()
{
  SeekLeader(GetLeader());

  if(FollowLeader(GetLeader()))
    return;

  character* NearestEnemy = 0;
  sLong NearestEnemyDistance = 0x7FFFFFFF;
  v2 Pos = GetPos();

  for(int c = 0; c < game::GetTeams(); ++c)
    if(GetTeam()->GetRelation(game::GetTeam(c)) == HOSTILE)
    {
      for(std::list<character*>::const_iterator i = game::GetTeam(c)->GetMember().begin(); i != game::GetTeam(c)->GetMember().end(); ++i)
  if((*i)->IsEnabled())
  {
    sLong ThisDistance = Max<sLong>(abs((*i)->GetPos().X - Pos.X), abs((*i)->GetPos().Y - Pos.Y));

    if((ThisDistance < NearestEnemyDistance || (ThisDistance == NearestEnemyDistance && !(RAND() % 3))) && (*i)->CanBeSeenBy(this))
    {
      NearestEnemy = *i;
      NearestEnemyDistance = ThisDistance;
    }
  }
    }

  if(NearestEnemy && NearestEnemy->GetPos().IsAdjacent(Pos))
  {
    if(GetConfig() == MASTER_NECROMANCER && !(RAND() & 3))
    {
      if(CanBeSeenByPlayer())
  ADD_MESSAGE("%s invokes a spell and disappears.", CHAR_NAME(DEFINITE));

      TeleportRandomly(true);
      EditAP(-GetSpellAPCost());
      return;
    }
    else if(NearestEnemy->IsSmall()
      && GetAttribute(WISDOM) < NearestEnemy->GetAttackWisdomLimit()
      && !(RAND() & 3)
      && Hit(NearestEnemy, NearestEnemy->GetPos(), game::GetDirectionForVector(NearestEnemy->GetPos() - GetPos())))
      return;
  }

  if(!NearestEnemy)
  {
    if(!RAND_N(3) && TryToRaiseZombie())
      return;
  }
  else
  {
    if(!RAND_N(6) && TryToRaiseZombie())
      return;
  }

  if(NearestEnemy && !(RAND() % (GetConfig() == APPRENTICE_NECROMANCER ? 3 : 2)))
  {
    lsquare* Square = NearestEnemy->GetLSquareUnder();
    EditAP(-GetSpellAPCost());

    if(CanBeSeenByPlayer())
      ADD_MESSAGE("%s invokes a spell!", CHAR_NAME(DEFINITE));

    truth Interrupt = false;

    switch(GetConfig())
    {
     case APPRENTICE_NECROMANCER:
      RaiseSkeleton();
      break;
     case MASTER_NECROMANCER:
      if(RAND() % 5)
  RaiseSkeleton();
      else
      {
  Square->DrawLightning(v2(8, 8), WHITE, YOURSELF);

  beamdata Beam
    (
      this,
      CONST_S("killed by the spells of ") + GetName(INDEFINITE),
      YOURSELF,
      0
    );

  Square->Lightning(Beam);
  Interrupt = true;
      }

      break;
    }

    if(Interrupt) {
      if(CanBeSeenByPlayer())
        NearestEnemy->DeActivateVoluntaryAction(CONST_S("The spell of ") + GetName(DEFINITE) + CONST_S(" interrupts you."));
      else
        NearestEnemy->DeActivateVoluntaryAction(CONST_S("The spell interrupts you."));
    }
    return;
  }

  if(NearestEnemy && (NearestEnemyDistance < 10 || StateIsActivated(PANIC)) && RAND() & 3)
  {
    SetGoingTo((Pos << 1) - NearestEnemy->GetPos());

    if(MoveTowardsTarget(true))
      return;
  }

  if(CheckForDoors())
    return;

  if(CheckSadism())
    return;

  if(MoveRandomly())
    return;

  EditAP(-1000);
}



truth necromancer::TryToRaiseZombie()
{
  for(int c = 0; c < game::GetTeams(); ++c)
    for(std::list<character*>::const_iterator i = game::GetTeam(c)->GetMember().begin();
  i != game::GetTeam(c)->GetMember().end(); ++i)
      if(!(*i)->IsEnabled() && (*i)->GetMotherEntity()
   && (*i)->GetMotherEntity()->Exists()
   && (GetConfig() == MASTER_NECROMANCER
       || (*i)->GetMotherEntity()->GetSquareUnderEntity()->CanBeSeenBy(this)))
      {
  character* Zombie = (*i)->GetMotherEntity()->TryNecromancy(this);

  if(Zombie)
  {
    if(Zombie->CanBeSeenByPlayer())
      ADD_MESSAGE("%s calls %s back to cursed undead life.", CHAR_DESCRIPTION(DEFINITE), Zombie->CHAR_NAME(INDEFINITE));
    else if(CanBeSeenByPlayer())
      ADD_MESSAGE("%s casts a spell, but you notice no effect.", CHAR_NAME(DEFINITE));

    EditAP(-GetSpellAPCost());
    return true;
  }
      }

  return false;
}



void necromancer::RaiseSkeleton()
{
  /* Gum solution */

  const database* WarLordDataBase;
  databasecreator<character>::FindDataBase(WarLordDataBase, &skeleton::ProtoType, WAR_LORD);
  skeleton* Skeleton;

  if(GetConfig() == MASTER_NECROMANCER && !(WarLordDataBase->Flags & HAS_BEEN_GENERATED) && !(RAND() % 250))
  {
    Skeleton = skeleton::Spawn(WAR_LORD);
    Skeleton->SetTeam(GetTeam());
    Skeleton->PutNear(GetPos());
    Skeleton->SignalGeneration();

    if(Skeleton->CanBeSeenByPlayer())
      ADD_MESSAGE("The whole area trembles terribly as %s emerges from the ground.", Skeleton->CHAR_NAME(DEFINITE));
    else if(CanBeSeenByPlayer())
      ADD_MESSAGE("%s casts a powerful spell which makes the whole area tremble.", CHAR_NAME(DEFINITE));
    else
      ADD_MESSAGE("You feel the presence of an ancient evil being awakened from its long slumber. You shiver.");
  }
  else
  {
    Skeleton = skeleton::Spawn(GetConfig() == APPRENTICE_NECROMANCER ? 0 : WARRIOR, NO_EQUIPMENT);
    Skeleton->SetTeam(GetTeam());
    Skeleton->PutNear(GetPos());

    if(Skeleton->CanBeSeenByPlayer())
      ADD_MESSAGE("The ground shakes and %s emerges from it.", Skeleton->CHAR_NAME(INDEFINITE));
    else if(CanBeSeenByPlayer())
      ADD_MESSAGE("%s casts a spell, but you notice no effect.", CHAR_NAME(DEFINITE));
  }

  Skeleton->SetGenerationDanger(GetGenerationDanger());
  EditAP(-GetSpellAPCost());
}



int necromancer::GetSpellAPCost() const
{
  switch(GetConfig())
  {
   case APPRENTICE_NECROMANCER: return 2000;
   case MASTER_NECROMANCER: return 1000;
  }

  return 4000;
}
#endif
