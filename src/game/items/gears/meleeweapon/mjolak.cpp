#ifdef HEADER_PHASE
ITEM(mjolak, meleeweapon)
{
 public:
  virtual truth HitEffect(character*, character*, v2, int, int, truth);
};


#else



truth mjolak::HitEffect(character* Enemy, character* Hitter, v2 HitPos, int BodyPartIndex, int Direction, truth BlockedByArmour)
{
  truth BaseSuccess = meleeweapon::HitEffect(Enemy, Hitter, HitPos, BodyPartIndex, Direction, BlockedByArmour);

  if(!IsBroken() && Enemy->IsEnabled() && !(RAND() % 3))
  {
    if(Hitter)
    {
      if(Hitter->IsPlayer())
  game::DoEvilDeed(10);

      if(Enemy->IsPlayer() || Hitter->IsPlayer() || Enemy->CanBeSeenByPlayer() || Hitter->CanBeSeenByPlayer())
  ADD_MESSAGE("A burst of %s Mjolak's unholy energy fries %s.", Hitter->CHAR_POSSESSIVE_PRONOUN, Enemy->CHAR_DESCRIPTION(DEFINITE));
    }
    else
    {
      if(Enemy->IsPlayer() || Enemy->CanBeSeenByPlayer())
  ADD_MESSAGE("A burst of Mjolak's unholy energy fries %s.", Enemy->CHAR_DESCRIPTION(DEFINITE));
    }

    return Enemy->ReceiveBodyPartDamage(Hitter, 5 + (RAND() % 6), ENERGY, BodyPartIndex, Direction) || BaseSuccess;
  }
  else
    return BaseSuccess;
}
#endif
