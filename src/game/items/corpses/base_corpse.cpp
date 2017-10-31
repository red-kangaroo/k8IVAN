#ifdef HEADER_PHASE
ITEM(corpse, item)
{
 public:
  corpse() { }
  corpse(const corpse&);
  virtual ~corpse();

  virtual int GetOfferValue(int) const;
  virtual double GetWeaponStrength() const;
  virtual truth CanBeEatenByAI(ccharacter*) const;
  virtual int GetStrengthValue() const;
  character* GetDeceased() const { return Deceased; }
  void SetDeceased(character*);
  virtual void Save(outputfile&) const;
  virtual void Load(inputfile&);
  virtual truth IsDestroyable(ccharacter*) const;
  virtual sLong GetTruePrice() const;
  virtual int GetMaterials() const { return 2; }
  virtual truth RaiseTheDead(character*);
  virtual void CalculateVolumeAndWeight();
  virtual void CalculateEmitation();
  virtual void SignalSpoil(material*);
  virtual truth CanBePiledWith(citem*, ccharacter*) const;
  virtual int GetSpoilLevel() const;
  virtual material* GetMaterial(int) const;
  virtual head* Behead();
  virtual truth CanBeCloned() const;
  virtual int GetAttachedGod() const;
  virtual void PreProcessForBone();
  virtual void PostProcessForBone();
  virtual void FinalProcessForBone();
  virtual truth SuckSoul(character*, character* = 0);
  virtual character* TryNecromancy(character*);
  virtual void Cannibalize();
  virtual material* GetConsumeMaterial(ccharacter*, materialpredicate) const;
  virtual truth DetectMaterial(cmaterial*) const;
  virtual void SetLifeExpectancy(int, int);
  virtual void Be();
  virtual void SignalDisappearance();
  virtual truth IsValuable() const;
  virtual truth AddRustLevelDescription(festring&, truth) const { return false; }
  virtual truth Necromancy(character*);
  virtual int GetSparkleFlags() const;
  virtual truth IsRusted() const { return false; }
  virtual truth CanBeHardened(ccharacter*) const { return false; }
  virtual truth IsCorpse () const { return true; }
 protected:
  virtual void GenerateMaterials() { }
  virtual col16 GetMaterialColorA(int) const;
  virtual col16 GetMaterialColorB(int) const;
  virtual alpha GetAlphaA(int) const;
  virtual alpha GetAlphaB(int) const;
  virtual truth ShowMaterial() const { return false; }
  virtual void AddPostFix(festring&, int) const;
  virtual v2 GetBitmapPos(int) const;
  virtual int GetSize() const;
  virtual int GetArticleMode() const;
  virtual int GetRustDataA() const;
  virtual truth AddStateDescription(festring&, truth) const;
  character* Deceased;
};


#else



head* corpse::Behead() { return Deceased->Behead(); }



truth corpse::CanBeCloned() const { return GetDeceased()->CanBeCloned(); }



int corpse::GetAttachedGod() const { return GetDeceased()->GetTorso()->GetAttachedGod(); }



void corpse::Save(outputfile& SaveFile) const
{
  item::Save(SaveFile);
  SaveFile << Deceased;
}



void corpse::Load(inputfile& SaveFile)
{
  item::Load(SaveFile);
  SaveFile >> Deceased;
  Deceased->SetMotherEntity(this);
  Enable();
}



void corpse::AddPostFix(festring& String, int) const
{
  String << " of ";
  GetDeceased()->AddName(String, INDEFINITE);
}



int corpse::GetOfferValue(int Receiver) const
{
  int OfferValue = 0;

  for(int c = 0; c < GetDeceased()->GetBodyParts(); ++c)
  {
    bodypart* BodyPart = GetDeceased()->GetBodyPart(c);

    if(BodyPart)
      OfferValue += BodyPart->GetOfferValue(Receiver);
  }

  return OfferValue;
}



double corpse::GetWeaponStrength() const
{
  return GetFormModifier() * GetDeceased()->GetTorso()->GetMainMaterial()->GetStrengthValue() * sqrt(GetDeceased()->GetTorso()->GetMainMaterial()->GetWeight());
}



truth corpse::CanBeEatenByAI(ccharacter* Eater) const
{
  for(int c = 0; c < GetDeceased()->GetBodyParts(); ++c)
  {
    bodypart* BodyPart = GetDeceased()->GetBodyPart(c);

    if(BodyPart && !BodyPart->CanBeEatenByAI(Eater))
      return false;
  }

  return true;
}



int corpse::GetStrengthValue() const
{
  return sLong(GetStrengthModifier()) * GetDeceased()->GetTorso()->GetMainMaterial()->GetStrengthValue() / 2000;
}



corpse::~corpse()
{
  delete Deceased;
}



col16 corpse::GetMaterialColorA(int) const
{
  return GetDeceased()->GetTorso()->GetMainMaterial()->GetColor();
}



alpha corpse::GetAlphaA(int) const
{
  return GetDeceased()->GetTorso()->GetMainMaterial()->GetAlpha();
}



col16 corpse::GetMaterialColorB(int) const
{
  torso* Torso = GetDeceased()->GetTorso();
  return Torso->IsAlive() ? material::GetDataBase(GetDeceased()->GetBloodMaterial())->Color : Torso->GetMainMaterial()->GetColor();
}



alpha corpse::GetAlphaB(int) const
{
  torso* Torso = GetDeceased()->GetTorso();
  return Torso->IsAlive() ? 175 : Torso->GetMainMaterial()->GetAlpha();
}



int corpse::GetSparkleFlags() const
{
  torso* Torso = GetDeceased()->GetTorso();
  material* Material = Torso->GetMainMaterial();
  return Material->IsSparkling() ? SPARKLING_A|(!Torso->IsAlive() ? SPARKLING_B : 0) : 0;
}



v2 corpse::GetBitmapPos(int) const
{
  if(GetDeceased()->GetSize() < 50)
    return v2(32, 64);
  else if(GetDeceased()->GetSize() < 150)
    return v2(16, 192);
  else
    return v2(16, 272);
}



int corpse::GetSize() const
{
  return GetDeceased()->GetSize();
}



void corpse::SetDeceased(character* What)
{
  Deceased = What;
  Deceased->SetMotherEntity(this);
  SignalVolumeAndWeightChange();
  SignalEmitationIncrease(Deceased->GetEmitation());
  UpdatePictures();
  Enable();
}



truth corpse::IsDestroyable(ccharacter* Char) const
{
  for(int c = 0; c < GetDeceased()->GetBodyParts(); ++c)
  {
    bodypart* BodyPart = GetDeceased()->GetBodyPart(c);

    if(BodyPart && !BodyPart->IsDestroyable(Char))
      return false;
  }

  return true;
}



sLong corpse::GetTruePrice() const
{
  sLong Price = 0;

  for(int c = 0; c < GetDeceased()->GetBodyParts(); ++c)
  {
    bodypart* BodyPart = GetDeceased()->GetBodyPart(c);

    if(BodyPart)
      Price += BodyPart->GetTruePrice();
  }

  return Price;
}



material* corpse::GetMaterial(int I) const
{
  return GetDeceased()->GetTorso()->GetMaterial(I);
}



truth corpse::RaiseTheDead(character* Summoner)
{
  if(Summoner && Summoner->IsPlayer())
    game::DoEvilDeed(50);

  GetDeceased()->Enable();

  if(GetDeceased()->TryToRiseFromTheDead())
  {
    v2 Pos = GetPos();
    RemoveFromSlot();
    GetDeceased()->SetMotherEntity(0);

    if(Summoner && GetDeceased()->CanTameWithResurrection(Summoner) && !GetDeceased()->IsPlayer())
      GetDeceased()->ChangeTeam(Summoner->GetTeam());

    GetDeceased()->PutToOrNear(Pos);
    GetDeceased()->SignalStepFrom(0);
    Deceased = 0;
    SendToHell();
    return true;
  }
  else
  {
    GetDeceased()->Disable();
    return false;
  }
}



void corpse::CalculateVolumeAndWeight()
{
  Volume = Deceased->GetVolume();
  Weight = Deceased->GetWeight();
}



void corpse::CalculateEmitation()
{
  Emitation = Deceased->GetEmitation();
}



corpse::corpse(const corpse& Corpse) : mybase(Corpse)
{
  Deceased = Corpse.Deceased->Duplicate();
  Deceased->SetMotherEntity(this);
}



void corpse::SignalSpoil(material*)
{
  GetDeceased()->Disappear(this, "spoil", &item::IsVeryCloseToSpoiling);
}



void corpse::SignalDisappearance()
{
  GetDeceased()->Disappear(this, "disappear", &item::IsVeryCloseToDisappearance);
}



truth corpse::CanBePiledWith(citem* Item, ccharacter* Viewer) const
{
  if(GetType() != Item->GetType()
     || GetConfig() != Item->GetConfig()
     || GetWeight() != Item->GetWeight()
     || Viewer->GetCWeaponSkillLevel(this) != Viewer->GetCWeaponSkillLevel(Item)
     || Viewer->GetSWeaponSkillLevel(this) != Viewer->GetSWeaponSkillLevel(Item))
    return false;

  const corpse* Corpse = static_cast<const corpse*>(Item);

  if(Deceased->GetBodyParts() != Corpse->Deceased->GetBodyParts())
    return false;

  for(int c = 0; c < Deceased->GetBodyParts(); ++c)
  {
    bodypart* BodyPart1 = Deceased->GetBodyPart(c);
    bodypart* BodyPart2 = Corpse->Deceased->GetBodyPart(c);

    if(!BodyPart1 && !BodyPart2)
      continue;

    if(!BodyPart1 || !BodyPart2 || !BodyPart1->CanBePiledWith(BodyPart2, Viewer))
      return false;
  }

  if(Deceased->GetName(UNARTICLED) != Corpse->Deceased->GetName(UNARTICLED))
    return false;

  return true;
}



int corpse::GetSpoilLevel() const
{
  int FlyAmount = 0;

  for(int c = 0; c < GetDeceased()->GetBodyParts(); ++c)
  {
    bodypart* BodyPart = GetDeceased()->GetBodyPart(c);

    if(BodyPart && FlyAmount < BodyPart->GetSpoilLevel())
      FlyAmount = BodyPart->GetSpoilLevel();
  }

  return FlyAmount;
}



int corpse::GetArticleMode() const
{
  return Deceased->LeftOversAreUnique() ? FORCE_THE : 0;
}



void corpse::PreProcessForBone()
{
  item::PreProcessForBone();

  if(!Deceased->PreProcessForBone())
  {
    RemoveFromSlot();
    SendToHell();
  }
}



void corpse::PostProcessForBone()
{
  item::PostProcessForBone();

  if(!Deceased->PostProcessForBone())
  {
    RemoveFromSlot();
    SendToHell();
  }
}



void corpse::FinalProcessForBone()
{
  item::FinalProcessForBone();
  Deceased->FinalProcessForBone();
}



truth corpse::SuckSoul(character* Soul, character* Summoner)
{
  v2 Pos = Soul->GetPos();

  if(Deceased->SuckSoul(Soul))
  {
    Soul->Remove();
    character* Deceased = GetDeceased();

    if(RaiseTheDead(Summoner))
    {
      Soul->SendToHell();
      return true;
    }
    else
    {
      Deceased->SetSoulID(Soul->GetID());
      Soul->PutTo(Pos);
      return false;
    }
  }
  else
    return false;
}



character* corpse::TryNecromancy(character* Summoner)
{
  if(Summoner && Summoner->IsPlayer())
    game::DoEvilDeed(50);

  character* Zombie = GetDeceased()->CreateZombie();

  if(Zombie)
  {
    v2 Pos = GetPos();
    RemoveFromSlot();
    Zombie->ChangeTeam(Summoner ? Summoner->GetTeam() : game::GetTeam(MONSTER_TEAM));
    Zombie->PutToOrNear(Pos);
    Zombie->SignalStepFrom(0);
    SendToHell();
    return Zombie;
  }

  return 0;
}



int corpse::GetRustDataA() const
{
  return Deceased->GetTorso()->GetMainMaterial()->GetRustData();
}



material* corpse::GetConsumeMaterial(ccharacter* Consumer, materialpredicate Predicate) const
{
  for(int c = GetDeceased()->GetBodyParts() - 1; c; --c)
  {
    bodypart* BodyPart = GetDeceased()->GetBodyPart(c);

    if(BodyPart)
    {
      material* CM = BodyPart->GetConsumeMaterial(Consumer, Predicate);

      if(CM)
  return CM;
    }
  }

  return GetDeceased()->GetTorso()->GetConsumeMaterial(Consumer, Predicate);
}



void corpse::Cannibalize()
{
  item::Cannibalize();

  for(int c = 0; c < GetDeceased()->GetBodyParts(); ++c)
  {
    bodypart* BodyPart = GetDeceased()->GetBodyPart(c);

    if(BodyPart)
      BodyPart->Cannibalize();
  }
}



truth corpse::DetectMaterial(const material* Material) const
{
  return GetDeceased()->DetectMaterial(Material);
}



void corpse::SetLifeExpectancy(int Base, int RandPlus)
{
  Deceased->SetLifeExpectancy(Base, RandPlus);
}



void corpse::Be()
{
  for(int c = 0; c < Deceased->GetBodyParts(); ++c)
  {
    bodypart* BodyPart = Deceased->GetBodyPart(c);

    if(BodyPart)
      BodyPart->Be();
  }
}



truth corpse::IsValuable() const
{
  for(int c = 0; c < Deceased->GetBodyParts(); ++c)
  {
    bodypart* BodyPart = Deceased->GetBodyPart(c);

    if(BodyPart && BodyPart->IsValuable())
      return true;
  }

  return false;
}



truth corpse::Necromancy(character* Necromancer)
{
  if(Necromancer && Necromancer->IsPlayer())
    game::DoEvilDeed(50);

  character* Zombie = GetDeceased()->CreateZombie();

  if(Zombie)
  {
    Zombie->ChangeTeam(Necromancer ? Necromancer->GetTeam() : game::GetTeam(MONSTER_TEAM));
    Zombie->PutToOrNear(GetPos());
    RemoveFromSlot();
    SendToHell();

    if(Zombie->CanBeSeenByPlayer())
      ADD_MESSAGE("%s rises back to cursed undead life.", Zombie->CHAR_DESCRIPTION(INDEFINITE));

    Zombie->SignalStepFrom(0);
    return true;
  }
  else
  {
    if(CanBeSeenByPlayer())
      ADD_MESSAGE("%s vibrates for some time.", CHAR_NAME(DEFINITE));

    return false;
  }
}


truth corpse::AddStateDescription (festring& Name, truth Articled) const {
  truth res = false;
  if (Spoils()) {
    truth Hasted = true, Slowed = true;
    for (int c = 0; c < GetDeceased()->GetBodyParts(); ++c) {
      bodypart* BodyPart = GetDeceased()->GetBodyPart(c);
      if (BodyPart) {
        if (!(BodyPart->ItemFlags & HASTE)) Hasted = false;
        if (!(BodyPart->ItemFlags & SLOW)) Slowed = false;
      }
    }
    if ((Hasted | Slowed) && Articled) { res = true; Name << "a "; }
    if (Hasted) { res = true; Name << "hasted "; }
    if (Slowed) { res = true; Name << "slowed "; }
  }
  return res;
}

#endif
