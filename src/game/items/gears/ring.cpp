#ifdef HEADER_PHASE
ITEM(ring, item)
{
 public:
  virtual truth IsRing(ccharacter*) const { return true; }
  virtual truth IsInCorrectSlot(int) const;
  virtual truth IsLuxuryItem(ccharacter*) const { return true; }
 protected:
  virtual col16 GetMaterialColorB(int) const;
};


#else



truth ring::IsInCorrectSlot(int I) const { return I == RIGHT_RING_INDEX || I == LEFT_RING_INDEX; }



col16 ring::GetMaterialColorB(int) const { return MakeRGB16(200, 200, 200); }
#endif
