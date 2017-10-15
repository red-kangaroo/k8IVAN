#ifdef HEADER_PHASE
ITEM(flamingsword, meleeweapon)
{
 public:
  virtual truth HitEffect (character*, character*, v2, int, int, truth);
  virtual int GetSpecialFlags () const;
  virtual truth IsLostRubyFlamingSword () const;
};


#else


int flamingsword::GetSpecialFlags () const { return meleeweapon::GetSpecialFlags()|ST_FLAME_1; }
truth flamingsword::IsLostRubyFlamingSword () const { return (GetConfig() == LOST_RUBY_FLAMING_SWORD); }


truth flamingsword::HitEffect (character* Enemy, character* Hitter, v2 HitPos, int BodyPartIndex, int Direction, truth BlockedByArmour) {
  truth BaseSuccess = meleeweapon::HitEffect(Enemy, Hitter, HitPos, BodyPartIndex, Direction, BlockedByArmour);
  if (Enemy->IsEnabled() && (RAND()&1)) {
    if (Hitter) {
      if (Enemy->IsPlayer() || Hitter->IsPlayer() || Enemy->CanBeSeenByPlayer() || Hitter->CanBeSeenByPlayer()) {
        ADD_MESSAGE("%s sword burns %s.", Hitter->CHAR_POSSESSIVE_PRONOUN, Enemy->CHAR_DESCRIPTION(DEFINITE));
      }
    } else {
      if (Enemy->IsPlayer() || Enemy->CanBeSeenByPlayer()) {
        ADD_MESSAGE("The sword burns %s.", Enemy->CHAR_DESCRIPTION(DEFINITE));
      }
    }
    return Enemy->ReceiveBodyPartDamage(Hitter, 3 + (RAND() & 3), FIRE, BodyPartIndex, Direction) || BaseSuccess;
  }
  return BaseSuccess;
}

#endif
