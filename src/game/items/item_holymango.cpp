#ifdef HEADER_PHASE
ITEM(holymango, item)
{
public:
  virtual truth HitEffect (character *, character *, v2, int, int, truth);
  //virtual truth Zap (character *, v2, int);
  //virtual void Be () {}
  virtual int GetSpecialFlags () const;
  virtual void AddInventoryEntry (ccharacter *, festring &, int, truth) const;
  virtual truth ReceiveDamage (character *, int, int, int);
  virtual truth IsMango () const { return true; }
};


#else


int holymango::GetSpecialFlags () const { return ST_FLAME_1; }


truth holymango::HitEffect (character *Enemy, character *Hitter, v2 HitPos, int BodyPartIndex, int Direction, truth BlockedByArmour) {
  truth BaseSuccess = 1;//mango::HitEffect(Enemy, Hitter, HitPos, BodyPartIndex, Direction, BlockedByArmour);
  //
  if (Enemy->IsEnabled() && RAND() & 1) {
    if (Enemy->IsPlayer() || Hitter->IsPlayer() || Enemy->CanBeSeenByPlayer() || Hitter->CanBeSeenByPlayer()) {
      ADD_MESSAGE("%s mango burns %s.", Hitter->CHAR_POSSESSIVE_PRONOUN, Enemy->CHAR_DESCRIPTION(DEFINITE));
    }
    return Enemy->ReceiveBodyPartDamage(Hitter, 2 + (RAND() & 3), FIRE, BodyPartIndex, Direction) || BaseSuccess;
  }
  return BaseSuccess;
}


/*
truth holymango::Zap (character *Zapper, v2, int Direction) {
  if (Charges > TimesUsed) {
    ADD_MESSAGE("BANG! You zap %s!", CHAR_NAME(DEFINITE));
    Zapper->EditExperience(PERCEPTION, 150, 1 << 10);
    beamdata Beam(
      Zapper,
      CONST_S("killed by ") + GetName(INDEFINITE),
      Zapper->GetPos(),
      YELLOW,
      BEAM_FIRE_BALL,
      Direction,
      50,
      0
    );
    (GetLevel()->*level::GetBeam(PARTICLE_BEAM))(Beam);
    ++TimesUsed;
  } else {
    ADD_MESSAGE("Click!");
  }
  return true;
}
*/


// never piled
void holymango::AddInventoryEntry (ccharacter *Viewer, festring& Entry, int, truth ShowSpecialInfo) const {
  AddName(Entry, INDEFINITE);
  if (ShowSpecialInfo) {
    Entry << " [\1W" << GetWeight() << "g\2, DAM \1Y" << GetBaseMinDamage() << "\2-\1Y" << GetBaseMaxDamage() << "\2";
    Entry << ", " << GetBaseToHitValueDescription();
    if (!IsBroken()) Entry << ", " << GetStrengthValueDescription();
    int CWeaponSkillLevel = Viewer->GetCWeaponSkillLevel(this);
    int SWeaponSkillLevel = Viewer->GetSWeaponSkillLevel(this);
    if (CWeaponSkillLevel || SWeaponSkillLevel) Entry << ", skill \1W" << CWeaponSkillLevel << '/' << SWeaponSkillLevel << "\2";
    Entry << ']';
  }
}


truth holymango::ReceiveDamage (character *Damager, int Damage, int Type, int) {
  if (Type & (PHYSICAL_DAMAGE|FIRE|ENERGY) && Damage && (Damage > 50 || !(RAND()%(100/Damage)))) {
    lsquare *Square = GetLSquareUnder();
    festring DeathMsg = CONST_S("killed by an explosion of ");
    //
    AddName(DeathMsg, INDEFINITE);
    if (Damager) DeathMsg << " caused @bk";
    if (GetSquareUnder()->CanBeSeenByPlayer(true)) ADD_MESSAGE("%s explodes!", GetExtendedDescription().CStr());
    RemoveFromSlot();
    SendToHell();
    Square->GetLevel()->Explosion(Damager, DeathMsg, Square->GetPos(), 6*50);
    return true;
  }
  return false;
}


#endif
