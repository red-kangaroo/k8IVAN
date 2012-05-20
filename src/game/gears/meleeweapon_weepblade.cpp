#ifdef HEADER_PHASE
ITEM(weepblade, meleeweapon)
{
 public:
  virtual truth HitEffect(character*, character*, v2, int, int, truth);
};


#else



truth weepblade::HitEffect(character* Enemy, character* Hitter, v2 HitPos, int BodyPartIndex, int Direction, truth BlockedByArmour)
{
  truth BaseSuccess = meleeweapon::HitEffect(Enemy, Hitter, HitPos, BodyPartIndex, Direction, BlockedByArmour);

  if(Enemy->IsEnabled() && !(RAND_N(3)))
  {
    if(Enemy->IsPlayer() || Hitter->IsPlayer()
       || Enemy->CanBeSeenByPlayer() || Hitter->CanBeSeenByPlayer())
      ADD_MESSAGE("%s weeping blade spills acid on %s.",
      Hitter->CHAR_POSSESSIVE_PRONOUN, Enemy->CHAR_DESCRIPTION(DEFINITE));
    Enemy->SpillFluid(PLAYER, liquid::Spawn(SULPHURIC_ACID, 25+RAND()%25));
    return BaseSuccess;
  }
  else
    return BaseSuccess;
}
#endif
