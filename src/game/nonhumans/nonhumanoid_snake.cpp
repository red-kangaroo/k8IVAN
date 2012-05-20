#ifdef HEADER_PHASE
CHARACTER(snake, nonhumanoid)
{
 protected:
  virtual truth SpecialBiteEffect(character*, v2, int, int, truth);
};


#else



truth snake::SpecialBiteEffect(character* Char, v2, int, int, truth BlockedByArmour)
{
  if(!BlockedByArmour)
  {
    Char->BeginTemporaryState(POISONED, 400 + RAND_N(200));
    return true;
  }
  else
    return false;
}
#endif
