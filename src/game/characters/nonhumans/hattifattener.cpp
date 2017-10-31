#ifdef HEADER_PHASE
CHARACTER(hattifattener, nonhumanoid)
{
 public:
  truth Hit(character*, v2, int, int = 0) { return false; }
 protected:
  virtual int GetBodyPartWobbleData(int) const;
  virtual int GetSpecialBodyPartFlags(int) const;
  virtual void GetAICommand();
  virtual void CreateCorpse(lsquare*);
};


#else



int hattifattener::GetSpecialBodyPartFlags(int) const { return ST_LIGHTNING; }



int hattifattener::GetBodyPartWobbleData(int) const { return WOBBLE_HORIZONTALLY|(1 << WOBBLE_SPEED_SHIFT)|(1 << WOBBLE_FREQ_SHIFT); }



void hattifattener::GetAICommand()
{
  if(!(RAND() % 7))
  {
    if(CanBeSeenByPlayer())
      ADD_MESSAGE("%s emits a lightning bolt!", CHAR_DESCRIPTION(DEFINITE));

    beamdata Beam
      (
  this,
  "killed by a hattifattener's lightning",
  GetPos(),
  WHITE,
  BEAM_LIGHTNING,
  RAND() & 7,
  1 + (RAND() & 7),
  0
      );

    GetLevel()->LightningBeam(Beam);
    EditAP(-1000);
    return;
  }

  SeekLeader(GetLeader());

  if(FollowLeader(GetLeader()))
    return;

  if(MoveRandomly())
    return;

  EditAP(-1000);
}



void hattifattener::CreateCorpse(lsquare* Square)
{
  level* Level = Square->GetLevel();
  feuLong StackSize = Level->AddRadiusToSquareStack(Square->GetPos(), 9);
  lsquare** SquareStack = Level->GetSquareStack();
  feuLong c;

  for(c = 0; c < StackSize; ++c)
    SquareStack[c]->RemoveFlags(IN_SQUARE_STACK);

  fearray<lsquare*> Stack(SquareStack, StackSize);
  Level->LightningVisualizer(Stack, WHITE);

  for(c = 0; c < Stack.Size; ++c)
  {
    beamdata Beam
      (
  this,
  CONST_S("killed by electricity released by a dying hattifattener"),
  YOURSELF,
  0
      );

    Stack[c]->Lightning(Beam);
  }

  SendToHell();
}
#endif
