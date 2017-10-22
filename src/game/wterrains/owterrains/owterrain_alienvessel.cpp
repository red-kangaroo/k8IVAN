#ifdef HEADER_PHASE
OWTERRAIN(alienvessel, owterrain)
{
 public:
  virtual cchar *GetNameStem () const;
  virtual v2 GetBitmapPos (int) const;
  virtual int GetAttachedDungeon () const;
};


#else


cchar* alienvessel::GetNameStem () const { return "strange vessel of unknow nature"; }
v2 alienvessel::GetBitmapPos (int) const { return v2(32, 48); }
int alienvessel::GetAttachedDungeon () const { return ALIEN_VESSEL; }


#endif
