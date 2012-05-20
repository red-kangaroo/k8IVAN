#ifdef HEADER_PHASE
ITEM(spidertorso, normaltorso)
{
 protected:
  virtual int GetClassAnimationFrames() const { return 16; }
  virtual v2 GetBitmapPos(int) const;
};


#else



v2 spidertorso::GetBitmapPos(int Frame) const
{
  v2 BasePos = torso::GetBitmapPos(Frame);
  Frame &= 0xF;
  return v2(BasePos.X + ((Frame &~ 7) << 1), BasePos.Y);
}
#endif
