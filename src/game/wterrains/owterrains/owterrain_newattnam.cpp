#ifdef HEADER_PHASE
OWTERRAIN(newattnam, owterrain)
{
public:
  virtual cchar *GetNameStem () const;
  virtual v2 GetBitmapPos (int) const;
  virtual int GetAttachedDungeon () const;
};


#else


cchar *newattnam::GetNameStem () const { return "primitive village"; }
v2 newattnam::GetBitmapPos (int) const { return v2(16, 64); }
int newattnam::GetAttachedDungeon () const { return NEW_ATTNAM; }


#endif
