#ifdef HEADER_PHASE
ITEM(mysticfrogtorso, normaltorso)
{
 public:
  virtual truth AllowAlphaEverywhere() const { return true; }
 protected:
  virtual int GetClassAnimationFrames() const { return 128; }
  virtual col16 GetOutlineColor(int) const;
  virtual alpha GetOutlineAlpha(int) const;
};


#else



alpha mysticfrogtorso::GetOutlineAlpha(int Frame) const
{
  Frame &= 31;
  return Frame * (31 - Frame) >> 1;
}



col16 mysticfrogtorso::GetOutlineColor(int Frame) const
{
  switch((Frame&127) >> 5)
  {
   case 0: return BLUE;
   case 1: return GREEN;
   case 2: return RED;
   case 3: return YELLOW;
  }

  return TRANSPARENT_COLOR;
}
#endif
