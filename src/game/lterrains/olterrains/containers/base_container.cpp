#ifdef HEADER_PHASE
OLTERRAIN(olterraincontainer, olterrain)
{
public:
  olterraincontainer ();
  virtual ~olterraincontainer ();

  virtual truth Open (character *);
  virtual truth CanBeOpened () const;
  virtual stack *GetContained () const;
  virtual void Load (inputfile &);
  virtual void Save (outputfile &) const;
  virtual void SetItemsInside (const fearray<contentscript<item> > &, int);
  virtual void Break ();
  virtual truth AllowContentEmitation () const;
  virtual void PreProcessForBone ();
  virtual void PostProcessForBone ();
  virtual void FinalProcessForBone ();

protected:
  stack *Contained;
};


#else


truth olterraincontainer::CanBeOpened () const { return true; }
stack *olterraincontainer::GetContained () const { return Contained; }
truth olterraincontainer::AllowContentEmitation () const { return false; }


olterraincontainer::olterraincontainer () {
  Contained = new stack(0, this, HIDDEN);
}


void olterraincontainer::Save (outputfile &SaveFile) const {
  olterrain::Save(SaveFile);
  Contained->Save(SaveFile);
}


void olterraincontainer::Load (inputfile &SaveFile) {
  olterrain::Load(SaveFile);
  Contained->Load(SaveFile);
}


truth olterraincontainer::Open (character *Opener) {
  if (!Opener->IsPlayer()) return false;
  truth Success = false;
  switch (game::KeyQuestion(CONST_S("Do you want to \1Gt\2ake something from or \1Gp\2ut something in this container?"), /*KEY_ESC*/REQUIRES_ANSWER, 5, 't', 'p', 'T', 'P', KEY_ESC)) {
    case 't': case 'T': Success = GetContained()->TakeSomethingFrom(Opener, GetName(DEFINITE)); break;
    case 'p': case 'P': Success = GetContained()->PutSomethingIn(Opener, GetName(DEFINITE), GetStorageVolume(), 0); break;
    default: return false;
  }
  if (Success) Opener->DexterityAction(Opener->OpenMultiplier()*5);
  return Success;
}


void olterraincontainer::SetItemsInside (const fearray<contentscript<item> > &ItemArray, int SpecialFlags) {
  GetContained()->Clean();
  for (uInt c1 = 0; c1 < ItemArray.Size; ++c1) {
    if (ItemArray[c1].IsValid()) {
      const interval *TimesPtr = ItemArray[c1].GetTimes();
      int Times = TimesPtr ? TimesPtr->Randomize() : 1;
      for (int c2 = 0; c2 < Times; ++c2) {
        item *Item = ItemArray[c1].Instantiate(SpecialFlags);
        if (Item) {
          Contained->AddItem(Item);
          Item->SpecialGenerationHandler();
        }
      }
    }
  }
}


olterraincontainer::~olterraincontainer () {
  delete Contained;
}


void olterraincontainer::Break () {
  GetContained()->MoveItemsTo(GetLSquareUnder()->GetStack());
  olterrain::Break();
}


void olterraincontainer::PreProcessForBone () {
  olterrain::PreProcessForBone();
  Contained->PreProcessForBone();
}


void olterraincontainer::PostProcessForBone () {
  olterrain::PostProcessForBone();
  Contained->PostProcessForBone();
}


void olterraincontainer::FinalProcessForBone () {
  olterrain::FinalProcessForBone();
  Contained->FinalProcessForBone();
}


#endif
