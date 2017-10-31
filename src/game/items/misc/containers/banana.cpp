#ifdef HEADER_PHASE
ITEM(banana, materialcontainer)
{
 public:
  banana() : TimesUsed(0), Charges(6), Jammed(false) { }
  virtual truth Zap(character*, v2, int);
  virtual void Save(outputfile&) const;
  virtual void Load(inputfile&);
  virtual void ChargeFully(character*) { TimesUsed = 0; }
  virtual truth IsZappable(ccharacter*) const { return true; }
  virtual truth IsChargeable(ccharacter*) const { return true; }
  virtual void SignalSpoil(material*);
  virtual truth IsBanana() const { return true; }
  virtual material* RemoveSecondaryMaterial();
 protected:
  int TimesUsed;
  int Charges;
  truth Jammed;
};


#else



void banana::Save(outputfile& SaveFile) const
{
  materialcontainer::Save(SaveFile);
  SaveFile << TimesUsed << Charges << Jammed;
}



void banana::Load(inputfile& SaveFile)
{
  materialcontainer::Load(SaveFile);
  SaveFile >> TimesUsed >> Charges >> Jammed;
}



truth banana::Zap(character*, v2, int)
{
  if(IsBroken())
  {
    ADD_MESSAGE("This banana seems to be somehow disfunctional.");
    return false;
  }

  if(Jammed)
  {
    ADD_MESSAGE("Unfortunately, your banana is jammed!");
    return false;
  }

  if(Charges > TimesUsed)
  {
    if(TimesUsed && !RAND_N(10))
    {
      ADD_MESSAGE("Oh no! Your banana jams in the middle of the firefight!");
      Jammed = true;
    }
    else
    {
      ADD_MESSAGE("BANG! You zap %s!", CHAR_NAME(DEFINITE));
      ++TimesUsed;
    }
  }
  else
    ADD_MESSAGE("Click!");

  return true;
}



void banana::SignalSpoil(material* Material)
{
  if(!Exists())
    return;

  if(Material == GetSecondaryMaterial() && !GetMainMaterial()->IsVeryCloseToSpoiling())
  {
    if(CanBeSeenByPlayer())
      ADD_MESSAGE("The inside of %s spoils completely.", GetExtendedDescription().CStr());

    delete RemoveSecondaryMaterial();
  }
  else
    item::SignalSpoil(Material);
}



material* banana::RemoveSecondaryMaterial()
{
  item* Peel = bananapeels::Spawn(0, NO_MATERIALS);
  Peel->InitMaterials(MainMaterial);
  DonateSlotTo(Peel);
  DonateIDTo(Peel);
  SetMainMaterial(0, NO_PIC_UPDATE|NO_SIGNALS);
  SendToHell();
  return 0;
}
#endif
