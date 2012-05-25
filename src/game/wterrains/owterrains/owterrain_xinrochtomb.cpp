#ifdef HEADER_PHASE
OWTERRAIN(xinrochtomb, owterrain)
{
public:
  virtual cchar *GetNameStem () const;
  virtual v2 GetBitmapPos (int) const;
  virtual int GetAttachedDungeon () const;
  virtual truth IsHidden () const { return true; }
};


#else


cchar *xinrochtomb::GetNameStem () const { return "tomb of utter darkness where dwells the soul of Xinroch"; }
v2 xinrochtomb::GetBitmapPos (int) const { return v2(64, 80); }
int xinrochtomb::GetAttachedDungeon () const { return XINROCH_TOMB; }


#endif
