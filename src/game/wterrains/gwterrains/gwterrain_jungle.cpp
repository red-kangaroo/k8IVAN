#ifdef HEADER_PHASE
GWTERRAIN(jungle, gwterrain)
{
public:
  virtual cchar *GetNameStem () const;
  virtual v2 GetBitmapPos (int) const;
  virtual int GetPriority () const;
};


#else


int jungle::GetPriority () const { return 50; }
cchar *jungle::GetNameStem () const { return "jungle"; }
v2 jungle::GetBitmapPos (int) const { return v2(208, 16); }


#endif
