#ifdef HEADER_PHASE
ITEM(slowaxe, meleeweapon)
{
 public:
  virtual truth HitEffect(character*, character*, v2, int, int, truth);
};


#else


truth slowaxe::HitEffect (character *Enemy, character *Hitter, v2 HitPos, int BodyPartIndex, int Direction, truth BlockedByArmour) {
  truth BaseSuccess = meleeweapon::HitEffect(Enemy, Hitter, HitPos, BodyPartIndex, Direction, BlockedByArmour);
  if (!IsBroken() && Enemy->IsEnabled() && !(RAND()%3)) {
    if (Hitter) {
      if (Enemy->IsPlayer() || Hitter->IsPlayer() || Enemy->CanBeSeenByPlayer() || Hitter->CanBeSeenByPlayer()) {
        ADD_MESSAGE("%s axe chills %s.", Hitter->CHAR_POSSESSIVE_PRONOUN, Enemy->CHAR_DESCRIPTION(DEFINITE));
      }
    } else {
      if (Enemy->IsPlayer() || Enemy->CanBeSeenByPlayer()) {
        ADD_MESSAGE("The axe chills %s.", Enemy->CHAR_DESCRIPTION(DEFINITE));
      }
    }
    Enemy->BeginTemporaryState(SLOW, 400+RAND_N(200));
    return BaseSuccess;
  }
  return BaseSuccess;
}


#endif
