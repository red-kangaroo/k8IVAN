#ifdef HEADER_PHASE
OWTERRAIN(mondedr, owterrain)
{
public:
  virtual cchar *GetNameStem () const;
  virtual v2 GetBitmapPos (int) const;
  virtual int GetAttachedDungeon () const;
};


#else


cchar *mondedr::GetNameStem () const { return "hidden village"; }
v2 mondedr::GetBitmapPos (int) const { return v2(16, 64); }
int mondedr::GetAttachedDungeon () const { return MONDEDR; }


#endif
