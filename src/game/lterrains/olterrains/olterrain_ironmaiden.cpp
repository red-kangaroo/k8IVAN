#ifdef HEADER_PHASE
OLTERRAIN(ironmaiden, olterrain)
{
public:
  ironmaiden () : Opened(false) {}

  virtual void Save (outputfile &) const;
  virtual void Load (inputfile &);
  virtual truth Open (character *);
  virtual truth CanBeOpened () const;
  virtual truth CanBeClosed () const;
  virtual truth Close (character *);

protected:
  virtual v2 GetBitmapPos (int) const;

protected:
  truth Opened;
};


#else


truth ironmaiden::CanBeOpened () const { return !Opened; }
truth ironmaiden::CanBeClosed () const { return Opened; }


void ironmaiden::Save (outputfile &SaveFile) const {
  olterrain::Save(SaveFile);
  SaveFile << Opened;
}


void ironmaiden::Load (inputfile &SaveFile) {
  olterrain::Load(SaveFile);
  SaveFile >> Opened;
}


v2 ironmaiden::GetBitmapPos (int) const {
  return Opened ? v2(48,64) : v2(32,64);
}


truth ironmaiden::Open (character *Opener) {
  if (!Opened) {
    truth WasSeenByPlayer = CanBeSeenByPlayer();
    Opened = true;
    UpdatePictures();
    GetLSquareUnder()->SendNewDrawRequest();
    GetLSquareUnder()->SendMemorizedUpdateRequest();
    if (Opener->IsPlayer()) ADD_MESSAGE("You open %s.", CHAR_NAME(DEFINITE));
    else if (WasSeenByPlayer) {
      if (Opener->CanBeSeenByPlayer()) ADD_MESSAGE("%s opens %s.", Opener->CHAR_NAME(DEFINITE), CHAR_NAME(DEFINITE));
      else ADD_MESSAGE("Something opens %s.", CHAR_NAME(DEFINITE));
    }
    return true;
  } else {
    if (Opener->IsPlayer()) ADD_MESSAGE("%s is already open, %s.", CHAR_NAME(DEFINITE), game::Insult());
    return false;
  }
  Opener->DexterityAction(Opener->OpenMultiplier()*5);
  return true;
}


truth ironmaiden::Close (character *Closer) {
  if (Closer->IsPlayer()) {
    if (Opened) ADD_MESSAGE("You close %s.", CHAR_NAME(DEFINITE));
    else {
      ADD_MESSAGE("%s is already closed, %s.", CHAR_NAME(DEFINITE),
      game::Insult());
      return false;
    }
  }
  Opened = false;
  UpdatePictures();
  Closer->DexterityAction(Closer->OpenMultiplier()*5);
  return true;
}


#endif
