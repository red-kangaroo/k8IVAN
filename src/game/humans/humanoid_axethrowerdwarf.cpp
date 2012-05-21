#ifdef HEADER_PHASE
CHARACTER(axethrowerdwarf, humanoid)
{
public:
  virtual void GetAICommand ();
  virtual void CreateInitialEquipment (int);

protected:
  virtual int GetTorsoMainColor () const;
  virtual int GetGauntletColor () const;
  virtual int GetLegMainColor () const;
  virtual v2 GetDrawDisplacement (int) const;
  virtual int GetWSkillHits () const { return 10000; }
  virtual truth IsElite () const { return false; }
};


#else


void axethrowerdwarf::CreateInitialEquipment (int SpecialFlags) {
  SetRightWielded(meleeweapon::Spawn(AXE, SpecialFlags));
  SetLeftWielded(meleeweapon::Spawn(AXE, SpecialFlags));
  for (int k = 0; k < 6; ++k) GetStack()->AddItem(meleeweapon::Spawn(AXE));
  GetStack()->AddItem(lantern::Spawn());
  GetCWeaponSkill(AXES)->AddHit(GetWSkillHits());
  GetCurrentRightSWeaponSkill()->AddHit(GetWSkillHits());
}


void axethrowerdwarf::GetAICommand () {
  if (CheckThrowItemOpportunity()) return;
  character::GetAICommand();
}


v2 axethrowerdwarf::GetDrawDisplacement (int j) const {
  static const v2 DrawDisplacement[] = { v2(0, 0), v2(0, 1), v2(0, -1), v2(0, -1), v2(0, -1), v2(0, 0), v2(0, 0) };
  return DrawDisplacement[j];
}


col16 axethrowerdwarf::GetTorsoMainColor () const {
  return GetMasterGod()->GetColor();
}


col16 axethrowerdwarf::GetGauntletColor () const {
  return GetMasterGod()->GetColor();
}


col16 axethrowerdwarf::GetLegMainColor () const {
  return GetMasterGod()->GetColor();
}


#endif
