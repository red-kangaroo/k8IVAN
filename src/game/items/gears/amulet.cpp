#ifdef HEADER_PHASE
ITEM(amulet, item)
{
 public:
  virtual truth IsAmulet(ccharacter*) const { return true; }
  virtual truth IsInCorrectSlot(int) const;
  virtual truth IsLuxuryItem(ccharacter*) const { return true; }
 protected:
  virtual col16 GetMaterialColorB(int) const;
};


#else



truth amulet::IsInCorrectSlot(int I) const { return I == AMULET_INDEX; }



col16 amulet::GetMaterialColorB(int) const { return MakeRGB16(111, 64, 37); }
#endif
