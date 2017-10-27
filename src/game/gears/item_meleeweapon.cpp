#ifdef HEADER_PHASE
ITEM(meleeweapon, item)
{
 public:
  meleeweapon () {}
  meleeweapon (const meleeweapon &);
  virtual ~meleeweapon ();

  virtual truth HitEffect(character*, character*, v2, int, int, truth);
  virtual void DipInto(liquid*, character*);
  virtual sLong GetPrice() const;
  virtual truth IsDippable(ccharacter*) const;
  virtual material* GetSecondaryMaterial() const { return SecondaryMaterial; }
  virtual void SetSecondaryMaterial(material*, int = 0);
  virtual void ChangeSecondaryMaterial(material*, int = 0);
  void InitMaterials(material*, material*, truth = true);
  virtual void Save(outputfile&) const;
  virtual void Load(inputfile&);
  virtual int GetMaterials() const { return 2; }
  virtual void AddInventoryEntry(ccharacter*, festring&, int, truth) const;
  virtual void SignalSpoil(material*);
  virtual void Be();
  virtual truth IsWeapon(ccharacter*) const { return true; }
  virtual int GetEnchantment() const { return Enchantment; }
  virtual void SetEnchantment(int);
  virtual void EditEnchantment(int);
  virtual int GetStrengthValue() const;
  virtual truth IsFixableBySmith(ccharacter*) const { return IsBroken() || IsRusted(); }
  virtual truth IsFixableByTailor(ccharacter*) const { return IsBroken(); }
  virtual double GetTHVBonus() const;
  virtual double GetDamageBonus() const;
  virtual int GetSpoilLevel() const;
  virtual material* GetMaterial(int) const;
  virtual void TryToRust(sLong);
  virtual material* GetConsumeMaterial(ccharacter*, materialpredicate = TrueMaterialPredicate) const;
  virtual pixelpredicate GetFluidPixelAllowedPredicate() const;
  virtual material* RemoveMaterial(material*);
  material* RemoveMainMaterial();
  material* RemoveSecondaryMaterial();
  virtual v2 GetWieldedBitmapPos(int) const;
  virtual void CalculateEmitation();
  virtual void InitMaterials(const materialscript*, const materialscript*, truth);
  virtual item* Fix();
  virtual void CalculateEnchantment();
  virtual truth AllowFluids() const { return true; }
  virtual int GetSparkleFlags() const;
  virtual truth AllowDetailedDescription () const { return true; }
 protected:
  virtual sLong GetMaterialPrice() const;
  virtual truth CalculateHasBe() const;
  virtual void PostConstruct();
  virtual void AddPostFix(festring&, int) const;
  virtual void GenerateMaterials();
  virtual col16 GetMaterialColorB(int) const;
  virtual col16 GetMaterialColorC(int) const;
  virtual alpha GetAlphaB(int) const;
  virtual int GetRustDataB() const;
  virtual col16 GetDripColor() const;
  virtual truth AllowRegularColors() const;
  material* SecondaryMaterial;
  int Enchantment;
};


#else


void meleeweapon::SetSecondaryMaterial(material* What, int SpecialFlags) { SetMaterial(SecondaryMaterial, What, GetDefaultSecondaryVolume(), SpecialFlags); }
void meleeweapon::ChangeSecondaryMaterial(material* What, int SpecialFlags) { ChangeMaterial(SecondaryMaterial, What, GetDefaultSecondaryVolume(), SpecialFlags); }
void meleeweapon::InitMaterials(material* M1, material* M2, truth CUP) { ObjectInitMaterials(MainMaterial, M1, GetDefaultMainVolume(), SecondaryMaterial, M2, GetDefaultSecondaryVolume(), CUP); }
double meleeweapon::GetTHVBonus() const { return Enchantment * .5; }
double meleeweapon::GetDamageBonus() const { return Enchantment; }
col16 meleeweapon::GetDripColor() const { return Fluid[0]->GetLiquid()->GetColor(); }
truth meleeweapon::IsDippable(ccharacter*) const { return !Fluid; }
truth meleeweapon::AllowRegularColors() const { return SecondaryMaterial->GetVolume(); }
v2 meleeweapon::GetWieldedBitmapPos(int I) const { return SecondaryMaterial->GetVolume() ? item::GetWieldedBitmapPos(I) : v2(160, 128); }
void meleeweapon::InitMaterials(const materialscript* M, const materialscript* S, truth CUP) { InitMaterials(M->Instantiate(), S->Instantiate(), CUP); }
alpha meleeweapon::GetAlphaB(int) const { return SecondaryMaterial->GetAlpha(); }
int meleeweapon::GetRustDataB() const { return SecondaryMaterial->GetRustData(); }
col16 meleeweapon::GetMaterialColorC(int Frame) const { return SecondaryMaterial->GetVolume() ? GetMaterialColorA(Frame) : TRANSPARENT_COLOR; }


material *meleeweapon::GetMaterial (int I) const {
  return !I ? MainMaterial : SecondaryMaterial;
}


col16 meleeweapon::GetMaterialColorB (int) const {
  return SecondaryMaterial->GetVolume() ? SecondaryMaterial->GetColor() : TRANSPARENT_COLOR;
}


meleeweapon::meleeweapon (const meleeweapon &MW) : mybase(MW), Enchantment(MW.Enchantment) {
  CopyMaterial(MW.SecondaryMaterial, SecondaryMaterial);
}


meleeweapon::~meleeweapon () {
  delete SecondaryMaterial;
}


void meleeweapon::PostConstruct()
{
  Enchantment = GetBaseEnchantment();
}


void meleeweapon::Save (outputfile &SaveFile) const {
  item::Save(SaveFile);
  SaveFile << Enchantment;
  SaveFile << SecondaryMaterial;
}


void meleeweapon::Load (inputfile &SaveFile) {
  item::Load(SaveFile);
  SaveFile >> Enchantment;
  LoadMaterial(SaveFile, SecondaryMaterial);
}


truth meleeweapon::HitEffect (character *Enemy, character *, v2, int BodyPartIndex, int, truth BlockedByArmour) {
  if (!BlockedByArmour && Fluid) {
    truth Success = false;
    fluidvector FluidVector;
    //
    FillFluidVector(FluidVector);
    for (uInt c = 0; c < FluidVector.size(); ++c) {
      if (FluidVector[c]->Exists() && FluidVector[c]->GetLiquid()->HitEffect(Enemy, Enemy->GetBodyPart(BodyPartIndex))) {
        Success = true;
      }
    }
    return Success;
  }
  return false;
}


void meleeweapon::DipInto (liquid *Liquid, character *Dipper) {
  if (Dipper->IsPlayer()) ADD_MESSAGE("%s is now covered with %s.", CHAR_NAME(DEFINITE), Liquid->GetName(false, false).CStr());
  SpillFluid(Dipper, Liquid);
  Dipper->DexterityAction(10);
}


sLong meleeweapon::GetPrice () const {
  double WeaponStrengthModifier = GetFormModifier() * GetMainMaterial()->GetStrengthValue();
  //
  WeaponStrengthModifier *= WeaponStrengthModifier;
  WeaponStrengthModifier *= GetMainMaterial()->GetWeight();
  WeaponStrengthModifier *= Max((10 + Enchantment) * 0.1, 0.1);
  return sLong(WeaponStrengthModifier / (20000000.0 * sqrt(GetWeight()))) + GetEnchantedPrice(Enchantment);
}


// never piled
void meleeweapon::AddInventoryEntry (ccharacter *Viewer, festring &Entry, int, truth ShowSpecialInfo) const {
  AddName(Entry, INDEFINITE);
  if (ShowSpecialInfo) {
    Entry << " [\1C" << GetWeight() << "g\2, DAM \1Y" << GetBaseMinDamage() << "\2-\1Y" << GetBaseMaxDamage() << "\2";
    Entry << ", " << GetBaseToHitValueDescription();
    if (!IsBroken()) Entry << ", " << GetStrengthValueDescription();
    //
    int CWeaponSkillLevel = Viewer->GetCWeaponSkillLevel(this);
    int SWeaponSkillLevel = Viewer->GetSWeaponSkillLevel(this);
    //
    if (CWeaponSkillLevel || SWeaponSkillLevel) Entry << ", skill \1C" << CWeaponSkillLevel << '/' << SWeaponSkillLevel << "\2";
    Entry << ']';
  }
}


void meleeweapon::SignalSpoil (material *Material) {
  if (!Exists()) return;
  if (Material == MainMaterial) {
    if (CanBeSeenByPlayer()) {
      if (SecondaryMaterial->GetVolume()) ADD_MESSAGE("The edge of %s spoils.", GetExtendedDescription().CStr());
      else ADD_MESSAGE("%s spoils.", GetExtendedDescription().CStr());
    }
    RemoveMainMaterial();
  } else {
    if (CanBeSeenByPlayer()) ADD_MESSAGE("The handle of %s spoils", GetExtendedDescription().CStr());
    delete RemoveSecondaryMaterial();
  }
}


void meleeweapon::AddPostFix (festring &String, int Case) const {
  item::AddPostFix(String, Case);
  if (Fluid) {
    String << " covered with ";
    fluid::AddFluidInfo(Fluid[0], String);
  }
       if (Enchantment > 0) String << " \1G+" << Enchantment << "\2";
  else if (Enchantment < 0) String << " \1R" << Enchantment << "\2";
}


void meleeweapon::Be () {
  item::Be();
  if (Exists() && SecondaryMaterial->GetVolume()) SecondaryMaterial->Be(ItemFlags);
}


int meleeweapon::GetSparkleFlags () const {
  return
    (MainMaterial->IsSparkling() ? SPARKLING_A|(SecondaryMaterial->GetVolume() ? SPARKLING_C : 0) : 0) |
    (SecondaryMaterial->IsSparkling() ? SPARKLING_B : 0);
}


void meleeweapon::SetEnchantment (int Amount) {
  Enchantment = Amount;
  SignalEnchantmentChange();
}


void meleeweapon::EditEnchantment (int Amount) {
  Enchantment += Amount;
  SignalEnchantmentChange();
}


int meleeweapon::GetStrengthValue () const {
  return Max<int>(sLong(GetStrengthModifier())*GetMainMaterial()->GetStrengthValue()/2000+Enchantment, 0);
}


int meleeweapon::GetSpoilLevel () const {
  int MainSpoilLevel = MainMaterial->GetSpoilLevel();
  //
  if (SecondaryMaterial->GetVolume()) return Max(MainSpoilLevel, SecondaryMaterial->GetSpoilLevel());
  return MainSpoilLevel;
}


void meleeweapon::GenerateMaterials () {
  int Chosen = RandomizeMaterialConfiguration();
  //
  const fearray<sLong> &MMC = GetMainMaterialConfig();
  InitMaterial(MainMaterial, MAKE_MATERIAL(MMC.Data[MMC.Size == 1 ? 0 : Chosen]), GetDefaultMainVolume());
  //
  const fearray<sLong> &SMC = GetSecondaryMaterialConfig();
  InitMaterial(SecondaryMaterial, MAKE_MATERIAL(SMC.Data[SMC.Size == 1 ? 0 : Chosen]), GetDefaultSecondaryVolume());
}


void meleeweapon::TryToRust (sLong LiquidModifier) {
  item::TryToRust(LiquidModifier);
  if (SecondaryMaterial->GetVolume() && SecondaryMaterial->TryToRust(LiquidModifier)) {
    SecondaryMaterial->SetRustLevel(SecondaryMaterial->GetRustLevel()+1);
  }
}


material *meleeweapon::GetConsumeMaterial (ccharacter *Consumer, materialpredicate Predicate) const {
  if ((SecondaryMaterial->*Predicate)() && SecondaryMaterial->GetVolume() && Consumer->CanConsume(SecondaryMaterial)) {
    return SecondaryMaterial;
  }
  return item::GetConsumeMaterial(Consumer, Predicate);
}


material *meleeweapon::RemoveMaterial (material *Material) {
  if (Material == MainMaterial) return RemoveMainMaterial();
  return RemoveSecondaryMaterial();
}


material *meleeweapon::RemoveMainMaterial () {
  truth Equipped = PLAYER->Equips(this);
  //
  if (SecondaryMaterial->GetVolume()) {
    item *Lump = SecondaryMaterial->CreateNaturalForm(SecondaryMaterial->GetVolume());
    //
    DonateFluidsTo(Lump);
    DonateIDTo(Lump);
    DonateSlotTo(Lump);
  } else {
    RemoveFromSlot();
  }
  //
  if (Equipped) game::AskForEscPress(CONST_S("Equipment destroyed!"));
  SendToHell();
  return 0;
}


material *meleeweapon::RemoveSecondaryMaterial () {
  SecondaryMaterial->SetVolume(0);
  //
  if (!IsBroken()) {
    Break(0);
  } else {
    RedistributeFluids();
    UpdatePictures();
    SendNewDrawAndMemorizedUpdateRequest();
  }
  return 0;
}


pixelpredicate meleeweapon::GetFluidPixelAllowedPredicate () const {
  if (SecondaryMaterial->GetVolume()) return &rawbitmap::IsTransparent;
  return &rawbitmap::IsMaterialColor1;
}


void meleeweapon::CalculateEmitation () {
  Emitation = GetBaseEmitation();
  //
  if (MainMaterial) game::CombineLights(Emitation, MainMaterial->GetEmitation());
  if (SecondaryMaterial->GetVolume()) game::CombineLights(Emitation, SecondaryMaterial->GetEmitation());
}


truth meleeweapon::CalculateHasBe () const {
  return
    LifeExpectancy || (MainMaterial && MainMaterial->HasBe()) ||
    (SecondaryMaterial && SecondaryMaterial->GetVolume() && SecondaryMaterial->HasBe());
}


item *meleeweapon::Fix () {
  SecondaryMaterial->SetVolumeNoSignals(GetDefaultSecondaryVolume());
  return item::Fix();
}


sLong meleeweapon::GetMaterialPrice () const {
  return MainMaterial->GetRawPrice()+SecondaryMaterial->GetRawPrice();
}


void meleeweapon::CalculateEnchantment () {
  Enchantment -= femath::LoopRoll(game::GetCurrentLevel()->GetEnchantmentMinusChance(), 5);
  Enchantment += femath::LoopRoll(game::GetCurrentLevel()->GetEnchantmentPlusChance(), 5);
  Enchantment -= femath::LoopRoll(GetEnchantmentMinusChance(), 5);
  Enchantment += femath::LoopRoll(GetEnchantmentPlusChance(), 5);
}


#endif
