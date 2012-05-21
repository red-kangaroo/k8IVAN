#ifdef HEADER_PHASE
CHARACTER(thunderbird, nonhumanoid)
{
protected:
  virtual cchar *FirstPersonBiteVerb () const;
  virtual cchar *FirstPersonCriticalBiteVerb () const;
  virtual cchar *ThirdPersonBiteVerb () const;
  virtual cchar *ThirdPersonCriticalBiteVerb () const;
  virtual int GetSpecialBodyPartFlags (int) const;
  virtual truth SpecialBiteEffect (character *, v2, int, int, truth);
};


#else


cchar *thunderbird::FirstPersonBiteVerb () const { return "peck"; }
cchar *thunderbird::FirstPersonCriticalBiteVerb () const { return "critically peck"; }
cchar *thunderbird::ThirdPersonBiteVerb () const { return "pecks"; }
cchar *thunderbird::ThirdPersonCriticalBiteVerb () const { return "critically pecks"; }
int thunderbird::GetSpecialBodyPartFlags (int) const { return ST_LIGHTNING; }


truth thunderbird::SpecialBiteEffect (character *Enemy, v2 HitPos, int BodyPartIndex, int Direction, truth BlockedByArmour) {
  if (Enemy->IsEnabled() && (RAND()&1)) {
    if (Enemy->CanBeSeenByPlayer() && IsPlayer()) {
      ADD_MESSAGE("Your beak emits a thunderous peal directed at %s.", Enemy->CHAR_DESCRIPTION(DEFINITE));
    } else if (Enemy->IsPlayer() || Enemy->CanBeSeenByPlayer() || CanBeSeenByPlayer()) {
      ADD_MESSAGE("Thunder is emitted from the beak of %s and the sound waves hit %s.", CHAR_DESCRIPTION(DEFINITE), Enemy->CHAR_DESCRIPTION(DEFINITE));
    }
    Enemy->ReceiveBodyPartDamage(this, 2+(RAND()&3), SOUND, BodyPartIndex, Direction);
    Enemy->CheckDeath(CONST_S("killed by the thunderous attack of ") + GetName(INDEFINITE), this);
    return true;
  }
  return false;
}


#endif
