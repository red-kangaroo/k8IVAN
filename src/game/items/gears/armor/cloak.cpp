#ifdef HEADER_PHASE
ITEM(cloak, armor)
{
 public:
  virtual sLong GetPrice() const;
  virtual truth IsCloak(ccharacter*) const { return true; }
  virtual truth IsInCorrectSlot(int) const;
  virtual truth ReceiveDamage(character*, int, int, int);
  virtual truth IsShadowVeil() const;
 protected:
  virtual int GetSpecialFlags() const;
  virtual cchar* GetBreakVerb() const;
  virtual truth AddAdjective(festring&, truth) const;
  virtual col16 GetMaterialColorB(int) const;
};


#else


sLong cloak::GetPrice() const { return armor::GetPrice() * 10 + GetEnchantedPrice(Enchantment); }
truth cloak::IsInCorrectSlot(int I) const { return I == CLOAK_INDEX; }
col16 cloak::GetMaterialColorB(int) const { return MakeRGB16(111, 64, 37); }
cchar* cloak::GetBreakVerb() const { return GetMainMaterial()->GetFlexibility() >= 5 ? "is torn apart" : "breaks"; }
truth cloak::ReceiveDamage(character* Damager, int Damage,  int Type, int Dir) { return armor::ReceiveDamage(Damager, Damage >> 1, Type, Dir); }
int cloak::GetSpecialFlags() const { return ST_CLOAK; }
truth cloak::IsShadowVeil () const { return (GetConfig() == CLOAK_OF_SHADOWS); }


truth cloak::AddAdjective (festring& String, truth Articled) const {
  if (IsBroken()) {
    if (Articled) String << "a ";

    if (GetMainMaterial()->GetFlexibility() >= 5) String << "torn"; else String << "broken";
    String << ' ';
    return true;
  }
  return false;
}

#endif
