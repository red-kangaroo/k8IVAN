#ifdef HEADER_PHASE
GWTERRAIN(evergreenforest, gwterrain)
{
public:
  virtual cchar *GetNameStem () const;
  virtual v2 GetBitmapPos (int) const;
  virtual truth UsesLongArticle () const { return true; }
  virtual int GetPriority () const;
};


#else


int evergreenforest::GetPriority () const { return 70; }
cchar *evergreenforest::GetNameStem () const { return "evergreen forest"; }
v2 evergreenforest::GetBitmapPos (int) const { return v2(352, 16); }


#endif
