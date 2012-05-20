#ifdef HEADER_PHASE
ITEM(belderiver, meleeweapon)
{
public:
  virtual truth AllowAlphaEverywhere () const { return true; }
protected:
  virtual int GetClassAnimationFrames () const { return 32; }
  virtual col16 GetOutlineColor (int) const;
  virtual alpha GetOutlineAlpha (int) const;
};


#else



col16 belderiver::GetOutlineColor (int) const { return MakeRGB16(180, 50, 50); }



alpha belderiver::GetOutlineAlpha (int Frame) const {
  Frame &= 31;
  return 50 + (Frame * (31 - Frame) >> 1);
}
#endif
