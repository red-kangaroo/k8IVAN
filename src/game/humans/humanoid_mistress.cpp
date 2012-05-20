#ifdef HEADER_PHASE
CHARACTER(mistress, humanoid)
{
 public:
  virtual int TakeHit(character*, item*, bodypart*, v2, double, double, int, int, int, truth, truth);
  virtual truth ReceiveDamage(character*, int, int, int = ALL, int = 8, truth = false, truth = false, truth = false, truth = true);
  virtual truth AllowEquipment(citem*, int) const;
};


#else



int mistress::TakeHit(character* Enemy, item* Weapon, bodypart* EnemyBodyPart, v2 HitPos, double Damage, double ToHitValue, int Success, int Type, int Direction, truth Critical, truth ForceHit)
{
  int Return = humanoid::TakeHit(Enemy, Weapon, EnemyBodyPart, HitPos, Damage, ToHitValue, Success, Type, Direction, Critical, ForceHit);

  if(Return == HAS_HIT && Critical)
  {
    if(IsPlayer())
      ADD_MESSAGE("Aahhh. The pain feels unbelievably good.");
    else if(CanBeSeenByPlayer())
      ADD_MESSAGE("%s screams: \"Oh the delightful pain!\"", CHAR_NAME(DEFINITE));
    else
      ADD_MESSAGE("You hear someone screaming: \"Oh the delightful pain!\"");
  }

  return Return;
}



truth mistress::ReceiveDamage(character* Damager, int Damage, int Type, int TargetFlags, int Direction, truth Divide, truth PenetrateArmor, truth Critical, truth ShowMsg)
{
  truth Success = humanoid::ReceiveDamage(Damager, Damage, Type, TargetFlags, Direction, Divide, PenetrateArmor, Critical, ShowMsg);

  if(Type & SOUND && Success && !(RAND() & 7))
  {
    if(IsPlayer())
      ADD_MESSAGE("Aahhh. The pain feels unbelievably good.");
    else if(CanBeSeenByPlayer())
      ADD_MESSAGE("%s screams: \"Oh the delightful pain!\"", CHAR_NAME(DEFINITE));
    else
      ADD_MESSAGE("You hear someone screaming: \"Oh the delightful pain!\"");
  }

  return Success;
}



truth mistress::AllowEquipment(citem* Item, int EquipmentIndex) const
{
  return ((EquipmentIndex != RIGHT_WIELDED_INDEX
     && EquipmentIndex != LEFT_WIELDED_INDEX)
    || Item->IsWhip());
}
#endif
