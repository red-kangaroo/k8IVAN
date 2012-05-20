#ifdef HEADER_PHASE
ITEM(turox, meleeweapon)
{
 public:
  virtual truth HitEffect(character*, character*, v2, int, int, truth);
};


#else



truth turox::HitEffect(character* Enemy, character* Hitter, v2 HitPos, int BodyPartIndex, int Direction, truth BlockedByArmour)
{
  truth BaseSuccess = meleeweapon::HitEffect(Enemy, Hitter, HitPos, BodyPartIndex, Direction, BlockedByArmour);

  if(!IsBroken() && Enemy->IsEnabled() && !(RAND() % 5))
  {
    if(Hitter)
    {
      if(Enemy->IsPlayer() || Hitter->IsPlayer() || Enemy->CanBeSeenByPlayer() || Hitter->CanBeSeenByPlayer())
  ADD_MESSAGE("%s smash%s %s with the full force of Turox.", Hitter->CHAR_PERSONAL_PRONOUN, Hitter->IsPlayer() ? "" : "es", Enemy->CHAR_DESCRIPTION(DEFINITE));
    }
    else
    {
      if(Enemy->IsPlayer() || Enemy->CanBeSeenByPlayer())
  ADD_MESSAGE("Turox is smashed against %s with full force.", Enemy->CHAR_DESCRIPTION(DEFINITE));
    }

    if(GetSquareUnder()->CanBeSeenByPlayer(true))
      ADD_MESSAGE("A magical explosion is triggered!");

    Enemy->GetLevel()->Explosion(Hitter, CONST_S("burned @bkp Turox's explosion"), HitPos, 10 + RAND() % 100);
    return true;
  }
  else
    return BaseSuccess;
}
#endif
