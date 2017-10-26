#ifdef HEADER_PHASE
CHARACTER(genetrixvesana, largecreature)
{
 public:
  genetrixvesana() : TurnsExisted(0) { }
  virtual void Save(outputfile&) const;
  virtual void Load(inputfile&);
  virtual void FinalProcessForBone();
 protected:
  virtual void GetAICommand();
  virtual void CreateCorpse(lsquare*);
  //virtual truth MustBeRemovedFromBone() const;
  sLong TurnsExisted;
};


#else



void genetrixvesana::GetAICommand()
{
  ++TurnsExisted;

  SeekLeader(GetLeader());

  if(FollowLeader(GetLeader()))
    return;

  if(!(RAND() % 60))
  {
    int NumberOfPlants = RAND() % 3 + RAND() % 3 + RAND() % 3 + RAND() % 3;

    for(int c1 = 0; c1 < 50 && NumberOfPlants; ++c1)
    {
      for(int c2 = 0; c2 < game::GetTeams() && NumberOfPlants; ++c2)
  if(GetTeam()->GetRelation(game::GetTeam(c2)) == HOSTILE)
    for(std::list<character*>::const_iterator i = game::GetTeam(c2)->GetMember().begin(); i != game::GetTeam(c2)->GetMember().end() && NumberOfPlants; ++i)
      if((*i)->IsEnabled())
      {
        lsquare* LSquare = (*i)->GetNeighbourLSquare(RAND() % GetNeighbourSquares());

        if(LSquare && (LSquare->GetWalkability() & WALK) && !LSquare->GetCharacter())
        {
    character* NewPlant;
    sLong RandomValue = RAND() % TurnsExisted;

    if(RandomValue < 250)
      NewPlant = carnivorousplant::Spawn();
    else if(RandomValue < 1500)
      NewPlant = carnivorousplant::Spawn(GREATER);
    else
      NewPlant = carnivorousplant::Spawn(GIANTIC);

    for(int c = 3; c < TurnsExisted / 500; ++c)
      NewPlant->EditAllAttributes(1);

    NewPlant->SetGenerationDanger(GetGenerationDanger());
    NewPlant->SetTeam(GetTeam());
    NewPlant->PutTo(LSquare->GetPos());
    --NumberOfPlants;

    if(NewPlant->CanBeSeenByPlayer())
    {
      if((*i)->IsPlayer())
        ADD_MESSAGE("%s sprouts from the ground near you.", NewPlant->CHAR_NAME(INDEFINITE));
      else if((*i)->CanBeSeenByPlayer())
        ADD_MESSAGE("%s sprouts from the ground near %s.", NewPlant->CHAR_NAME(INDEFINITE), (*i)->CHAR_NAME(DEFINITE));
      else
        ADD_MESSAGE("%s sprouts from the ground.", NewPlant->CHAR_NAME(INDEFINITE));
    }
        }
      }
    }

    EditAP(-2000);
    return;
  }

  if(AttackAdjacentEnemyAI())
    return;

  if(MoveRandomly())
    return;

  EditAP(-1000);
}



void genetrixvesana::CreateCorpse(lsquare* Square)
{
  for(int c = 0; c < 3; ++c)
    Square->AddItem(pineapple::Spawn());

  largecreature::CreateCorpse(Square);
}


/*
truth genetrixvesana::MustBeRemovedFromBone() const
{
  //return !IsEnabled() || GetTeam()->GetID() != MONSTER_TEAM || GetDungeon()->GetIndex() != UNDER_WATER_TUNNEL || GetLevel()->GetIndex() != VESANA_LEVEL;
  return (!IsEnabled() || GetTeam()->GetID() != MONSTER_TEAM || GetLevel()->IsUTVesanaLevel());
}
*/


void genetrixvesana::Save(outputfile& SaveFile) const
{
  nonhumanoid::Save(SaveFile);
  SaveFile << TurnsExisted;
}



void genetrixvesana::Load(inputfile& SaveFile)
{
  nonhumanoid::Load(SaveFile);
  SaveFile >> TurnsExisted;
}



void genetrixvesana::FinalProcessForBone()
{
  largecreature::FinalProcessForBone();
  TurnsExisted = 0;
}
#endif
