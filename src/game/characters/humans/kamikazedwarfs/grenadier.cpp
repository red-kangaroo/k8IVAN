#ifdef HEADER_PHASE
CHARACTER(grenadierdwarf, kamikazedwarf)
{
public:
  virtual void GetAICommand ();
  //virtual void CreateInitialEquipment (int);

protected:
  virtual void PostConstruct ();
  virtual int GetTorsoMainColor () const;
  virtual int GetGauntletColor () const;
  virtual int GetLegMainColor() const;
  virtual int GetWSkillHits () const { return 50000; }
  virtual truth IsGrenadier () const { return true; }
};


#else


void grenadierdwarf::GetAICommand () {
  if (CheckThrowItemOpportunity()) return;
  if (!RAND_N(50)) { SingRandomSong(); return; }
  character::GetAICommand();
}


/*
void grenadierdwarf::CreateInitialEquipment (int SpecialFlags) {
  SetLeftWielded(meleeweapon::Spawn(GREAT_AXE, SpecialFlags));
  for (int k = 0; k < 3; ++k) GetStack()->AddItem(gasgrenade::Spawn());
  GetStack()->AddItem(lantern::Spawn());
  GetCWeaponSkill(AXES)->AddHit(GetWSkillHits());
  GetCurrentRightSWeaponSkill()->AddHit(GetWSkillHits());
  humanoid::CreateInitialEquipment(SpecialFlags);
  SetRightWielded(holybook::Spawn(GetConfig(), SpecialFlags));
}
*/


col16 grenadierdwarf::GetTorsoMainColor () const {
  return GetMasterGod()->GetEliteColor();
}


col16 grenadierdwarf::GetGauntletColor () const {
  return GetMasterGod()->GetEliteColor();
}


col16 grenadierdwarf::GetLegMainColor () const {
  return GetMasterGod()->GetEliteColor();
}


void grenadierdwarf::PostConstruct () {
  kamikazedwarf::PostConstruct();
  //
  ivantime Time;
  game::GetTime(Time);
  int Modifier = Time.Day-KAMIKAZE_INVISIBILITY_DAY_MIN;
  //
  if (Time.Day >= KAMIKAZE_INVISIBILITY_DAY_MAX || (Modifier > 0 && RAND_N(KAMIKAZE_INVISIBILITY_DAY_MAX-KAMIKAZE_INVISIBILITY_DAY_MIN) < Modifier)) {
    GainIntrinsic(INVISIBLE);
  }
}


#endif
