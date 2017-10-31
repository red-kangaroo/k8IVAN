#ifdef HEADER_PHASE
CHARACTER(dolphin, nonhumanoid)
{
 protected:
  virtual int GetSpecialBodyPartFlags(int) const;
  virtual void SpecialTurnHandler() { UpdatePictures(); }
};


#else



int dolphin::GetSpecialBodyPartFlags(int) const { return RAND() & (MIRROR|ROTATE); }
#endif
