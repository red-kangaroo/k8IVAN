#ifdef HEADER_PHASE
CHARACTER(mindworm, nonhumanoid)
{
 protected:
  virtual void GetAICommand();
  virtual void TryToImplantLarvae(character*);
  virtual void PsiAttack(character*);
};


#else



void mindworm::GetAICommand()
{
  character* NeighbourEnemy = GetRandomNeighbour(HOSTILE);


  if(NeighbourEnemy && NeighbourEnemy->IsHumanoid() && NeighbourEnemy->HasHead()
  && !NeighbourEnemy->IsInfectedByMindWorm())
  {
    TryToImplantLarvae(NeighbourEnemy);
    return;
  }

  character* NearestEnemy = GetNearestEnemy();

  if(NearestEnemy)
  {
    PsiAttack(NearestEnemy);
    return;
  }

  if(MoveRandomly())
    return;

  EditAP(-1000);
}



void mindworm::TryToImplantLarvae(character* Victim)
{
  if(Victim->MindWormCanPenetrateSkull(this))
  {
    Victim->SetCounterToMindWormHatch(100);
    if(Victim->IsPlayer())
    {
      ADD_MESSAGE("%s penetrates digs through your skull, lays %s eggs and jumps out.",
      CHAR_NAME(DEFINITE), CHAR_POSSESSIVE_PRONOUN);
    }
    else if(Victim->CanBeSeenByPlayer())
    {
      ADD_MESSAGE("%s penetrates digs through %s's skull, lays %s eggs and jumps out.",
      CHAR_NAME(DEFINITE), Victim->CHAR_NAME(DEFINITE), CHAR_POSSESSIVE_PRONOUN);
    }
    MoveRandomly();
  }
}



void mindworm::PsiAttack(character* Victim)
{
  if(Victim->IsPlayer())
  {
    ADD_MESSAGE("Your brain is on fire.");
  }
  else if(Victim->CanBeSeenByPlayer() && PLAYER->GetAttribute(PERCEPTION) > RAND_N(20))
  {
    ADD_MESSAGE("%s looks scared.", Victim->CHAR_NAME(DEFINITE));
  }


  Victim->ReceiveDamage(this, 1 + RAND_N(5), PSI, ALL, 8, false, false, false, false);
  Victim->CheckDeath(CONST_S("killed by ") + GetName(INDEFINITE) + "'s psi attack", this);
}
#endif
