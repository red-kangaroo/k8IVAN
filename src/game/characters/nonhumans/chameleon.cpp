#ifdef HEADER_PHASE
CHARACTER(chameleon, nonhumanoid)
{
 public:
  virtual int TakeHit(character*, item*, bodypart*, v2, double, double, int, int, int, truth, truth);
  virtual truth SpecialEnemySightedReaction(character*);
 protected:
  virtual col16 GetSkinColor() const;
  virtual void SpecialTurnHandler() { UpdatePictures(); }
};


#else



col16 chameleon::GetSkinColor() const { return MakeRGB16(60 + RAND() % 190, 60 + RAND() % 190, 60 + RAND() % 190); }



truth chameleon::SpecialEnemySightedReaction(character*)
{
  if(HP != MaxHP || !(RAND() % 3))
  {
    character* NewForm = PolymorphRandomly(100, 1000, 500 + RAND() % 500);
    NewForm->GainIntrinsic(POLYMORPH);
    return true;
  }

  return false;
}



int chameleon::TakeHit(character* Enemy, item* Weapon, bodypart* EnemyBodyPart, v2 HitPos, double Damage, double ToHitValue, int Success, int Type, int Direction, truth Critical, truth ForceHit)
{
  int Return = nonhumanoid::TakeHit(Enemy, Weapon, EnemyBodyPart, HitPos, Damage, ToHitValue, Success, Type, Direction, Critical, ForceHit);

  if(Return != HAS_DIED)
  {
    character* NewForm = PolymorphRandomly(100, 1000, 500 + RAND() % 500);
    NewForm->GainIntrinsic(POLYMORPH);
  }

  return Return;
}
#endif
