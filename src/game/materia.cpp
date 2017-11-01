/*
 *
 *  Iter Vehemens ad Necem (IVAN)
 *  Copyright (C) Timo Kiviluoto
 *  Released under the GNU General
 *  Public License
 *
 *  See LICENSING which should be included
 *  along with this file for more details
 *
 */

/* Compiled through materset.cpp */

materialprototype::materialprototype(const materialprototype* Base,
             materialspawner Spawner,
             materialcloner Cloner,
             cchar* ClassID)
: Base(Base), Spawner(Spawner), Cloner(Cloner), ClassID(ClassID)
{ Index = protocontainer<material>::Add(this); }

sLong material::GetRawPrice() const
{ return GetPriceModifier() * GetWeight() / 10000; }
truth material::CanBeDug(material* ShovelMaterial) const
{ return ShovelMaterial->GetStrengthValue() > GetStrengthValue(); }
sLong material::GetTotalExplosivePower() const
{ return sLong(double(Volume) * GetExplosivePower() / 1000000); }
cchar* material::GetConsumeVerb() const { return "eating"; }

materialpredicate TrueMaterialPredicate = &material::True;

void material::AddName (festring &Name, truth Articled, truth Adjective) const {
  if (Articled) {
    if (GetNameFlags() & USE_AN) Name << "an "; else Name << "a ";
  }
  Name << (Adjective ? GetAdjectiveStem() : GetNameStem());
}

festring material::GetName(truth Articled, truth Adjective) const
{
  festring Name;
  Name.Empty();
  AddName(Name, Articled, Adjective);
  return Name;
}

material* material::TakeDipVolumeAway()
{
  if(Volume > 500)
  {
    EditVolume(-500);
    return SpawnMore(500);
  }
  else
    return MotherEntity->RemoveMaterial(this);
}

material *material::TakeAllVolumeAway () {
  return MotherEntity->RemoveMaterial(this);
}

void material::Save(outputfile& SaveFile) const
{
  SaveFile << Volume;
  SaveFile << (uShort)GetConfig();
}

void material::Load(inputfile& SaveFile)
{
  SaveFile >> Volume;
  databasecreator<material>::InstallDataBase(this, ReadType(uShort, SaveFile));
}


truth material::Effect (character *Char, int BodyPart, sLong Amount) {
  /* Receivexxx should return truth! */

  Amount = Amount*GetEffectStrength()/100;

  if (!Amount) return false;

  auto eff = GetEffect();

       if (eff == EFFECT_POISON) Char->BeginTemporaryState(POISONED, Amount);
  else if (eff == EFFECT_DARKNESS) Char->ReceiveDarkness(Amount);
  else if (eff == EFFECT_OMMEL_URINE) Char->ReceiveOmmelUrine(Amount);
  else if (eff == EFFECT_PEPSI) Char->ReceivePepsi(Amount);
  else if (eff == EFFECT_KOBOLD_FLESH) Char->ReceiveKoboldFlesh(Amount);
  else if (eff == EFFECT_HEAL) Char->ReceiveHeal(Amount);
  else if (eff == EFFECT_LYCANTHROPY) { if (!Char->StateIsActivated(DISEASE_IMMUNITY)) Char->BeginTemporaryState(LYCANTHROPY, Amount); }
  else if (eff == EFFECT_SCHOOL_FOOD) Char->ReceiveSchoolFood(Amount);
  else if (eff == EFFECT_ANTIDOTE) Char->ReceiveAntidote(Amount);
  else if (eff == EFFECT_CONFUSE) Char->BeginTemporaryState(CONFUSED, Amount);
  else if (eff == EFFECT_POLYMORPH) Char->BeginTemporaryState(POLYMORPH, Amount);
  else if (eff == EFFECT_ESP) Char->BeginTemporaryState(ESP, Amount);
  else if (eff == EFFECT_SKUNK_SMELL) Char->BeginTemporaryState(POISONED, Amount);
  else if (eff == EFFECT_MAGIC_MUSHROOM) {
    v2 Pos = GetMotherEntity()->GetSquareUnderEntity()->GetPos();
    Char->ActivateRandomState(SRC_MAGIC_MUSHROOM, Amount, Volume%250+Pos.X+Pos.Y+1);
  }
  else if (eff == EFFECT_TRAIN_PERCEPTION) Char->EditExperience(PERCEPTION, Amount, 1<<14);
  else if (eff == EFFECT_HOLY_BANANA) Char->ReceiveHolyBanana(Amount);
  else if (eff == EFFECT_EVIL_WONDER_STAFF_VAPOUR) {
    v2 Pos = GetMotherEntity()->GetSquareUnderEntity()->GetPos();
    Char->ActivateRandomState(SRC_EVIL, Amount, Volume%250+Pos.X+Pos.Y+1);
  }
  else if (eff == EFFECT_GOOD_WONDER_STAFF_VAPOUR) {
    v2 Pos = GetMotherEntity()->GetSquareUnderEntity()->GetPos();
    Char->ActivateRandomState(SRC_GOOD, Amount, Volume%250+Pos.X+Pos.Y+1);
  }
  else if (eff == EFFECT_PEA_SOUP) Char->ReceivePeaSoup(Amount);
  else if (eff == EFFECT_BLACK_UNICORN_FLESH) Char->ReceiveBlackUnicorn(Amount);
  else if (eff == EFFECT_GRAY_UNICORN_FLESH) Char->ReceiveGrayUnicorn(Amount);
  else if (eff == EFFECT_WHITE_UNICORN_FLESH) Char->ReceiveWhiteUnicorn(Amount);
  else if (eff == EFFECT_TELEPORT_CONTROL) Char->BeginTemporaryState(TELEPORT_CONTROL, Amount);
  else if (eff == EFFECT_MUSHROOM) {
    v2 Pos = GetMotherEntity()->GetSquareUnderEntity()->GetPos();
    Char->ActivateRandomState(SRC_MUSHROOM, Amount, Volume%250+Pos.X+Pos.Y+1);
  }
  else if (eff == EFFECT_OMMEL_CERUMEN) Char->ReceiveOmmelCerumen(Amount);
  else if (eff == EFFECT_OMMEL_SWEAT) Char->ReceiveOmmelSweat(Amount);
  else if (eff == EFFECT_OMMEL_TEARS) Char->ReceiveOmmelTears(Amount);
  else if (eff == EFFECT_OMMEL_SNOT) Char->ReceiveOmmelSnot(Amount);
  else if (eff == EFFECT_OMMEL_BONE) Char->ReceiveOmmelBone(Amount);
  else if (eff == EFFECT_MUSTARD_GAS) Char->ReceiveMustardGas(BodyPart, Amount);
  else if (eff == EFFECT_MUSTARD_GAS_LIQUID) Char->ReceiveMustardGasLiquid(BodyPart, Amount);
  else if (eff == EFFECT_PANIC) { if (!Char->StateIsActivated(FEARLESS)) Char->BeginTemporaryState(PANIC, Amount); }
  else if (eff == EFFECT_TELEPORT) Char->BeginTemporaryState(TELEPORT, Amount);
  else if (eff == EFFECT_VAMPIRISM) { if (!Char->StateIsActivated(DISEASE_IMMUNITY)) Char->BeginTemporaryState(VAMPIRISM, Amount); }
  else if (eff == EFFECT_DETECTING) Char->BeginTemporaryState(DETECTING, Amount);
  else return false;

  return true;
}

material* material::EatEffect(character* Eater, sLong Amount)
{
  Amount = Volume > Amount ? Amount : Volume;
  Eater->ReceiveNutrition(GetNutritionValue() * Amount / 50);
  if(Amount && Volume)
  {
    if(DisablesPanicWhenConsumed() && Eater->TemporaryStateIsActivated(PANIC))
    {
      if(Eater->IsPlayer())
      {
  ADD_MESSAGE("You relax a bit.");
      }
      else if(Eater->CanBeSeenByPlayer())
      {
  ADD_MESSAGE("%s relaxes a bit.", Eater->CHAR_NAME(DEFINITE));
      }
      Eater->DeActivateTemporaryState(PANIC);
    }
  }

  if(GetInteractionFlags() & AFFECT_INSIDE)
  {
    head* Head = Eater->GetVirtualHead();
    sLong NewAmount = Amount;

    if(Head && Amount >= 8)
    {
      Head->AddFluid(static_cast<liquid*>(SpawnMore(Amount >> 3)),
         CONST_S("throat"), 0, true);
      NewAmount -= Amount >> 3;
    }

    Eater->GetTorso()->AddFluid(static_cast<liquid*>(SpawnMore(NewAmount)),
        CONST_S("stomach"), 0, true);
  }
  else
  {
    Effect(Eater, TORSO_INDEX, Amount);

    if(IsLiquid())
      Eater->EditStamina(int(50. * Amount * Eater->GetMaxStamina()
           / Eater->GetBodyVolume()),
       false);
  }

  if(Volume != Amount)
  {
    EditVolume(-Amount);
    return 0;
  }
  else
    return MotherEntity->RemoveMaterial(this);
}

truth material::HitEffect(character* Enemy, bodypart* BodyPart)
{
  if(!Volume)
    return false;

  auto mtp = GetHitMessage();
       if (mtp == HM_SCHOOL_FOOD) Enemy->AddSchoolFoodHitMessage();
  else if (mtp == HM_FROG_FLESH) Enemy->AddFrogFleshConsumeEndMessage();
  else if (mtp == HM_OMMEL) Enemy->AddOmmelConsumeEndMessage();
  else if (mtp == HM_PEPSI) Enemy->AddPepsiConsumeEndMessage();
  else if (mtp == HM_KOBOLD_FLESH) Enemy->AddKoboldFleshHitMessage();
  else if (mtp == HM_HEALING_LIQUID) Enemy->AddHealingLiquidConsumeEndMessage();
  else if (mtp == HM_ANTIDOTE) Enemy->AddAntidoteConsumeEndMessage();
  else if (mtp == HM_CONFUSE) Enemy->AddConfuseHitMessage();
  else if (mtp == HM_HOLY_BANANA) Enemy->AddHolyBananaConsumeEndMessage();
  else if (mtp == HM_HOLY_MANGO) Enemy->AddHolyMangoConsumeEndMessage();
  else if (mtp == HM_ALIEN_FLESH) Enemy->AddAlienFleshConsumeEndMessage();

  sLong Amount = Max<sLong>(GetVolume() >> 1, 1);
  truth Success;

  if(GetInteractionFlags() & AFFECT_INSIDE)
  {
    if(BodyPart)
    {
      BodyPart->AddFluid(static_cast<liquid*>(SpawnMore(Amount)),
       CONST_S(""), 0, true);
      Success = true;
    }
    else
      Success = false;
  }
  else
  {
    int BPIndex = BodyPart ? BodyPart->GetBodyPartIndex() : NONE_INDEX;
    Success = Effect(Enemy, BPIndex, Amount);
  }

  if(Amount != Volume)
    EditVolume(-Amount);
  else
    delete MotherEntity->RemoveMaterial(this);

  return Success;
}


void material::AddConsumeEndMessage (character *Eater) const {
  auto mtp = GetConsumeEndMessage();
  if (mtp == CEM_SCHOOL_FOOD) { Eater->AddSchoolFoodConsumeEndMessage(); return; }
  if (mtp == CEM_BONE) { Eater->AddBoneConsumeEndMessage(); return; }
  if (mtp == CEM_FROG_FLESH) { Eater->AddFrogFleshConsumeEndMessage(); return; }
  if (mtp == CEM_OMMEL) { Eater->AddOmmelConsumeEndMessage(); return; }
  if (mtp == CEM_PEPSI) { Eater->AddPepsiConsumeEndMessage(); return; }
  if (mtp == CEM_KOBOLD_FLESH) { Eater->AddKoboldFleshConsumeEndMessage(); return; }
  if (mtp == CEM_HEALING_LIQUID) { Eater->AddHealingLiquidConsumeEndMessage(); return; }
  if (mtp == CEM_ANTIDOTE) { Eater->AddAntidoteConsumeEndMessage(); return; }
  if (mtp == CEM_ESP) { Eater->AddESPConsumeMessage(); return; }
  if (mtp == CEM_HOLY_BANANA) { Eater->AddHolyBananaConsumeEndMessage(); return; }
  if (mtp == CEM_PEA_SOUP) { Eater->AddPeaSoupConsumeEndMessage(); return; }
  if (mtp == CEM_BLACK_UNICORN_FLESH) { Eater->AddBlackUnicornConsumeEndMessage(); return; }
  if (mtp == CEM_GRAY_UNICORN_FLESH) { Eater->AddGrayUnicornConsumeEndMessage(); return; }
  if (mtp == CEM_WHITE_UNICORN_FLESH) { Eater->AddWhiteUnicornConsumeEndMessage(); return; }
  if (mtp == CEM_OMMEL_BONE) { Eater->AddOmmelBoneConsumeEndMessage(); return; }
  if (mtp == CEM_LIQUID_HORROR) { Eater->AddLiquidHorrorConsumeEndMessage(); return; }
  if (mtp == CEM_HOLY_MANGO) { Eater->AddHolyMangoConsumeEndMessage(); return; }
  if (mtp == CEM_ALIEN_FLESH) { Eater->AddAlienFleshConsumeEndMessage(); return; }
}


material *materialprototype::SpawnAndLoad (inputfile &SaveFile) const {
  material *Material = Spawner(0, 0, true);
  Material->Load(SaveFile);
  return Material;
}


material *material::MakeMaterial (int Config, sLong Volume) {
  if (!Config) return 0;

  auto cfg = (Config>>12);
  if (cfg == SOLID_ID >> 12) return solid::Spawn(Config, Volume);
  if (cfg == ORGANIC_ID >> 12) return organic::Spawn(Config, Volume);
  if (cfg == GAS_ID >> 12) return gas::Spawn(Config, Volume);
  if (cfg == LIQUID_ID >> 12) return liquid::Spawn(Config, Volume);
  if (cfg == FLESH_ID >> 12) return flesh::Spawn(Config, Volume);
  if (cfg == POWDER_ID >> 12) return powder::Spawn(Config, Volume);
  if (cfg == IRON_ALLOY_ID >> 12) return ironalloy::Spawn(Config, Volume);

  ABORT("Odd material configuration number %d of volume %d requested!", Config, Volume);
  return 0;
}

void material::SetVolume(sLong What)
{
  Volume = What;

  if(MotherEntity)
    MotherEntity->SignalVolumeAndWeightChange();
}

void material::Initialize(int NewConfig, sLong InitVolume, truth Load)
{
  if(!Load)
  {
    databasecreator<material>::InstallDataBase(this, NewConfig);
    Volume = InitVolume;
    PostConstruct();
  }
}

sLong material::GetTotalNutritionValue() const
{
  return GetNutritionValue() * GetVolume() / 50;
}

truth material::CanBeEatenByAI(ccharacter* Eater) const
{
  return ((Eater->GetAttribute(WISDOM) < GetConsumeWisdomLimit()
     || (Eater->IsAlcoholic() && (GetCategoryFlags() & IS_BEVERAGE)))
    && !GetSpoilLevel() && !Eater->CheckCannibalism(this));
}

truth material::BreatheEffect(character* Enemy)
{
  return Effect(Enemy, TORSO_INDEX, Max<sLong>(GetVolume() / 10, 50));
}


const materialdatabase* material::GetDataBase (int Config) {
  const prototype *Proto = 0;

  auto cfg = (Config >> 12);
       if (cfg == SOLID_ID >> 12) Proto = &solid::ProtoType;
  else if (cfg == ORGANIC_ID >> 12) Proto = &organic::ProtoType;
  else if (cfg == GAS_ID >> 12) Proto = &gas::ProtoType;
  else if (cfg == LIQUID_ID >> 12) Proto = &liquid::ProtoType;
  else if (cfg == FLESH_ID >> 12) Proto = &flesh::ProtoType;
  else if (cfg == POWDER_ID >> 12) Proto = &powder::ProtoType;
  else if (cfg == IRON_ALLOY_ID >> 12) Proto = &ironalloy::ProtoType;

  //FIXME: k8: check for nullptr `Proto`?

  const database *DataBase;
  databasecreator<material>::FindDataBase(DataBase, Proto, Config);

  if (DataBase) return DataBase;

  ABORT("Odd material configuration number %d requested!", Config);
  return 0;
}


void material::FinishConsuming(character* Consumer)
{
  if(!Consumer->IsPlayer() && GetConsumeWisdomLimit() != NO_LIMIT)
    Consumer->EditExperience(WISDOM, 150, 1 << 13); /** C **/

  AddConsumeEndMessage(Consumer);
}

void materialdatabase::InitDefaults (const materialprototype *NewProtoType, int NewConfig, cfestring &acfgstrname)
{
  ProtoType = NewProtoType;
  DigProductMaterial = Config = NewConfig;
  CommonFlags |= IS_ABSTRACT; // dummy value for configcontainer
  CfgStrName = acfgstrname;
}

item* material::CreateNaturalForm(int Config, sLong Volume)
{
  item* Item = GetDataBase(Config)->NaturalForm.Instantiate(NO_MATERIALS
                  |NO_PIC_UPDATE);
  Item->InitMaterials(MAKE_MATERIAL(Config, Volume));
  return Item;
}

item* material::CreateNaturalForm(sLong Volume) const
{
  item* Item = GetNaturalForm().Instantiate(NO_MATERIALS|NO_PIC_UPDATE);
  Item->InitMaterials(SpawnMore(Volume));
  return Item;
}


int material::GetHardenedMaterial (citem *Item) const {
  const materialdatabase *DB = DataBase;
  //
  if (!Item->FlexibilityIsEssential()) return DB->HardenedMaterial;
  while (DB->HardenedMaterial != NONE) {
    DB = material::GetDataBase(DB->HardenedMaterial);
    if (DataBase->Flexibility <= DB->Flexibility) return DB->Config;
  }
  return DB->HardenedMaterial;
}


int material::GetSoftenedMaterial (citem *Item) const {
  const materialdatabase *DB = DataBase;
  //
  if (!Item->FlexibilityIsEssential()) return DB->SoftenedMaterial;
  while (DB->SoftenedMaterial != NONE) {
    DB = material::GetDataBase(DB->SoftenedMaterial);
    if (DataBase->Flexibility <= DB->Flexibility) return DB->Config;
  }
  return DB->SoftenedMaterial;
}


int material::GetHardenModifier(citem* Item) const
{
  int M = GetFlexibility() << 2;

  if(!Item || !Item->FlexibilityIsEssential())
    M += GetStrengthValue();

  return M;
}

truth material::IsExplosive() const
{
  return DataBase->InteractionFlags & CAN_EXPLODE;
}

truth material::IsSparkling() const
{
  return DataBase->CategoryFlags & IS_SPARKLING;
}

truth material::IsStuckTo(ccharacter* Char) const
{
  return MotherEntity->IsStuckTo(Char);
}
