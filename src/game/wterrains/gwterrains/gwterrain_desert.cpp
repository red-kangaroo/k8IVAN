#ifdef HEADER_PHASE
GWTERRAIN(desert, gwterrain)
{
public:
  virtual cchar *GetNameStem () const;
  virtual v2 GetBitmapPos (int) const;
  virtual int GetPriority () const;
};


#else


int desert::GetPriority () const { return 20; }
cchar *desert::GetNameStem () const { return "desert"; }
v2 desert::GetBitmapPos (int) const { return v2(64, 16); }


#endif
