#ifdef HEADER_PHASE
CHARACTER(playerkind, humanoid)
{
 public:
  playerkind();
  playerkind(const playerkind&);
  virtual void Save(outputfile&) const;
  virtual void Load(inputfile&);
  virtual void SetSoulID(feuLong);
  virtual truth SuckSoul(character*);
  virtual truth TryToRiseFromTheDead();
  virtual void FinalProcessForBone();
  virtual void BeTalkedTo();
  virtual truth IsHuman() const { return true; }
  virtual col16 GetHairColor() const { return HairColor; }
  virtual col16 GetEyeColor() const { return EyeColor; }
  virtual v2 GetHeadBitmapPos() const;
  virtual v2 GetRightArmBitmapPos() const;
  virtual v2 GetLeftArmBitmapPos() const;
  virtual int GetNaturalSparkleFlags() const;
  virtual truth IsPlayerKind() const { return true; }
  virtual double GetNaturalExperience(int) const;
 protected:
  virtual bodypart* MakeBodyPart(int) const;
  virtual void PostConstruct();
  feuLong SoulID;
  col16 HairColor;
  col16 EyeColor;
  int Talent;
  int Weakness;
  truth IsBonePlayer;
  truth IsClone;
};


#else


static cint TalentOfAttribute[ATTRIBUTES] = { TALENT_HEALTHY, TALENT_FAST_N_ACCURATE, TALENT_CLEVER, TALENT_CLEVER, TALENT_CLEVER, TALENT_CLEVER, TALENT_CLEVER, TALENT_STRONG, TALENT_STRONG, TALENT_FAST_N_ACCURATE, TALENT_FAST_N_ACCURATE };
static const double TalentBonusOfAttribute[ATTRIBUTES] = { 1.1, 1.25, 1.25, 1.25, 1.25, 1.25, 1.25, 1.25, 1.25, 1.25, 1.25 };


playerkind::playerkind() : SoulID(0), IsBonePlayer(false), IsClone(false)
{
}



void playerkind::Save(outputfile& SaveFile) const
{
  humanoid::Save(SaveFile);
  SaveFile << SoulID << HairColor << EyeColor << Talent << Weakness << IsBonePlayer << IsClone;
}



void playerkind::Load(inputfile& SaveFile)
{
  humanoid::Load(SaveFile);
  SaveFile >> SoulID >> HairColor >> EyeColor >> Talent >> Weakness >> IsBonePlayer >> IsClone;
}



void playerkind::SetSoulID(feuLong What)
{
  SoulID = What;

  if(GetPolymorphBackup())
    GetPolymorphBackup()->SetSoulID(What);
}



truth playerkind::SuckSoul(character* Soul)
{
  if(Soul->GetID() == SoulID)
  {
    SoulID = 0;
    return true;
  }

  return false;
}



truth playerkind::TryToRiseFromTheDead()
{
  if(humanoid::TryToRiseFromTheDead())
  {
    if(IsEnabled() && SoulID)
    {
      ADD_MESSAGE("The soulless body of %s wobbles for a moment.", CHAR_NAME(DEFINITE));
      return false;
    }

    return true;
  }
  else
    return false;
}



void playerkind::FinalProcessForBone()
{
  humanoid::FinalProcessForBone();
  IsBonePlayer = true;

  if(SoulID)
  {
    boneidmap::iterator BI = game::GetBoneCharacterIDMap().find(SoulID);

    if(BI != game::GetBoneCharacterIDMap().end())
      SoulID = BI->second;
    else
      SoulID = 0;
  }
}



playerkind::playerkind(const playerkind& Char) : mybase(Char), SoulID(Char.SoulID), HairColor(Char.HairColor), EyeColor(Char.EyeColor), Talent(Char.Talent), Weakness(Char.Weakness), IsBonePlayer(Char.IsBonePlayer), IsClone(true)
{
}



void playerkind::BeTalkedTo()
{
  if(IsClone && IsBonePlayer)
  {
    if(GetRelation(PLAYER) == HOSTILE)
    {
      ADD_MESSAGE("Oh no, you too! Why does everyone bully me!");
      return;
    }

    static sLong Said;

    switch(RandomizeReply(Said, 4))
    {
     case 0:
      ADD_MESSAGE("\"I'd like to write a memoir, but alas I doubt anyone would believe it.\"");
      break;
     case 1:
      ADD_MESSAGE("\"Then that damned clone appeared, took all my equipment and claimed I was his slave...\"");
      break;
     case 2:
      ADD_MESSAGE("\"The level was a catastrophe for the party, but luckily you saved the day.\"");
      break;
     case 3:
      ADD_MESSAGE("\"Oh, how I hate bananas. I Hate Them! I HATE THEM SO MUCH!!!\"");
      break;
    }
  }
  else if(IsClone)
  {
    if(GetRelation(PLAYER) == HOSTILE)
    {
      ADD_MESSAGE("%s seems extremely irritated. \"Vanish, you foul mirror image!\"", CHAR_DESCRIPTION(DEFINITE));
      return;
    }

    static sLong Said;

    switch(RandomizeReply(Said, 4))
    {
     case 0:
      ADD_MESSAGE("\"Hey, those clothes are mine! Give them back!\"");
      break;
     case 1:
      ADD_MESSAGE("\"What, you summoned me? What a coincidence, I remember summoning you, too.\"");
      break;
     case 2:
      ADD_MESSAGE("\"I'm leading this party, not you, Mr. copy guy!\"");
      break;
     case 3:
      ADD_MESSAGE("\"Oh, how I hate bananas. I Hate Them! I HATE THEM SO MUCH!!!\"");
      break;
    }
  }
  else
  {
    if(GetRelation(PLAYER) == HOSTILE)
    {
      ADD_MESSAGE("Let's finish what my ghost failed to do!");
      return;
    }

    static sLong Said;

    switch(RandomizeReply(Said, 4))
    {
     case 0:
      ADD_MESSAGE("\"What was it like? Death, you mean? Well, just like New Attnam. Very hot and whips everywhere.\"");
      break;
     case 1:
      ADD_MESSAGE("\"Stop it already! I *don't* want to know how my corpse smelled!\"");
      break;
     case 2:
      ADD_MESSAGE("\"I'm sorry about that ghost thing. That YASD was just a bit too much to handle, so I lost myself.\"");
      break;
     case 3:
      ADD_MESSAGE("\"Oh, how I hate bananas. I Hate Them! I HATE THEM SO MUCH!!!\"");
      break;
    }
  }
}



bodypart* playerkind::MakeBodyPart(int I) const
{
  switch(I)
  {
   case TORSO_INDEX: return playerkindtorso::Spawn(0, NO_MATERIALS);
   case HEAD_INDEX: return playerkindhead::Spawn(0, NO_MATERIALS);
   case RIGHT_ARM_INDEX: return playerkindrightarm::Spawn(0, NO_MATERIALS);
   case LEFT_ARM_INDEX: return playerkindleftarm::Spawn(0, NO_MATERIALS);
   case GROIN_INDEX: return playerkindgroin::Spawn(0, NO_MATERIALS);
   case RIGHT_LEG_INDEX: return playerkindrightleg::Spawn(0, NO_MATERIALS);
   case LEFT_LEG_INDEX: return playerkindleftleg::Spawn(0, NO_MATERIALS);
  }

  ABORT("Weird bodypart to make for a playerkind. It must be your fault!");
  return 0;
}



void playerkind::PostConstruct()
{
  int R = 0, G = 0, B = 0;

  switch(RAND_N(4))
  {
   case 0: R = 195; G = 165; B = 40; break;
   case 1: R = 130; G = 30; B = 0; break;
   case 2: R = 30; G = 30; B = 15; break;
   case 3: R = 50; G = 30; B = 5; break;
  }

  HairColor = MakeRGB16(R + RAND_N(41), G + RAND_N(41), B + RAND_N(41));

  switch(RAND_N(4))
  {
   case 0: R = 25; G = 0; B = 70; break;
   case 1: R = 5; G = 0; B = 50; break;
   case 2: R = 10; G = 10; B = 10; break;
   case 3: R = 60; G = 20; B = 0; break;
  }

  EyeColor = MakeRGB16(R + RAND_N(41), G + RAND_N(41), B + RAND_N(41));
  Talent = RAND_N(TALENTS);
  Weakness = RAND_N(TALENTS);
}



v2 playerkind::GetHeadBitmapPos() const
{
  int Sum = GetAttribute(INTELLIGENCE, false) + GetAttribute(WISDOM, false);

  if(Sum >= 60)
    return v2(96, 480);
  else if(Sum >= 40)
    return v2(96, 464);
  else
    return v2(96, 416);
}



v2 playerkind::GetRightArmBitmapPos() const
{
  if(GetRightArm()->GetAttribute(ARM_STRENGTH, false) >= 20)
    return v2(64, 448);
  else
    return v2(64, 416);
}



v2 playerkind::GetLeftArmBitmapPos() const
{
  if(GetLeftArm()->GetAttribute(ARM_STRENGTH, false) >= 20)
    return v2(64, 448);
  else
    return v2(64, 416);
}



int playerkind::GetNaturalSparkleFlags() const
{
  return GetAttribute(CHARISMA) >= 30 ? SKIN_COLOR : 0;
}



double playerkind::GetNaturalExperience(int Identifier) const
{
  double NE = DataBase->NaturalExperience[Identifier];

  if(Talent == TalentOfAttribute[Identifier])
    NE *= TalentBonusOfAttribute[Identifier];

  if(Weakness == TalentOfAttribute[Identifier])
    NE /= TalentBonusOfAttribute[Identifier];

  return NE;
}
#endif
