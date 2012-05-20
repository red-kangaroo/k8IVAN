#ifdef HEADER_PHASE
CHARACTER(magicmushroom, mushroom)
{
 protected:
  virtual bodypart* MakeBodyPart(int) const;
  virtual void GetAICommand();
};


#else



bodypart* magicmushroom::MakeBodyPart(int) const { return magicmushroomtorso::Spawn(0, NO_MATERIALS); }



void magicmushroom::GetAICommand()
{
  if(!(RAND() % 750))
  {
    if(CanBeSeenByPlayer())
      ADD_MESSAGE("%s disappears.", CHAR_NAME(DEFINITE));

    TeleportRandomly(true);
    EditAP(-1000);
  }
  else if(!(RAND() % 50))
  {
    lsquare* Square = GetNeighbourLSquare(RAND() % 8);

    if(Square && Square->IsFlyable())
    {
      if(CanBeSeenByPlayer())
  ADD_MESSAGE("%s releases odd-looking gas.", CHAR_NAME(DEFINITE));

      Square->AddSmoke(gas::Spawn(MAGIC_VAPOUR, 1000));
      EditAP(-1000);
    }
  }
  else
    mushroom::GetAICommand();
}
#endif
