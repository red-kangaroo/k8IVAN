#ifdef HEADER_PHASE
GWTERRAIN(glacier, gwterrain)
{
public:
  virtual festring GetNameStem () const;
  virtual v2 GetBitmapPos (int) const;
  virtual int GetPriority () const;
};


#else


int glacier::GetPriority () const { return 90; }
festring glacier::GetNameStem () const { return "glacier"; }
v2 glacier::GetBitmapPos (int) const { return v2(16, 16); }


#endif
