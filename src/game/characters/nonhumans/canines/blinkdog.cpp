#ifdef HEADER_PHASE
CHARACTER(blinkdog, dog)
{
 public:
  blinkdog();
  virtual void Save(outputfile&) const;
  virtual void Load(inputfile&);
  virtual int TakeHit(character*, item*, bodypart*, v2, double, double, int, int, int, truth, truth);
  virtual truth SpecialEnemySightedReaction(character*);
 protected:
  virtual bodypart* MakeBodyPart(int) const;
  void MonsterTeleport(cchar*);
  truth SummonFriend();
  int SummonModifier;
};


#else



bodypart* blinkdog::MakeBodyPart(int) const { return blinkdogtorso::Spawn(0, NO_MATERIALS); }



truth blinkdog::SpecialEnemySightedReaction(character*)
{
  if(!(RAND() & 15) && SummonFriend())
    return true;

  if(!(RAND() & 31))
  {
    MonsterTeleport("a playful bark");
    return true;
  }

  if((!(RAND() & 3) && StateIsActivated(PANIC))
     || (!(RAND() & 7) && IsInBadCondition()))
  {
    MonsterTeleport("a frightened howl");
    return true;
  }

  return false;
}



void blinkdog::MonsterTeleport(cchar* BarkMsg)
{
       if (IsPlayer()) ADD_MESSAGE("You vanish.");
  else if (CanBeSeenByPlayer()) ADD_MESSAGE("You hear %s inside your head as %s vanishes!", BarkMsg, CHAR_NAME(DEFINITE));
  else ADD_MESSAGE("You hear %s inside your head.", BarkMsg);

  v2 Pos = GetPos();
  rect Border(Pos + v2(-5, -5), Pos + v2(5, 5));
  Pos = GetLevel()->GetRandomSquare(this, 0, &Border);

  if(Pos == ERROR_V2)
    Pos = GetLevel()->GetRandomSquare(this);

  Move(Pos, true);

  if (IsPlayer()) ADD_MESSAGE("You materialize.");
  else if (CanBeSeenByPlayer()) ADD_MESSAGE("%s materializes from nowhere!", CHAR_NAME(INDEFINITE));

  EditAP(-1000);
}



int blinkdog::TakeHit(character* Enemy, item* Weapon, bodypart* EnemyBodyPart, v2 HitPos, double Damage, double ToHitValue, int Success, int Type, int Direction, truth Critical, truth ForceHit)
{
  int Return = nonhumanoid::TakeHit(Enemy, Weapon, EnemyBodyPart, HitPos, Damage, ToHitValue, Success, Type, Direction, Critical, ForceHit);

  if(Return != HAS_DIED)
  {
    if(!(RAND() & 15) && SummonFriend())
      return Return;

    if((RAND() & 1 && StateIsActivated(PANIC))
       || (!(RAND() & 3) && IsInBadCondition())
       || !(RAND() & 15))
    MonsterTeleport("a terrified yelp");
  }

  return Return;
}



truth blinkdog::SummonFriend()
{
  if(!SummonModifier)
    return false;

  --SummonModifier;
  blinkdog* Buddy = blinkdog::Spawn();
  Buddy->SummonModifier = SummonModifier;
  Buddy->SetTeam(GetTeam());
  Buddy->SetGenerationDanger(GetGenerationDanger());
  Buddy->PutNear(GetPos());

  if(CanBeSeenByPlayer())
  {
    ADD_MESSAGE("%s wags its tail in a mysterious pattern.", CHAR_NAME(DEFINITE));

    if(Buddy->CanBeSeenByPlayer())
      ADD_MESSAGE("Another of its kin appears!");
  }
  else if(Buddy->CanBeSeenByPlayer())
    ADD_MESSAGE("%s appears!", Buddy->CHAR_NAME(INDEFINITE));

  EditAP(-1000);
  return true;
}



blinkdog::blinkdog()
{
  if(!game::IsLoading())
    SummonModifier = RAND_2 + RAND_2 + RAND_2 + RAND_2 + RAND_2 + RAND_2 + RAND_2;
}



void blinkdog::Save(outputfile& SaveFile) const
{
  dog::Save(SaveFile);
  SaveFile << SummonModifier;
}



void blinkdog::Load(inputfile& SaveFile)
{
  dog::Load(SaveFile);
  SaveFile >> SummonModifier;
}
#endif
