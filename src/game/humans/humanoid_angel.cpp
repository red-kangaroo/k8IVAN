#ifdef HEADER_PHASE
CHARACTER(angel, humanoid)
{
 public:
  angel() : LastHealed(0) { }
  virtual void Load(inputfile&);
  virtual void Save(outputfile&) const;
  virtual truth AttachBodyPartsOfFriendsNear();
  virtual truth BodyPartIsVital(int) const;
  virtual int GetAttribute(int, truth = true) const;
  virtual col24 GetBaseEmitation() const;
  virtual truth CanCreateBodyPart(int) const;
  virtual cfestring& GetStandVerb() const { return character::GetStandVerb(); }
  virtual void FinalProcessForBone();
  virtual void CreateInitialEquipment(int);
 protected:
  virtual int GetTorsoMainColor() const;
  virtual int GetArmMainColor() const;
  virtual void GetAICommand();
  feuLong LastHealed;
};


#else



truth angel::BodyPartIsVital(int I) const { return I == TORSO_INDEX || I == HEAD_INDEX; }



void angel::Save(outputfile& SaveFile) const
{
  humanoid::Save(SaveFile);
  SaveFile << LastHealed;
}



void angel::Load(inputfile& SaveFile)
{
  humanoid::Load(SaveFile);
  SaveFile >> LastHealed;
}



void angel::CreateInitialEquipment(int SpecialFlags)
{
  humanoid::CreateInitialEquipment(SpecialFlags);
  GetStack()->AddItem(holybook::Spawn(GetConfig(), SpecialFlags));
  armor* Equipment;
  meleeweapon* Weapon;

  switch(GetMasterGod()->GetBasicAlignment())
  {
   case GOOD:
    Equipment = bodyarmor::Spawn(PLATE_MAIL, SpecialFlags|NO_MATERIALS);
    Equipment->InitMaterials(MAKE_MATERIAL(ANGEL_HAIR), !(SpecialFlags & NO_PIC_UPDATE));
    Equipment->SetEnchantment(1);
    SetBodyArmor(Equipment);
    Weapon = meleeweapon::Spawn(LONG_SWORD, SpecialFlags|NO_MATERIALS);
    Weapon->InitMaterials(MAKE_MATERIAL(MITHRIL), MAKE_MATERIAL(MITHRIL), !(SpecialFlags & NO_PIC_UPDATE));
    Weapon->SetEnchantment(2);
    SetRightWielded(Weapon);
    Equipment = shield::Spawn(0, SpecialFlags|NO_MATERIALS);
    Equipment->InitMaterials(MAKE_MATERIAL(MITHRIL), !(SpecialFlags & NO_PIC_UPDATE));
    Equipment->SetEnchantment(2);
    SetLeftWielded(Equipment);
    GetCWeaponSkill(LARGE_SWORDS)->AddHit(20000);
    GetCWeaponSkill(SHIELDS)->AddHit(50000);
    GetCurrentRightSWeaponSkill()->AddHit(20000);
    GetCurrentLeftSWeaponSkill()->AddHit(20000);
    GetRightArm()->SetDexterity(40);
    GetLeftArm()->SetDexterity(40);
    break;
   case NEUTRAL:
    Equipment = cloak::Spawn(0, SpecialFlags|NO_MATERIALS);
    Equipment->InitMaterials(MAKE_MATERIAL(ANGEL_HAIR), !(SpecialFlags & NO_PIC_UPDATE));
    Equipment->SetEnchantment(1);
    SetCloak(Equipment);
    Weapon = meleeweapon::Spawn(WAR_HAMMER, SpecialFlags|NO_MATERIALS);
    Weapon->InitMaterials(MAKE_MATERIAL(MITHRIL), MAKE_MATERIAL(TEAK_WOOD), !(SpecialFlags & NO_PIC_UPDATE));
    Weapon->SetEnchantment(2);
    SetRightWielded(Weapon);
    Weapon = meleeweapon::Spawn(WAR_HAMMER, SpecialFlags|NO_MATERIALS);
    Weapon->InitMaterials(MAKE_MATERIAL(MITHRIL), MAKE_MATERIAL(TEAK_WOOD), !(SpecialFlags & NO_PIC_UPDATE));
    Weapon->SetEnchantment(2);
    SetLeftWielded(Weapon);
    GetCWeaponSkill(BLUNT_WEAPONS)->AddHit(50000);
    GetCurrentRightSWeaponSkill()->AddHit(20000);
    GetCurrentLeftSWeaponSkill()->AddHit(20000);
    SetEndurance(40);
    break;
   case EVIL:
    Weapon = meleeweapon::Spawn(HALBERD, SpecialFlags|NO_MATERIALS);
    Weapon->InitMaterials(MAKE_MATERIAL(MITHRIL), MAKE_MATERIAL(EBONY_WOOD), !(SpecialFlags & NO_PIC_UPDATE));
    Weapon->SetEnchantment(2);
    SetRightWielded(Weapon);
    Equipment = gauntlet::Spawn(0, SpecialFlags|NO_MATERIALS);
    Equipment->InitMaterials(MAKE_MATERIAL(ANGEL_HAIR), !(SpecialFlags & NO_PIC_UPDATE));
    Equipment->SetEnchantment(1);
    SetRightGauntlet(Equipment);
    Equipment = gauntlet::Spawn(0, SpecialFlags|NO_MATERIALS);
    Equipment->InitMaterials(MAKE_MATERIAL(ANGEL_HAIR), !(SpecialFlags & NO_PIC_UPDATE));
    Equipment->SetEnchantment(1);
    SetLeftGauntlet(Equipment);
    GetCWeaponSkill(POLE_ARMS)->AddHit(100000);
    GetCurrentRightSWeaponSkill()->AddHit(100000);
    GetRightArm()->SetStrength(40);
    GetLeftArm()->SetStrength(40);
  }
}



void angel::GetAICommand()
{
  if((LastHealed || game::GetTick() - LastHealed > 10000) && AttachBodyPartsOfFriendsNear())
    return;

  humanoid::GetAICommand();
}



truth angel::AttachBodyPartsOfFriendsNear()
{
  character* HurtOne = 0;
  bodypart* SeveredOne = 0;

  for(int d = 0; d < GetNeighbourSquares(); ++d)
  {
    square* Square = GetNeighbourSquare(d);

    if(Square)
    {
      character* Char = Square->GetCharacter();

      if(Char && (!HurtOne || Char->IsPlayer()) && GetRelation(Char) == FRIEND && !Char->HasAllBodyParts())
      {
  bodypart* BodyPart = Char->FindRandomOwnBodyPart(false);

  if(BodyPart)
  {
    HurtOne = Char;
    SeveredOne = BodyPart;
  }
      }
    }
  }

  if(HurtOne)
  {
    if(HurtOne->IsPlayer())
      ADD_MESSAGE("%s puts your %s back to its place.", CHAR_DESCRIPTION(DEFINITE), SeveredOne->GetBodyPartName().CStr());
    else if(CanBeSeenByPlayer())
      ADD_MESSAGE("%s helps %s by putting %s %s in its old place.", CHAR_DESCRIPTION(DEFINITE), HurtOne->CHAR_DESCRIPTION(DEFINITE), HurtOne->GetPossessivePronoun().CStr(), SeveredOne->GetBodyPartName().CStr());

    SeveredOne->SetHP(1);
    SeveredOne->RemoveFromSlot();
    HurtOne->AttachBodyPart(SeveredOne);
    LastHealed = game::GetTick();
    DexterityAction(10);
    return true;
  }
  else
    return false;
}



col16 angel::GetTorsoMainColor() const
{
  return GetMasterGod()->GetColor();
}



col16 angel::GetArmMainColor() const
{
  return GetMasterGod()->GetColor();
}



int angel::GetAttribute(int Identifier, truth AllowBonus) const // temporary until wings are bodyparts
{
  if(Identifier == LEG_STRENGTH)
    return GetDefaultLegStrength();
  else if(Identifier == AGILITY)
    return GetDefaultAgility();
  else
    return humanoid::GetAttribute(Identifier, AllowBonus);
}



col24 angel::GetBaseEmitation() const
{
  switch(GetMasterGod()->GetBasicAlignment())
  {
   case GOOD: return MakeRGB24(150, 150, 150);
   case NEUTRAL: return MakeRGB24(120, 120, 150);
   case EVIL: return MakeRGB24(150, 110, 110);
  }

  return 0;
}



truth angel::CanCreateBodyPart(int I) const
{
  return I == TORSO_INDEX || I == HEAD_INDEX || I == RIGHT_ARM_INDEX || I == LEFT_ARM_INDEX;
}



void angel::FinalProcessForBone()
{
  humanoid::FinalProcessForBone();
  LastHealed = 0;
}
#endif
