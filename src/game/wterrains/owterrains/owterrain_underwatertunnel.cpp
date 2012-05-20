#ifdef HEADER_PHASE
OWTERRAIN(underwatertunnel, owterrain)
{
public:
  virtual cchar *GetNameStem () const;
  virtual v2 GetBitmapPos (int) const;
  virtual int GetAttachedDungeon () const;
  virtual truth UsesLongArticle () const;
};


#else


truth underwatertunnel::UsesLongArticle () const { return true; }
cchar *underwatertunnel::GetNameStem () const { return "entrance to an underwater tunnel"; }
v2 underwatertunnel::GetBitmapPos (int) const { return v2(32, 64); }
int underwatertunnel::GetAttachedDungeon () const { return UNDER_WATER_TUNNEL; }


#endif
