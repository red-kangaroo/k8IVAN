#ifdef HEADER_PHASE
ITEM(holybanana, banana)
{
 public:
  virtual truth HitEffect(character*, character*, v2, int, int, truth);
  virtual truth Zap(character*, v2, int);
  virtual void Be() { }
  virtual int GetSpecialFlags() const;
  virtual void AddInventoryEntry(ccharacter*, festring&, int, truth) const;
  virtual truth ReceiveDamage(character*, int, int, int);
};


#else



int holybanana::GetSpecialFlags() const { return ST_FLAME_1; }



truth holybanana::HitEffect(character* Enemy, character* Hitter, v2 HitPos, int BodyPartIndex, int Direction, truth BlockedByArmour)
{
  truth BaseSuccess = banana::HitEffect(Enemy, Hitter, HitPos, BodyPartIndex, Direction, BlockedByArmour);

  if(Enemy->IsEnabled() && RAND() & 1)
  {
    if(Enemy->IsPlayer() || Hitter->IsPlayer() || Enemy->CanBeSeenByPlayer() || Hitter->CanBeSeenByPlayer())
      ADD_MESSAGE("%s banana burns %s.", Hitter->CHAR_POSSESSIVE_PRONOUN, Enemy->CHAR_DESCRIPTION(DEFINITE));

    return Enemy->ReceiveBodyPartDamage(Hitter, 2 + (RAND() & 3), FIRE, BodyPartIndex, Direction) || BaseSuccess;
  }
  else
    return BaseSuccess;
}



truth holybanana::Zap(character* Zapper, v2, int Direction)
{
  if(Charges > TimesUsed)
  {
    ADD_MESSAGE("BANG! You zap %s!", CHAR_NAME(DEFINITE));
    Zapper->EditExperience(PERCEPTION, 150, 1 << 10);

    beamdata Beam
      (
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
  }
  else
    ADD_MESSAGE("Click!");

  return true;
}



void holybanana::AddInventoryEntry(ccharacter* Viewer, festring& Entry, int, truth ShowSpecialInfo) const // never piled
{
  AddName(Entry, INDEFINITE);

  if(ShowSpecialInfo)
  {
    Entry << " [\1C" << GetWeight() << "g\2, DAM \1Y" << GetBaseMinDamage() << "\2-\1Y" << GetBaseMaxDamage() << "\2";
    Entry << ", " << GetBaseToHitValueDescription();

    if(!IsBroken())
      Entry << ", " << GetStrengthValueDescription();

    int CWeaponSkillLevel = Viewer->GetCWeaponSkillLevel(this);
    int SWeaponSkillLevel = Viewer->GetSWeaponSkillLevel(this);

    if(CWeaponSkillLevel || SWeaponSkillLevel)
      Entry << ", skill \1C" << CWeaponSkillLevel << '/' << SWeaponSkillLevel << "\2";

    if(TimesUsed == 1)
      Entry << ", used \1C1\2 time";
    else if(TimesUsed)
      Entry << ", used \1C" << TimesUsed << "\2 times";

    Entry << ']';
  }
}



truth holybanana::ReceiveDamage(character* Damager, int Damage, int Type, int)
{
  if(TimesUsed != 6 && Type & (PHYSICAL_DAMAGE|FIRE|ENERGY) && Damage && (Damage > 50 || !(RAND() % (100 / Damage))))
  {
    festring DeathMsg = CONST_S("killed by an explosion of ");
    AddName(DeathMsg, INDEFINITE);

    if(Damager)
      DeathMsg << " caused @bk";

    if(GetSquareUnder()->CanBeSeenByPlayer(true))
      ADD_MESSAGE("%s explodes!", GetExtendedDescription().CStr());

    lsquare* Square = GetLSquareUnder();
    RemoveFromSlot();
    SendToHell();
    Square->GetLevel()->Explosion(Damager, DeathMsg, Square->GetPos(), (6 - TimesUsed) * 50);
    return true;
  }

  return false;
}
#endif
