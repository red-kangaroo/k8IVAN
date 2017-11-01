#ifdef HEADER_PHASE
CHARACTER(werewolfwolf, humanoid)
{
 public:
  virtual festring GetKillName() const;
  virtual truth SpecialBiteEffect(character* Victim, v2 HitPos, int BodyPartIndex, int Direction, truth BlockedByArmour, truth Critical, int DoneDamage);
};


#else


festring werewolfwolf::GetKillName() const
{
  if (GetPolymorphBackup() && GetPolymorphBackup()->IsOfType("werewolfhuman")) return GetName(INDEFINITE);
  return humanoid::GetKillName();
}


truth werewolfwolf::SpecialBiteEffect(character* Victim, v2 HitPos, int BodyPartIndex, int Direction, truth BlockedByArmour, truth Critical, int DoneDamage) {
  if (!BlockedByArmour && Victim->IsWarmBlooded() && (!(RAND() % 2) || Critical) && !Victim->AllowSpoil()) {
    // Werewolf wolf gives lycanthropy
    if (Victim->IsHumanoid() && !Victim->StateIsActivated(VAMPIRISM) && !Victim->StateIsActivated(LYCANTHROPY) && !Victim->StateIsActivated(DISEASE_IMMUNITY)) {
      Victim->BeginTemporaryState(LYCANTHROPY, 6000 + RAND_N(2000));
    }

    // Werewolves do double damage against vampires and this is a drain attack
    if (Victim->StateIsActivated(VAMPIRISM) && (DoneDamage >= 1)) {
      if (IsPlayer())
        ADD_MESSAGE("You drain some life force from %s!", Victim->CHAR_DESCRIPTION(DEFINITE));
      else if (Victim->IsPlayer() || Victim->CanBeSeenByPlayer() || CanBeSeenByPlayer())
        ADD_MESSAGE("%s drains some life force from %s!", CHAR_DESCRIPTION(DEFINITE), Victim->CHAR_DESCRIPTION(DEFINITE));
      return Victim->ReceiveBodyPartDamage(this, DoneDamage, DRAIN, BodyPartIndex, Direction);
    }
    return false;
  }
  return false;
}

#endif
