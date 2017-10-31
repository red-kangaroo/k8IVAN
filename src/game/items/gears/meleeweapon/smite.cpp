#ifdef HEADER_PHASE
ITEM(smite, meleeweapon)
{
public:
  virtual truth HitEffect (character *, character *, v2, int, int, truth);
  virtual int GetSpecialFlags () const;
  virtual truth ReceiveDamage (character*, int, int, int);
};


#else



int smite::GetSpecialFlags() const {
  return !IsBroken() ? meleeweapon::GetSpecialFlags()|ST_LIGHTNING : meleeweapon::GetSpecialFlags();
}



truth smite::HitEffect (character *Enemy, character *Hitter, v2 HitPos, int BodyPartIndex, int Direction, truth BlockedByArmour) {
  truth BaseSuccess = meleeweapon::HitEffect(Enemy, Hitter, HitPos, BodyPartIndex, Direction, BlockedByArmour);
  if (!IsBroken() && Enemy->IsEnabled() && !(RAND() % 5)) {
    if (Enemy->IsPlayer() || Hitter->IsPlayer() || Enemy->CanBeSeenByPlayer() || Hitter->CanBeSeenByPlayer())
      ADD_MESSAGE("%s mace named Smite shoots a lightning bolt at %s!", Hitter->CHAR_POSSESSIVE_PRONOUN, Enemy->CHAR_DESCRIPTION(DEFINITE));
    beamdata Beam (
      Hitter,
      CONST_S("electrocuted @bkp mace named Smite"),
      Hitter->GetPos(),
      WHITE,
      BEAM_LIGHTNING,
      Direction,
      5,
      0
    );
    GetLevel()->LightningBeam(Beam);
    return true;
  }
  return BaseSuccess;
}



truth smite::ReceiveDamage (character *Damager, int Damage, int Type, int Dir) {
  return Type & ELECTRICITY ? false : meleeweapon::ReceiveDamage(Damager, Damage, Type, Dir);
}
#endif
