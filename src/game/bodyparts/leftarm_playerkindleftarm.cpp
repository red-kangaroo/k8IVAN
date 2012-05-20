#ifdef HEADER_PHASE
ITEM(playerkindleftarm, leftarm)
{
 public:
  playerkindleftarm() { }
  playerkindleftarm(const playerkindleftarm& Arm) : mybase(Arm) { }
  virtual void Save(outputfile&) const;
  virtual void Load(inputfile&);
  virtual truth UpdateArmorPictures();
  virtual void DrawArmor(blitdata&) const;
  virtual truth ShowFluids() const { return true; }
  virtual truth IsAnimated() const { return true; }
 protected:
  graphicdata ArmArmorGraphicData;
  graphicdata GauntletGraphicData;
};


#else



truth playerkindleftarm::UpdateArmorPictures()
{
  UpdateArmArmorPictures(ArmArmorGraphicData,
       GauntletGraphicData,
       ST_LEFT_ARM);
  return true;
}



void playerkindleftarm::DrawArmor(blitdata& BlitData) const
{
  DrawEquipment(ArmArmorGraphicData, BlitData);

  if(Master && GetExternalBodyArmor())
    GetExternalBodyArmor()->DrawFluidBodyArmorPictures(BlitData, ST_LEFT_ARM);

  DrawEquipment(GauntletGraphicData, BlitData);

  if(GetGauntlet())
    GetGauntlet()->DrawFluidGearPictures(BlitData);
}



void playerkindleftarm::Save(outputfile& SaveFile) const
{
  leftarm::Save(SaveFile);
  SaveFile << ArmArmorGraphicData << GauntletGraphicData;
}



void playerkindleftarm::Load(inputfile& SaveFile)
{
  leftarm::Load(SaveFile);
  SaveFile >> ArmArmorGraphicData >> GauntletGraphicData;
}
#endif
