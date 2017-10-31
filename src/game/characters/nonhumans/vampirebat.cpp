#ifdef HEADER_PHASE
CHARACTER(vampirebat, bat)
{
 protected:
  virtual truth SpecialBiteEffect(character *Victim, v2 HitPos, int BodyPartIndex, int Direction, truth BlockedByArmour, truth Critical, int DoneDamage);
};


#else


truth vampirebat::SpecialBiteEffect (character *Victim, v2 HitPos, int BodyPartIndex, int Direction, truth BlockedByArmour, truth Critical, int DoneDamage) {
  if (!BlockedByArmour && Victim->IsWarmBlooded() && (!(RAND() % 3) || Critical) && !Victim->AllowSpoil()) {
    if (IsPlayer())
      ADD_MESSAGE("You drain some precious lifeblood from %s!", Victim->CHAR_DESCRIPTION(DEFINITE));
    else if (Victim->IsPlayer() || Victim->CanBeSeenByPlayer() || CanBeSeenByPlayer())
      ADD_MESSAGE("%s drains some precious lifeblood from %s!", CHAR_DESCRIPTION(DEFINITE), Victim->CHAR_DESCRIPTION(DEFINITE));

    if (Victim->IsHumanoid() && !Victim->StateIsActivated(LYCANTHROPY) && !Victim->StateIsActivated(DISEASE_IMMUNITY)) {
      Victim->BeginTemporaryState(VAMPIRISM, 500 + RAND_N(250));
    }

    // HP recieved is about half the damage done; against werewolves this is full
    int DrainDamage = (DoneDamage>>1)+1;
    if (Victim->StateIsActivated(LYCANTHROPY)) DrainDamage = DoneDamage+1;

    return Victim->ReceiveBodyPartDamage(this, DrainDamage, DRAIN, BodyPartIndex, Direction);
  }
  return false;
}


#endif
