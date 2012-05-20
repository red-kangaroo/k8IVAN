#ifdef HEADER_PHASE
CHARACTER(lobhse, largecreature)
{
 protected:
  virtual truth SpecialBiteEffect(character*, v2, int, int, truth);
  virtual void GetAICommand();
  virtual void CreateCorpse(lsquare*);
  virtual truth MustBeRemovedFromBone() const;
  virtual bodypart* MakeBodyPart(int) const;
};


#else



bodypart* lobhse::MakeBodyPart(int) const { return lobhsetorso::Spawn(0, NO_MATERIALS); }



truth lobhse::MustBeRemovedFromBone() const
{
  return !IsEnabled() || GetTeam()->GetID() != MONSTER_TEAM || GetDungeon()->GetIndex() != UNDER_WATER_TUNNEL || GetLevel()->GetIndex() != SPIDER_LEVEL;
}



truth lobhse::SpecialBiteEffect(character* Char, v2, int, int, truth BlockedByArmour)
{
  if(!BlockedByArmour)
  {
    Char->BeginTemporaryState(POISONED, 80 + RAND() % 40);
    return true;
  }
  else
    return false;
}



void lobhse::GetAICommand()
{
  if(MoveRandomly())
    return;

  EditAP(-1000);
}



void lobhse::CreateCorpse(lsquare* Square)
{
  largecreature::CreateCorpse(Square);
}
#endif
