#ifdef HEADER_PHASE
ITEM(boot, armor)
{
 public:
  virtual sLong GetPrice() const;
  virtual truth IsBoot(ccharacter*) const { return true; }
  virtual truth IsInCorrectSlot(int) const;
};


#else



sLong boot::GetPrice() const { return armor::GetPrice() / 5 + GetEnchantedPrice(Enchantment); }



truth boot::IsInCorrectSlot(int I) const { return I == RIGHT_BOOT_INDEX || I == LEFT_BOOT_INDEX; }
#endif
