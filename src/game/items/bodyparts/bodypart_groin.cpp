#ifdef HEADER_PHASE
ITEM(groin, bodypart)
{
 public:
  virtual int GetTotalResistance(int) const;
  virtual int GetBodyPartIndex() const;
  virtual truth DamageArmor(character*, int, int);
  virtual int GetSpecialFlags() const;
  virtual item* GetArmorToReceiveFluid(truth) const;
  void UpdateGroinArmorPictures(graphicdata&) const;
};


#else



int groin::GetBodyPartIndex() const { return GROIN_INDEX; }



int groin::GetSpecialFlags() const { return SpecialFlags|ST_GROIN; }



int groin::GetTotalResistance(int Type) const
{
  if(Master)
  {
    int Resistance = GetResistance(Type) + Master->GetGlobalResistance(Type);

    if(GetExternalBodyArmor())
      Resistance += GetExternalBodyArmor()->GetResistance(Type);

    if(GetHumanoidMaster()->GetBelt())
      Resistance += GetHumanoidMaster()->GetBelt()->GetResistance(Type);

    return Resistance;
  }
  else
    return GetResistance(Type);
}



truth groin::DamageArmor(character* Damager, int Damage, int Type)
{
  return Master->GetTorso()->DamageArmor(Damager, Damage, Type);
}



item* groin::GetArmorToReceiveFluid(truth) const
{
  item* Cloak = GetExternalCloak();

  if(Cloak && !(RAND() % 3))
    return Cloak;

  item* BodyArmor = GetExternalBodyArmor();
  return BodyArmor ? BodyArmor : 0;
}



void groin::UpdateGroinArmorPictures(graphicdata& GroinArmorGraphicData) const
{
  if(!Master || !Master->PictureUpdatesAreForbidden())
  {
    UpdateArmorPicture(GroinArmorGraphicData,
           Master ? GetExternalBodyArmor() : 0,
           ST_GROIN,
           &item::GetLegArmorBitmapPos,
           true);
  }
}
#endif
