#ifdef HEADER_PHASE
CHARACTER(skunk, nonhumanoid)
{
 public:
  virtual void GetAICommand();
};


#else



void skunk::GetAICommand()
{
  if(!IsRetreating())
  {
    if(!RAND_N(4))
    {
      character* Char = GetRandomNeighbour(HOSTILE);

      if(Char)
      {
  int Amount = 500 / Char->GetSquaresUnder();
  truth Success = false;

  for(int c = 0; c < Char->GetSquaresUnder(); ++c)
    if(Char->GetLSquareUnder(c)->IsFlyable())
    {
      Success = true;
      Char->GetLSquareUnder(c)->AddSmoke(gas::Spawn(SKUNK_SMELL, Amount));
    }

  if(Success)
  {
    if(CanBeSeenByPlayer())
      ADD_MESSAGE("%s stinks.", CHAR_NAME(DEFINITE));

    EditAP(-1000);
    return;
  }
      }
    }
  }
  else if(RAND_N(2))
  {
    if(CanBeSeenByPlayer())
      ADD_MESSAGE("%s stinks.", CHAR_NAME(DEFINITE));

    GetLSquareUnder()->AddSmoke(gas::Spawn(SKUNK_SMELL, 500));
  }

  nonhumanoid::GetAICommand();
}
#endif
