#ifdef HEADER_PHASE
ITEM(masamune, meleeweapon)
{
public:
  virtual truth HitEffect (character *, character *, v2, int, int, truth);
  virtual truth AllowAlphaEverywhere () const { return true; }
protected:
  virtual int GetClassAnimationFrames () const { return 32; }
  virtual col16 GetOutlineColor(int) const;
  virtual alpha GetOutlineAlpha(int) const;
};


#else



col16 masamune::GetOutlineColor (int) const { return MakeRGB16(0, 0, 255); }



alpha masamune::GetOutlineAlpha (int Frame) const {
  Frame &= 31;
  return 50+(Frame*(31-Frame)>>1);
}



truth masamune::HitEffect (character *Enemy, character *Hitter, v2 HitPos, int BodyPartIndex, int Direction, truth BlockedByArmour) {
  truth BaseSuccess = meleeweapon::HitEffect(Enemy, Hitter, HitPos, BodyPartIndex, Direction, BlockedByArmour);
  if (!IsBroken() && Enemy->IsEnabled() && !(RAND() % 4)) {
    if (Enemy->IsPlayer() || Hitter->IsPlayer() || Enemy->CanBeSeenByPlayer() || Hitter->CanBeSeenByPlayer())
      ADD_MESSAGE("%s Masamune's slash cuts %s so deep you thought it was cut in half for a moment.", Hitter->CHAR_POSSESSIVE_PRONOUN, Enemy->CHAR_DESCRIPTION(DEFINITE));
    return Enemy->ReceiveBodyPartDamage(Hitter, 4 + (RAND() % 4), PHYSICAL_DAMAGE, BodyPartIndex, Direction) || BaseSuccess;
  }
  return BaseSuccess;
}
#endif
