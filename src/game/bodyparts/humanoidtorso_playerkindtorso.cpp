#ifdef HEADER_PHASE
ITEM(playerkindtorso, humanoidtorso)
{
 public:
  playerkindtorso() { }
  playerkindtorso(const playerkindtorso& Torso) : mybase(Torso) { }
  virtual void Save(outputfile&) const;
  virtual void Load(inputfile&);
  virtual truth UpdateArmorPictures();
  virtual void DrawArmor(blitdata&) const;
  virtual void SignalVolumeAndWeightChange();
  virtual truth ShowFluids() const { return true; }
  virtual truth IsAnimated() const { return true; }
 protected:
  graphicdata TorsoArmorGraphicData;
  graphicdata CloakGraphicData;
  graphicdata BeltGraphicData;
};


#else



truth playerkindtorso::UpdateArmorPictures()
{
  UpdateTorsoArmorPictures(TorsoArmorGraphicData,
         CloakGraphicData,
         BeltGraphicData);
  return true;
}



void playerkindtorso::DrawArmor(blitdata& BlitData) const
{
  DrawEquipment(TorsoArmorGraphicData, BlitData);

  if(GetBodyArmor())
    GetBodyArmor()->DrawFluidGearPictures(BlitData);

  DrawEquipment(CloakGraphicData, BlitData);

  if(GetCloak())
    GetCloak()->DrawFluidGearPictures(BlitData);

  DrawEquipment(BeltGraphicData, BlitData);

  if(GetBelt())
    GetBelt()->DrawFluidGearPictures(BlitData);
}



void playerkindtorso::Save(outputfile& SaveFile) const
{
  humanoidtorso::Save(SaveFile);
  SaveFile << TorsoArmorGraphicData << CloakGraphicData << BeltGraphicData;
}



void playerkindtorso::Load(inputfile& SaveFile)
{
  humanoidtorso::Load(SaveFile);
  SaveFile >> TorsoArmorGraphicData >> CloakGraphicData >> BeltGraphicData;
}



void playerkindtorso::SignalVolumeAndWeightChange()
{
  humanoidtorso::SignalVolumeAndWeightChange();

  if(Master && !Master->IsInitializing())
    Master->UpdatePictures();
}
#endif
