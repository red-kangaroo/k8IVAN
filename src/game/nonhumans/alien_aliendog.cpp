#ifdef HEADER_PHASE
CHARACTER(aliendog, nonhumanoid)
{
 protected:
  virtual bodypart *MakeBodyPart (int) const;
};


#else


bodypart *aliendog::MakeBodyPart (int) const { return aliendogtorso::Spawn(0, NO_MATERIALS); }


#endif
