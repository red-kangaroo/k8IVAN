#ifdef HEADER_PHASE
ITEM(vacuumblade, meleeweapon)
{
public:
  virtual truth HitEffect (character *, character *, v2, int, int, truth);
};


#else



truth vacuumblade::HitEffect (character *Enemy, character *Hitter, v2 HitPos, int BodyPartIndex, int Direction, truth BlockedByArmour) {
  truth BaseSuccess = meleeweapon::HitEffect(Enemy, Hitter, HitPos, BodyPartIndex, Direction, BlockedByArmour);
  if (!IsBroken() && Enemy->IsEnabled() && !(RAND() % 2)) {
    if (Hitter) {
      if (Enemy->IsPlayer() || Hitter->IsPlayer() || Enemy->CanBeSeenByPlayer() || Hitter->CanBeSeenByPlayer())
        ADD_MESSAGE("%s blade creates a wind vortex cutting %s.", Hitter->CHAR_POSSESSIVE_PRONOUN, Enemy->CHAR_DESCRIPTION(DEFINITE));
    } else {
      if (Enemy->IsPlayer() || Enemy->CanBeSeenByPlayer())
        ADD_MESSAGE("The blade creates a wind vortex cutting %s.", Enemy->CHAR_DESCRIPTION(DEFINITE));
    }
    return Enemy->ReceiveBodyPartDamage(Hitter, 6 + (RAND() % 6), ENERGY, BodyPartIndex, Direction) || BaseSuccess;
  }
  return BaseSuccess;
}
#endif
