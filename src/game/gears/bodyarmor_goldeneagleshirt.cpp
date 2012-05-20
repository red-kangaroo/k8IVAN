#ifdef HEADER_PHASE
ITEM(goldeneagleshirt, bodyarmor)
{
 public:
  virtual truth IsGoldenEagleShirt() const { return true; }
  virtual truth IsConsumable() const { return false; }
  virtual truth AllowAlphaEverywhere() const { return true; }
 protected:
  virtual int GetClassAnimationFrames() const { return 32; }
  virtual col16 GetOutlineColor(int) const;
  virtual alpha GetOutlineAlpha(int) const;
};


#else



col16 goldeneagleshirt::GetOutlineColor(int) const { return MakeRGB16(0, 255, 255); }



alpha goldeneagleshirt::GetOutlineAlpha(int Frame) const
{
  Frame &= 31;
  return 50 + (Frame * (31 - Frame) >> 1);
}
#endif
