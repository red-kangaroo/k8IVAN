#ifdef HEADER_PHASE
CHARACTER(oree, humanoid)
{
 public:
  virtual void Bite(character*, v2, int, truth = false);
  virtual void GetAICommand();
 protected:
  virtual cchar* FirstPersonBiteVerb() const;
  virtual cchar* FirstPersonCriticalBiteVerb() const;
  virtual cchar* ThirdPersonBiteVerb() const;
  virtual cchar* ThirdPersonCriticalBiteVerb() const;
  virtual cchar* BiteNoun() const;
  void CallForMonsters();
};


#else



cchar* oree::FirstPersonBiteVerb() const { return "vomit acidous blood at"; }



cchar* oree::FirstPersonCriticalBiteVerb() const { return "vomit very acidous blood at"; }



cchar* oree::ThirdPersonBiteVerb() const { return "vomits acidous blood at"; }



cchar* oree::ThirdPersonCriticalBiteVerb() const { return "vomits very acidous blood at"; }



cchar* oree::BiteNoun() const { return "liquid"; }



void oree::Bite(character* Enemy, v2 HitPos, int, truth)
{
  if(IsPlayer())
    ADD_MESSAGE("You vomit acidous blood at %s.", Enemy->CHAR_DESCRIPTION(DEFINITE));
  else if(Enemy->IsPlayer() || CanBeSeenByPlayer() || Enemy->CanBeSeenByPlayer())
    ADD_MESSAGE("%s vomits acidous blood at %s.", CHAR_DESCRIPTION(DEFINITE), Enemy->CHAR_DESCRIPTION(DEFINITE));

  Vomit(HitPos, 500 + RAND() % 500, false);
}



void oree::GetAICommand()
{
  if(!RAND_N(50))
    CallForMonsters();

  humanoid::GetAICommand();
}



void oree::CallForMonsters()
{
  if(GetDungeon()->GetIndex() != ELPURI_CAVE || GetLevel()->GetIndex() != OREE_LAIR)
    return;

  character* ToBeCalled = 0;

  switch(RAND_N(6))
  {
   case 0:
    ToBeCalled = darkknight::Spawn(ELITE);
    break;
   case 1:
    ToBeCalled = frog::Spawn(RAND_2 ? GREATER_DARK : GIANT_DARK);
    break;
   case 2:
    ToBeCalled = frog::Spawn(DARK);
    break;
   case 3:
    ToBeCalled = darkmage::Spawn(RAND_2 ? APPRENTICE : BATTLE_MAGE);
    break;
   case 4:
    ToBeCalled = darkmage::Spawn(RAND_2 ? APPRENTICE : ELDER);
    break;
   case 5:
    ToBeCalled = necromancer::Spawn(RAND_2 ? APPRENTICE_NECROMANCER : MASTER_NECROMANCER);
    break;
  }

  v2 TryToCreate;

  for(int c = 0; c < 100; ++c)
  {
    TryToCreate = game::GetMonsterPortal()->GetPos() + game::GetMoveVector(RAND() % DIRECTION_COMMAND_KEYS);

    if(GetArea()->IsValidPos(TryToCreate) && ToBeCalled->CanMoveOn(GetNearLSquare(TryToCreate)) && ToBeCalled->IsFreeForMe(GetNearLSquare(TryToCreate)))
    {
      ToBeCalled->SetTeam(game::GetTeam(MONSTER_TEAM));
      ToBeCalled->PutTo(TryToCreate);
      return;
    }
  }

  delete ToBeCalled;
}
#endif
