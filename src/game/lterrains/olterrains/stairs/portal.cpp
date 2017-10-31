#ifdef HEADER_PHASE
OLTERRAIN(portal, stairs)
{
protected:
  virtual int GetClassAnimationFrames () const;
  virtual v2 GetBitmapPos (int) const;
};


#else


int portal::GetClassAnimationFrames () const { return 32; }
v2 portal::GetBitmapPos (int Frame) const { return v2(16 + (((Frame & 31) << 3)&~8), 0); } // gum solution, should come from script


#endif
