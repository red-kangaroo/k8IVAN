#ifdef HEADER_PHASE
ITEM(gorovitsweapon, meleeweapon)
{
 public:
  virtual truth IsGorovitsFamilyRelic() const { return true; }
  virtual truth AllowAlphaEverywhere() const { return true; }
 protected:
  virtual int GetClassAnimationFrames() const { return 32; }
  virtual col16 GetOutlineColor(int) const;
  virtual alpha GetOutlineAlpha(int) const;
};


#else



col16 gorovitsweapon::GetOutlineColor(int) const { return MakeRGB16(255, 0, 0); }



alpha gorovitsweapon::GetOutlineAlpha(int Frame) const
{
  Frame &= 31;
  return 50 + (Frame * (31 - Frame) >> 1);
}
#endif
