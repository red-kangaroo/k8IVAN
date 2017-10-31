#ifdef HEADER_PHASE
CHARACTER(alchemist, humanoid)
{
protected:
  virtual void GetAICommand ();
  int GetSpellAPCost () const;
};


#else



void alchemist::GetAICommand () {
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
          sLong ThisDistance = Max<sLong>(abs((*i)->GetPos().X - Pos.X), abs((*i)->GetPos().Y-Pos.Y));
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
  truth Interrupt = false;
  // here we insert code for the alchemy
  if (NearestEnemy && !StateIsActivated(CONFUSED) && !(RAND()%2)) {
    if (NearestEnemy->IsHumanoid()) {
      beamdata Beam(
        this,
        CONST_S("killed by the spells of ") + GetName(INDEFINITE),
        YOURSELF,
        0
      );
      lsquare *Square = NearestEnemy->GetLSquareUnder();
      EditAP(-GetSpellAPCost());
      if (CanBeSeenByPlayer()) ADD_MESSAGE("%s invokes a spell!", CHAR_NAME(DEFINITE));
      Square->DrawParticles(RED);
      Square->SoftenMaterial(Beam);
      Interrupt = true;
    }
    if (Interrupt) {
      if (CanBeSeenByPlayer()) {
        NearestEnemy->DeActivateVoluntaryAction(CONST_S("The spell of ")+GetName(DEFINITE)+CONST_S(" interrupts you."));
      } else {
        NearestEnemy->DeActivateVoluntaryAction(CONST_S("The spell interrupts you."));
      }
    }
    return;
  }
  /*
  truth Changed = false;
  item* MainArmor = 0;

    int BodyPartToHit = NearestEnemy->GetRandomBodyPart();

    switch(BodyPartToHit) // okay, this is broken because it seg-faults, it needs to be replaced by a BEAM, in lsquare.cpp, as per mage: LowerEnchantment(), say
    {
      case TORSO_INDEX:
      MainArmor = NearestEnemy->GetEquipment(BODY_ARMOR_INDEX);
      break;
      case HEAD_INDEX:
      MainArmor = NearestEnemy->GetEquipment(HELMET_INDEX);
      break;
      case RIGHT_ARM_INDEX:
      MainArmor = NearestEnemy->GetEquipment(RIGHT_GAUNTLET_INDEX);
      break;
      case LEFT_ARM_INDEX:
      MainArmor = NearestEnemy->GetEquipment(LEFT_GAUNTLET_INDEX);
      break;
      case GROIN_INDEX:
      MainArmor = NearestEnemy->GetEquipment(BELT_INDEX);
      break;
      case RIGHT_LEG_INDEX:
      MainArmor = NearestEnemy->GetEquipment(RIGHT_BOOT_INDEX);
      break;
      case LEFT_LEG_INDEX:
      MainArmor = NearestEnemy->GetEquipment(LEFT_BOOT_INDEX);
      break;
      case NONE_INDEX:
      MainArmor = 0;
      break;
    }

    if(MainArmor)
    {
      festring Desc;
      MainArmor->AddName(Desc, UNARTICLED);

      if((MainArmor->GetMainMaterial()->GetCategoryFlags() & IS_METAL) && (MainArmor->GetMainMaterial()->GetConfig() != TIN))
      {
        MainArmor->ChangeMainMaterial(MAKE_MATERIAL(TIN));
        Changed = true;
        Interrupt = true;
      }
      else if((MainArmor->GetMainMaterial()->GetCategoryFlags() & CAN_BE_TAILORED) && (MainArmor->GetMainMaterial()->GetConfig() != CLOTH))
      {
        MainArmor->ChangeMainMaterial(MAKE_MATERIAL(CLOTH));
        Changed = true;
        Interrupt = true;
      }

      if(Changed && NearestEnemy->IsPlayer())
      {
        ADD_MESSAGE("Your %s softens into %s!", Desc.CStr(), MainArmor->GetMainMaterial()->GetName(false, false).CStr());
      }
      else if(Changed)
        ADD_MESSAGE("%s's %s softens into %s!", NearestEnemy->CHAR_DESCRIPTION(DEFINITE), Desc.CStr(), MainArmor->GetMainMaterial()->GetName(false, false).CStr());
    }
    if(!Changed) //may not need this message
      if(NearestEnemy->IsPlayer())
      {
        ADD_MESSAGE("Your %s vibrates slightly but remains unchanged.", MainArmor->CHAR_NAME(UNARTICLED) );
      }
      else
        ADD_MESSAGE("%s's %s vibrates slightly but remains unchanged.", NearestEnemy->CHAR_DESCRIPTION(DEFINITE), MainArmor->CHAR_NAME(UNARTICLED) );
  */
  //
  if (NearestEnemy && (NearestEnemyDistance < 10 || StateIsActivated(PANIC)) && (RAND()&3)) {
    SetGoingTo((Pos << 1)-NearestEnemy->GetPos());
    if (MoveTowardsTarget(true)) return;
  }
  //
  if (CheckForUsefulItemsOnGround()) return;
  if (CheckForDoors()) return;
  if (CheckSadism()) return;
  if (MoveRandomly()) return;
  EditAP(-1000);
}



int alchemist::GetSpellAPCost () const {
  //switch (GetConfig()) {
  //  case APPRENTICE_NECROMANCER: return 2000;
  //  case MASTER_NECROMANCER: return 1000;
  //}
  return 2000;
}
#endif
