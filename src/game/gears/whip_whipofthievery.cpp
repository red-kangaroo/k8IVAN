#ifdef HEADER_PHASE
ITEM(whipofthievery, whip)
{
 public:
  virtual sLong GetPrice() const;
  virtual truth HitEffect(character*, character*, v2, int, int, truth);
 protected:
  virtual truth CleptiaHelps(ccharacter*, ccharacter*) const;
};


#else



truth whipofthievery::HitEffect(character* Enemy, character* Hitter, v2 HitPos, int BodyPartIndex, int Direction, truth BlockedByArmour)
{
  truth BaseSuccess = meleeweapon::HitEffect(Enemy, Hitter, HitPos, BodyPartIndex, Direction, BlockedByArmour);

  if (!IsBroken() && Enemy->IsEnabled() && Hitter && CleptiaHelps(Enemy, Hitter))
  {
    if(Hitter->IsPlayer())
    {
      game::DoEvilDeed(10);
      game::GetGod(CLEPTIA)->AdjustRelation(10);
    }

    if(Enemy->IsPlayer() || Hitter->IsPlayer() || Enemy->CanBeSeenByPlayer() || Hitter->CanBeSeenByPlayer())
      ADD_MESSAGE("%s whip asks for the help of Cleptia as it steals %s %s.", Hitter->CHAR_POSSESSIVE_PRONOUN, Enemy->CHAR_POSSESSIVE_PRONOUN, Enemy->GetMainWielded()->CHAR_NAME(UNARTICLED));

    Enemy->GetMainWielded()->MoveTo(GetLSquareUnder()->GetStack());
    return true;
  }
  else
    return BaseSuccess;
}



truth whipofthievery::CleptiaHelps(ccharacter* Enemy, ccharacter* Hitter) const
{
  if(Enemy->IsImmuneToWhipOfThievery() || !Enemy->GetMainWielded() || GetMainMaterial()->GetFlexibility() <= 5)
    return false;

  if(Hitter->IsPlayer())
  {
    if(game::GetGod(CLEPTIA)->GetRelation() < 0)
      return false;
    else
      return !RAND_N(10 - game::GetGod(CLEPTIA)->GetRelation() / 200);
  }
  else
    return !RAND_N(10);
}



sLong whipofthievery::GetPrice() const
{
  /* If intact but not flexible enough to work, special thievery bonus must be removed */

  return GetMainMaterial()->GetFlexibility() > 5 || IsBroken() ? whip::GetPrice() : whip::GetPrice() - item::GetPrice();
}
#endif
