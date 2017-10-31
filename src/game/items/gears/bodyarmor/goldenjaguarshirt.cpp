#ifdef HEADER_PHASE
ITEM(goldenjaguarshirt, bodyarmor)
{
public:
  virtual truth IsConsumable () const { return false; }
  virtual truth AllowAlphaEverywhere () const { return true; }
protected:
  virtual int GetClassAnimationFrames () const { return 32; }
  virtual col16 GetOutlineColor (int) const;
  virtual alpha GetOutlineAlpha (int) const;
};


#else



col16 goldenjaguarshirt::GetOutlineColor (int) const { return MakeRGB16(255, 255, 128); }



alpha goldenjaguarshirt::GetOutlineAlpha (int Frame) const {
  Frame &= 31;
  return 50 + (Frame * (31 - Frame) >> 1);
}
#endif
