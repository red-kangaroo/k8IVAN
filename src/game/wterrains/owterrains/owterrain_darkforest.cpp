#ifdef HEADER_PHASE
OWTERRAIN(darkforest, owterrain)
{
public:
  virtual const char *GetNameStem () const;
  virtual v2 GetBitmapPos (int) const;
  virtual int GetAttachedDungeon () const;
};


#else


cchar *darkforest::GetNameStem () const { return "dark forest"; }
v2 darkforest::GetBitmapPos (int) const { return v2(80, 80); }
int darkforest::GetAttachedDungeon () const { return DARK_FOREST; }


#endif
