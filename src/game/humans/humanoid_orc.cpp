#ifdef HEADER_PHASE
CHARACTER(orc, humanoid)
{
 protected:
  virtual void PostConstruct();
};


#else



void orc::PostConstruct()
{
  if(!RAND_N(25))
    GainIntrinsic(LEPROSY);
}
#endif
