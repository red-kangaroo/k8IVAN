#ifdef HEADER_PHASE
ITEM(justifier, meleeweapon)
{
 public:
  virtual truth AllowAlphaEverywhere() const { return true; }
 protected:
  virtual int GetClassAnimationFrames() const { return 32; }
  virtual col16 GetOutlineColor(int) const;
  virtual alpha GetOutlineAlpha(int) const;
};


#else



col16 justifier::GetOutlineColor(int) const { return MakeRGB16(0, 255, 0); }



alpha justifier::GetOutlineAlpha(int Frame) const
{
  Frame &= 31;
  return 50 + (Frame * (31 - Frame) >> 1);
}
#endif
