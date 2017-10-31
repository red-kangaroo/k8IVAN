#ifdef HEADER_PHASE
CHARACTER(adultalien, nonhumanoid)
{
 protected:
  virtual bodypart* MakeBodyPart (int) const;
  virtual void GetAICommand ();
};


#else


bodypart *adultalien::MakeBodyPart (int) const { return adultalientorso::Spawn(0, NO_MATERIALS); }


void adultalien::GetAICommand () {
  SeekLeader(GetLeader());

  if (FollowLeader(GetLeader())) return;

  if (!game::IsInWilderness() && !(RAND() & 7)) {
    GetLSquareUnder()->SpillFluid(this, liquid::Spawn(ALIEN_DROOL, 25 + RAND() % 50), false, false);
  }

  character *NearestChar = 0;
  long NearestDistance = 0x7FFFFFFF;
  v2 Pos = GetPos();
  int Hostiles = 0;

  for (int c = 0; c < game::GetTeams(); ++c) {
    if (GetTeam()->GetRelation(game::GetTeam(c)) == HOSTILE) {
      for(std::list<character*>::const_iterator i = game::GetTeam(c)->GetMember().begin(); i != game::GetTeam(c)->GetMember().end(); ++i) {
        if((*i)->IsEnabled() && GetAttribute(WISDOM) < (*i)->GetAttackWisdomLimit())
        {
          long ThisDistance = Max<long>(abs((*i)->GetPos().X - Pos.X), abs((*i)->GetPos().Y - Pos.Y));
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
      }
    }
  }

  if (NearestChar) {
    if (NearestChar->IsStuck() || !NearestChar->HasAllBodyParts() || !NearestChar->IsConscious() ||
        NearestChar->GetHP() < GetHP() || NearestChar->IsInBadCondition() || NearestChar->IsRetreating())
    {
      SetGoingTo(NearestChar->GetPos());
    } else {
      SetGoingTo((Pos << 1) - NearestChar->GetPos());
    }

    if (MoveTowardsTarget(true)) return;
  }

  if (MoveRandomly()) return;

  EditAP(-1000);
}


#endif
