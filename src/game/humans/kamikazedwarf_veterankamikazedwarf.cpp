#ifdef HEADER_PHASE
CHARACTER(veterankamikazedwarf, kamikazedwarf)
{
 protected:
  virtual void PostConstruct();
  virtual int GetTorsoMainColor() const;
  virtual int GetGauntletColor() const;
  virtual int GetLegMainColor() const;
  virtual int GetWSkillHits() const { return 50000; }
  virtual truth IsElite() const { return true; }
};


#else



col16 veterankamikazedwarf::GetTorsoMainColor() const
{
  return GetMasterGod()->GetEliteColor();
}



col16 veterankamikazedwarf::GetGauntletColor() const
{
  return GetMasterGod()->GetEliteColor();
}



col16 veterankamikazedwarf::GetLegMainColor() const
{
  return GetMasterGod()->GetEliteColor();
}



void veterankamikazedwarf::PostConstruct()
{
  kamikazedwarf::PostConstruct();
  ivantime Time;
  game::GetTime(Time);
  int Modifier = Time.Day - KAMIKAZE_INVISIBILITY_DAY_MIN;

  if(Time.Day >= KAMIKAZE_INVISIBILITY_DAY_MAX
     || (Modifier > 0
   && RAND_N(KAMIKAZE_INVISIBILITY_DAY_MAX - KAMIKAZE_INVISIBILITY_DAY_MIN) < Modifier))
    GainIntrinsic(INVISIBLE);
}
#endif
