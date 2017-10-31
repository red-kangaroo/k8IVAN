#ifdef HEADER_PHASE
ITEM(battorso, normaltorso)
{
 protected:
  virtual int GetClassAnimationFrames() const { return 16; }
  virtual v2 GetBitmapPos(int) const;
};


#else



v2 battorso::GetBitmapPos(int Frame) const
{
  v2 BasePos = torso::GetBitmapPos(Frame);
  Frame &= 0xF;
  return v2(BasePos.X + ((Frame &~ 3) << 2), BasePos.Y);
}
#endif
