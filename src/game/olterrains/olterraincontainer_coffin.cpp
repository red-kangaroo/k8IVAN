#ifdef HEADER_PHASE
OLTERRAIN(coffin, olterraincontainer)
{
 public:
  virtual truth Open(character*);
  virtual void /*coffin::*/Break();
 protected:
  virtual void GenerateGhost(lsquare*);
};


#else


class ghost;


truth coffin::Open (character *Opener) {
  if (!Opener->IsPlayer()) return false;
  if (!game::TruthQuestion(CONST_S("Disturbing the dead might not be wise... Continue? [y/N]"))) return false;
  truth Success = olterraincontainer::Open(Opener);
  if (Success) {
    game::DoEvilDeed(25);
    for (int c = 0; c < RAND_N(10); ++c) {
      v2 Pos = GetLevel()->GetRandomSquare();
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
#endif
