#ifdef HEADER_PHASE
CHARACTER(floatingeye, nonhumanoid)
{
 public:
  floatingeye() : NextWayPoint(0) { }
  virtual truth Hit(character*, v2, int, int = 0);
  virtual int TakeHit(character*, item*, bodypart*, v2, double, double, int, int, int, truth, truth);
  virtual void SetWayPoints(const fearray<packv2>&);
  virtual void Save(outputfile&) const;
  virtual void Load(inputfile&);
  virtual truth IsSpy() const { return true; }
  virtual truth CanAttack() const { return false; }
 protected:
  virtual void GetAICommand();
  std::vector<v2> WayPoints;
  uInt NextWayPoint;
};


#else



void floatingeye::SetWayPoints(const fearray<packv2>& What) { ArrayToVector(What, WayPoints); }



void floatingeye::Save(outputfile& SaveFile) const
{
  nonhumanoid::Save(SaveFile);
  SaveFile << WayPoints << NextWayPoint;
}



void floatingeye::Load(inputfile& SaveFile)
{
  nonhumanoid::Load(SaveFile);
  SaveFile >> WayPoints >> NextWayPoint;
}



void floatingeye::GetAICommand()
{
  if(WayPoints.size() && !IsGoingSomeWhere())
  {
    if(GetPos() == WayPoints[NextWayPoint]) {
      if(NextWayPoint < WayPoints.size() - 1) ++NextWayPoint; else NextWayPoint = 0;
    }

    GoingTo = WayPoints[NextWayPoint];
  }

  SeekLeader(GetLeader());

  if(CheckForEnemies(false, false, true))
    return;

  if(FollowLeader(GetLeader()))
    return;

  if(MoveRandomly())
    return;

  EditAP(-1000);
}



truth floatingeye::Hit(character* Enemy, v2, int, int)
{
  if(IsPlayer())
    ADD_MESSAGE("You stare at %s.", Enemy->CHAR_DESCRIPTION(DEFINITE));
  else if(Enemy->IsPlayer() && CanBeSeenByPlayer())
    ADD_MESSAGE("%s stares at you.", CHAR_NAME(DEFINITE));

  EditAP(-1000);
  return true;
}



int floatingeye::TakeHit(character* Enemy, item* Weapon, bodypart* EnemyBodyPart, v2 HitPos, double Damage, double ToHitValue, int Success, int Type, int Direction, truth Critical, truth ForceHit)
{
  if(CanBeSeenBy(Enemy) && Enemy->HasEyes() && RAND() % 3 && Enemy->LoseConsciousness(150 + RAND_N(150))) /* Changes for fainting 2 out of 3 */
  {
    if(!Enemy->IsPlayer())
      Enemy->EditExperience(WISDOM, 75, 1 << 13);

    return HAS_FAILED;
  }
  else
    return nonhumanoid::TakeHit(Enemy, Weapon, EnemyBodyPart, HitPos, Damage, ToHitValue, Success, Type, Direction, Critical, ForceHit);
}
#endif
