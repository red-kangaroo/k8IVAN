#ifdef HEADER_PHASE
CHARACTER(darkmage, humanoid)
{
 protected:
  virtual void GetAICommand();
  int GetSpellAPCost() const;
};


#else



void darkmage::GetAICommand()
{
  SeekLeader(GetLeader());

  if(FollowLeader(GetLeader()))
    return;

  character* NearestEnemy = 0;
  sLong NearestEnemyDistance = 0x7FFFFFFF;
  character* RandomFriend = 0;
  charactervector Friend;
  v2 Pos = GetPos();

  for (int c = 0; c < game::GetTeams(); ++c) {
    if (GetTeam()->GetRelation(game::GetTeam(c)) == HOSTILE) {
      for (auto &tm : game::GetTeam(c)->GetMember()) {
        if (tm->IsEnabled()) {
          sLong ThisDistance = Max<sLong>(abs(tm->GetPos().X-Pos.X), abs(tm->GetPos().Y-Pos.Y));
          if ((ThisDistance < NearestEnemyDistance || (ThisDistance == NearestEnemyDistance && !(RAND()%3))) && tm->CanBeSeenBy(this)) {
            NearestEnemy = tm;
            NearestEnemyDistance = ThisDistance;
          }
        }
      }
    } else if (GetTeam()->GetRelation(game::GetTeam(c)) == FRIEND) {
      for (auto &tm : game::GetTeam(c)->GetMember()) {
        if (tm->IsEnabled() && tm->CanBeSeenBy(this)) Friend.push_back(tm);
      }
    }
  }

  if (NearestEnemy && NearestEnemy->GetPos().IsAdjacent(Pos)) {
    if (NearestEnemy->IsSmall() && GetAttribute(WISDOM) < NearestEnemy->GetAttackWisdomLimit() && !(RAND()%5) &&
        Hit(NearestEnemy, NearestEnemy->GetPos(), game::GetDirectionForVector(NearestEnemy->GetPos()-GetPos())))
    {
      return;
    }
    if ((GetConfig() == ARCH_MAGE && (RAND()&1)) || (GetConfig() == ELDER && !(RAND()&3))) {
      if (CanBeSeenByPlayer()) ADD_MESSAGE("%s invokes a spell and disappears.", CHAR_NAME(DEFINITE));
      TeleportRandomly(true);
      EditAP(-GetSpellAPCost());
      return;
    }
  }

  if (NearestEnemy && ((GetConfig() != APPRENTICE && NearestEnemyDistance < 10) || StateIsActivated(PANIC)) && (RAND()&3)) {
    SetGoingTo((Pos<<1)-NearestEnemy->GetPos());
    if (MoveTowardsTarget(true)) return;
  }

  if (Friend.size() && !(RAND()&3)) {
    RandomFriend = Friend[RAND()%Friend.size()];
    NearestEnemy = 0;
  }

  beamdata Beam (
    this,
    CONST_S("killed by the spells of ") + GetName(INDEFINITE),
    YOURSELF,
    0
  );

  if (NearestEnemy) {
    lsquare *Square = NearestEnemy->GetLSquareUnder();
    EditAP(-GetSpellAPCost());

    if (CanBeSeenByPlayer()) ADD_MESSAGE("%s invokes a spell!", CHAR_NAME(DEFINITE));

    auto cfg = GetConfig();
    if (cfg == APPRENTICE) {
      Square->DrawLightning(v2(8, 8), WHITE, YOURSELF);
      Square->Lightning(Beam);
    } else if (cfg == BATTLE_MAGE) {
      if (RAND()%20) {
        Square->DrawLightning(v2(8, 8), WHITE, YOURSELF);
        Square->Lightning(Beam);
      } else {
        Square->DrawParticles(RED);
        Square->LowerEnchantment(Beam);
      }
    } else if (cfg == ELDER) {
      switch (RAND()%20) {
        case 0: case 1: case 2: Square->DrawParticles(RED); Square->Strike(Beam); break;
        case 3: Square->DrawParticles(RED); Square->FireBall(Beam); break;
        case 4: case 5: case 6: Square->DrawParticles(RED); Square->Slow(Beam); break;
        case 7: Square->DrawParticles(RED); Square->Teleport(Beam); break;
        case 8: case 9: case 10: Square->DrawParticles(RED); Square->LowerEnchantment(Beam); break;
        default: Square->DrawLightning(v2(8, 8), WHITE, YOURSELF); Square->Lightning(Beam); break;
      }
    } else if (cfg == ARCH_MAGE) {
      switch (RAND()%20) {
        case 0: case 1: case 2: Square->DrawParticles(RED); Square->FireBall(Beam); break;
        case 3:
          {
            character *Char = NearestEnemy->DuplicateToNearestSquare(this, CHANGE_TEAM|MIRROR|(1000 << LE_BASE_SHIFT)|(1000 << LE_RAND_SHIFT));
            if (Char) {
              if (Char->CanBeSeenByPlayer()) ADD_MESSAGE("%s materializes!", Char->CHAR_NAME(INDEFINITE));
              break;
            }
          }
          /*k8:falltrough?*/
        case 4: case 5: Square->DrawParticles(RED); Square->Slow(Beam); break;
        case 6: Square->DrawParticles(RED); Square->Teleport(Beam); break;
        case 7: case 8: case 9: Square->DrawParticles(RED); Square->LowerEnchantment(Beam); break;
        case 10:
          {
            golem *Golem = golem::Spawn(RAND()%3 ? ARCANITE : OCTIRON);
            v2 Where = GetLevel()->GetNearestFreeSquare(Golem, Square->GetPos());
            if (Where == ERROR_V2) {
              if (CanBeSeenByPlayer()) ADD_MESSAGE("Nothing happens.");
              delete Golem;
            } else {
              Golem->SetGenerationDanger(GetGenerationDanger());
              Golem->SetTeam(GetTeam());
              Golem->PutTo(Where);
              if (Golem->CanBeSeenByPlayer()) ADD_MESSAGE("Suddenly %s materializes!", Golem->CHAR_NAME(INDEFINITE));
              Golem->GetLSquareUnder()->DrawParticles(RED);
            }
            break;
          }
        default: Square->DrawParticles(RED); Square->Strike(Beam); break;
      }
    }

    if (CanBeSeenByPlayer()) {
      NearestEnemy->DeActivateVoluntaryAction(CONST_S("The spell of ") + GetName(DEFINITE) + CONST_S(" interrupts you."));
    } else {
      NearestEnemy->DeActivateVoluntaryAction(CONST_S("The spell interrupts you."));
    }

    return;
  }

  if (RandomFriend) {
    lsquare *Square = RandomFriend->GetLSquareUnder();
    EditAP(-GetSpellAPCost());
    Square->DrawParticles(RED);

    auto cfg = GetConfig();
    if (cfg == APPRENTICE || cfg == BATTLE_MAGE) {
      Square->Haste(Beam);
    } else {
      if (cfg == ARCH_MAGE) {
        if (!(RAND()&31)) {
          RandomFriend->DuplicateToNearestSquare(this, CHANGE_TEAM);
          return;
        }
      }
      if (cfg == ELDER || cfg == ARCH_MAGE) {
        if (RAND()&1) Square->Invisibility(Beam); else Square->Haste(Beam);
      }
    }

    return;
  }

  if (CheckForDoors()) return;
  if (CheckSadism()) return;
  if (MoveRandomly()) return;

  EditAP(-1000);
}


int darkmage::GetSpellAPCost () const {
  auto cfg = GetConfig();
  if (cfg == APPRENTICE) return 4000;
  if (cfg == BATTLE_MAGE) return 2000;
  if (cfg == ELDER) return 1000;
  if (cfg == ARCH_MAGE) return 500;
  return 4000;
}


#endif
