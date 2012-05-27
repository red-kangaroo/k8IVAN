#ifdef HEADER_PHASE
MATERIAL(organic, solid)
{
 public:
  virtual void Be(feuLong);
  virtual truth HasBe() const { return true; }
  virtual void Save(outputfile&) const;
  virtual void Load(inputfile&);
  virtual truth IsVeryCloseToSpoiling() const { return SpoilLevel == 8; }
  virtual int GetSpoilLevel() const { return SpoilLevel; }
  virtual void ResetSpoiling();
  virtual material* EatEffect(character*, sLong);
  virtual void AddConsumeEndMessage(character*) const;
  virtual void SetSpoilCounter(int);
  virtual truth CanSpoil() const { return true; }
  virtual int GetSpoilPercentage() const;
  virtual truth Spoils() const { return true; }
 protected:
  virtual void PostConstruct();
  uShort SpoilCounter;
  uChar SpoilCheckCounter;
  uChar SpoilLevel;
};


#else



void organic::ResetSpoiling() { SpoilCounter = SpoilLevel = 0; }



void organic::Be(feuLong Flags)
{
  if(SpoilCheckCounter++ >= 50)
  {
    if(MotherEntity->AllowSpoil())
    {
      if(Flags & HASTE)
  SpoilCounter += 125;
      else if(Flags & SLOW)
  SpoilCounter += 5;
      else
  SpoilCounter += 25;

      if(SpoilCounter < GetSpoilModifier())
      {
  if(SpoilCounter << 1 >= GetSpoilModifier())
  {
    int NewSpoilLevel = ((SpoilCounter << 4) / GetSpoilModifier()) - 7;

    if(NewSpoilLevel != SpoilLevel)
    {
      SpoilLevel = NewSpoilLevel;
      MotherEntity->SignalSpoilLevelChange(this);
    }
  }
      }
      else
      {
  SpoilLevel = 8;
  MotherEntity->SignalSpoil(this);
      }
    }

    SpoilCheckCounter = 0;
  }
}



void organic::Save(outputfile& SaveFile) const
{
  material::Save(SaveFile);
  SaveFile << SpoilCounter << SpoilCheckCounter << SpoilLevel;
}



void organic::Load(inputfile& SaveFile)
{
  material::Load(SaveFile);
  SaveFile >> SpoilCounter >> SpoilCheckCounter >> SpoilLevel;
}



void organic::PostConstruct()
{
  SpoilLevel = SpoilCheckCounter = 0;
  SpoilCounter = (RAND() % GetSpoilModifier()) >> 5;
}



material* organic::EatEffect(character* Eater, sLong Amount)
{
  Amount = Volume > Amount ? Amount : Volume;
  GetMotherEntity()->SpecialEatEffect(Eater, Amount);
  Effect(Eater, TORSO_INDEX, Amount);
  Eater->ReceiveNutrition(GetNutritionValue() * Amount * 20 / (1000 * (GetSpoilLevel() + 1)));

  if(IsInfectedByLeprosy() && Amount && !RAND_N(25000 / Amount))
    Eater->GainIntrinsic(LEPROSY);

  if(GetSpoilLevel() > 0)
  {
    Eater->BeginTemporaryState(CONFUSED, int(Amount * GetSpoilLevel() * sqrt(GetNutritionValue()) / 1000));

    if(GetBodyFlags() & CAN_HAVE_PARASITE
       && !(RAND() % (250 / GetSpoilLevel())))
      Eater->GainIntrinsic(PARASITIZED);
  }

  if(GetSpoilLevel() > 4)
    Eater->BeginTemporaryState(POISONED, int(Amount * (GetSpoilLevel() - 4) * sqrt(GetNutritionValue()) / 1000));

  if(Volume != Amount)
  {
    EditVolume(-Amount);
    return 0;
  }
  else
    return MotherEntity->RemoveMaterial(this);
}



void organic::AddConsumeEndMessage(character* Eater) const
{
  if(Eater->IsPlayer())
  {
    if(GetSpoilLevel() > 0 && GetSpoilLevel() <= 4)
      ADD_MESSAGE("Ugh. This stuff was slightly spoiled.");
    else if(GetSpoilLevel() > 4)
      ADD_MESSAGE("Ugh. This stuff was terribly spoiled!");
  }

  material::AddConsumeEndMessage(Eater);
}



void organic::SetSpoilCounter(int What)
{
  SpoilCounter = What;

  if(SpoilCounter < GetSpoilModifier())
  {
    if(SpoilCounter << 1 >= GetSpoilModifier())
    {
      int NewSpoilLevel = ((SpoilCounter << 4) / GetSpoilModifier()) - 7;

      if(NewSpoilLevel != SpoilLevel)
      {
  SpoilLevel = NewSpoilLevel;
  MotherEntity->SignalSpoilLevelChange(this);
      }
    }
  }
  else
    MotherEntity->SignalSpoil(this);
}



int organic::GetSpoilPercentage() const
{
  return 100 * SpoilCounter / GetSpoilModifier();
}
#endif
