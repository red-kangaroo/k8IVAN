#ifdef HEADER_PHASE
MATERIAL(flesh, organic)
{
 public:
  virtual void Save(outputfile&) const;
  virtual void Load(inputfile&);
  virtual col16 GetSkinColor() const { return SkinColor; }
  virtual void SetSkinColor(int What) { SkinColor = What; }
  virtual truth SkinColorIsSparkling() const { return SkinColorSparkling; }
  virtual void SetSkinColorIsSparkling(truth What) { SkinColorSparkling = What; }
  virtual truth IsFlesh() const { return true; }
  virtual void SetIsInfectedByLeprosy(truth What) { InfectedByLeprosy = What; }
  virtual truth IsInfectedByLeprosy() const { return InfectedByLeprosy; }
 protected:
  virtual void PostConstruct();
  col16 SkinColor;
  truth SkinColorSparkling;
  truth InfectedByLeprosy;
};


#else



void flesh::PostConstruct()
{
  organic::PostConstruct();
  SkinColorSparkling = InfectedByLeprosy = false;
  SkinColor = GetColor();
}



void flesh::Save(outputfile& SaveFile) const
{
  organic::Save(SaveFile);
  SaveFile << SkinColor << SkinColorSparkling << InfectedByLeprosy;
}



void flesh::Load(inputfile& SaveFile)
{
  organic::Load(SaveFile);
  SaveFile >> SkinColor >> SkinColorSparkling >> InfectedByLeprosy;
}
#endif
