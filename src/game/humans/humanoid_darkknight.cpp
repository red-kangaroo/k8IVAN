#ifdef HEADER_PHASE
CHARACTER(darkknight, humanoid)
{
 protected:
  virtual int ModifyBodyPartHitPreference(int, int) const;
  virtual int ModifyBodyPartToHitChance(int, int) const;
  virtual truth CanPanicFromSeveredBodyPart() const { return false; }
  virtual void SpecialBodyPartSeverReaction();
};


#else



int darkknight::ModifyBodyPartHitPreference(int I, int Modifier) const
{
  return IsLimbIndex(I) ? Modifier << 1 : Modifier;
}



int darkknight::ModifyBodyPartToHitChance(int I, int Chance) const
{
  return IsLimbIndex(I) ? Chance << 1 : Chance;
}



void darkknight::SpecialBodyPartSeverReaction()
{
  if(!IsPlayer())
  {
    if(IsUsingHead())
      ADD_MESSAGE("%s screams: \"I'll do you for that! I'll bite your legs off!\"", CHAR_DESCRIPTION(DEFINITE));
    else if(!(RAND() % 5))
      switch(RAND() % 3)
      {
       case 0:
  ADD_MESSAGE("%s states calmly: \"'Tis but a scratch.\"", CHAR_DESCRIPTION(DEFINITE)); break;
       case 1:
  ADD_MESSAGE("%s states calmly: \"Just a flesh wound.\"", CHAR_DESCRIPTION(DEFINITE)); break;
       case 2:
  ADD_MESSAGE("%s shouts: \"I'm invincible!\"", CHAR_DESCRIPTION(DEFINITE)); break;
      }
  }
}
#endif
