#ifdef HEADER_PHASE
OWTERRAIN(dragontower, owterrain)
{
public:
  virtual cchar *GetNameStem () const;
  virtual v2 GetBitmapPos (int) const;
  virtual int GetAttachedDungeon () const;
};


#else


cchar *dragontower::GetNameStem () const { return "ruined tower"; }
v2 dragontower::GetBitmapPos (int) const { return v2(50, 80); }
int dragontower::GetAttachedDungeon () const { return DRAGON_TOWER; }


#endif
