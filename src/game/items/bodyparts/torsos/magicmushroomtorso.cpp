#ifdef HEADER_PHASE
ITEM(magicmushroomtorso, normaltorso)
{
 protected:
  virtual int GetClassAnimationFrames() const { return 64; }
  virtual v2 GetBitmapPos(int) const;
};


#else



v2 magicmushroomtorso::GetBitmapPos(int Frame) const
{
  v2 BasePos = torso::GetBitmapPos(Frame);
  Frame &= 0x3F;

  if(!(Frame & 0x30))
  {
    if(Frame <= 8)
      return v2(BasePos.X + 64 - (abs(Frame - 4) << 4), BasePos.Y);
    else
      return v2(BasePos.X + 64 - (abs(Frame - 12) << 4), BasePos.Y + 16);
  }
  else
    return BasePos;
}
#endif
