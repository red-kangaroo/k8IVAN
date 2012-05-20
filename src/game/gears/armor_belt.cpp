#ifdef HEADER_PHASE
ITEM(belt, armor)
{
 public:
  virtual sLong GetPrice() const;
  virtual truth IsBelt(ccharacter*) const { return true; }
  virtual int GetFormModifier() const;
  virtual truth IsInCorrectSlot(int) const;
  virtual col16 GetMaterialColorB(int Frame) const { return GetMaterialColorA(Frame); }
};


#else



sLong belt::GetPrice() const { return armor::GetPrice() * 5 + GetEnchantedPrice(Enchantment); }



truth belt::IsInCorrectSlot(int I) const { return I == BELT_INDEX; }



int belt::GetFormModifier() const
{
  return item::GetFormModifier() * GetMainMaterial()->GetFlexibility();
}
#endif
