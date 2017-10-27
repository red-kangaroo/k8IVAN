#ifdef HEADER_PHASE
ITEM(holyhandgrenade, item)
{
 public:
  virtual truth Apply(character*);
  virtual truth IsAppliable(ccharacter*) const { return true; }
  virtual truth CalculateHasBe() const;
  virtual void Explode();
  virtual void Be();
  virtual void AddInventoryEntry (const character *, festring &, int, truth) const;
  virtual v2 GetBitmapPos(int) const;
  virtual int GetClassAnimationFrames() const;
  virtual alpha GetOutlineAlpha(int) const;
  virtual col16 GetOutlineColor(int) const;
  virtual void Save(outputfile&) const;
  virtual void Load(inputfile&);
  virtual void PreProcessForBone();
  virtual void PostConstruct();
  virtual truth AllowAlphaEverywhere() const { return true; }
  virtual col16 GetMaterialColorB(int) const;
  virtual bool WillExplodeSoon() const;
  virtual truth IsKamikazeWeapon(ccharacter*) const;
 protected:
  feuLong PinPulledTick;
  int Count;
  feuLong PinPullerID;
};


#else

truth holyhandgrenade::IsKamikazeWeapon (ccharacter *) const { return CalculateHasBe(); }


truth holyhandgrenade::Apply (character* Applier) {
  if (PinPulledTick) {
    ADD_MESSAGE("The pin of %s has already been removed.", CHAR_NAME(DEFINITE));
    return false;
  }

  if (Applier->IsPlayer()) {
    ADD_MESSAGE("You pull the pin off the grenade.");
  }

  PinPulledTick = game::GetTick();
  Count = 0;
  PinPullerID = Applier->GetID();
  Enable();
  UpdatePictures();
  return true;
}


truth holyhandgrenade::CalculateHasBe () const {
  return PinPulledTick;
}


void holyhandgrenade::Be () {
  item::Be();
  if (PinPulledTick && 3*(game::GetTick()-PinPulledTick) > (feuLong)(Count)*100) {
    ++Count;
    festring Msg = "A voice loudly declares: \"";
    switch (Count) {
      case 1: Msg << "ONE"; break;
      case 2: Msg << "TWO"; break;
      case 3: Msg << "THREE"; break;
    }
    Msg << "\".";
    ADD_MESSAGE("%s", Msg.CStr());
    if (Count == 3) Explode();
  }
}


void holyhandgrenade::AddInventoryEntry (const character *Viewer, festring& Entry, int, truth ShowSpecialInfo) const {
  // never piled
  AddName(Entry, DEFINITE);
  if (ShowSpecialInfo) {
    Entry << " [" << GetWeight() << "g";
    if (!!WillExplodeSoon()) Entry << ", " << "\1R(armed)\2";
    Entry << ']';
  }
}


void holyhandgrenade::Explode () {
  if (game::IsInWilderness()) {
    ADD_MESSAGE("You manage to dispose of %s.", CHAR_NAME(DEFINITE));
    RemoveFromSlot();
    SendToHell();
    return;
  }

  character* Damager = game::SearchCharacter(PinPullerID);
  festring DeathMsg = CONST_S("killed by an explosion of ");
  AddName(DeathMsg, INDEFINITE);

  if (Damager) DeathMsg << " caused @bk";

  if (GetSquareUnder()->CanBeSeenByPlayer(true)) {
    ADD_MESSAGE("%s explodes!", GetExtendedDescription().CStr());
  }

  lsquare* Square = GetLSquareUnder();
  RemoveFromSlot();
  SendToHell();
  Square->GetLevel()->Explosion(Damager, DeathMsg, Square->GetPos(), 300);
}


v2 holyhandgrenade::GetBitmapPos (int Frame) const {
  return (PinPulledTick ? v2(96, 64) : v2(96, 32));
}


int holyhandgrenade::GetClassAnimationFrames () const {
  return 32;
}


alpha holyhandgrenade::GetOutlineAlpha (int Frame) const {
  if (!PinPulledTick) return 0;
  Frame &= 31;
  return 50+(Frame*(31-Frame)>>1);
}


col16 holyhandgrenade::GetOutlineColor (int) const {
  return MakeRGB16(0, 255, 0);
}


void holyhandgrenade::Save (outputfile& SaveFile) const {
  item::Save(SaveFile);
  SaveFile << PinPulledTick << Count << PinPullerID;
}


void holyhandgrenade::Load (inputfile& SaveFile) {
  item::Load(SaveFile);
  SaveFile >> PinPulledTick >> Count >> PinPullerID;
}


void holyhandgrenade::PreProcessForBone () {
  if (PinPulledTick) {
    RemoveFromSlot();
    SendToHell();
  }
}


void holyhandgrenade::PostConstruct () {
  PinPulledTick = 0;
  Count = 0;
  PinPullerID = 0;
}


col16 holyhandgrenade::GetMaterialColorB (int) const {
  return MakeRGB16(200, 10, 10);
}


bool holyhandgrenade::WillExplodeSoon () const {
  return (PinPulledTick != 0);
}

#endif
