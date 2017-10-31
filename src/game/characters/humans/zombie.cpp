#ifdef HEADER_PHASE
CHARACTER(zombie, humanoid)
{
 public:
  virtual void BeTalkedTo();
  virtual truth BodyPartIsVital(int) const;
  virtual void CreateBodyParts(int);
  virtual truth AllowSpoil() const { return true; }
  void SetDescription(cfestring What) { Description = What; }
  virtual void Save(outputfile&) const;
  virtual void Load(inputfile&);
  virtual festring GetZombieDescription() const;
 protected:
  virtual void PostConstruct();
  virtual void AddPostFix(festring&, int) const;
  virtual void GetAICommand();
  virtual truth AllowExperience() const { return false; }
  festring Description;
};


#else



truth zombie::BodyPartIsVital(int I) const { return I == GROIN_INDEX || I == TORSO_INDEX; }



festring zombie::GetZombieDescription() const { return Description; }



void zombie::BeTalkedTo()
{
  if(GetRelation(PLAYER) == HOSTILE && PLAYER->GetAttribute(INTELLIGENCE) > 5)
  {
    if(RAND() % 5)
    {
      if(GetHead())
  ADD_MESSAGE("\"Need brain!!\"");
      else
  ADD_MESSAGE("\"Need head with brain!!\"");
    }
    else
      ADD_MESSAGE("\"Redrum! Redrum! Redrum!\"");
  }
  else
    ADD_MESSAGE("\"Need brain but you too stoopid!\"");
}



void zombie::CreateBodyParts(int SpecialFlags)
{
  bool Anyway = false;
  if(GetConfig() == ZOMBIE_OF_KHAZ_ZADM)
  {
    Anyway = true;
  } // Khaz-Zadm needs his hands...

  for(int c = 0; c < BodyParts; ++c)
    if(Anyway || BodyPartIsVital(c) || RAND_N(3) || (c == HEAD_INDEX && !RAND_N(3)))
    {
      bodypart* BodyPart = CreateBodyPart(c, SpecialFlags|NO_PIC_UPDATE);
      BodyPart->GetMainMaterial()->SetSpoilCounter(2000 + RAND_N(1000));
    }
}



void zombie::GetAICommand()
{
  if(!GetHead())
  {
    for(stackiterator i = GetLSquareUnder()->GetStack()->GetBottom(); i.HasItem(); ++i)
    {
      head* Head = i->Behead();

      if(Head)
      {
  if(CanBeSeenByPlayer())
    ADD_MESSAGE("%s takes %s and attaches it to its torso.", CHAR_NAME(DEFINITE), Head->CHAR_NAME(INDEFINITE));

  Head->RemoveFromSlot();
  AttachBodyPart(Head);
  Head->SetHP(1);
  DexterityAction(10);
  return;
      }
    }
  }

  humanoid::GetAICommand();
}



void zombie::AddPostFix(festring& String, int Case) const
{
  if(!Description.IsEmpty())
    String << Description;
  else
    humanoid::AddPostFix(String, Case);
}



void zombie::Save(outputfile& SaveFile) const
{
  humanoid::Save(SaveFile);
  SaveFile << Description;
}



void zombie::Load(inputfile& SaveFile)
{
  humanoid::Load(SaveFile);
  SaveFile >> Description;
}



void zombie::PostConstruct()
{
  if(!RAND_N(3))
    GainIntrinsic(LEPROSY);
}
#endif
