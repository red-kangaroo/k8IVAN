#ifdef HEADER_PHASE
ITEM(itemcontainer, lockableitem)
{
 public:
  itemcontainer();
  itemcontainer(const itemcontainer&);
  virtual ~itemcontainer();

  virtual truth Open(character*);
  virtual void Load(inputfile&);
  virtual void Save(outputfile&) const;
  virtual truth Polymorph(character*, stack*);
  virtual void CalculateVolumeAndWeight();
  virtual truth ContentsCanBeSeenBy(ccharacter*) const;
  virtual sLong GetTruePrice() const;
  virtual truth ReceiveDamage(character*, int, int, int);
  virtual void DrawContents(ccharacter*);
  virtual truth Apply(character* Applier) { return Open(Applier); }
  virtual truth IsAppliable(ccharacter*) const { return true; }
  virtual void SetItemsInside(const fearray<contentscript<item> >&, int);
  virtual truth AllowContentEmitation() const { return false; }
  virtual truth IsDestroyable(ccharacter*) const;
  virtual int GetOfferValue(int) const;
  virtual void SortAllItems(const sortdata&) const;
  virtual void PreProcessForBone();
  virtual void PostProcessForBone();
  virtual void FinalProcessForBone();
  virtual material* RemoveMaterial(material*);
  virtual void SetLifeExpectancy(int, int);
  virtual void CalculateEnchantment();
  virtual int GetTeleportPriority() const;
  virtual void SetParameters(int);
  virtual void Disappear();
  virtual stack* GetContained() const { return Contained; }
 protected:
  virtual col16 GetMaterialColorB(int) const;
  virtual void PostConstruct();
  stack* Contained;
};


#else



col16 itemcontainer::GetMaterialColorB(int) const { return MakeRGB16(80, 80, 80); }



itemcontainer::itemcontainer()
{
  Contained = new stack(0, this, HIDDEN);
}



void itemcontainer::PostConstruct()
{
  lockableitem::PostConstruct();
  SetIsLocked(RAND_N(3));
  sLong ItemNumber = RAND() % (GetMaxGeneratedContainedItems() + 1);

  for(int c = 0; c < ItemNumber; ++c)
  {
    item* NewItem = protosystem::BalancedCreateItem();
    sLong Volume = NewItem->GetVolume();

    if(NewItem->HandleInPairs())
      Volume <<= 1;

    if(NewItem->CanBeGeneratedInContainer()
       && (GetStorageVolume() - GetContained()->GetVolume()) >= Volume)
    {
      GetContained()->AddItem(NewItem);
      NewItem->SpecialGenerationHandler();
    }
    else
      delete NewItem;
  }
}



truth itemcontainer::Open(character* Opener)
{
  if(IsLocked())
  {
    ADD_MESSAGE("%s seems to be locked.", CHAR_NAME(DEFINITE));
    return false;
  }

  festring Question = CONST_S("Do you want to (t)ake something from or (p)ut something in this container? [t,p]");
  truth Success;

  switch(game::KeyQuestion(Question, KEY_ESC, 3, 't', 'p', KEY_ESC))
  {
   case 't':
   case 'T':
    Success = GetContained()->TakeSomethingFrom(Opener, GetName(DEFINITE));
    break;
   case 'p':
   case 'P':
    Success = GetContained()->PutSomethingIn(Opener, GetName(DEFINITE), GetStorageVolume(), GetID());
    break;
   default:
    return false;
  }

  if(Success)
    Opener->DexterityAction(Opener->OpenMultiplier() * 5);

  return Success;
}



void itemcontainer::Save(outputfile& SaveFile) const
{
  lockableitem::Save(SaveFile);
  Contained->Save(SaveFile);}



void itemcontainer::Load(inputfile& SaveFile)
{
  lockableitem::Load(SaveFile);
  Contained->Load(SaveFile);
}



truth itemcontainer::Polymorph(character* Polymorpher, stack* CurrentStack)
{
  GetContained()->MoveItemsTo(CurrentStack);
  item::Polymorph(Polymorpher, CurrentStack);
  return true;
}



itemcontainer::~itemcontainer()
{
  delete Contained;
}



void itemcontainer::CalculateVolumeAndWeight()
{
  item::CalculateVolumeAndWeight();
  Volume += Contained->GetVolume();
  Weight += Contained->GetWeight();
}



truth itemcontainer::ContentsCanBeSeenBy(ccharacter* Viewer) const
{
  return GetMainMaterial()->IsTransparent() && CanBeSeenBy(Viewer);
}



itemcontainer::itemcontainer(const itemcontainer& Container) : mybase(Container)
{
  Contained = new stack(0, this, HIDDEN);
  CalculateAll();
}



sLong itemcontainer::GetTruePrice() const
{
  return GetContained()->GetTruePrice() + item::GetTruePrice();
}



truth itemcontainer::ReceiveDamage (character *Damager, int Damage, int Type, int) {
  if (Type & (PHYSICAL_DAMAGE|SOUND|ENERGY)) {
    Contained->ReceiveDamage(Damager, Damage/GetDamageDivider(), Type);
    int SV = Max(GetStrengthValue(), 1);
    if (IsLocked() && Damage > SV && RAND()%(100*Damage/SV) >= 100) {
      SetIsLocked(false);
      SetConfig((GetConfig()&~LOCK_BITS)|BROKEN_LOCK);
      if (CanBeSeenByPlayer()) ADD_MESSAGE("The %s's lock shatters to pieces.", GetNameSingular().CStr());
      return true;
    }
    if (Damager && Damager->IsPlayer()) ADD_MESSAGE("THUMP!");
  }
  return false;
}



void itemcontainer::DrawContents(ccharacter* Char)
{
  festring Topic = CONST_S("Contents of your ") + GetName(UNARTICLED);
  GetContained()->DrawContents(Char, Topic, NO_SELECT);

  for(stackiterator i = GetContained()->GetBottom(); i.HasItem(); ++i)
    i->DrawContents(Char);
}



void itemcontainer::SetItemsInside(const fearray<contentscript<item> >& ItemArray, int SpecialFlags)
{
  GetContained()->Clean();

  for(uInt c1 = 0; c1 < ItemArray.Size; ++c1)
    if(ItemArray[c1].IsValid())
    {
      const interval* TimesPtr = ItemArray[c1].GetTimes();
      int Times = TimesPtr ? TimesPtr->Randomize() : 1;

      for(int c2 = 0; c2 < Times; ++c2)
      {
  item* Item = ItemArray[c1].Instantiate(SpecialFlags);

  if(Item)
  {
    Contained->AddItem(Item);
    Item->SpecialGenerationHandler();
  }
      }
    }
}



int itemcontainer::GetOfferValue(int Receiver) const
{
  int Sum = 0;

  for(int c = 0; c < GetContained()->GetItems(); ++c)
    Sum += GetContained()->GetItem(c)->GetOfferValue(Receiver);

  return item::GetOfferValue(Receiver) + Sum;
}



truth itemcontainer::IsDestroyable(ccharacter* Char) const
{
  for(int c = 0; c < GetContained()->GetItems(); ++c)
    if(!GetContained()->GetItem(c)->IsDestroyable(Char))
      return false;

  return true;
}



void itemcontainer::SortAllItems(const sortdata& SortData) const
{
  item::SortAllItems(SortData);

  if(SortData.Recurse)
    GetContained()->SortAllItems(SortData);
}



void itemcontainer::PreProcessForBone()
{
  item::PreProcessForBone();
  Contained->PreProcessForBone();
}



void itemcontainer::PostProcessForBone()
{
  item::PostProcessForBone();
  Contained->PostProcessForBone();
}



void itemcontainer::FinalProcessForBone()
{
  item::FinalProcessForBone();
  Contained->FinalProcessForBone();
}



material* itemcontainer::RemoveMaterial(material* Material)
{
  Contained->MoveItemsTo(GetSlot());
  return item::RemoveMaterial(Material);
}



void itemcontainer::SetLifeExpectancy(int Base, int RandPlus)
{
  LifeExpectancy = RandPlus > 1 ? Base + RAND_N(RandPlus) : Base;
  Enable();
  Contained->SetLifeExpectancy(Base, RandPlus);
}



void itemcontainer::CalculateEnchantment()
{
  Contained->CalculateEnchantments();
}



int itemcontainer::GetTeleportPriority() const
{
  sLong Priority = item::GetTeleportPriority();

  for(stackiterator i = Contained->GetBottom(); i.HasItem(); ++i)
    Priority += i->GetTeleportPriority();

  return Priority;
}



void itemcontainer::SetParameters(int Param)
{
  SetIsLocked(Param & LOCKED);
}



void itemcontainer::Disappear()
{
  Contained->MoveItemsTo(GetSlot());
  item::Disappear();
}
#endif
