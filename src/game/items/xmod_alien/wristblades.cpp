#ifdef HEADER_PHASE
ITEM(wristblades, meleeweapon)
{
 public:
  virtual truth AllowAlphaEverywhere() const { return true; }
  //virtual truth HitEffect(character*, character*, v2, int, int, truth);
 protected:
  virtual int GetClassAnimationFrames() const { return 32; }
  virtual col16 GetOutlineColor(int) const;
  virtual alpha GetOutlineAlpha(int) const;
};


#else


col16 wristblades::GetOutlineColor (int) const { return MakeRGB16(128, 128, 192); }

alpha wristblades::GetOutlineAlpha (int Frame) const {
  Frame &= 31;
  return 50+(Frame*(31-Frame)>>1);
}


#endif
