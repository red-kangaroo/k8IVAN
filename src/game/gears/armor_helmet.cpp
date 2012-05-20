#ifdef HEADER_PHASE
ITEM(helmet, armor)
{
 public:
  virtual truth IsGorovitsFamilyRelic() const;
  virtual sLong GetPrice() const;
  virtual truth IsHelmet(ccharacter*) const { return true; }
  virtual truth IsInCorrectSlot(int) const;
 protected:
  virtual col16 GetMaterialColorB(int) const;
  virtual col16 GetMaterialColorC(int) const;
};


#else



truth helmet::IsGorovitsFamilyRelic() const { return GetConfig() == GOROVITS_FAMILY_GAS_MASK; }



sLong helmet::GetPrice() const { return armor::GetPrice() + GetEnchantedPrice(Enchantment); }



truth helmet::IsInCorrectSlot(int I) const { return I == HELMET_INDEX; }



col16 helmet::GetMaterialColorB(int) const { return GetConfig() != GOROVITS_FAMILY_GAS_MASK ? (GetConfig() & ~BROKEN) ? MakeRGB16(140, 70, 70) : MakeRGB16(111, 64, 37) : MakeRGB16(0, 40, 0); }



col16 helmet::GetMaterialColorC(int) const { return MakeRGB16(180, 200, 180); }
#endif
