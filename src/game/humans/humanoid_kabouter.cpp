#ifdef HEADER_PHASE
CHARACTER(kabouter, humanoid)
{
public:
  kabouter() : LastTamed(0) {}

  virtual void Load(inputfile &);
  virtual void Save(outputfile &) const;
  //virtual int GetAttribute (int, truth = true) const;
  virtual void FinalProcessForBone ();

protected:
  virtual void GetAICommand ();
  int GetSpellAPCost () const;

protected:
  ulong LastTamed;
};


#else


void kabouter::Save (outputfile &SaveFile) const {
  humanoid::Save(SaveFile);
  SaveFile << LastTamed;
}


void kabouter::Load (inputfile &SaveFile) {
  humanoid::Load(SaveFile);
  SaveFile >> LastTamed;
}


void kabouter::FinalProcessForBone()
{
  humanoid::FinalProcessForBone();
  LastTamed = 0;
}


void kabouter::GetAICommand () {
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
  if (NearestEnemy && (NearestEnemyDistance < 10 || StateIsActivated(PANIC)) && (RAND()&3)) {
    SetGoingTo((Pos << 1)-NearestEnemy->GetPos());
    if (MoveTowardsTarget(true)) return;
  }
  //
  if ((/*LastTamed &&*/ game::GetTick()-LastTamed < 100) && NearestEnemy && NearestEnemyDistance < 2) {
    // how about LastTamed < 500
    //do nothing, otherwise do the charming
  } else if (NearestEnemy) {
    LastTamed = game::GetTick();
    for (int d = 0; d < GetNeighbourSquares(); ++d) {
      square *Square = GetNeighbourSquare(d);
      //
      if (Square) {
        character *Char = Square->GetCharacter();
        if (Char) {
          if (!Char->IsPlayer() /*&& (GetRelation(Char) == HOSTILE)*/) {
            //check the creature's hostility at this point
            if (CanBeSeenByPlayer()) {
              if (PLAYER->CanHear()) ADD_MESSAGE("%s whispers mesmerizing spells.", CHAR_NAME(DEFINITE));
              else ADD_MESSAGE("You see %s mouthing mesmerizing spells.", CHAR_NAME(DEFINITE));
            }
            if (Char->CanHear()) {
              if (Char->CanTameWithLyre(this)) {
                //if(Char->GetTeam() == GetTeam() && Char->CanBeSeenByPlayer()) ADD_MESSAGE("%s seems to be very happy.", Char->CHAR_NAME(DEFINITE)); else
                if (Char->GetRelation(this) == HOSTILE && Char->CanBeSeenByPlayer()) {
                  ADD_MESSAGE("%s stops fighting.", Char->CHAR_NAME(DEFINITE));
                  if (Char->CanBeSeenByPlayer()) ADD_MESSAGE("%s seems to be very friendly towards %s.", Char->CHAR_NAME(DEFINITE), CHAR_NAME(DEFINITE));
                }
                Char->ChangeTeam(GetTeam());
              } else if (Char->CanBeSeenByPlayer()) {
                ADD_MESSAGE("%s resists %s's charming spells.", Char->CHAR_NAME(DEFINITE), CHAR_NAME(DEFINITE));
              }
            } else if (Char->CanBeSeenByPlayer()) {
              ADD_MESSAGE("%s seems not affected.", Char->CHAR_NAME(DEFINITE));
            }
          }
        }
      }
    }
    EditAP(-1000);
    game::CallForAttention(GetPos(), 100);
    return;
  }
  //
  if (CheckAIZapOpportunity()) return;
  if (CheckForUsefulItemsOnGround()) return;
  if (CheckForDoors()) return;
  if (CheckSadism()) return;
  if (MoveRandomly()) return;
  EditAP(-1000);
}


int kabouter::GetSpellAPCost () const {
  return 4000;
}


#endif
