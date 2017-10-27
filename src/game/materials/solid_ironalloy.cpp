#ifdef HEADER_PHASE
MATERIAL(ironalloy, solid)
{
 public:
  ironalloy() : RustData(NOT_RUSTED) { }
  virtual void SetRustLevel(int);
  virtual int GetStrengthValue() const;
  virtual int GetRustLevel() const { return RustData & 3; }
  virtual truth IsSparkling() const;
  virtual void Save(outputfile&) const;
  virtual void Load(inputfile&);
  virtual int GetRustData() const { return RustData; }
  virtual truth TryToRust(sLong, sLong = 0);
  virtual truth AddRustLevelDescription(festring&, truth) const;
 protected:
  int RustData;
};


#else



truth ironalloy::IsSparkling() const { return material::IsSparkling() && GetRustLevel() == NOT_RUSTED; }



void ironalloy::SetRustLevel (int What) {
  if (GetRustLevel() != What) {
    if (!RustData) RustData = (RAND() & 0xFC) | What;
    else if (!What) RustData = 0;
    else RustData = (RustData & 0xFC) | What;
    if (MotherEntity) MotherEntity->SignalRustLevelChange();
  }
}



int ironalloy::GetStrengthValue() const
{
  int Base = material::GetStrengthValue();

  auto rlev = GetRustLevel();
  if (rlev == NOT_RUSTED) return Base;
  if (rlev == SLIGHTLY_RUSTED) return ((Base << 3) + Base) / 10;
  if (rlev == RUSTED) return ((Base << 1) + Base) >> 2;
  if (rlev == VERY_RUSTED) return Base >> 1;

  return 0; /* not possible */
}



truth ironalloy::AddRustLevelDescription(festring& Name, truth Articled) const
{
  if(GetRustLevel() == NOT_RUSTED)
    return false;

  if(Articled)
    Name << "a ";

  auto rlev = GetRustLevel();
  if (rlev == SLIGHTLY_RUSTED) Name << "slightly rusted ";
  if (rlev == RUSTED) Name << "rusted ";
  if (rlev == VERY_RUSTED) Name << "very rusted ";

  return true;
}



void ironalloy::Save(outputfile& SaveFile) const
{
  material::Save(SaveFile);
  SaveFile << RustData;
}



void ironalloy::Load(inputfile& SaveFile)
{
  material::Load(SaveFile);
  SaveFile >> RustData;
}



truth ironalloy::TryToRust(sLong Modifier, sLong Volume)
{
  if(GetRustLevel() != VERY_RUSTED)
  {
    if(!Volume)
      Volume = GetVolume();

    sLong Chance = sLong(30000000. * sqrt(Volume) / (Modifier * GetRustModifier()));

    if(Chance <= 1 || !(RAND() % Chance))
      return true;
  }

  return false;
}
#endif
