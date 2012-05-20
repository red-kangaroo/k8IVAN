#ifdef HEADER_PHASE
GWTERRAIN(snow, gwterrain)
{
public:
  virtual cchar *GetNameStem () const;
  virtual v2 GetBitmapPos (int) const;
  virtual int GetPriority () const { return 80; }
};


#else



cchar *snow::GetNameStem () const { return "tundra"; }



v2 snow::GetBitmapPos (int) const { return v2(112, 16); }
#endif
