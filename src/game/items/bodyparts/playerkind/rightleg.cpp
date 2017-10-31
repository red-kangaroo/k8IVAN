#ifdef HEADER_PHASE
ITEM(playerkindrightleg, rightleg)
{
 public:
  playerkindrightleg() { }
  playerkindrightleg(const playerkindrightleg& Leg) : mybase(Leg) { }
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



truth playerkindrightleg::UpdateArmorPictures()
{
  UpdateLegArmorPictures(LegArmorGraphicData,
       BootGraphicData,
       ST_RIGHT_LEG);
  return true;
}



void playerkindrightleg::DrawArmor(blitdata& BlitData) const
{
  DrawEquipment(LegArmorGraphicData, BlitData);

  if(Master && GetExternalBodyArmor())
    GetExternalBodyArmor()->DrawFluidBodyArmorPictures(BlitData, ST_RIGHT_LEG);

  DrawEquipment(BootGraphicData, BlitData);

  if(GetBoot())
    GetBoot()->DrawFluidGearPictures(BlitData);
}



void playerkindrightleg::Save(outputfile& SaveFile) const
{
  rightleg::Save(SaveFile);
  SaveFile << LegArmorGraphicData << BootGraphicData;
}



void playerkindrightleg::Load(inputfile& SaveFile)
{
  rightleg::Load(SaveFile);
  SaveFile >> LegArmorGraphicData >> BootGraphicData;
}
#endif
