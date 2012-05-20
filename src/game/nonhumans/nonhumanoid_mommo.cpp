#ifdef HEADER_PHASE
CHARACTER(mommo, nonhumanoid)
{
 protected:
  virtual int GetBodyPartWobbleData(int) const;
  virtual truth CanVomit() const { return true; }
  virtual void CreateCorpse(lsquare*);
  virtual truth Hit(character*, v2, int, int = 0);
  virtual void GetAICommand();
};


#else



int mommo::GetBodyPartWobbleData(int) const { return (GetConfig() == CONICAL ? WOBBLE_HORIZONTALLY : WOBBLE_VERTICALLY)|(2 << WOBBLE_FREQ_SHIFT); }



void mommo::CreateCorpse(lsquare* Square)
{
  for(int d = 0; d < GetExtendedNeighbourSquares(); ++d)
  {
    lsquare* NeighbourSquare = Square->GetNeighbourLSquare(d);

    if(NeighbourSquare)
      NeighbourSquare->SpillFluid(0, static_cast<liquid*>(GetTorso()->GetMainMaterial()->SpawnMore(250 + RAND() % 250)));
  }

  SendToHell();
}



truth mommo::Hit(character* Enemy, v2 Pos, int, int)
{
  if(CheckIfTooScaredToHit(Enemy))
    return false;

  if(IsPlayer())
  {
    if(!(Enemy->IsMasochist() && GetRelation(Enemy) == FRIEND) && GetRelation(Enemy) != HOSTILE && !game::TruthQuestion(CONST_S("This might cause a hostile reaction. Are you sure? [y/N]")))
      return false;
  }
  else if(GetAttribute(WISDOM) >= Enemy->GetAttackWisdomLimit())
    return false;

  Hostility(Enemy);

  if(IsPlayer())
    ADD_MESSAGE("You spill acidous slime at %s.", Enemy->CHAR_DESCRIPTION(DEFINITE));
  else if(Enemy->IsPlayer() || CanBeSeenByPlayer() || Enemy->CanBeSeenByPlayer())
    ADD_MESSAGE("%s spills acidous slime at %s.", CHAR_DESCRIPTION(DEFINITE), Enemy->CHAR_DESCRIPTION(DEFINITE));

  Vomit(Pos, 250 + RAND() % 250, false);
  EditAP(-1000);
  return true;
}



void mommo::GetAICommand()
{
  SeekLeader(GetLeader());

  if(CheckForEnemies(false, false, true))
    return;

  if(!(RAND() % 10))
  {
    VomitAtRandomDirection(350 + RAND() % 350);
    EditAP(-1000);
    return;
  }

  if(FollowLeader(GetLeader()))
    return;

  if(MoveRandomly())
    return;

  EditAP(-1000);
}
#endif
