#ifdef HEADER_PHASE
CHARACTER(mushroom, nonhumanoid)
{
 public:
  virtual void Save(outputfile&) const;
  virtual void Load(inputfile&);
  void SetSpecies(int);
  int GetSpecies() const { return Species; }
  virtual truth IsMushroom() const { return true; }
 protected:
  virtual void PostConstruct();
  virtual void GetAICommand();
  virtual col16 GetTorsoMainColor() const { return Species; }
  int Species;
};


#else



void mushroom::Save(outputfile& SaveFile) const
{
  nonhumanoid::Save(SaveFile);
  SaveFile << Species;
}



void mushroom::Load(inputfile& SaveFile)
{
  nonhumanoid::Load(SaveFile);
  SaveFile >> Species;
}



void mushroom::GetAICommand()
{
  SeekLeader(GetLeader());

  if(FollowLeader(GetLeader()))
    return;

  lsquare* CradleSquare = GetNeighbourLSquare(RAND() % 8);

  if(CradleSquare && !CradleSquare->GetCharacter()
     && (CradleSquare->GetWalkability() & WALK))
  {
    int SpoiledItems = 0;
    int MushroomsNear = 0;

    for(int d = 0; d < 8; ++d)
    {
      lsquare* Square = CradleSquare->GetNeighbourLSquare(d);

      if(Square)
      {
  character* Char = Square->GetCharacter();

  if(Char && Char->IsMushroom())
    ++MushroomsNear;

  SpoiledItems += Square->GetSpoiledItems();
      }
    }

    if((SpoiledItems && MushroomsNear < 5 && !RAND_N(50)) || (MushroomsNear < 3 && !RAND_N((1 + MushroomsNear) * 100)))
    {
      mushroom* Child = static_cast<mushroom*>(GetProtoType()->Spawn(GetConfig()));
      Child->SetSpecies(Species);
      Child->SetTeam(GetTeam());
      Child->SetGenerationDanger(GetGenerationDanger());
      Child->PutTo(CradleSquare->GetPos());

      for(int c = 0; c < BASE_ATTRIBUTES; ++c)
  Child->BaseExperience[c] = RandomizeBabyExperience(BaseExperience[c] * 4);

      if(Child->CanBeSeenByPlayer())
  ADD_MESSAGE("%s pops out from the ground.", Child->CHAR_NAME(INDEFINITE));
    }
  }

  if(AttackAdjacentEnemyAI())
    return;

  if(MoveRandomly())
    return;

  EditAP(-1000);
}



void mushroom::PostConstruct()
{
  switch(RAND() % 3)
  {
   case 0: SetSpecies(MakeRGB16(125 + RAND() % 125, RAND() % 100, RAND() % 100)); break;
   case 1: SetSpecies(MakeRGB16(RAND() % 100, 125 + RAND() % 125, RAND() % 100)); break;
   case 2: SetSpecies(MakeRGB16(RAND() % 100, RAND() % 100, 125 + RAND() % 125)); break;
  }
}



void mushroom::SetSpecies(int What)
{
  Species = What;
  UpdatePictures();
}
#endif
