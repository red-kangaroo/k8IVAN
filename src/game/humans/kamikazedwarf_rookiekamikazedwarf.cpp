#ifdef HEADER_PHASE
CHARACTER(rookiekamikazedwarf, kamikazedwarf)
{
public:
  virtual void GetAICommand ();
  virtual truth CheckForUsefulItemsOnGround (truth = true);
  //virtual void CreateInitialEquipment (int);

protected:
  //virtual void PostConstruct ();
  //virtual int GetTorsoMainColor () const;
  //virtual int GetGauntletColor () const;
  //virtual int GetLegMainColor () const;
  //virtual int GetWSkillHits () const { return 10000; }
  virtual truth IsRookie () const { return true; }
};


#else


/*
void rookiekamikazedwarf::CreateInitialEquipment(int SpecialFlags) {
  SetRightWielded(meleeweapon::Spawn(AXE, SpecialFlags));
  SetLeftWielded(meleeweapon::Spawn(AXE));
  for (int k = 0; k < 6; ++k) GetStack()->AddItem(meleeweapon::Spawn(AXE));
  GetStack()->AddItem(lantern::Spawn());
  GetCWeaponSkill(AXES)->AddHit(GetWSkillHits());
  GetCurrentRightSWeaponSkill()->AddHit(GetWSkillHits());
  humanoid::CreateInitialEquipment(SpecialFlags);
  SetRightWielded(holybook::Spawn(GetConfig(), SpecialFlags));
}
*/


truth rookiekamikazedwarf::CheckForUsefulItemsOnGround (truth CheckFood) {
  return character::CheckForUsefulItemsOnGround(CheckFood);
}


void rookiekamikazedwarf::GetAICommand () {
  if (CheckThrowItemOpportunity()) return;
  character::GetAICommand();
}


#endif
