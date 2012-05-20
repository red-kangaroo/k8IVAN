#ifdef HEADER_PHASE
OWTERRAIN(muntuo, owterrain)
{
public:
  virtual cchar *GetNameStem () const;
  virtual v2 GetBitmapPos (int) const;
  virtual int GetAttachedDungeon () const;
};


#else


cchar *muntuo::GetNameStem () const { return "isolated, ruined temple"; }
v2 muntuo::GetBitmapPos (int) const { return v2(80, 96); }
int muntuo::GetAttachedDungeon () const { return MUNTUO; }


#endif
