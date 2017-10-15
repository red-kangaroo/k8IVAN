#ifdef HEADER_PHASE
CHARACTER(vampire, humanoid)
{
public:
  virtual truth SpecialBiteEffect (character*, v2, int, int, truth, truth Critical, int DoneDamage);
};


#else



truth vampire::SpecialBiteEffect (character *Char, v2 HitPos, int BodyPartIndex, int Direction, truth BlockedByArmour, truth Critical, int DoneDamage) {
  if (!BlockedByArmour && !(RAND() % 2) && Char->IsWarm()) {
    if (Char->IsHumanoid()) Char->BeginTemporaryState(VAMPIRISM, 1500+RAND_N(2000)); // Randomly instigate vampirism
    if (Char->IsPlayer() || IsPlayer() || Char->CanBeSeenByPlayer() || CanBeSeenByPlayer()) {
      ADD_MESSAGE("%s drains some precious lifeblood from %s!", CHAR_DESCRIPTION(DEFINITE), Char->CHAR_DESCRIPTION(DEFINITE));
    }
    return Char->ReceiveBodyPartDamage(this, 10 + (RAND() % 11), DRAIN, BodyPartIndex, Direction);
  }
  return false;
}
#endif
