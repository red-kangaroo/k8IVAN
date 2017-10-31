#ifdef HEADER_PHASE
ITEM(decosadshirt, bodyarmor)
{
 public:
  decosadshirt();
  virtual void Be();
  virtual void Save(outputfile&) const;
  virtual void Load(inputfile&);
  feuLong GetEquippedTicks() { return EquippedTicks; }
  void SetEquippedTicks(feuLong What) { EquippedTicks = What; }
  virtual truth IsDecosAdShirt(ccharacter*) const { return true; }
 protected:
  virtual truth CalculateHasBe() const { return true; }
  feuLong EquippedTicks;
};


#else



void decosadshirt::Be()
{
  if(PLAYER->Equips(this))
    ++EquippedTicks;

  bodyarmor::Be();
}



decosadshirt::decosadshirt() : EquippedTicks(0)
{
  Enable();
}



void decosadshirt::Save(outputfile& SaveFile) const
{
  bodyarmor::Save(SaveFile);
  SaveFile << EquippedTicks;
}



void decosadshirt::Load(inputfile& SaveFile)
{
  bodyarmor::Load(SaveFile);
  SaveFile >> EquippedTicks;
  Enable();
}
#endif
