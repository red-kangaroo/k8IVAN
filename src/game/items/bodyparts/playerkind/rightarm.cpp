#ifdef HEADER_PHASE
ITEM(playerkindrightarm, rightarm)
{
 public:
  playerkindrightarm() { }
  playerkindrightarm(const playerkindrightarm& Arm) : mybase(Arm) { }
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



truth playerkindrightarm::UpdateArmorPictures()
{
  UpdateArmArmorPictures(ArmArmorGraphicData,
       GauntletGraphicData,
       ST_RIGHT_ARM);
  return true;
}



void playerkindrightarm::DrawArmor(blitdata& BlitData) const
{
  DrawEquipment(ArmArmorGraphicData, BlitData);

  if(Master && GetExternalBodyArmor())
    GetExternalBodyArmor()->DrawFluidBodyArmorPictures(BlitData, ST_RIGHT_ARM);

  DrawEquipment(GauntletGraphicData, BlitData);

  if(GetGauntlet())
    GetGauntlet()->DrawFluidGearPictures(BlitData);
}



void playerkindrightarm::Save(outputfile& SaveFile) const
{
  rightarm::Save(SaveFile);
  SaveFile << ArmArmorGraphicData << GauntletGraphicData;
}



void playerkindrightarm::Load(inputfile& SaveFile)
{
  rightarm::Load(SaveFile);
  SaveFile >> ArmArmorGraphicData >> GauntletGraphicData;
}
#endif
