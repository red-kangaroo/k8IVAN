#ifdef HEADER_PHASE
CHARACTER(archangel, angel)
{
public:
  virtual void CreateInitialEquipment (int);

protected:
  virtual int GetTorsoMainColor () const;
  virtual int GetArmMainColor () const;
};


#else


col16 archangel::GetTorsoMainColor () const { return GetMasterGod()->GetEliteColor(); }
col16 archangel::GetArmMainColor () const { return GetMasterGod()->GetEliteColor(); }


void archangel::CreateInitialEquipment (int SpecialFlags) {
  humanoid::CreateInitialEquipment(SpecialFlags);
  GetStack()->AddItem(holybook::Spawn(GetConfig(), SpecialFlags));
  //
  armor *Equipment;
  meleeweapon *Weapon;
  //
  switch (GetMasterGod()->GetBasicAlignment()) {
    case GOOD:
      Weapon = flamingsword::Spawn(0, SpecialFlags|NO_MATERIALS);
      Weapon->InitMaterials(MAKE_MATERIAL(DIAMOND), MAKE_MATERIAL(ADAMANT), !(SpecialFlags & NO_PIC_UPDATE));
      Weapon->SetEnchantment(4);
      SetRightWielded(Weapon);
      Equipment = shield::Spawn(0, SpecialFlags|NO_MATERIALS);
      Equipment->InitMaterials(MAKE_MATERIAL(DIAMOND), !(SpecialFlags & NO_PIC_UPDATE));
      Equipment->SetEnchantment(4);
      SetLeftWielded(Equipment);
      GetCWeaponSkill(LARGE_SWORDS)->AddHit(200000);
      GetCWeaponSkill(SHIELDS)->AddHit(500000);
      GetCurrentRightSWeaponSkill()->AddHit(200000);
      GetCurrentLeftSWeaponSkill()->AddHit(200000);
      GetRightArm()->SetDexterity(70);
      GetLeftArm()->SetDexterity(70);
      break;
    case NEUTRAL:
      Weapon = meleeweapon::Spawn(WAR_HAMMER, SpecialFlags|NO_MATERIALS);
      Weapon->InitMaterials(MAKE_MATERIAL(SAPPHIRE), MAKE_MATERIAL(OCTIRON), !(SpecialFlags & NO_PIC_UPDATE));
      Weapon->SetEnchantment(4);
      SetRightWielded(Weapon);
      GetCWeaponSkill(BLUNT_WEAPONS)->AddHit(500000);
      GetCurrentRightSWeaponSkill()->AddHit(200000);
      SetEndurance(70);
      break;
    case EVIL:
      Weapon = meleeweapon::Spawn(HALBERD, SpecialFlags|NO_MATERIALS);
      Weapon->InitMaterials(MAKE_MATERIAL(RUBY), MAKE_MATERIAL(OCTIRON), !(SpecialFlags & NO_PIC_UPDATE));
      Weapon->SetEnchantment(4);
      SetLeftWielded(Weapon);
      GetCWeaponSkill(POLE_ARMS)->AddHit(500000);
      GetCurrentLeftSWeaponSkill()->AddHit(500000);
      GetRightArm()->SetStrength(70);
      GetLeftArm()->SetStrength(70);
    case TOPPLED:
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
      break;
  }
}


#endif
