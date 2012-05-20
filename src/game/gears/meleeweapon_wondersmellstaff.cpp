#ifdef HEADER_PHASE
ITEM(wondersmellstaff, meleeweapon)
{
 public:
  virtual truth HitEffect(character*, character*, v2, int, int, truth);
  virtual truth AllowAlphaEverywhere() const { return true; }
  virtual void Break(character*, int);
 protected:
  virtual int GetClassAnimationFrames() const;
  virtual col16 GetOutlineColor(int) const;
  virtual alpha GetOutlineAlpha(int) const;
};


#else



int wondersmellstaff::GetClassAnimationFrames() const { return !IsBroken() ? 128 : 1; }



alpha wondersmellstaff::GetOutlineAlpha(int Frame) const
{
  if(!IsBroken())
  {
    Frame &= 31;
    return Frame * (31 - Frame) >> 1;
  }
  else
    return 255;
}



col16 wondersmellstaff::GetOutlineColor(int Frame) const
{
  if(!IsBroken())
    switch((Frame&127) >> 5)
    {
     case 0: return BLUE;
     case 1: return GREEN;
     case 2: return RED;
     case 3: return YELLOW;
    }

  return TRANSPARENT_COLOR;
}



truth wondersmellstaff::HitEffect(character* Enemy, character* Hitter, v2 HitPos, int BodyPartIndex, int Direction, truth BlockedByArmour)
{
  truth BaseSuccess = meleeweapon::HitEffect(Enemy, Hitter, HitPos, BodyPartIndex, Direction, BlockedByArmour);

  if(!IsBroken() && Enemy->IsEnabled() && !(RAND() % 5))
  {
    if(RAND() & 3)
    {
      truth Seen = false;
      int Amount = 250 / Enemy->GetSquaresUnder();

      for(int c = 0; c < Enemy->GetSquaresUnder(); ++c)
      {
  lsquare* Square = Enemy->GetLSquareUnder(c);

  if(Square->IsFlyable())
  {
    Square->AddSmoke(gas::Spawn(EVIL_WONDER_STAFF_VAPOUR, Amount));

    if(!Seen && Square->CanBeSeenByPlayer())
      Seen = true;
  }
      }

      if(Seen)
  ADD_MESSAGE("Strange red smoke billows out of %s staff.", Hitter->CHAR_POSSESSIVE_PRONOUN);
    }
    else
    {
      /* Can a multitiled creature ever be the hitter? */

      lsquare* Square = Hitter->GetLSquareUnder();

      if(Square->IsFlyable())
      {
  if(Square->CanBeSeenByPlayer())
    ADD_MESSAGE("Strange blue smoke billows out of %s staff.", Hitter->CHAR_POSSESSIVE_PRONOUN);

  Square->AddSmoke(gas::Spawn(GOOD_WONDER_STAFF_VAPOUR, 100));
      }
    }

    return true;
  }
  else
    return BaseSuccess;
}



void wondersmellstaff::Break(character* Who, int Much)
{
    /*k8: material* GasMaterial = GetSecondaryMaterial(); */
    GetLevel()->GasExplosion(gas::Spawn(GOOD_WONDER_STAFF_VAPOUR, 100), GetLSquareUnder());

    if(CanBeSeenByPlayer())
    {
      ADD_MESSAGE("%s unleashes a puff of a wonderous gas.", CHAR_NAME(DEFINITE));

    }
    meleeweapon::Break(Who,Much);
}
#endif
