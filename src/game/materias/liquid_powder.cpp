#ifdef HEADER_PHASE
MATERIAL(powder, liquid)
{
 public:
  powder() : Wetness(0) { }
  virtual truth IsPowder() const { return true; }
  virtual truth IsExplosive() const;
  virtual void AddWetness(sLong What) { Wetness += What; }
  virtual void Be(feuLong);
  virtual truth HasBe() const { return true; }
  virtual void Save(outputfile&) const;
  virtual void Load(inputfile&);
 protected:
  sLong Wetness;
};


#else



truth powder::IsExplosive() const { return !Wetness && material::IsExplosive(); }



void powder::Be(feuLong)
{
  if(Wetness > 0)
    --Wetness;
}



void powder::Save(outputfile& SaveFile) const
{
  material::Save(SaveFile);
  SaveFile << Wetness;
}



void powder::Load(inputfile& SaveFile)
{
  material::Load(SaveFile);
  SaveFile >> Wetness;
}
#endif
