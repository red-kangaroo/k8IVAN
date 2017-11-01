#ifdef HEADER_PHASE
CHARACTER(lobhse, largecreature)
{
 public:
  virtual truth IsSpider() const { return true; }
 protected:
  virtual truth SpecialBiteEffect(character*, v2, int, int, truth, truth Critical, int DoneDamage);
  virtual void GetAICommand();
  virtual void CreateCorpse(lsquare*);
  //virtual truth MustBeRemovedFromBone() const;
  virtual bodypart* MakeBodyPart(int) const;
};


#else



bodypart* lobhse::MakeBodyPart(int) const { return lobhsetorso::Spawn(0, NO_MATERIALS); }



truth lobhse::SpecialBiteEffect(character* Char, v2, int, int, truth BlockedByArmour, truth Critical, int DoneDamage)
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
  SeekLeader(GetLeader()); // will follow if tamed
  if (FollowLeader(GetLeader())) return;
  //StandIdleAI(); //k8:??? removed in comm. fork
  if (MoveRandomly()) {
    web *Web = web::Spawn();
    Web->SetStrength(GetConfig() == LARGE ? 50 : 100);
    if (GetLSquareUnder()->AddTrap(Web)) {
      if (CanBeSeenByPlayer()) ADD_MESSAGE("%s spins a web.", CHAR_NAME(DEFINITE));
      EditAP(-1000);
      return;
    }
  }
}



void lobhse::CreateCorpse(lsquare* Square)
{
  largecreature::CreateCorpse(Square);
}
#endif
