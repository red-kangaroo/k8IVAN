#ifdef HEADER_PHASE
CHARACTER(eddy, nonhumanoid)
{
 public:
  virtual truth Hit(character*, v2, int, int = 0);
 protected:
  virtual int GetBodyPartWobbleData(int) const;
  virtual bodypart* MakeBodyPart(int) const;
  virtual void GetAICommand();
};


#else



bodypart* eddy::MakeBodyPart(int) const { return eddytorso::Spawn(0, NO_MATERIALS); }



int eddy::GetBodyPartWobbleData(int) const { return WOBBLE_VERTICALLY|(2 << WOBBLE_FREQ_SHIFT); }



truth eddy::Hit(character* Enemy, v2, int, int)
{
  if(IsPlayer())
  {
    if(!(Enemy->IsMasochist() && GetRelation(Enemy) == FRIEND) && GetRelation(Enemy) != HOSTILE && !game::TruthQuestion(CONST_S("This might cause a hostile reaction. Are you sure?")))
      return false;
  }

  Hostility(Enemy);

  if(RAND() & 1)
  {
    if(IsPlayer())
      ADD_MESSAGE("You engulf %s.", Enemy->CHAR_DESCRIPTION(DEFINITE));
    else if(Enemy->IsPlayer() || CanBeSeenByPlayer() || Enemy->CanBeSeenByPlayer())
      ADD_MESSAGE("%s engulfs %s.", CHAR_DESCRIPTION(DEFINITE), Enemy->CHAR_DESCRIPTION(DEFINITE));

    Enemy->TeleportRandomly();
  }
  else if(IsPlayer())
    ADD_MESSAGE("You miss %s.", Enemy->CHAR_DESCRIPTION(DEFINITE));

  EditAP(-500);
  return true;
}



void eddy::GetAICommand()
{
  if(!GetLSquareUnder()->GetOLTerrain() && !(RAND() % 500))
  {
    decoration* Couch = decoration::Spawn(RAND_N(5) ? COUCH : DOUBLE_BED);

    if(CanBeSeenByPlayer())
      ADD_MESSAGE("%s spits out %s.", CHAR_NAME(DEFINITE), Couch->CHAR_NAME(INDEFINITE));

    GetLSquareUnder()->ChangeOLTerrainAndUpdateLights(Couch);
    EditAP(-1000);
    return;
  }

  if(GetStackUnder()->GetItems() && !(RAND() % 10))
  {
    if(CanBeSeenByPlayer())
      ADD_MESSAGE("%s engulfs something under it.", CHAR_NAME(DEFINITE));

    GetStackUnder()->TeleportRandomly(3);
    EditAP(-1000);
    return;
  }

  if(!(RAND() % 100))
  {
    if(CanBeSeenByPlayer())
      ADD_MESSAGE("%s engulfs itself.", CHAR_NAME(DEFINITE));

    TeleportRandomly(true);
    EditAP(-1000);
    return;
  }

  nonhumanoid::GetAICommand();
}
#endif
