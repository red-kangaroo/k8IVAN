#ifdef HEADER_PHASE
CHARACTER(largecreature, nonhumanoid)
{
 public:
  virtual void CalculateSquaresUnder() { SquaresUnder = 4; }
  virtual int GetSquareIndex(v2) const;
  virtual int GetNeighbourSquares() const { return 12; }
  virtual int GetExtendedNeighbourSquares() const { return 16; }
  virtual square* GetNeighbourSquare(int) const;
  virtual lsquare* GetNeighbourLSquare(int) const;
  virtual wsquare* GetNeighbourWSquare(int) const;
  virtual int CalculateNewSquaresUnder(lsquare**, v2) const;
  virtual truth IsFreeForMe(square*) const;
  virtual truth CanMoveOn(const lsquare*) const;
  virtual truth CanMoveOn(const square*) const;
  virtual void PutTo(v2);
  virtual void Remove();
  virtual truth CreateRoute();
  virtual truth CanTheoreticallyMoveOn(const lsquare*) const;
  virtual int GetFlySymbolSquareIndex() const { return 1; }
  virtual int GetSwimmingSymbolSquareIndex() const { return 3; }
  virtual int GetUnconsciousSymbolSquareIndex() const { return 2; }
  virtual truth PlaceIsIllegal(v2, v2) const;
  truth PartCanMoveOn(const lsquare*) const;
 protected:
  virtual bodypart* MakeBodyPart(int) const;
  virtual void CreateCorpse(lsquare*);
  virtual void LoadSquaresUnder();
};


#else



bodypart* largecreature::MakeBodyPart(int) const { return largetorso::Spawn(0, NO_MATERIALS); }



lsquare* largecreature::GetNeighbourLSquare(int I) const { return static_cast<lsquare*>(GetNeighbourSquare(I)); }



wsquare* largecreature::GetNeighbourWSquare(int I) const { return static_cast<wsquare*>(GetNeighbourSquare(I)); }



int largecreature::GetSquareIndex(v2 Pos) const
{
  v2 RelativePos = Pos - GetPos();
  return RelativePos.X + (RelativePos.Y << 1);
}



square* largecreature::GetNeighbourSquare(int I) const
{
  square* SquareUnder = GetSquareUnder();
  area* Area = SquareUnder->GetArea();
  v2 Pos = SquareUnder->GetPos() + game::GetLargeMoveVector(I);
  return Area->IsValidPos(Pos) ? SquareUnder->GetArea()->GetSquare(Pos) : 0;
}



int largecreature::CalculateNewSquaresUnder(lsquare** NewSquare, v2 Pos) const
{
  level* Level = GetLevel();

  for(int c = 0; c < 4; ++c)
  {
    v2 SquarePos = Pos + game::GetLargeMoveVector(12 + c);

    if(Level->IsValidPos(SquarePos))
      NewSquare[c] = Level->GetLSquare(SquarePos);
    else
      return 0;
  }

  return 4;
}



truth largecreature::IsFreeForMe(square* Square) const
{
  v2 Pos = Square->GetPos();
  area* Area = Square->GetArea();

  for(int c = 0; c < 4; ++c)
  {
    v2 SquarePos = Pos + game::GetLargeMoveVector(12 + c);

    if(!Area->IsValidPos(SquarePos) || (Area->GetSquare(SquarePos)->GetCharacter() && Area->GetSquare(SquarePos)->GetCharacter() != static_cast<ccharacter*>(this)))
      return false;
  }

  return true;
}



truth largecreature::CanTheoreticallyMoveOn(const lsquare* LSquare) const
{
  v2 Pos = LSquare->GetPos();
  level* Level = LSquare->GetLevel();

  for(int c = 0; c < 4; ++c)
  {
    v2 SquarePos = Pos + game::GetLargeMoveVector(12 + c);

    if(!Level->IsValidPos(SquarePos) || !(GetMoveType() & Level->GetLSquare(SquarePos)->GetTheoreticalWalkability()))
      return false;
  }

  return true;
}



truth largecreature::CanMoveOn(const lsquare* LSquare) const
{
  v2 Pos = LSquare->GetPos();
  level* Level = LSquare->GetLevel();

  for(int c = 0; c < 4; ++c)
  {
    v2 SquarePos = Pos + game::GetLargeMoveVector(12 + c);

    if(!Level->IsValidPos(SquarePos) || !PartCanMoveOn(Level->GetLSquare(SquarePos)))
      return false;
  }

  return true;
}



truth largecreature::CanMoveOn(const square* Square) const
{
  v2 Pos = Square->GetPos();
  area* Area = Square->GetArea();

  for(int c = 0; c < 4; ++c)
  {
    v2 SquarePos = Pos + game::GetLargeMoveVector(12 + c);
    if(!Area->IsValidPos(SquarePos) || !(GetMoveType() & Area->GetSquare(SquarePos)->GetSquareWalkability()))
      return false;
  }

  return true;
}



void largecreature::PutTo(v2 Pos)
{
  for(int c = 0; c < 4; ++c)
  {
    SquareUnder[c] = game::GetCurrentArea()->GetSquare(Pos + game::GetLargeMoveVector(12 + c));
    SquareUnder[c]->AddCharacter(this);
  }
}



void largecreature::Remove()
{
  for(int c = 0; c < 4; ++c)
  {
    SquareUnder[c]->RemoveCharacter();
    SquareUnder[c] = 0;
  }
}



void largecreature::CreateCorpse(lsquare* Square)
{
  if(!BodyPartsDisappearWhenSevered() && !game::AllBodyPartsVanish())
  {
    corpse* Corpse = largecorpse::Spawn(0, NO_MATERIALS);
    Corpse->SetDeceased(this);
    Square->AddItem(Corpse);
    Disable();
  }
  else
    SendToHell();
}



void largecreature::LoadSquaresUnder()
{
  for(int c = 0; c < 4; ++c)
    SquareUnder[c] = game::GetSquareInLoad()->GetArea()->GetSquare(game::GetSquareInLoad()->GetPos() + game::GetLargeMoveVector(12 + c));
}



truth largecreature::CreateRoute()
{
  Route.clear();

  if(GetAttribute(INTELLIGENCE) >= 10 && !StateIsActivated(CONFUSED))
  {
    node* Node = GetLevel()->FindRoute(GetPos(), GoingTo, Illegal, 0, this);

    if(Node)
      while(Node->Last)
      {
  Route.push_back(Node->Pos);
  Node = Node->Last;
      }
    else
      TerminateGoingTo();

    IntelligenceAction(5);
    return true;
  }
  else
    return false;
}



truth largecreature::PlaceIsIllegal(v2 Pos, v2 Illegal) const
{
  for(int c = 0; c < 4; ++c)
    if(Pos + game::GetLargeMoveVector(12 + c) == Illegal)
      return true;

  return false;
}



truth largecreature::PartCanMoveOn(const lsquare* LSquare) const
{
  int Walkability = LSquare->GetWalkability();

  if(GetMoveType() & Walkability)
    return true;

  if(DestroysWalls() && Walkability & ETHEREAL)
  {
    olterrain* Terrain = LSquare->GetOLTerrain();

    if(Terrain && Terrain->WillBeDestroyedBy(this))
    {
      room* Room = LSquare->GetRoom();

      if(!Room || Room->IsOKToDestroyWalls(this))
  return true;
    }
  }

  return false;
}
#endif
