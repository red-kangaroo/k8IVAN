#ifdef HEADER_PHASE
OWTERRAIN(underwatertunnelexit, owterrain)
{
public:
  virtual cchar *GetNameStem () const;
  virtual v2 GetBitmapPos (int) const;
  virtual int GetAttachedDungeon () const;
  virtual truth UsesLongArticle () const { return true; }
  virtual int GetAttachedArea () const { return 2; }
};


#else



cchar *underwatertunnelexit::GetNameStem () const { return "exit from an underwater tunnel"; }



v2 underwatertunnelexit::GetBitmapPos (int) const { return v2(32, 64); }



int underwatertunnelexit::GetAttachedDungeon () const { return UNDER_WATER_TUNNEL; }
#endif
