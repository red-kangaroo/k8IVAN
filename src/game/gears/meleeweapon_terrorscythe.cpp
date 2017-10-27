#ifdef HEADER_PHASE
ITEM(terrorscythe, meleeweapon)
{
 public:
  virtual truth HitEffect(character*, character*, v2, int, int, truth);
};


#else


truth terrorscythe::HitEffect (character *Enemy, character* Hitter, v2 HitPos, int BodyPartIndex, int Direction, truth BlockedByArmour) {
  truth BaseSuccess = meleeweapon::HitEffect(Enemy, Hitter, HitPos, BodyPartIndex, Direction, BlockedByArmour);
  if (!IsBroken() && Enemy->IsEnabled() && !(RAND() % 3)) {
    if (Hitter) {
      if (Enemy->IsPlayer() || Hitter->IsPlayer() || Enemy->CanBeSeenByPlayer() || Hitter->CanBeSeenByPlayer()) {
        ADD_MESSAGE("%s scythe terrifies %s.", Hitter->CHAR_POSSESSIVE_PRONOUN, Enemy->CHAR_DESCRIPTION(DEFINITE));
      }
    } else {
      if (Enemy->IsPlayer() || Enemy->CanBeSeenByPlayer()) {
        ADD_MESSAGE("The scythe terrifies %s.", Enemy->CHAR_DESCRIPTION(DEFINITE));
      }
    }
    Enemy->BeginTemporaryState(PANIC, 200 + RAND_N(100));
    return BaseSuccess;
  }
  return BaseSuccess;
}


#endif
