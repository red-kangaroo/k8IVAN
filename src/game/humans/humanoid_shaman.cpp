#ifdef HEADER_PHASE
CHARACTER(shaman, humanoid)
{
protected:
  virtual void GetAICommand ();
  virtual void PostConstruct ();
  int GetSpellAPCost () const;
};


#else



void shaman::GetAICommand() {
  //SeekLeader(GetLeader());
  ////
  //if (FollowLeader(GetLeader())) return;
  character *NearestEnemy = 0;
  sLong NearestEnemyDistance = 0x7FFFFFFF;
  v2 Pos = GetPos();
  //
  for (int c = 0; c < game::GetTeams(); ++c) {
    if (GetTeam()->GetRelation(game::GetTeam(c)) == HOSTILE) {
      for(std::list<character*>::const_iterator i = game::GetTeam(c)->GetMember().begin(); i != game::GetTeam(c)->GetMember().end(); ++i) {
        if ((*i)->IsEnabled()) {
          sLong ThisDistance = Max<sLong>(abs((*i)->GetPos().X-Pos.X), abs((*i)->GetPos().Y-Pos.Y));
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
  beamdata Beam (
    this,
    CONST_S("killed by the spells of ")+GetName(INDEFINITE),
    YOURSELF,
    0
  );
  //
  if (NearestEnemy && !StateIsActivated(CONFUSED) && !(RAND()%3)) {
    lsquare *Square = NearestEnemy->GetLSquareUnder();
    EditAP(-GetSpellAPCost());
    if (CanBeSeenByPlayer()) ADD_MESSAGE("%s invokes a spell!", CHAR_NAME(DEFINITE));
    switch (RAND()%33) {
     case 0: case 1: case 2: Square->DrawParticles(RED); Square->Parasitize(Beam); break;
     case 3: case 4: case 5: case 6: Square->DrawParticles(RED); Square->Slow(Beam); break;
     case 7: Square->DrawParticles(RED); Square->Confuse(Beam); break;
     case 8: case 9: case 10: Square->DrawParticles(RED); Square->InstillPanic(Beam); break;
     default: break;
    }
    if (CanBeSeenByPlayer()) {
      NearestEnemy->DeActivateVoluntaryAction(CONST_S("The spell of ")+GetName(DEFINITE)+CONST_S(" interrupts you."));
    } else {
      NearestEnemy->DeActivateVoluntaryAction(CONST_S("The spell interrupts you."));
    }
    return;
  }
  //
  //fprintf(stderr, "shaman tries to throw...\n");
  if (!StateIsActivated(CONFUSED) && CheckThrowItemOpportunity()) return;
  //fprintf(stderr, "shaman can't throw...\n");
  character::GetAICommand();
}



int shaman::GetSpellAPCost () const {
  //switch (GetConfig()) {
  //  case APPRENTICE_NECROMANCER: return 2000;
  //  case MASTER_NECROMANCER: return 1000;
  //}
  return 2000;
}



/*
void shaman::CreateInitialEquipment(int SpecialFlags)
{
  //SetRightWielded(meleeweapon::Spawn(AXE, SpecialFlags));
  //SetLeftWielded(meleeweapon::Spawn(AXE, SpecialFlags));
  GetStack()->AddItem(potion::Spawn(GLASS, POISON_LIQUID));

  if(RAND() & 2)
    GetStack()->AddItem(potion::Spawn(GLASS, VINEGAR));

  if(RAND() & 2)
    GetStack()->AddItem(potion::Spawn(GLASS, LIQUID_HORROR));

  if(RAND() & 5)
    GetStack()->AddItem(potion::Spawn(GLASS, SULPHURIC_ACID));

  GetStack()->AddItem(lantern::Spawn());
  //GetCWeaponSkill(AXES)->AddHit(GetWSkillHits());
  //GetCurrentRightSWeaponSkill()->AddHit(GetWSkillHits());
}
*/


void shaman::PostConstruct () {
  if (!RAND_N(25)) GainIntrinsic(LEPROSY);
}
#endif
