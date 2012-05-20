#ifdef HEADER_PHASE
ITEM(solstone, stone)
{
public:
  virtual truth AllowAlphaEverywhere () const { return true; }
protected:
  virtual int GetClassAnimationFrames () const;
  virtual col16 GetOutlineColor (int) const;
  virtual alpha GetOutlineAlpha (int) const;
};


#else



int solstone::GetClassAnimationFrames () const { return !IsBroken() ? 128 : 1; }



alpha solstone::GetOutlineAlpha (int Frame) const {
  if (!IsBroken()) {
    Frame &= 31;
    return Frame*(31-Frame)>>1;
  }
  return 255;
}



col16 solstone::GetOutlineColor (int Frame) const {
  if (!IsBroken()) {
    switch ((Frame&127)>>5) {
      case 0: return BLUE;
      case 1: return GREEN;
      case 2: return RED;
      case 3: return YELLOW;
    }
  }
  return TRANSPARENT_COLOR;
}
#endif
