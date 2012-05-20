#ifdef HEADER_PHASE
ITEM(zulfiqar, meleeweapon)
{
public:
  virtual truth HitEffect (character *, character *, v2, int, int, truth);
};


#else



truth zulfiqar::HitEffect (character *Enemy, character *Hitter, v2 HitPos, int BodyPartIndex, int Direction, truth BlockedByArmour) {
  truth BaseSuccess = meleeweapon::HitEffect(Enemy, Hitter, HitPos, BodyPartIndex, Direction, BlockedByArmour);
  if (!IsBroken() && Enemy->IsEnabled() && !(RAND() % 3)) {
    if (Enemy->IsPlayer() || Hitter->IsPlayer() || Enemy->CanBeSeenByPlayer() || Hitter->CanBeSeenByPlayer())
      ADD_MESSAGE("%s Zulfiqar's slash deeply cuts %s.", Hitter->CHAR_POSSESSIVE_PRONOUN, Enemy->CHAR_DESCRIPTION(DEFINITE));
    return Enemy->ReceiveBodyPartDamage(Hitter, 4 + (RAND() % 5), PHYSICAL_DAMAGE, BodyPartIndex, Direction) || BaseSuccess;
  }
  return BaseSuccess;
}
#endif
