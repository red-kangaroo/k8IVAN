#ifdef HEADER_PHASE
CHARACTER(insudo, angel)
{
public:
  virtual col24 GetBaseEmitation() const { return MakeRGB24(120, 120, 150); }
  virtual void CreateInitialEquipment(int); //has a bespoke function to handle this, need to adapt from the archangel code
protected:
  virtual int GetTorsoMainColor() const { return MakeRGB16(120, 120, 120); }
  virtual int GetArmMainColor() const { return MakeRGB16(120, 120, 120); }
  virtual void GetAICommand();
};


#else



void insudo::GetAICommand () {
  if ((LastHealed || game::GetTick()-LastHealed > 10000) && AttachBodyPartsOfFriendsNear()) return;
  StandIdleAI();
  //humanoid::GetAICommand();
}



void insudo::CreateInitialEquipment (int SpecialFlags) {
  armor *Equipment;
  meleeweapon *Weapon;
  //
  humanoid::CreateInitialEquipment(SpecialFlags);
  Weapon = eptyron::Spawn();
  Weapon->InitMaterials(MAKE_MATERIAL(EMERALD), MAKE_MATERIAL(OCTIRON), !(SpecialFlags & NO_PIC_UPDATE));
  Weapon->SetEnchantment(4);
  SetRightWielded(Weapon);
  Equipment = shield::Spawn(0, SpecialFlags|NO_MATERIALS);
  Equipment->InitMaterials(MAKE_MATERIAL(EMERALD), !(SpecialFlags & NO_PIC_UPDATE));
  Equipment->SetEnchantment(4);
  SetLeftWielded(Equipment);
  GetCWeaponSkill(AXES)->AddHit(200000);
  GetCWeaponSkill(SHIELDS)->AddHit(500000);
  GetCurrentRightSWeaponSkill()->AddHit(200000);
  GetCurrentLeftSWeaponSkill()->AddHit(200000);
  GetRightArm()->SetDexterity(70);
  GetLeftArm()->SetDexterity(70);
}
#endif
