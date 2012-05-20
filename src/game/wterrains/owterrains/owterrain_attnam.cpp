#ifdef HEADER_PHASE
OWTERRAIN(attnam, owterrain)
{
public:
  virtual cchar *GetNameStem () const;
  virtual v2 GetBitmapPos (int) const;
  virtual int GetAttachedDungeon () const;
};


#else



cchar *attnam::GetNameStem () const { return "migthy cathedral reaching the clouds"; }



v2 attnam::GetBitmapPos (int) const { return v2(0, 48); }



int attnam::GetAttachedDungeon () const { return ATTNAM; }
#endif
