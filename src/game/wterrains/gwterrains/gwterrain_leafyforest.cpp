#ifdef HEADER_PHASE
GWTERRAIN(leafyforest, gwterrain)
{
public:
  virtual festring GetNameStem () const;
  virtual v2 GetBitmapPos (int) const;
  virtual int GetPriority () const;
};


#else


int leafyforest::GetPriority () const { return 60; }
festring leafyforest::GetNameStem () const { return "leafy forest"; }
v2 leafyforest::GetBitmapPos (int) const { return v2(304, 16); }


#endif
