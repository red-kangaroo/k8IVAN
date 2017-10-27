#ifdef HEADER_PHASE
CHARACTER(vampire, humanoid)
{
public:
  virtual truth SpecialBiteEffect (character*, v2, int, int, truth, truth Critical, int DoneDamage);
};


#else


truth vampire::SpecialBiteEffect (character *Victim, v2 HitPos, int BodyPartIndex, int Direction, truth BlockedByArmour, truth Critical, int DoneDamage) {
  /*
  if (!BlockedByArmour && Victim->IsWarmBlooded() && (!(RAND()%2) || Critical) && !Victim->AllowSpoil()) {
    if (Victim->IsHumanoid()) Victim->BeginTemporaryState(VAMPIRISM, 1500+RAND_N(2000)); // Randomly instigate vampirism
    if (Victim->IsPlayer() || IsPlayer() || Victim->CanBeSeenByPlayer() || CanBeSeenByPlayer()) {
      ADD_MESSAGE("%s drains some precious lifeblood from %s!", CHAR_DESCRIPTION(DEFINITE), Victim->CHAR_DESCRIPTION(DEFINITE));
    }
    return Victim->ReceiveBodyPartDamage(this, 10 + (RAND() % 11), DRAIN, BodyPartIndex, Direction);
  }
  return false;
  */
  if (!BlockedByArmour && Victim->IsWarmBlooded() && (!(RAND()%2) || Critical) && !Victim->AllowSpoil()) {
    if (IsPlayer()) {
      ADD_MESSAGE("You drain some precious lifeblood from %s!", Victim->CHAR_DESCRIPTION(DEFINITE));
    } else if (Victim->IsPlayer() || Victim->CanBeSeenByPlayer() || CanBeSeenByPlayer()) {
      ADD_MESSAGE("%s drains some precious lifeblood from %s!", CHAR_DESCRIPTION(DEFINITE), Victim->CHAR_DESCRIPTION(DEFINITE));
    }

    if (Victim->IsHumanoid() && !Victim->StateIsActivated(VAMPIRISM) && !Victim->StateIsActivated(LYCANTHROPY) && !Victim->StateIsActivated(DISEASE_IMMUNITY)) {
      Victim->BeginTemporaryState(VAMPIRISM, 5000 + RAND_N(2500));
    }

    // HP recieved is about half the damage done; against werewolves this is full
    int DrainDamage = (DoneDamage>>1)+1;
    if (Victim->StateIsActivated(LYCANTHROPY)) DrainDamage = DoneDamage+1;

    if (IsPlayer()) game::DoEvilDeed(10);

    return Victim->ReceiveBodyPartDamage(this, DrainDamage, DRAIN, BodyPartIndex, Direction);
  }

  return false;
}

#endif
