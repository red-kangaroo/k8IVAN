#ifdef HEADER_PHASE
CHARACTER(warlock, humanoid)
{
protected:
  virtual void GetAICommand ();
  int GetSpellAPCost() const;
};


#else



void warlock::GetAICommand () {
  SeekLeader(GetLeader());
  if (FollowLeader(GetLeader())) return;
  //
  character *NearestEnemy = 0;
  sLong NearestEnemyDistance = 0x7FFFFFFF;
  v2 Pos = GetPos();
  //
  for (int c = 0; c < game::GetTeams(); ++c) {
    if (GetTeam()->GetRelation(game::GetTeam(c)) == HOSTILE) {
      for (std::list<character*>::const_iterator i = game::GetTeam(c)->GetMember().begin(); i != game::GetTeam(c)->GetMember().end(); ++i) {
        if ((*i)->IsEnabled()) {
          sLong ThisDistance = Max<sLong>(abs((*i)->GetPos().X - Pos.X), abs((*i)->GetPos().Y - Pos.Y));
          //
          if ((ThisDistance < NearestEnemyDistance || (ThisDistance == NearestEnemyDistance && !(RAND()%3))) && (*i)->CanBeSeenBy(this)) {
            NearestEnemy = *i;
            NearestEnemyDistance = ThisDistance;
          }
        }
      }
    }
  }
  //
  if (NearestEnemy && NearestEnemy->GetPos().IsAdjacent(Pos)) {
    if (/*GetConfig() == MASTER_NECROMANCER && */!StateIsActivated(CONFUSED) && !(RAND()&3)) {
      if (CanBeSeenByPlayer()) ADD_MESSAGE("%s invokes a spell and disappears.", CHAR_NAME(DEFINITE));
      TeleportRandomly(true);
      EditAP(-GetSpellAPCost());
      return;
    } else if (NearestEnemy->IsSmall() && GetAttribute(WISDOM) < NearestEnemy->GetAttackWisdomLimit() && !(RAND()&3) &&
               Hit(NearestEnemy, NearestEnemy->GetPos(), game::GetDirectionForVector(NearestEnemy->GetPos()-GetPos()))) {
      return;
    }
  }
  //
  if (NearestEnemy && (NearestEnemyDistance < 10 || StateIsActivated(PANIC)) && (RAND()&3)) {
    SetGoingTo((Pos << 1)-NearestEnemy->GetPos());
    if (MoveTowardsTarget(true)) return;
  }
  //
  if (!StateIsActivated(CONFUSED) && CheckAIZapOpportunity()) return;
  if (CheckForUsefulItemsOnGround()) return;
  if (CheckForDoors()) return;
  if (CheckSadism()) return;
  if (MoveRandomly()) return;
  EditAP(-1000);
}



int warlock::GetSpellAPCost () const {
  //switch(GetConfig()) {
  //  case APPRENTICE_NECROMANCER: return 2000;
  //  case MASTER_NECROMANCER: return 1000;
  //}
  return 2000;
}
#endif
