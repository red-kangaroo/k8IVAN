#ifdef HEADER_PHASE
ITEM(lobhsetorso, largetorso)
{
 protected:
  virtual int GetClassAnimationFrames() const { return 32; }
  virtual col16 GetMaterialColorD(int) const;
};


#else



col16 lobhsetorso::GetMaterialColorD(int Frame) const
{
  Frame &= 31;
  int Modifier = Frame * (31 - Frame);
  return MakeRGB16(220 - (Modifier >> 2), 220 - (Modifier >> 1), 0);
}
#endif
