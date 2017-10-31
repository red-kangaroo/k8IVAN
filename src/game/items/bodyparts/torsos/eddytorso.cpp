#ifdef HEADER_PHASE
ITEM(eddytorso, normaltorso)
{
 protected:
  virtual int GetClassAnimationFrames() const { return 8; }
  virtual v2 GetBitmapPos(int) const;
};


#else



v2 eddytorso::GetBitmapPos(int Frame) const { return torso::GetBitmapPos(Frame) + v2((Frame&0x6) << 3, 0); }
#endif
