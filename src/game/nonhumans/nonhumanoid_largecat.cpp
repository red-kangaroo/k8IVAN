#ifdef HEADER_PHASE
CHARACTER(largecat, nonhumanoid)
{
 public:
  largecat() : Lives(7) { }
  virtual void Save(outputfile&) const;
  virtual void Load(inputfile&);
  virtual truth SpecialSaveLife();
 protected:
  int Lives;
};


#else



void largecat::Save(outputfile& SaveFile) const
{
  nonhumanoid::Save(SaveFile);
  SaveFile << Lives;
}



void largecat::Load(inputfile& SaveFile)
{
  nonhumanoid::Load(SaveFile);
  SaveFile >> Lives;
}



truth largecat::SpecialSaveLife()
{
  if(--Lives <= 0 || game::IsInWilderness())
    return false;

  if(IsPlayer())
    ADD_MESSAGE("But wait! You seem to have miraculously avoided certain death!");
  else if(CanBeSeenByPlayer())
    ADD_MESSAGE("But wait, %s seems to have miraculously avoided certain death!", GetPersonalPronoun().CStr());

  v2 Pos = GetPos();
  rect Border(Pos + v2(-20, -20), Pos + v2(20, 20));
  Pos = GetLevel()->GetRandomSquare(this, 0, &Border);

  if(Pos == ERROR_V2)
    Pos = GetLevel()->GetRandomSquare(this);

  Move(Pos, true);

  if(!IsPlayer() && CanBeSeenByPlayer())
    ADD_MESSAGE("%s appears!", CHAR_NAME(INDEFINITE));

  if(IsPlayer())
    game::AskForEscPress(CONST_S("Life saved!"));

  RestoreBodyParts();
  ResetSpoiling();
  RestoreHP();
  RestoreStamina();
  ResetStates();

  if(GetNP() < SATIATED_LEVEL)
    SetNP(SATIATED_LEVEL);

  SendNewDrawRequest();

  if(GetAction())
    GetAction()->Terminate(false);

  return true;
}
#endif
