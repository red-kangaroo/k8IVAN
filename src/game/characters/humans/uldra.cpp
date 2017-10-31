#ifdef HEADER_PHASE
CHARACTER(uldra, humanoid)
{
protected:
  virtual void GetAICommand ();
  int GetSpellAPCost () const;
};


#else


void uldra::GetAICommand () {
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
          if ((ThisDistance < NearestEnemyDistance || (ThisDistance == NearestEnemyDistance && !(RAND() % 3))) && (*i)->CanBeSeenBy(this)) {
            NearestEnemy = *i;
            NearestEnemyDistance = ThisDistance;
          }
        }
      }
    }
  }
  //
  if (NearestEnemy && NearestEnemy->GetPos().IsAdjacent(Pos)) {
    if (!(RAND()&3)) {
      if (CanBeSeenByPlayer()) ADD_MESSAGE("%s grins at you and disappears.", CHAR_NAME(DEFINITE));
      TeleportRandomly(true);
      EditAP(-GetSpellAPCost());
      return;
    } else if (NearestEnemy->IsSmall() &&
               GetAttribute(WISDOM) < NearestEnemy->GetAttackWisdomLimit() &&
               !(RAND()&3) &&
               Hit(NearestEnemy, NearestEnemy->GetPos(), game::GetDirectionForVector(NearestEnemy->GetPos()-GetPos()))) {
      return;
    }
  }
  //
  if (NearestEnemy && (NearestEnemyDistance < 10 || StateIsActivated(PANIC)) && (RAND()&3)) {
    SetGoingTo((Pos<<1)-NearestEnemy->GetPos());
    if (MoveTowardsTarget(true)) return;
  }
  //
  if (NearestEnemy) {
    lsquare *Square = NearestEnemy->GetLSquareUnder();
    //
    EditAP(-GetSpellAPCost());
    switch (RAND()%50) {
      case 0:
      case 1:
      case 2: {
        //
        character *Buffalo = buffalo::Spawn();
        v2 Where = GetLevel()->GetNearestFreeSquare(Buffalo, Square->GetPos());
        //
        if (Where == ERROR_V2) {
          if (CanBeSeenByPlayer()) ADD_MESSAGE("%s grins and invokes a spell, but failed miserably.", CHAR_NAME(DEFINITE));
          delete Buffalo;
        } else {
          if (CanBeSeenByPlayer()) ADD_MESSAGE("%s grins and invokes a spell!", CHAR_NAME(DEFINITE));
          Buffalo->SetGenerationDanger(GetGenerationDanger());
          Buffalo->SetTeam(GetTeam());
          Buffalo->PutTo(Where);
          if (Buffalo->CanBeSeenByPlayer()) ADD_MESSAGE("Suddenly %s materializes!", Buffalo->CHAR_NAME(INDEFINITE));
          Buffalo->GetLSquareUnder()->DrawParticles(RED);
        }
        } break;
      case 3: {
        if (CanBeSeenByPlayer()) ADD_MESSAGE("%s grins and invokes a spell!", CHAR_NAME(DEFINITE));
        //
        character *Twoheadedmoose = twoheadedmoose::Spawn();
        v2 Where = GetLevel()->GetNearestFreeSquare(Twoheadedmoose, Square->GetPos());
        //
        if (Where == ERROR_V2) {
          if (CanBeSeenByPlayer()) ADD_MESSAGE("Nothing happens."); delete Twoheadedmoose;
        } else {
          Twoheadedmoose->SetGenerationDanger(GetGenerationDanger());
          Twoheadedmoose->SetTeam(GetTeam());
          Twoheadedmoose->PutTo(Where);
          if (Twoheadedmoose->CanBeSeenByPlayer()) ADD_MESSAGE("Suddenly %s materializes!", Twoheadedmoose->CHAR_NAME(INDEFINITE));
          Twoheadedmoose->GetLSquareUnder()->DrawParticles(RED);
        }
        } break;
      case 4: {
        if (CanBeSeenByPlayer()) ADD_MESSAGE("%s grins and invokes a spell!", CHAR_NAME(DEFINITE));
        //
        character *Bear = bear::Spawn(RAND() % 3 ? BLACK_BEAR : GRIZZLY_BEAR);
        v2 Where = GetLevel()->GetNearestFreeSquare(Bear, Square->GetPos());
        //
        if (Where == ERROR_V2) {
          if (CanBeSeenByPlayer()) ADD_MESSAGE("Nothing happens.");
          delete Bear;
        } else {
          Bear->SetGenerationDanger(GetGenerationDanger());
          Bear->SetTeam(GetTeam());
          Bear->PutTo(Where);
          if (Bear->CanBeSeenByPlayer()) ADD_MESSAGE("Suddenly %s materializes!", Bear->CHAR_NAME(INDEFINITE));
          Bear->GetLSquareUnder()->DrawParticles(RED);
        }
        } break;
      default: break;
    }
    return;
  }
  //
  if (CheckForUsefulItemsOnGround()) return;
  if (CheckForDoors()) return;
  if (CheckSadism()) return;
  if (MoveRandomly()) return;
  EditAP(-1000);
}


int uldra::GetSpellAPCost () const {
  return 4000;
}


#endif
