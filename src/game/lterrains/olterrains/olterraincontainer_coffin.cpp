#ifdef HEADER_PHASE
OLTERRAIN(coffin, olterraincontainer)
{
public:
  coffin ();
  virtual ~coffin ();

  virtual truth Open (character *);
  virtual truth CanBeOpened () const;
  virtual stack *GetContained () const;
  virtual void Load (inputfile &);
  virtual void Save (outputfile &) const;
  virtual void SetItemsInside (const fearray<contentscript<item> > &, int);
  virtual void Break ();
  virtual truth AllowContentEmitation () const;
  virtual void PreProcessForBone ();
  virtual void PostProcessForBone ();
  virtual void FinalProcessForBone ();

protected:
  virtual void GenerateGhost (lsquare *);

protected:
  stack *Contained;
};


#else


class ghost;


truth coffin::CanBeOpened () const { return true; }
stack *coffin::GetContained () const { return Contained; }
truth coffin::AllowContentEmitation () const { return false; }


coffin::coffin () {
  Contained = new stack(0, this, HIDDEN);
}


coffin::~coffin () {
  delete Contained;
}


void coffin::Save (outputfile &SaveFile) const {
  olterrain::Save(SaveFile);
  Contained->Save(SaveFile);
}


void coffin::Load (inputfile &SaveFile) {
  olterrain::Load(SaveFile);
  Contained->Load(SaveFile);
}


void coffin::PreProcessForBone () {
  olterrain::PreProcessForBone();
  Contained->PreProcessForBone();
}


void coffin::PostProcessForBone () {
  olterrain::PostProcessForBone();
  Contained->PostProcessForBone();
}


void coffin::FinalProcessForBone () {
  olterrain::FinalProcessForBone();
  Contained->FinalProcessForBone();
}


truth coffin::Open (character *Opener) {
  if (!Opener->IsPlayer()) return false;
  if (!game::TruthQuestion(CONST_S("Disturbing the dead might not be wise... Continue? [y/N]"))) return false;
  truth Success = olterraincontainer::Open(Opener);
  if (Success) {
    game::DoEvilDeed(25);
    for (int c = 0; c < RAND_N(10); ++c) {
      v2 Pos = GetLevel()->GetRandomSquare();
      //
      if (Pos != ERROR_V2) GenerateGhost(GetLevel()->GetLSquare(Pos));
    }
  }
  return Success;
}


void coffin::Break () {
  for (int c = 0; c < 9; ++c) {
    lsquare *Neighbour = GetLSquareUnder()->GetNeighbourLSquare(c);
    if (!RAND_4 && Neighbour && Neighbour->IsFlyable()) GenerateGhost(Neighbour);
  }
  olterraincontainer::Break();
}


void coffin::GenerateGhost (lsquare *Square) {
  if ((!Square->GetRoomIndex() || !Square->GetRoom()->DontGenerateMonsters())) {
    v2 Pos = Square->GetPos();
    character *Char = ghost::Spawn();
    Char->SetTeam(game::GetTeam(MONSTER_TEAM));
    //k8:Char->PutTo(Pos);
    Char->PutToOrNear(Pos);
    Char->SignalGeneration();
    if (Char->CanBeSeenByPlayer()) ADD_MESSAGE("%s appears.", Char->CHAR_NAME(DEFINITE));
  }
}



void coffin::SetItemsInside (const fearray<contentscript<item> > &ItemArray, int SpecialFlags) {
  GetContained()->Clean();
  for (unsigned int c1 = 0; c1 < ItemArray.Size; ++c1) {
    if (ItemArray[c1].IsValid()) {
      const interval *TimesPtr = ItemArray[c1].GetTimes();
      int Times = TimesPtr ? TimesPtr->Randomize() : 1;
      //
      for (int c2 = 0; c2 < Times; ++c2) {
        item *Item = ItemArray[c1].Instantiate(SpecialFlags);
        //
        if (Item) {
          Contained->AddItem(Item);
          Item->SpecialGenerationHandler();
        }
      }
    }
  }
}


#endif
