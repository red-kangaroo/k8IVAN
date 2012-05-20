#ifdef HEADER_PHASE
CHARACTER(bunny, nonhumanoid)
{
 public:
  virtual truth CheckIfSatiated() { return GetNP() > BLOATED_LEVEL; }
  virtual void SignalNaturalGeneration();
  virtual truth IsBunny() const { return true; }
  virtual truth Catches(item*);
 protected:
  truth CheckForMatePartner();
  virtual void GetAICommand();
};


#else



void bunny::GetAICommand()
{
  if(GetConfig() < 4 && GetNP() > (SATIATED_LEVEL + BLOATED_LEVEL) >> 1)
  {
    if(CanBeSeenByPlayer())
      ADD_MESSAGE("%s looks more mature.", CHAR_NAME(DEFINITE));

    GetTorso()->SetSize(GetTorso()->GetSize() << 1);
    LimitRef(StrengthExperience *= 2, MIN_EXP, MAX_EXP);
    LimitRef(AgilityExperience *= 2, MIN_EXP, MAX_EXP);

    for(int c = 0; c < BASE_ATTRIBUTES; ++c)
      BaseExperience[c] = Limit(BaseExperience[c] * 2, MIN_EXP, MAX_EXP);

    GetTorso()->GetMainMaterial()->SetVolume(GetTorso()->GetMainMaterial()->GetVolume() << 1);
    SetConfig(GetConfig() + 2);
    RestoreHP();
    RestoreStamina();
  }

  SeekLeader(GetLeader());

  if(FollowLeader(GetLeader()))
    return;

  if(CheckForEnemies(true, true, true))
    return;

  if(CheckForUsefulItemsOnGround())
    return;

  if(CheckForDoors())
    return;

  if(CheckForFood(5))
    return;

  if(CheckForMatePartner())
    return;

  if(MoveRandomly())
    return;

  EditAP(-1000);
}



void bunny::SignalNaturalGeneration()
{
  character* Partner = bunny::Spawn(GetConfig()^1);
  Partner->SetTeam(GetTeam());
  Partner->SetGenerationDanger(GetGenerationDanger());
  Partner->PutNear(GetPos());
}



truth bunny::CheckForMatePartner()
{
  if(GetConfig() == ADULT_MALE)
  {
    character* BestPartner = 0;
    double BestPartnerDanger = 0;

    for(int c = 0; c < game::GetTeams(); ++c)
      if(GetTeam()->GetRelation(game::GetTeam(c)) != HOSTILE)
  for(std::list<character*>::const_iterator i = game::GetTeam(c)->GetMember().begin(); i != game::GetTeam(c)->GetMember().end(); ++i)
    if((*i)->IsEnabled() && (*i)->IsBunny() && (*i)->GetConfig() == ADULT_FEMALE && (*i)->GetNP() > SATIATED_LEVEL)
    {
      double Danger = (*i)->GetRelativeDanger(this, true);

      if(Danger > BestPartnerDanger)
      {
        BestPartner = *i;
        BestPartnerDanger = Danger;
      }
    }

    if(BestPartner && !GetPos().IsAdjacent(BestPartner->GetPos()))
    {
      SetGoingTo(BestPartner->GetPos());
      MoveTowardsTarget(true);
      return true;
    }
  }

  if(GetConfig() == ADULT_FEMALE && GetNP() > NOT_HUNGER_LEVEL + 10000)
  {
    for(int d = 0; d < GetNeighbourSquares(); ++d)
    {
      lsquare* Square = GetNeighbourLSquare(d);

      if(Square)
      {
  character* Father = Square->GetCharacter();

  if(Father && Father->IsBunny() && Father->GetConfig() == ADULT_MALE && GetRelation(Father) != HOSTILE)
  {
    if(CanBeSeenByPlayer())
    {
      if(Father->IsPlayer())
        ADD_MESSAGE("You have much fun with %s.", CHAR_NAME(DEFINITE));
      else if(Father->CanBeSeenByPlayer())
        ADD_MESSAGE("%s and %s seem to have much fun together.", Father->CHAR_NAME(DEFINITE), CHAR_NAME(DEFINITE));
      else
        ADD_MESSAGE("%s seems to have much fun.", CHAR_NAME(DEFINITE));
    }
    else
    {
      if(Father->IsPlayer())
        ADD_MESSAGE("You have much fun with something.");
      else if(Father->CanBeSeenByPlayer())
        ADD_MESSAGE("%s seems to have much fun.", Father->CHAR_NAME(DEFINITE));
    }

    bunny* Baby = bunny::Spawn(BABY_MALE + (RAND() & 1));
    Baby->StrengthExperience = RandomizeBabyExperience(StrengthExperience + static_cast<bunny*>(Father)->StrengthExperience);
    Baby->AgilityExperience = RandomizeBabyExperience(AgilityExperience + static_cast<bunny*>(Father)->AgilityExperience);

    if(Baby->GetConfig() == BABY_MALE)
    {
      Baby->StrengthExperience *= 4;
      Baby->AgilityExperience *= 4;
    }
    else
    {
      Baby->StrengthExperience *= 2;
      Baby->AgilityExperience *= 6;
    }

    Baby->StrengthExperience /= 3;
    Baby->AgilityExperience /= 5;

    for(int c = 0; c < BASE_ATTRIBUTES; ++c)
      Baby->BaseExperience[c] = RandomizeBabyExperience(BaseExperience[c] + static_cast<bunny*>(Father)->BaseExperience[c]);

    Baby->CalculateAll();
    Baby->RestoreHP();
    Baby->RestoreStamina();
    Baby->SetTeam(GetTeam());
    Baby->SetGenerationDanger(GetGenerationDanger());
    Baby->PutNear(GetPos());

    if(Baby->CanBeSeenByPlayer())
      ADD_MESSAGE("%s is born.", Baby->CHAR_NAME(INDEFINITE));

    EditNP(-10000);
    Father->EditAP(-3000);
    EditAP(-5000);
    EditStamina(-GetMaxStamina() >> 1, true);
    Father->EditStamina(-(Father->GetMaxStamina() << 2) / 5, true);
    return true;
  }
      }
    }
  }

  return false;
}



truth bunny::Catches(item* Thingy)
{
  if(Thingy->BunnyWillCatchAndConsume(this))
  {
    if(ConsumeItem(Thingy, CONST_S("eating")))
    {
      if(IsPlayer())
  ADD_MESSAGE("You catch %s in mid-air and consume it.", Thingy->CHAR_NAME(DEFINITE));
      else
      {
  if(CanBeSeenByPlayer())
    ADD_MESSAGE("%s catches %s and eats it.", CHAR_NAME(DEFINITE), Thingy->CHAR_NAME(DEFINITE));

  ChangeTeam(PLAYER->GetTeam());
      }
    }
    else if(IsPlayer())
      ADD_MESSAGE("You catch %s in mid-air.", Thingy->CHAR_NAME(DEFINITE));
    else if(CanBeSeenByPlayer())
      ADD_MESSAGE("%s catches %s.", CHAR_NAME(DEFINITE), Thingy->CHAR_NAME(DEFINITE));

    return true;
  }
  else
    return false;
}
#endif
