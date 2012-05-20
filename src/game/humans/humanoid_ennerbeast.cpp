#ifdef HEADER_PHASE
CHARACTER(ennerbeast, humanoid)
{
 public:
  virtual truth Hit(character*, v2, int, int = 0);
  virtual truth MustBeRemovedFromBone() const;
 protected:
  virtual bodypart* MakeBodyPart(int) const;
  virtual void GetAICommand();
  virtual truth AttackIsBlockable(int) const { return false; }
};


#else



truth ennerbeast::Hit(character* Enemy, v2, int, int)
{
  if(CheckIfTooScaredToHit(Enemy))
    return false;

  if(RAND() & 1)
    ADD_MESSAGE("%s yells: UGH UGHAaaa!", CHAR_DESCRIPTION(DEFINITE));
  else
    ADD_MESSAGE("%s yells: Uga Ugar Ugade Ugat!", CHAR_DESCRIPTION(DEFINITE));

  rect Rect;
  femath::CalculateEnvironmentRectangle(Rect, GetLevel()->GetBorder(), GetPos(), 30);

  for(int x = Rect.X1; x <= Rect.X2; ++x)
    for(int y = Rect.Y1; y <= Rect.Y2; ++y)
    {
      int ScreamStrength = int(70 / (hypot(GetPos().X - x, GetPos().Y - y) + 1));

      if(ScreamStrength)
      {
  character* Char = GetNearSquare(x, y)->GetCharacter();

  if(Char && Char != this)
  {
    msgsystem::EnterBigMessageMode();

    if(Char->IsPlayer())
      ADD_MESSAGE("You are hit by the horrible waves of high sound.");
    else if(Char->CanBeSeenByPlayer())
      ADD_MESSAGE("%s is hit by the horrible waves of high sound.", Char->CHAR_NAME(DEFINITE));

    Char->ReceiveDamage(this, ScreamStrength, SOUND, ALL, YOURSELF, true);
    Char->CheckDeath(CONST_S("killed @bkp scream"), this);
    msgsystem::LeaveBigMessageMode();
  }

  GetNearLSquare(x, y)->GetStack()->ReceiveDamage(this, ScreamStrength, SOUND);
      }
    }

  EditNP(-100);
  EditAP(-1000000 / GetCWeaponSkill(BITE)->GetBonus());
  EditStamina(-1000, false);
  return true;
}



void ennerbeast::GetAICommand()
{
  SeekLeader(GetLeader());

  if(StateIsActivated(PANIC) || !(RAND() % 3))
    Hit(0, ZERO_V2, YOURSELF);

  if(CheckForEnemies(false, false, true))
    return;

  if(FollowLeader(GetLeader()))
    return;

  if(MoveRandomly())
    return;

  EditAP(-1000);
}



truth ennerbeast::MustBeRemovedFromBone() const
{
  return !IsEnabled() || GetTeam()->GetID() != MONSTER_TEAM || GetDungeon()->GetIndex() != ELPURI_CAVE || GetLevel()->GetIndex() != ENNER_BEAST_LEVEL;
}



bodypart* ennerbeast::MakeBodyPart(int I) const
{
  if(I == HEAD_INDEX)
    return ennerhead::Spawn(0, NO_MATERIALS);
  else
    return humanoid::MakeBodyPart(I);
}
#endif
