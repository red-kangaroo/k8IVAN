#ifdef HEADER_PHASE
CHARACTER(twoheadedmoose, nonhumanoid)
{
 public:
  virtual truth Hit(character*, v2, int, int = 0);
};


#else



truth twoheadedmoose::Hit(character* Enemy, v2 HitPos, int Direction, int Flags)
{
  if(CheckIfTooScaredToHit(Enemy))
    return false;

  if(IsPlayer())
  {
    if(!(Enemy->IsMasochist() && GetRelation(Enemy) == FRIEND) && GetRelation(Enemy) != HOSTILE && !game::TruthQuestion(CONST_S("This might cause a hostile reaction. Are you sure? [y/N]")))
      return false;
  }
  else if(GetAttribute(WISDOM) >= Enemy->GetAttackWisdomLimit())
    return false;

  if(GetBurdenState() == OVER_LOADED)
  {
    if(IsPlayer())
      ADD_MESSAGE("You cannot fight while carrying so much.");

    return false;
  }

  Hostility(Enemy);
  msgsystem::EnterBigMessageMode();
  Bite(Enemy, HitPos, Direction, Flags & SADIST_HIT);
  v2 Pos[MAX_NEIGHBOUR_SQUARES];
  character* Char[MAX_NEIGHBOUR_SQUARES];
  int Index = 0;

  for(int d = 0; d < GetNeighbourSquares(); ++d)
  {
    lsquare* LSquare = GetNeighbourLSquare(d);

    if(LSquare)
    {
      character* Enemy = LSquare->GetCharacter();

      if(Enemy && GetRelation(Enemy) == HOSTILE && GetAttribute(WISDOM) < Enemy->GetAttackWisdomLimit())
      {
  Pos[Index] = LSquare->GetPos();
  Char[Index++] = Enemy;
      }
    }
  }

  if(Index)
  {
    int ChosenIndex = RAND() % Index;
    Bite(Char[ChosenIndex], Pos[ChosenIndex], game::GetDirectionForVector(Pos[ChosenIndex] - GetPos()), Flags & SADIST_HIT);
  }

  msgsystem::LeaveBigMessageMode();
  return true;
}
#endif
