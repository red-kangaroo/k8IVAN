#ifdef HEADER_PHASE
CHARACTER(firefox, canine)
{
protected:
  virtual truth SpecialBiteEffect (character *, v2, int, int, truth);
  virtual int GetSpecialBodyPartFlags (int) const;
};


#else


int firefox::GetSpecialBodyPartFlags (int) const { return ST_FLAMES; }


truth firefox::SpecialBiteEffect (character * Enemy, v2 HitPos, int BodyPartIndex, int Direction, truth BlockedByArmour) {
  if (Enemy->IsEnabled() && (RAND()&1)) {
    if (Enemy->CanBeSeenByPlayer() && IsPlayer()) {
      ADD_MESSAGE("Your bite burns %s.", Enemy->CHAR_DESCRIPTION(DEFINITE));
    } else if (Enemy->IsPlayer() || Enemy->CanBeSeenByPlayer() || CanBeSeenByPlayer()) {
      ADD_MESSAGE("The bite of %s burns %s.", CHAR_DESCRIPTION(DEFINITE), Enemy->CHAR_DESCRIPTION(DEFINITE));
    }
    Enemy->ReceiveBodyPartDamage(this, 2+(RAND()&3), FIRE, BodyPartIndex, Direction);
    Enemy->CheckDeath(CONST_S("killed by the firey bite of ") + GetName(INDEFINITE), this);
    return true;
  }
  return false;
}


/*
truth firefox::Bite (character *Enemy, v2 HitPos, int BodyPartIndex, int Direction, truth BlockedByArmour) {
  truth BaseSuccess = nonhumanoid::Bite(Enemy, HitPos, BodyPartIndex, Direction, BlockedByArmour);
  //
  if (Enemy->IsEnabled() && (RAND()&1)) {
    if (Enemy->CanBeSeenByPlayer() && IsPlayer()) {
      ADD_MESSAGE("Your bite burns %s.", Enemy->CHAR_DESCRIPTION(DEFINITE));
    } else if (Enemy->IsPlayer() || Enemy->CanBeSeenByPlayer() || CanBeSeenByPlayer()) {
      ADD_MESSAGE("The bite of %s burns %s.", CHAR_DESCRIPTION(DEFINITE), Enemy->CHAR_DESCRIPTION(DEFINITE));
    }
    return Enemy->ReceiveBodyPartDamage(this, 2 + (RAND() & 3), FIRE, BodyPartIndex, Direction) || BaseSuccess;
  }
  return BaseSuccess;
}
*/


#endif
