#ifdef HEADER_PHASE
ITEM(head, bodypart)
{
 public:
  head();
  head(const head&);
  virtual ~head();

  virtual void Save(outputfile&) const;
  virtual void Load(inputfile&);
  virtual int GetTotalResistance(int) const;
  void SetHelmet(item* What) { HelmetSlot.PutInItem(What); }
  item* GetHelmet() const { return *HelmetSlot; }
  void SetAmulet(item* What) { AmuletSlot.PutInItem(What); }
  item* GetAmulet() const { return *AmuletSlot; }
  virtual void DropEquipment(stack* = 0);
  virtual int GetBodyPartIndex() const;
  double GetBiteDamage() const { return BiteDamage; }
  int GetBiteMinDamage() const;
  int GetBiteMaxDamage() const;
  double GetBiteToHitValue() const { return BiteToHitValue; }
  sLong GetBiteAPCost() const { return BiteAPCost; }
  virtual void InitSpecialAttributes();
  virtual item* GetEquipment(int) const;
  virtual int GetEquipments() const { return 2; }
  int GetBaseBiteStrength() const { return BaseBiteStrength; }
  void SetBaseBiteStrength(sLong What) { BaseBiteStrength = What; }
  virtual void CalculateDamage();
  virtual void CalculateToHitValue();
  virtual void CalculateAPCost();
  virtual truth DamageArmor(character*, int, int);
  virtual head* Behead();
  virtual item* GetArmorToReceiveFluid(truth) const;
  virtual void SignalPossibleUsabilityChange();
 protected:
  void UpdateHeadArmorPictures(graphicdata&) const;
  gearslot HelmetSlot;
  gearslot AmuletSlot;
  int BaseBiteStrength;
  double BiteToHitValue;
  double BiteDamage;
  sLong BiteAPCost;
};


#else


head::head () {
  HelmetSlot.Init(this, HELMET_INDEX);
  AmuletSlot.Init(this, AMULET_INDEX);
}


head::head (const head& Head) : mybase(Head), BaseBiteStrength(Head.BaseBiteStrength) {
  HelmetSlot.Init(this, HELMET_INDEX);
  AmuletSlot.Init(this, AMULET_INDEX);
}


head::~head () {
  delete GetHelmet();
  delete GetAmulet();
}


void head::InitSpecialAttributes () { BaseBiteStrength = Master->GetBaseBiteStrength(); }

int head::GetBodyPartIndex() const { return HEAD_INDEX; }
int head::GetBiteMinDamage() const { return int(BiteDamage * 0.75); }
int head::GetBiteMaxDamage() const { return int(BiteDamage * 1.25 + 1); }


int head::GetTotalResistance (int Type) const {
  if (Master) {
    int Resistance = GetResistance(Type) + Master->GetGlobalResistance(Type);
    if (GetHelmet()) Resistance += GetHelmet()->GetResistance(Type);
    if (GetExternalBodyArmor()) Resistance += GetExternalBodyArmor()->GetResistance(Type) >> 2;
    return Resistance;
  }
  return GetResistance(Type);
}


void head::Save (outputfile& SaveFile) const {
  bodypart::Save(SaveFile);
  SaveFile << (int)BaseBiteStrength;
  SaveFile << HelmetSlot << AmuletSlot;
}


void head::Load (inputfile& SaveFile) {
  bodypart::Load(SaveFile);
  SaveFile >> (int&)BaseBiteStrength;
  SaveFile >> HelmetSlot >> AmuletSlot;
}


void head::CalculateDamage () {
  if (!Master) return;
  BiteDamage = 7.07e-6 * GetBaseBiteStrength() * GetHumanoidMaster()->GetCWeaponSkill(BITE)->GetBonus();
}


void head::CalculateToHitValue () {
  if (!Master) return;
  BiteToHitValue = Master->GetAttribute(AGILITY) * sqrt(2.5 * Master->GetAttribute(PERCEPTION)) * GetHumanoidMaster()->GetCWeaponSkill(KICK)->GetBonus() * Master->GetMoveEase() / 1000000;
}


void head::CalculateAPCost () {
  if (!Master) return;
  BiteAPCost = Max(sLong(10000000000. / (APBonus(Master->GetAttribute(AGILITY)) * Master->GetMoveEase() * Master->GetCWeaponSkill(BITE)->GetBonus())), 100);
}


void head::DropEquipment (stack* Stack) {
  if (Stack) {
    if (GetHelmet()) GetHelmet()->MoveTo(Stack);
    if (GetAmulet()) GetAmulet()->MoveTo(Stack);
  } else {
    if (GetHelmet()) GetSlot()->AddFriendItem(GetHelmet());
    if (GetAmulet()) GetSlot()->AddFriendItem(GetAmulet());
  }
}


item *head::GetEquipment (int I) const {
  switch (I) {
    case 0: return GetHelmet();
    case 1: return GetAmulet();
  }
  return 0;
}


truth head::DamageArmor (character *Damager, int Damage, int Type) {
  sLong AV[3] = { 0, 0, 0 }, AVSum = 0;
  item *Armor[3];
  if ((Armor[0] = GetHelmet())) AVSum += AV[0] = Max(Armor[0]->GetStrengthValue(), 1);
  if ((Armor[1] = GetExternalBodyArmor())) AVSum += AV[1] = Max(Armor[1]->GetStrengthValue() >> 2, 1);
  if ((Armor[2] = GetExternalCloak())) AVSum += AV[2] = Max(Armor[2]->GetStrengthValue(), 1);
  return (AVSum ? Armor[femath::WeightedRand(AV, AVSum)]->ReceiveDamage(Damager, Damage, Type) : false);
}


head *head::Behead () {
  RemoveFromSlot();
  return this;
}


item *head::GetArmorToReceiveFluid (truth) const {
  item *Helmet = GetHelmet();
  if (Helmet && Helmet->GetCoverPercentile() > RAND() % 100) return Helmet;
  return 0;
}


void head::UpdateHeadArmorPictures (graphicdata &HelmetGraphicData) const {
  if (!Master || !Master->PictureUpdatesAreForbidden()) {
    UpdateArmorPicture(HelmetGraphicData, GetHelmet(), ST_OTHER_BODYPART, &item::GetHelmetBitmapPos);
  }
}


void head::SignalPossibleUsabilityChange () {
  feuLong OldFlags = Flags;
  UpdateFlags();
  if (!Master->IsInitializing() && HP > 0 && Flags & BADLY_HURT && !(OldFlags & BADLY_HURT)) {
    switch (RAND_N(8)) {
      case 0: case 1: case 2: Master->LoseConsciousness(50 + RAND_N(50)); break;
      case 3: case 4: case 5: Master->BeginTemporaryState(CONFUSED, 500 + RAND_N(500)); break;
      case 6:
        if (Master->IsPlayer() && !RAND_N(3)) {
          if (RAND_N(5)) {
            ADD_MESSAGE("Your memory becomes blurred.");
            GetLevel()->Amnesia(25 + RAND_N(50));
            Master->EditExperience(INTELLIGENCE, -80, 1 << 13);
            game::SendLOSUpdateRequest();
          } else {
            ADD_MESSAGE("A terrible concussion garbles your consciousness.");
            Master->BeginTemporaryState(CONFUSED, 5000 + RAND_N(5000));
            Master->EditExperience(INTELLIGENCE, -100, 1 << 14);
            GetLevel()->BlurMemory();
            game::SendLOSUpdateRequest();
          }
        } else {
          Master->EditExperience(INTELLIGENCE, -60, 1 << 12);
        }
        break;
      case 7:
        Master->ForgetRandomThing();
        break;
    }
  }
}

#endif
