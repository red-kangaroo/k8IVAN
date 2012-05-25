#ifdef HEADER_PHASE
OWTERRAIN(elpuricave, owterrain)
{
public:
  virtual cchar *GetNameStem () const;
  virtual v2 GetBitmapPos (int) const;
  virtual int GetAttachedDungeon () const;
  virtual truth IsHidden () const { return true; }
};


#else


cchar *elpuricave::GetNameStem () const { return "hideous cave entry radiating pure evil"; }
v2 elpuricave::GetBitmapPos (int) const { return v2(16, 48); }
int elpuricave::GetAttachedDungeon () const { return ELPURI_CAVE; }


#endif
