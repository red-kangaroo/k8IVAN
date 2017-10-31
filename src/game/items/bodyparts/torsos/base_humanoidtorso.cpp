#ifdef HEADER_PHASE
ITEM(humanoidtorso, torso)
{
 public:
  humanoidtorso();
  humanoidtorso(const humanoidtorso&);
  virtual ~humanoidtorso();

  virtual void Save(outputfile&) const;
  virtual void Load(inputfile&);
  virtual int GetTotalResistance(int) const;
  void SetBodyArmor(item* What) { BodyArmorSlot.PutInItem(What); }
  item* GetBodyArmor() const { return *BodyArmorSlot; }
  void SetCloak(item* What) { CloakSlot.PutInItem(What); }
  item* GetCloak() const { return *CloakSlot; }
  void SetBelt(item* What) { BeltSlot.PutInItem(What); }
  item* GetBelt() const { return *BeltSlot; }
  virtual void DropEquipment(stack* = 0);
  virtual item* GetEquipment(int) const;
  virtual int GetEquipments() const { return 3; }
  virtual void SignalEquipmentAdd(gearslot*);
  virtual void SignalVolumeAndWeightChange();
  virtual truth DamageArmor(character*, int, int);
  virtual item* GetArmorToReceiveFluid(truth) const;
 protected:
  void UpdateTorsoArmorPictures(graphicdata&, graphicdata&, graphicdata&) const;
  gearslot BodyArmorSlot;
  gearslot CloakSlot;
  gearslot BeltSlot;
};


#else



int humanoidtorso::GetTotalResistance(int Type) const
{
  if(Master)
  {
    int Resistance = GetResistance(Type) + Master->GetGlobalResistance(Type);

    if(GetBodyArmor())
      Resistance += GetBodyArmor()->GetResistance(Type);

    if(GetBelt())
      Resistance += GetBelt()->GetResistance(Type);

    return Resistance;
  }
  else
    return GetResistance(Type);
}



void humanoidtorso::Save(outputfile& SaveFile) const
{
  bodypart::Save(SaveFile);
  SaveFile << BodyArmorSlot << CloakSlot << BeltSlot;
}



void humanoidtorso::Load(inputfile& SaveFile)
{
  bodypart::Load(SaveFile);
  SaveFile >> BodyArmorSlot >> CloakSlot >> BeltSlot;
}



void humanoidtorso::DropEquipment(stack* Stack)
{
  if(Stack)
  {
    if(GetBodyArmor())
      GetBodyArmor()->MoveTo(Stack);

    if(GetCloak())
      GetCloak()->MoveTo(Stack);

    if(GetBelt())
      GetBelt()->MoveTo(Stack);
  }
  else
  {
    if(GetBodyArmor())
      GetSlot()->AddFriendItem(GetBodyArmor());

    if(GetCloak())
      GetSlot()->AddFriendItem(GetCloak());

    if(GetBelt())
      GetSlot()->AddFriendItem(GetBelt());
  }
}



humanoidtorso::~humanoidtorso()
{
  delete GetBodyArmor();
  delete GetCloak();
  delete GetBelt();
}



humanoidtorso::humanoidtorso()
{
  BodyArmorSlot.Init(this, BODY_ARMOR_INDEX);
  CloakSlot.Init(this, CLOAK_INDEX);
  BeltSlot.Init(this, BELT_INDEX);
}



item* humanoidtorso::GetEquipment(int I) const
{
  switch(I)
  {
   case 0: return GetBodyArmor();
   case 1: return GetCloak();
   case 2: return GetBelt();
  }

  return 0;
}



void humanoidtorso::SignalVolumeAndWeightChange()
{
  bodypart::SignalVolumeAndWeightChange();

  if(Master && !Master->IsInitializing())
  {
    humanoid* HumanoidMaster = GetHumanoidMaster();

    if(HumanoidMaster->GetRightArm())
      HumanoidMaster->GetRightArm()->CalculateAttributeBonuses();

    if(HumanoidMaster->GetLeftArm())
      HumanoidMaster->GetLeftArm()->CalculateAttributeBonuses();

    if(HumanoidMaster->GetRightLeg())
      HumanoidMaster->GetRightLeg()->CalculateAttributeBonuses();

    if(HumanoidMaster->GetLeftLeg())
      HumanoidMaster->GetLeftLeg()->CalculateAttributeBonuses();
  }
}



humanoidtorso::humanoidtorso(const humanoidtorso& Torso) : mybase(Torso)
{
  BodyArmorSlot.Init(this, BODY_ARMOR_INDEX);
  CloakSlot.Init(this, CLOAK_INDEX);
  BeltSlot.Init(this, BELT_INDEX);
}



void humanoidtorso::SignalEquipmentAdd(gearslot* Slot)
{
  if(!Master)
    return;

  humanoid* Master = GetHumanoidMaster();
  int EquipmentIndex = Slot->GetEquipmentIndex();

  if(!Master->IsInitializing()
     && (EquipmentIndex == CLOAK_INDEX || EquipmentIndex == BODY_ARMOR_INDEX))
  {
    item* Item = Slot->GetItem();

    if(Master->GetRightArm())
      Master->GetRightArm()->ApplyDexterityPenalty(Item);

    if(Master->GetLeftArm())
      Master->GetLeftArm()->ApplyDexterityPenalty(Item);

    if(Master->GetRightLeg())
      Master->GetRightLeg()->ApplyAgilityPenalty(Item);

    if(Master->GetLeftLeg())
      Master->GetLeftLeg()->ApplyAgilityPenalty(Item);
  }

  Master->SignalEquipmentAdd(EquipmentIndex);
}



truth humanoidtorso::DamageArmor(character* Damager, int Damage, int Type)
{
  sLong AV[3] = { 0, 0, 0 }, AVSum = 0;
  item* Armor[3];

  if((Armor[0] = GetBodyArmor()))
    AVSum += AV[0] = Max(Armor[0]->GetStrengthValue(), 1);

  if((Armor[1] = GetBelt()))
    AVSum += AV[1] = Max(Armor[1]->GetStrengthValue(), 1);

  if((Armor[2] = GetCloak()))
    AVSum += AV[2] = Max(Armor[2]->GetStrengthValue(), 1);

  return AVSum ? Armor[femath::WeightedRand(AV, AVSum)]->ReceiveDamage(Damager, Damage, Type) : false;
}



item* humanoidtorso::GetArmorToReceiveFluid(truth) const
{
  item* Cloak = GetCloak();

  if(Cloak && !(RAND() % 3))
    return Cloak;

  item* Belt = GetBelt();

  if(Belt && !(RAND() % 10))
    return Belt;

  item* BodyArmor = GetBodyArmor();
  return BodyArmor ? BodyArmor : 0;
}



void humanoidtorso::UpdateTorsoArmorPictures(graphicdata& TorsoArmorGraphicData, graphicdata& CloakGraphicData, graphicdata& BeltGraphicData) const
{
  if(!Master || !Master->PictureUpdatesAreForbidden())
  {
    UpdateArmorPicture(TorsoArmorGraphicData,
           GetBodyArmor(),
           ST_OTHER_BODYPART,
           &item::GetTorsoArmorBitmapPos,
           true);
    UpdateArmorPicture(CloakGraphicData,
           GetCloak(),
           ST_OTHER_BODYPART,
           &item::GetCloakBitmapPos);
    UpdateArmorPicture(BeltGraphicData,
           GetBelt(),
           ST_OTHER_BODYPART,
           &item::GetBeltBitmapPos);
  }
}
#endif
