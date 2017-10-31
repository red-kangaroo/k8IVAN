#ifdef HEADER_PHASE
CHARACTER(siren, humanoid)
{
 public:
  virtual void GetAICommand();
 protected:
  virtual truth TryToSing();
};


#else



void siren::GetAICommand()
{
  if(TryToSing())
    return;

  humanoid::GetAICommand();
}



truth siren::TryToSing()
{
  truth Success=false;
  for(int d = 0; d < GetNeighbourSquares(); ++d)
  {
    lsquare* Square = GetNeighbourLSquare(d);

    if(Square && Square->GetCharacter())
    {
      Success = true;
      Square->GetCharacter()->ReceiveSirenSong(this);
    }
  }
  if(Success)
    EditAP(-2000);

  return Success;
}
#endif
