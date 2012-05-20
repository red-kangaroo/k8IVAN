#ifdef HEADER_PHASE
ITEM(horn, item)
{
 public:
  horn() : LastUsed(0) { }
  virtual void Load(inputfile&);
  virtual void Save(outputfile&) const;
  virtual truth Apply(character*);
  virtual truth IsAppliable(ccharacter*) const { return true; }
  virtual void FinalProcessForBone();
 protected:
  feuLong LastUsed;
};


#else



truth horn::Apply(character* Blower)
{
  if(!Blower->HasHead())
  {
    if(Blower->IsPlayer())
      ADD_MESSAGE("You need a head to do this.");

    return false;
  }

  if(!LastUsed || game::GetTick() - LastUsed >= 2500)
  {
    LastUsed = game::GetTick();
    //cchar* SoundDescription = GetConfig() == BRAVERY ? "loud but calming" : "frightening, almost scream-like";
    cchar* SoundDescription;
    //
    switch (GetConfig()) {
      case BRAVERY: SoundDescription = "loud but calming"; break;
      case FEAR: SoundDescription = "frightening, almost scream-like"; break;
      case CONFUSION: SoundDescription = "strange and dissonant"; break;
      default: SoundDescription = "never-before heard"; break;
    }

    if(Blower->IsPlayer())
    {
      if(Blower->CanHear())
  ADD_MESSAGE("You produce a %s sound.", SoundDescription);
      else
  ADD_MESSAGE("You blow %s.", CHAR_NAME(DEFINITE));
    }
    else if(Blower->CanBeSeenByPlayer())
    {
      if(PLAYER->CanHear())
  ADD_MESSAGE("%s blows %s and produces a %s sound.", Blower->CHAR_NAME(DEFINITE), CHAR_NAME(DEFINITE), SoundDescription);
      else
  ADD_MESSAGE("%s blows %s.", Blower->CHAR_NAME(DEFINITE), CHAR_NAME(DEFINITE));
    }
    else if(PLAYER->CanHear())
      ADD_MESSAGE("You hear a %s sound echoing everywhere.", SoundDescription);

    rect Rect;
    femath::CalculateEnvironmentRectangle(Rect, GetLevel()->GetBorder(), GetPos(), 10);

    for(int x = Rect.X1; x <= Rect.X2; ++x)
      for(int y = Rect.Y1; y <= Rect.Y2; ++y)
      {
  character* Audience = GetNearSquare(x, y)->GetCharacter();

  if(Audience)
  {
    if(GetConfig() == BRAVERY && Audience->CanHear() && Audience->TemporaryStateIsActivated(PANIC)
       && Blower->IsAlly(Audience))
    {
      if(Audience->IsPlayer())
        ADD_MESSAGE("You calm down.");
      else if(CanBeSeenByPlayer())
        ADD_MESSAGE("%s calms down.", Audience->CHAR_NAME(DEFINITE));

      Audience->DeActivateTemporaryState(PANIC);
    }
    else if(GetConfig() == FEAR && !Audience->TemporaryStateIsActivated(PANIC) &&
            Blower->GetRelation(Audience) == HOSTILE && Audience->HornOfFearWorks())
      Audience->BeginTemporaryState(PANIC, 500 + RAND() % 500);
    else if(GetConfig() == CONFUSION && Blower->GetRelation(Audience) == HOSTILE && Audience->CanHear())
      Audience->BeginTemporaryState(CONFUSED, 500 + RAND() % 500);
  }
      }

  }
  else
  {
    if(Blower->IsPlayer())
    {
      if(Blower->CanHear())
  ADD_MESSAGE("You produce a mighty sound.");
      else
  ADD_MESSAGE("You blow %s.", CHAR_NAME(DEFINITE));
    }
    else if(Blower->CanBeSeenByPlayer())
    {
      if(PLAYER->CanHear())
  ADD_MESSAGE("%s blows %s and produces a mighty sound.", Blower->CHAR_NAME(DEFINITE), CHAR_NAME(DEFINITE));
      else
  ADD_MESSAGE("%s blows %s.", Blower->CHAR_NAME(DEFINITE), CHAR_NAME(DEFINITE));
    }
    else if(PLAYER->CanHear())
      ADD_MESSAGE("You hear a horn being blown.");
  }

  game::CallForAttention(GetPos(), 900);
  Blower->EditAP(-1000);
  return true;
}



void horn::Save(outputfile& SaveFile) const
{
  item::Save(SaveFile);
  SaveFile << LastUsed;
}



void horn::Load(inputfile& SaveFile)
{
  item::Load(SaveFile);
  SaveFile >> LastUsed;
}



void horn::FinalProcessForBone()
{
  item::FinalProcessForBone();
  LastUsed = 0;
}
#endif
