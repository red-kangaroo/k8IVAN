#ifdef HEADER_PHASE
CHARACTER(encourager, humanoid)
{
 /*public:
  encourager() : LastHit(0) { }
  virtual void Save(outputfile&) const;
  virtual void Load(inputfile&);
  virtual void FinalProcessForBone();
 protected:
  virtual void GetAICommand();
  feuLong LastHit;*/
 protected:
  virtual void GetAICommand() { StandIdleAI(); }
};


#else


/*
void encourager::GetAICommand()
{
  if(CheckForEnemies(true, true, true))
    return;

  if(CheckForUsefulItemsOnGround())
    return;

  if(CheckForDoors())
    return;

  if(game::GetTick() - LastHit > 200)
  {
    static int NotDiagonal[] = { 1, 3, 4, 6 };

    for(int d = 0; d < 4; ++d)
    {
      square* Square = GetNeighbourSquare(NotDiagonal[d]);

      if(Square)
      {
  character* Char = Square->GetCharacter();

  if(Char && Char->IsBananaGrower() && Hit(Char, Square->GetPos(), NotDiagonal[d], true))
  {
    LastHit = game::GetTick();
    TerminateGoingTo();
    return;
  }
      }
    }
  }

  if(MoveTowardsHomePos())
    return;

  EditAP(-1000);
}



void encourager::Save(outputfile& SaveFile) const
{
  humanoid::Save(SaveFile);
  SaveFile << LastHit;
}



void encourager::Load(inputfile& SaveFile)
{
  humanoid::Load(SaveFile);
  SaveFile >> LastHit;
}



void encourager::FinalProcessForBone()
{
  humanoid::FinalProcessForBone();
  LastHit = 0;
}
*/
#endif
