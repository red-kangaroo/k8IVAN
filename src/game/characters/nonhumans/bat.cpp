#ifdef HEADER_PHASE
CHARACTER(bat, nonhumanoid)
{
 protected:
  virtual bodypart* MakeBodyPart(int) const;
};


#else



bodypart* bat::MakeBodyPart(int) const { return battorso::Spawn(0, NO_MATERIALS); }
#endif
