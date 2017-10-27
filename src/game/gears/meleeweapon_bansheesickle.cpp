#ifdef HEADER_PHASE
ITEM(bansheesickle, meleeweapon)
{
 public:
  virtual truth HitEffect(character*, character*, v2, int, int, truth);
};


#else


truth bansheesickle::HitEffect (character *Enemy, character* Hitter, v2 HitPos, int BodyPartIndex, int Direction, truth BlockedByArmour) {
  truth BaseSuccess = meleeweapon::HitEffect(Enemy, Hitter, HitPos, BodyPartIndex, Direction, BlockedByArmour);
  if (!IsBroken() && Enemy->IsEnabled() && (RAND()&1)) {
    if (Hitter) {
      if (Enemy->IsPlayer() || Hitter->IsPlayer() || Enemy->CanBeSeenByPlayer() || Hitter->CanBeSeenByPlayer()) {
        ADD_MESSAGE("%s sickle shrieks at %s.", Hitter->CHAR_POSSESSIVE_PRONOUN, Enemy->CHAR_DESCRIPTION(DEFINITE));
      }
    } else {
      if (Enemy->IsPlayer() || Enemy->CanBeSeenByPlayer()) {
        ADD_MESSAGE("The sickle shrieks at %s.", Enemy->CHAR_DESCRIPTION(DEFINITE));
      }
    }
    return Enemy->ReceiveBodyPartDamage(Hitter, 4 + (RAND() & 4), SOUND, BodyPartIndex, Direction) || BaseSuccess;
  }
  return BaseSuccess;
}


#endif
