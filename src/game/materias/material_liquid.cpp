class lterrain;


#ifdef HEADER_PHASE
MATERIAL(liquid, material)
{
 public:
  virtual cchar* GetConsumeVerb() const;
  virtual truth IsLiquid() const { return true; }
  void TouchEffect(item*, cfestring&);
  void TouchEffect(character*, int);
  void TouchEffect(lterrain*);
  liquid* SpawnMoreLiquid(sLong Volume) const { return static_cast<liquid*>(SpawnMore(Volume)); }
};


#else



cchar* liquid::GetConsumeVerb() const { return "drinking"; }



void liquid::TouchEffect(item* Item, cfestring& LocationName)
{
  if(GetRustModifier())
    Item->TryToRust(GetRustModifier() * GetVolume());

  if(GetAcidicity())
    Item->ReceiveAcid(this, LocationName, Volume * GetAcidicity());

  character* Char = Item->GetBodyPartMaster();

  if(Char)
    Effect(Char, Item->GetBodyPartIndex(), Volume >> 6);
}



void liquid::TouchEffect(lterrain* Terrain)
{
  if(GetRustModifier())
    Terrain->TryToRust(GetRustModifier() * GetVolume());

  if(GetAcidicity())
    Terrain->ReceiveAcid(this, Volume * GetAcidicity());
}



void liquid::TouchEffect(character* Char, int BodyPartIndex)
{
  if(GetRustModifier())
    Char->GetBodyPart(BodyPartIndex)->TryToRust(GetRustModifier() * GetVolume());

  if(Char->IsEnabled() && GetAcidicity())
    Char->GetBodyPart(BodyPartIndex)->ReceiveAcid(this, CONST_S(""), Volume * GetAcidicity() >> 1);

  if(Char->IsEnabled())
    Effect(Char, BodyPartIndex, Volume >> 9);
}
#endif
