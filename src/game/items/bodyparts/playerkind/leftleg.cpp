#ifdef HEADER_PHASE
ITEM(playerkindleftleg, leftleg)
{
 public:
  playerkindleftleg() { }
  playerkindleftleg(const playerkindleftleg& Leg) : mybase(Leg) { }
  virtual void Save(outputfile&) const;
  virtual void Load(inputfile&);
  virtual truth UpdateArmorPictures();
  virtual void DrawArmor(blitdata&) const;
  virtual truth ShowFluids() const { return true; }
  virtual truth IsAnimated() const { return true; }
 protected:
  graphicdata LegArmorGraphicData;
  graphicdata BootGraphicData;
};


#else



truth playerkindleftleg::UpdateArmorPictures()
{
  UpdateLegArmorPictures(LegArmorGraphicData,
       BootGraphicData,
       ST_LEFT_LEG);
  return true;
}



void playerkindleftleg::DrawArmor(blitdata& BlitData) const
{
  DrawEquipment(LegArmorGraphicData, BlitData);

  if(Master && GetExternalBodyArmor())
    GetExternalBodyArmor()->DrawFluidBodyArmorPictures(BlitData, ST_LEFT_LEG);

  DrawEquipment(BootGraphicData, BlitData);

  if(GetBoot())
    GetBoot()->DrawFluidGearPictures(BlitData);
}



void playerkindleftleg::Save(outputfile& SaveFile) const
{
  leftleg::Save(SaveFile);
  SaveFile << LegArmorGraphicData << BootGraphicData;
}



void playerkindleftleg::Load(inputfile& SaveFile)
{
  leftleg::Load(SaveFile);
  SaveFile >> LegArmorGraphicData >> BootGraphicData;
}
#endif
