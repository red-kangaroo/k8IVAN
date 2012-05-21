#ifdef HEADER_PHASE
GWTERRAIN(steppe, gwterrain)
{
public:
  virtual cchar *GetNameStem () const;
  virtual v2 GetBitmapPos (int) const;
  virtual int GetPriority () const;
};


#else


int steppe::GetPriority () const { return 30; }
cchar *steppe::GetNameStem () const { return "steppe"; }
v2 steppe::GetBitmapPos (int) const { return v2(160, 16); }


#endif