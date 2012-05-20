#ifdef HEADER_PHASE
ITEM(ennerhead, head)
{
 protected:
  virtual int GetClassAnimationFrames() const { return 32; }
  virtual v2 GetBitmapPos(int) const;
};


#else



v2 ennerhead::GetBitmapPos(int Frame) const { return Frame & 16 ? head::GetBitmapPos(Frame) : head::GetBitmapPos(Frame) + v2(16, 0); }
#endif
