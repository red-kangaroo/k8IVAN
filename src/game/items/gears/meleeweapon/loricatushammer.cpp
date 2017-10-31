#ifdef HEADER_PHASE
ITEM(loricatushammer, meleeweapon)
{
public:
  virtual truth AllowAlphaEverywhere () const { return true; }
protected:
  virtual int GetClassAnimationFrames() const { return 32; }
  virtual col16 GetOutlineColor (int) const;
  virtual alpha GetOutlineAlpha (int) const;
};


#else



col16 loricatushammer::GetOutlineColor (int) const { return MakeRGB16(0, 0, 255); }



alpha loricatushammer::GetOutlineAlpha (int Frame) const {
  Frame &= 31;
  return 50 + (Frame * (31 - Frame) >> 1);
}
#endif
