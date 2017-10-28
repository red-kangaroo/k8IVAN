#ifdef HEADER_PHASE
GWTERRAIN(steppe, gwterrain)
{
public:
  virtual festring GetNameStem () const;
  virtual v2 GetBitmapPos (int) const;
  virtual int GetPriority () const;
};


#else


int steppe::GetPriority () const { return 30; }
festring steppe::GetNameStem () const { return "steppe"; }
v2 steppe::GetBitmapPos (int) const { return v2(160, 16); }


#endif
