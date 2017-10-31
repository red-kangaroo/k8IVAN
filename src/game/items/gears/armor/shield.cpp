#ifdef HEADER_PHASE
ITEM(shield, armor)
{
 public:
  virtual sLong GetPrice() const;
  virtual truth IsShield(ccharacter*) const { return true; }
  virtual void AddInventoryEntry(ccharacter*, festring&, int, truth) const;
};


#else



void shield::AddInventoryEntry(ccharacter* Viewer, festring& Entry, int, truth ShowSpecialInfo) const // never piled
{
  AddName(Entry, INDEFINITE);

  if(ShowSpecialInfo)
  {
    Entry << " [\1C" << GetWeight() << "g\2, "  << GetBaseBlockValueDescription();

    if(!IsBroken())
      Entry << ", " << GetStrengthValueDescription();

    int CWeaponSkillLevel = Viewer->GetCWeaponSkillLevel(this);
    int SWeaponSkillLevel = Viewer->GetSWeaponSkillLevel(this);

    if(CWeaponSkillLevel || SWeaponSkillLevel)
      Entry << ", skill \1C" << CWeaponSkillLevel << '/' << SWeaponSkillLevel << "\2";

    Entry << ']';
  }
}



sLong shield::GetPrice() const /* temporary... */
{
  double StrengthValue = GetStrengthValue();
  return sLong(sqrt(GetBaseBlockValue()) * StrengthValue * StrengthValue) + item::GetPrice();
}
#endif
