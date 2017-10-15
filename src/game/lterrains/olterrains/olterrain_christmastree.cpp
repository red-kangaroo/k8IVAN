#ifdef HEADER_PHASE
OLTERRAIN(christmastree, olterrain)
{
 public:
  virtual int GetSparkleFlags() const;
 protected:
  virtual int GetClassAnimationFrames() const;
  virtual v2 GetBitmapPos(int) const;
};


#else


int christmastree::GetClassAnimationFrames() const { return game::IsXMas() ? 32 : 1; }
v2 christmastree::GetBitmapPos(int Frame) const { return game::IsXMas() ? v2(16 + (((Frame & 31) << 3)&~8), 448) : v2(0, 448); }
int christmastree::GetSparkleFlags() const { return (game::IsXMas() ? SPARKLING_B : 0); }


#endif
