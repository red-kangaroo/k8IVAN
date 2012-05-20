#ifdef HEADER_PHASE
GWTERRAIN(snow, gwterrain)
{
public:
  virtual cchar *GetNameStem () const;
  virtual v2 GetBitmapPos (int) const;
  virtual int GetPriority () const;
};


#else


int snow::GetPriority () const { return 80; }
cchar *snow::GetNameStem () const { return "tundra"; }
v2 snow::GetBitmapPos (int) const { return v2(112, 16); }


#endif
