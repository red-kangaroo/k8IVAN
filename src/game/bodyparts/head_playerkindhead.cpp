#ifdef HEADER_PHASE
ITEM(playerkindhead, head)
{
 public:
  playerkindhead() { }
  playerkindhead(const playerkindhead& Head) : mybase(Head) { }
  virtual void Save(outputfile&) const;
  virtual void Load(inputfile&);
  virtual truth UpdateArmorPictures();
  virtual void DrawArmor(blitdata&) const;
  virtual truth ShowFluids() const { return true; }
  virtual truth IsAnimated() const { return true; }
 protected:
  graphicdata HelmetGraphicData;
};


#else



truth playerkindhead::UpdateArmorPictures()
{
  UpdateHeadArmorPictures(HelmetGraphicData);
  return true;
}



void playerkindhead::DrawArmor(blitdata& BlitData) const
{
  DrawEquipment(HelmetGraphicData, BlitData);

  if(GetHelmet())
    GetHelmet()->DrawFluidGearPictures(BlitData);
}



void playerkindhead::Save(outputfile& SaveFile) const
{
  head::Save(SaveFile);
  SaveFile << HelmetGraphicData;
}



void playerkindhead::Load(inputfile& SaveFile)
{
  head::Load(SaveFile);
  SaveFile >> HelmetGraphicData;
}
#endif
