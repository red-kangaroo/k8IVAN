#ifdef HEADER_PHASE
ITEM(demonhead, meleeweapon)
{
public:
  virtual truth AllowAlphaEverywhere () const { return true; }
protected:
  virtual int GetClassAnimationFrames () const { return 32; }
  virtual col16 GetOutlineColor (int) const;
  virtual alpha GetOutlineAlpha (int) const;
};


#else



col16 demonhead::GetOutlineColor (int) const { return MakeRGB16(255, 0, 0); }



alpha demonhead::GetOutlineAlpha (int Frame) const {
  Frame &= 31;
  return 50 + (Frame * (31 - Frame) >> 1);
}
#endif
