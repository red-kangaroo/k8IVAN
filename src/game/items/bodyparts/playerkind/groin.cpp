#ifdef HEADER_PHASE
ITEM(playerkindgroin, groin)
{
 public:
  playerkindgroin() { }
  playerkindgroin(const playerkindgroin& Groin) : mybase(Groin) { }
  virtual void Save(outputfile&) const;
  virtual void Load(inputfile&);
  virtual truth UpdateArmorPictures();
  virtual void DrawArmor(blitdata&) const;
  virtual truth ShowFluids() const { return true; }
  virtual truth IsAnimated() const { return true; }
 protected:
  graphicdata GroinArmorGraphicData;
};


#else



truth playerkindgroin::UpdateArmorPictures()
{
  UpdateGroinArmorPictures(GroinArmorGraphicData);
  return true;
}



void playerkindgroin::DrawArmor(blitdata& BlitData) const
{
  DrawEquipment(GroinArmorGraphicData, BlitData);

  if(Master && GetExternalBodyArmor())
    GetExternalBodyArmor()->DrawFluidBodyArmorPictures(BlitData, ST_GROIN);
}



void playerkindgroin::Save(outputfile& SaveFile) const
{
  groin::Save(SaveFile);
  SaveFile << GroinArmorGraphicData;
}



void playerkindgroin::Load(inputfile& SaveFile)
{
  groin::Load(SaveFile);
  SaveFile >> GroinArmorGraphicData;
}
#endif
