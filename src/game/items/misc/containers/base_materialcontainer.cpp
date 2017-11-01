#ifdef HEADER_PHASE
ITEM(materialcontainer, item)
{
 public:
  materialcontainer () {}
  materialcontainer (const materialcontainer &);
  virtual ~materialcontainer ();

  virtual material *GetSecondaryMaterial () const { return SecondaryMaterial; }
  virtual void SetSecondaryMaterial (material *, int = 0);
  virtual void ChangeSecondaryMaterial (material *, int = 0);
  void InitMaterials (material *, material *, truth = true);
  virtual void Save (outputfile &) const;
  virtual void Load (inputfile &);
  virtual int GetMaterials () const { return 2; }
  virtual void SignalSpoil (material *);
  virtual truth CanBePiledWith (citem *, ccharacter *) const;
  virtual void Be ();
  virtual int GetSpoilLevel () const;
  virtual material *GetMaterial (int) const;
  virtual int GetAttachedGod () const;
  virtual material *GetConsumeMaterial (ccharacter *, materialpredicate = TrueMaterialPredicate) const;
  virtual material *RemoveMaterial (material *);
  material *RemoveMainMaterial ();
  virtual material *RemoveSecondaryMaterial ();
  virtual void CalculateEmitation ();
  virtual void InitMaterials (const materialscript *, const materialscript *, truth);
  virtual int GetSparkleFlags () const;
  virtual truth DumpTo (character *dumper, v2 dest);
 protected:
  virtual sLong GetMaterialPrice () const;
  virtual truth CalculateHasBe () const;
  virtual void GenerateMaterials ();
  virtual col16 GetMaterialColorB (int) const;
  virtual alpha GetAlphaB (int) const;
  virtual int GetRustDataB () const;
  material *SecondaryMaterial;
};


#else


materialcontainer::~materialcontainer () {
  delete SecondaryMaterial;
}


materialcontainer::materialcontainer (const materialcontainer &MC) : mybase(MC) {
  CopyMaterial(MC.SecondaryMaterial, SecondaryMaterial);
}


void materialcontainer::SetSecondaryMaterial (material *What, int SpecialFlags) { SetMaterial(SecondaryMaterial, What, GetDefaultSecondaryVolume(), SpecialFlags); }
void materialcontainer::ChangeSecondaryMaterial(material *What, int SpecialFlags) { ChangeMaterial(SecondaryMaterial, What, GetDefaultSecondaryVolume(), SpecialFlags); }
void materialcontainer::InitMaterials (material *M1, material *M2, truth CUP) { ObjectInitMaterials(MainMaterial, M1, GetDefaultMainVolume(), SecondaryMaterial, M2, GetDefaultSecondaryVolume(), CUP); }
void materialcontainer::InitMaterials (const materialscript *M, const materialscript *C, truth CUP) { InitMaterials(M->Instantiate(), C->Instantiate(), CUP); }

material *materialcontainer::GetMaterial (int I) const { return (!I ? MainMaterial : SecondaryMaterial); }
col16 materialcontainer::GetMaterialColorB (int Frame) const { return (GetSecondaryMaterial() ? GetSecondaryMaterial()->GetColor() : GetMaterialColorA(Frame)); }
alpha materialcontainer::GetAlphaB (int Frame) const { return (GetSecondaryMaterial() && GetSecondaryMaterial()->GetAlpha() > GetAlphaA(Frame) ? GetSecondaryMaterial()->GetAlpha() : GetAlphaA(Frame)); }

int materialcontainer::GetSparkleFlags () const { return (MainMaterial->IsSparkling() ? SPARKLING_A : 0)|(SecondaryMaterial && SecondaryMaterial->IsSparkling() ? SPARKLING_B : 0); }
int materialcontainer::GetSpoilLevel () const { return Max(MainMaterial->GetSpoilLevel(), SecondaryMaterial ? SecondaryMaterial->GetSpoilLevel() : 0); }
int materialcontainer::GetAttachedGod () const { return (DataBase->AttachedGod ? DataBase->AttachedGod : SecondaryMaterial ? SecondaryMaterial->GetAttachedGod() : MainMaterial->GetAttachedGod()); }
int materialcontainer::GetRustDataB () const { return (SecondaryMaterial ? SecondaryMaterial->GetRustData() : GetRustDataA()); }


truth materialcontainer::DumpTo (character *dumper, v2 dest) {
  if (!dumper || (!IsCan() && !IsBottle()) || !GetSecondaryMaterial()) return false;
  if (!dumper->GetArea()->IsValidPos(dest)) return false;
  /*
  if (Item->IsOnGround()) {
    room *Room = Item->GetRoom();
    if (Room) Room->HostileAction(Char);
  }
  */
  lsquare *sqr = dumper->GetNearLSquare(dest);

  if (GetSecondaryMaterial()->IsLiquid()) {
    liquid *Liquid = static_cast<liquid *>(RemoveSecondaryMaterial());
    if (Liquid->GetVolume() > 0) {
      if (dumper->IsPlayer()) ADD_MESSAGE("Dumping %s.", Liquid->GetName(false, false).CStr());
      sqr->SpillFluid(dumper, Liquid, false, true); // ForceHit, ShowMsg
    }
  } else {
    item *Lump = lump::Spawn(0, NO_MATERIALS);
    Lump->InitMaterials(RemoveSecondaryMaterial());
    sqr->AddItem(Lump);
  }
  dumper->DexterityAction(10);
  return true;
}


void materialcontainer::Save (outputfile &SaveFile) const {
  item::Save(SaveFile);
  SaveFile << SecondaryMaterial;
}


void materialcontainer::Load (inputfile &SaveFile) {
  item::Load(SaveFile);
  LoadMaterial(SaveFile, SecondaryMaterial);
}


void materialcontainer::GenerateMaterials () {
  int Chosen = RandomizeMaterialConfiguration();
  const fearray<sLong>& MMC = GetMainMaterialConfig();
  InitMaterial(MainMaterial, MAKE_MATERIAL(MMC.Data[MMC.Size == 1 ? 0 : Chosen]), GetDefaultMainVolume());
  const fearray<sLong>& SMC = GetSecondaryMaterialConfig();
  InitMaterial(SecondaryMaterial, MAKE_MATERIAL(SMC.Data[SMC.Size == 1 ? 0 : Chosen]), GetDefaultSecondaryVolume());
}


void materialcontainer::SignalSpoil (material *Material) {
  if (!Exists()) return;
  if (Material == MainMaterial) {
    if (CanBeSeenByPlayer()) ADD_MESSAGE("%s becomes so spoiled that it cannot hold its contents anymore.", CHAR_NAME(DEFINITE));
    RemoveMainMaterial();
  } else {
    if (CanBeSeenByPlayer()) ADD_MESSAGE("The contents of %s spoil completely.", CHAR_NAME(DEFINITE));
    delete RemoveSecondaryMaterial();
  }
}


truth materialcontainer::CanBePiledWith (citem *Item, ccharacter *Viewer) const {
  if (!item::CanBePiledWith(Item, Viewer)) return false;

  const materialcontainer *Weapon = static_cast<const materialcontainer *>(Item);

  if (!SecondaryMaterial && !Weapon->SecondaryMaterial) return true;

  return
    SecondaryMaterial &&
    Weapon->SecondaryMaterial &&
    SecondaryMaterial->IsSameAs(Weapon->SecondaryMaterial) &&
    SecondaryMaterial->GetSpoilLevel() == Weapon->SecondaryMaterial->GetSpoilLevel();
}


void materialcontainer::Be () {
  item::Be();
  if (Exists() && SecondaryMaterial) SecondaryMaterial->Be(ItemFlags);
}


material *materialcontainer::GetConsumeMaterial (ccharacter *Consumer, materialpredicate Predicate) const {
  if (SecondaryMaterial && (SecondaryMaterial->*Predicate)() && Consumer->CanConsume(SecondaryMaterial)) return SecondaryMaterial;
  return item::GetConsumeMaterial(Consumer, Predicate);
}


material *materialcontainer::RemoveMaterial (material *Material) {
  return (Material == MainMaterial ? RemoveMainMaterial() : RemoveSecondaryMaterial());
}


material *materialcontainer::RemoveMainMaterial () {
  truth Equipped = PLAYER->Equips(this);

  if (!SecondaryMaterial) {
    RemoveFromSlot();
  } else if (SecondaryMaterial->IsLiquid()) {
    if (!game::IsInWilderness()) {
      lsquare *Square = GetLSquareUnder();
      RemoveFromSlot();
      Square->SpillFluid(0, static_cast<liquid*>(SecondaryMaterial));
      SetSecondaryMaterial(0, NO_PIC_UPDATE|NO_SIGNALS);
    } else {
      RemoveFromSlot();
    }
  } else {
    item *Lump = lump::Spawn(0, NO_MATERIALS);
    Lump->InitMaterials(SecondaryMaterial);
    DonateFluidsTo(Lump);
    DonateIDTo(Lump);
    DonateSlotTo(Lump);
    SetSecondaryMaterial(0, NO_PIC_UPDATE|NO_SIGNALS);
  }

  SendToHell();

  if (Equipped) game::AskForEscPress(CONST_S("Equipment destroyed!"));

  return 0;
}


material *materialcontainer::RemoveSecondaryMaterial () {
  material *Material = SecondaryMaterial;
  SetSecondaryMaterial(0);
  SendNewDrawAndMemorizedUpdateRequest();
  return Material;
}


void materialcontainer::CalculateEmitation () {
  Emitation = GetBaseEmitation();
  if (MainMaterial) game::CombineLights(Emitation, MainMaterial->GetEmitation());
  if (SecondaryMaterial) game::CombineLights(Emitation, SecondaryMaterial->GetEmitation());
}


truth materialcontainer::CalculateHasBe () const {
  return
    LifeExpectancy ||
    (MainMaterial && MainMaterial->HasBe()) ||
    (SecondaryMaterial && SecondaryMaterial->HasBe());
}


sLong materialcontainer::GetMaterialPrice () const {
  return MainMaterial->GetRawPrice()+(SecondaryMaterial ? SecondaryMaterial->GetRawPrice() : 0);
}

#endif
