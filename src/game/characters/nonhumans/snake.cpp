#ifdef HEADER_PHASE
CHARACTER(snake, nonhumanoid)
{
 protected:
  virtual truth SpecialBiteEffect(character*, v2, int, int, truth, truth Critical, int DoneDamage);
};


#else



truth snake::SpecialBiteEffect(character* Char, v2, int, int, truth BlockedByArmour, truth Critical, int DoneDamage)
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
