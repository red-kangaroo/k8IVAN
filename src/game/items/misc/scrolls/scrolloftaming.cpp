#ifdef HEADER_PHASE
ITEM(scrolloftaming, scroll)
{
 public:
  virtual void FinishReading(character*);
};


#else



void scrolloftaming::FinishReading(character* Reader)
{
  // First find all tameable characters in the squares around Reader

  character* CharacterNear[8];
  int Index = 0;

  for(int c = 0; c < 8; ++c)
  {
    v2 Test = Reader->GetPos() + game::GetMoveVector(c);

    if(GetArea()->IsValidPos(Test))
    {
      character* Char = GetNearSquare(Test)->GetCharacter();

      if(Char && Char->CanTameWithScroll(Reader) && Char->GetTeam() != Reader->GetTeam())
  CharacterNear[Index++] = Char;
    }
  }

  // Then pick one of the characters and set it to the same team as Reader

  if(!Index)
  {
    if(Reader->IsPlayer() || Reader->CanBeSeenByPlayer())
      ADD_MESSAGE("The scroll burns, but nothing happens.");
  }
  else
  {
    character* ToBeTamed = CharacterNear[RAND() % Index];
    ToBeTamed->ChangeTeam(Reader->GetTeam());

    if(Reader->IsPlayer())
    {
      if(ToBeTamed->CanBeSeenByPlayer())
  ADD_MESSAGE("%s looks much friendlier.", ToBeTamed->CHAR_NAME(INDEFINITE));
      else
  ADD_MESSAGE("You notice no effect.");
    }
    else if(Reader->CanBeSeenByPlayer())
    {
      if(ToBeTamed->CanBeSeenByPlayer())
  ADD_MESSAGE("%s seems to like %s far more.", ToBeTamed->CHAR_NAME(INDEFINITE), Reader->CHAR_NAME(DEFINITE));
    }
    else if(ToBeTamed->CanBeSeenByPlayer())
      ADD_MESSAGE("Suddenly %s looks like having fallen in love with something nearby.", ToBeTamed->CHAR_NAME(INDEFINITE));
  }

  RemoveFromSlot();
  SendToHell();
  Reader->EditExperience(INTELLIGENCE, 300, 1 << 12);
}
#endif
