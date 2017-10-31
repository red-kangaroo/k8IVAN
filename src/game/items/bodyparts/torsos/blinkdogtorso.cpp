#ifdef HEADER_PHASE
ITEM(blinkdogtorso, dogtorso)
{
 protected:
  virtual alpha GetAlphaA(int) const;
  virtual int GetClassAnimationFrames() const { return 64; }
};


#else



alpha blinkdogtorso::GetAlphaA(int Frame) const { return (Frame & 31) != 31 ? 255 : 0; }
#endif
