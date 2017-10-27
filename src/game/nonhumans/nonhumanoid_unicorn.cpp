#ifdef HEADER_PHASE
CHARACTER(unicorn, nonhumanoid)
{
 public:
  virtual int TakeHit(character*, item*, bodypart*, v2, double, double, int, int, int, truth, truth);
  virtual truth SpecialEnemySightedReaction(character*);
 protected:
  void MonsterTeleport(cchar*);
};


#else



truth unicorn::SpecialEnemySightedReaction(character*)
{
  if(!(RAND() & 15))
  {
    MonsterTeleport("neighs happily");
    return true;
  }

  if(StateIsActivated(PANIC) || (RAND() & 1 && IsInBadCondition()))
  {
    MonsterTeleport("neighs");
    return true;
  }

  if(!(RAND() % 3) && MoveRandomly())
    return true;

  return false;
}



int unicorn::TakeHit(character* Enemy, item* Weapon, bodypart* EnemyBodyPart, v2 HitPos, double Damage, double ToHitValue, int Success, int Type, int Direction, truth Critical, truth ForceHit)
{
  int Return = nonhumanoid::TakeHit(Enemy, Weapon, EnemyBodyPart, HitPos, Damage, ToHitValue, Success, Type, Direction, Critical, ForceHit);

  if(Return != HAS_DIED
     && (StateIsActivated(PANIC)
   || (RAND() & 1 && IsInBadCondition())
   || !(RAND() & 7)))
    MonsterTeleport(" in terror");

  return Return;
}



void unicorn::MonsterTeleport(cchar* NeighMsg)
{
       if (IsPlayer()) ADD_MESSAGE("You neigh%s and disappear.", NeighMsg);
  else if (CanBeSeenByPlayer()) ADD_MESSAGE("%s neighs%s and disappears!", CHAR_NAME(DEFINITE), NeighMsg);

  Move(GetLevel()->GetRandomSquare(this), true);

       if (IsPlayer()) ADD_MESSAGE("You reappear.");
  else if(CanBeSeenByPlayer()) ADD_MESSAGE("Suddenly %s appears from nothing!", CHAR_NAME(INDEFINITE));

  EditAP(-1000);
}
#endif
