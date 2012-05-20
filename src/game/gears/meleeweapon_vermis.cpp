#ifdef HEADER_PHASE
ITEM(vermis, meleeweapon)
{
 public:
  virtual truth HitEffect(character*, character*, v2, int, int, truth);
};


#else



truth vermis::HitEffect(character* Enemy, character* Hitter, v2 HitPos, int BodyPartIndex, int Direction, truth BlockedByArmour)
{
  truth BaseSuccess = meleeweapon::HitEffect(Enemy, Hitter, HitPos, BodyPartIndex, Direction, BlockedByArmour);

  if(!IsBroken() && Enemy->IsEnabled() && !(RAND() % 5))
  {
    if(Hitter)
    {
      if(Enemy->IsPlayer() || Enemy->CanBeSeenByPlayer())
  ADD_MESSAGE("%s Vermis sends %s on a sudden journey.", Hitter->CHAR_POSSESSIVE_PRONOUN, Enemy->CHAR_DESCRIPTION(DEFINITE));
    }
    else
    {
      if(Enemy->IsPlayer() || Enemy->CanBeSeenByPlayer())
  ADD_MESSAGE("Vermis sends %s on a sudden journey.", Enemy->CHAR_DESCRIPTION(DEFINITE));
    }

    Enemy->TeleportRandomly();
    return true;
  }
  else
    return BaseSuccess;
}
#endif
