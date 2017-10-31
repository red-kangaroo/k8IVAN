#ifdef HEADER_PHASE
CHARACTER(menatrixfusanga, largecreature)
{
public:
  menatrixfusanga() : TurnsExisted(0) {}
  void SetSpecies (int);
  int GetSpecies () const { return Species; }
  virtual void Save (outputfile &) const;
  virtual void Load (inputfile &);
  virtual void FinalProcessForBone ();
  virtual col16 GetSkinColor () const;
  virtual bodypart *MakeBodyPart (int) const;
protected:
  virtual void GetAICommand ();
  virtual void CreateCorpse (lsquare *);
  //virtual truth MustBeRemovedFromBone () const;
  sLong TurnsExisted;
  int Species;
};


#else



bodypart *menatrixfusanga::MakeBodyPart (int) const { return menatrixtorso::Spawn(0, NO_MATERIALS); }
col16 menatrixfusanga::GetSkinColor () const { return MakeRGB16(60+RAND()%190, 60+RAND()%190, 60+RAND()%190); }



void menatrixfusanga::GetAICommand () {
  StandIdleAI();
  ++TurnsExisted;
  SeekLeader(GetLeader());
  if (FollowLeader(GetLeader())) return;
  if (!(RAND()%10)) {
    int NumberOfPlants = RAND()%3+RAND()%3+RAND()%3+RAND()%3;
    for (int c1 = 0; c1 < 50 && NumberOfPlants; ++c1) {
      for (int c2 = 0; c2 < game::GetTeams() && NumberOfPlants; ++c2) {
        if (GetTeam()->GetRelation(game::GetTeam(c2)) == HOSTILE) {
          for (std::list<character*>::const_iterator i = game::GetTeam(c2)->GetMember().begin(); i != game::GetTeam(c2)->GetMember().end() && NumberOfPlants; ++i) {
            if ((*i)->IsEnabled()) {
              lsquare *LSquare = (*i)->GetNeighbourLSquare(RAND()%GetNeighbourSquares());
              if (LSquare && (LSquare->GetWalkability() & WALK) && !LSquare->GetCharacter()) {
                character *NewPlant;
                sLong RandomValue = RAND()%TurnsExisted;
                if (RandomValue < 250) NewPlant = mushroom::Spawn();
                else if (RandomValue < 1500) NewPlant = magicmushroom::Spawn();
                else NewPlant = magicmushroom::Spawn();
                for (int c = 3; c < TurnsExisted / 500; ++c) NewPlant->EditAllAttributes(1);
                NewPlant->SetGenerationDanger(GetGenerationDanger());
                NewPlant->SetTeam(GetTeam());
                NewPlant->PutTo(LSquare->GetPos());
                --NumberOfPlants;
                if (NewPlant->CanBeSeenByPlayer()) {
                  if ((*i)->IsPlayer()) ADD_MESSAGE("%s sprouts from the ground near you.", NewPlant->CHAR_NAME(INDEFINITE));
                  else if ((*i)->CanBeSeenByPlayer()) ADD_MESSAGE("%s sprouts from the ground near %s.", NewPlant->CHAR_NAME(INDEFINITE), (*i)->CHAR_NAME(DEFINITE));
                  else ADD_MESSAGE("%s sprouts from the ground.", NewPlant->CHAR_NAME(INDEFINITE));
                }
              }
            }
          }
        }
      }
      SeekLeader(GetLeader());
      if (FollowLeader(GetLeader())) return;
      lsquare *CradleSquare = GetNeighbourLSquare(RAND()%8);
      if (CradleSquare && !CradleSquare->GetCharacter() && (CradleSquare->GetWalkability() & WALK)) {
        int SpoiledItems = 0;
        int MushroomsNear = 0;
        for (int d = 0; d < 8; ++d) {
          lsquare *Square = CradleSquare->GetNeighbourLSquare(d);
          if (Square) {
            character *Char = Square->GetCharacter();
            if (Char && Char->IsMushroom()) ++MushroomsNear;
            SpoiledItems += Square->GetSpoiledItems();
          }
        }
        if ((SpoiledItems && MushroomsNear < 1 && !RAND_N(2)) || (MushroomsNear < 3 && !RAND_N((1+MushroomsNear)*2))) {
          magicmushroom *Child = magicmushroom::Spawn(GetConfig());
          switch (RAND()%3) {
            case 0: SetSpecies(MakeRGB16(125+RAND()%125, RAND()%100, RAND()%100)); break;
            case 1: SetSpecies(MakeRGB16(RAND()%100, 125+RAND()%125, RAND()%100)); break;
            case 2: SetSpecies(MakeRGB16(RAND()%100, RAND()%100, 125+RAND()%125)); break;
          }
          Child->SetSpecies(Species);
          Child->SetTeam(GetTeam());
          Child->SetGenerationDanger(GetGenerationDanger());
          Child->PutTo(CradleSquare->GetPos());
          if (Child->CanBeSeenByPlayer()) ADD_MESSAGE("%s pops out from the ground.", Child->CHAR_NAME(INDEFINITE));
        }
      }
      if (AttackAdjacentEnemyAI()) return;
      if (MoveRandomly()) return;
      EditAP(-1000);
    }
    EditAP(-2000);
    return;
  }
  if (AttackAdjacentEnemyAI()) return;
  if (MoveRandomly()) return;
  EditAP(-1000);
}



void menatrixfusanga::SetSpecies (int What) {
  Species = What;
  UpdatePictures();
}



void menatrixfusanga::CreateCorpse (lsquare *Square) {
  for (int c = 0; c < 1; ++c) Square->AddItem(wand::Spawn(WAND_OF_CLONING));
  Square->AddItem(wand::Spawn(WAND_OF_MIRRORING));
  Square->AddItem(solstone::Spawn());
  largecreature::CreateCorpse(Square);
}



void menatrixfusanga::Save (outputfile &SaveFile) const {
  nonhumanoid::Save(SaveFile);
  SaveFile << TurnsExisted;
  SaveFile << Species;
}



void menatrixfusanga::Load (inputfile &SaveFile) {
  nonhumanoid::Load(SaveFile);
  SaveFile >> TurnsExisted;
  SaveFile >> Species;
}


/*
truth menatrixfusanga::MustBeRemovedFromBone () const {
  //return !IsEnabled() || GetTeam()->GetID() != MONSTER_TEAM || GetDungeon()->GetIndex() != UNDER_WATER_TUNNEL || GetLevel()->GetIndex() != VESANA_LEVEL;
  return (!IsEnabled() || GetTeam()->GetID() != MONSTER_TEAM || GetLevel()->IsUTVesanaLevel());
}
*/


void menatrixfusanga::FinalProcessForBone () {
  largecreature::FinalProcessForBone();
  TurnsExisted = 0;
}
#endif
