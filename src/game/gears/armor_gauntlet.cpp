#ifdef HEADER_PHASE
ITEM(gauntlet, armor)
{
 public:
  virtual sLong GetPrice() const;
  virtual truth IsGauntlet(ccharacter*) const { return true; }
  virtual truth IsInCorrectSlot(int) const;
};


#else



sLong gauntlet::GetPrice() const { return armor::GetPrice() / 3 + GetEnchantedPrice(Enchantment); }



truth gauntlet::IsInCorrectSlot(int I) const { return I == RIGHT_GAUNTLET_INDEX || I == LEFT_GAUNTLET_INDEX; }
#endif
