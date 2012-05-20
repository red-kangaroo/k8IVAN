#ifdef HEADER_PHASE
GOD(nefas, god)
{
 public:
  virtual cchar* GetName() const;
  virtual cchar* GetDescription() const;
  virtual int GetAlignment() const;
  virtual int GetBasicAlignment() const;
  virtual col16 GetColor() const;
  virtual col16 GetEliteColor() const;
  virtual int GetSex() const { return FEMALE; }
 protected:
  virtual void PrayGoodEffect();
  virtual void PrayBadEffect();
};


#else



cchar* nefas::GetName() const { return "Nefas"; }



cchar* nefas::GetDescription() const { return "goddess of forbidden pleasures"; }



int nefas::GetAlignment() const { return AC; }



int nefas::GetBasicAlignment() const { return EVIL; }



col16 nefas::GetColor() const { return CHAOS_BASIC_COLOR; }



col16 nefas::GetEliteColor() const { return CHAOS_ELITE_COLOR; }



void nefas::PrayGoodEffect()
{
  rect Rect;
  femath::CalculateEnvironmentRectangle(Rect, game::GetCurrentLevel()->GetBorder(), PLAYER->GetPos(), 10);
  truth AudiencePresent = false;

  for(int x = Rect.X1; x <= Rect.X2; ++x)
  {
    for(int y = Rect.Y1; y <= Rect.Y2; ++y)
    {
      character* Audience = game::GetCurrentLevel()->GetSquare(x, y)->GetCharacter();

      if(Audience && Audience->CanBeSeenByPlayer() && !Audience->TemporaryStateIsActivated(CONFUSED) && Audience->CanBeConfused() && PLAYER->GetRelation(Audience) == HOSTILE)
      {
  AudiencePresent = true;
  break;
      }
    }

    if(AudiencePresent)
      break;
  }

  if(AudiencePresent)
  {
    for(int x = Rect.X1; x <= Rect.X2; ++x)
      for(int y = Rect.Y1; y <= Rect.Y2; ++y)
      {
  character* Audience = game::GetCurrentLevel()->GetSquare(x, y)->GetCharacter();

  if(Audience && !Audience->TemporaryStateIsActivated(CONFUSED) && Audience->CanBeConfused() && PLAYER->GetRelation(Audience) == HOSTILE)
  {
    if(Audience->CanBeSeenByPlayer())
      ADD_MESSAGE("%s confuses %s with her sweet lies.", GetName(), Audience->CHAR_NAME(DEFINITE));

    Audience->BeginTemporaryState(CONFUSED, 500 + RAND() % 500);
  }
      }
  }

  mistress* Mistress = mistress::Spawn(RAND() & 7 ? 0 : TORTURING_CHIEF);
  v2 Where = game::GetCurrentLevel()->GetNearestFreeSquare(Mistress, PLAYER->GetPos());

  if(Where == ERROR_V2)
  {
    if(PLAYER->CanHear())
      ADD_MESSAGE("You hear a strange scream from somewhere beneath.");
    else
      ADD_MESSAGE("You feel the air vibrating.");

    delete Mistress;
  }
  else
  {
    Mistress->SetTeam(PLAYER->GetTeam());
    Mistress->PutTo(Where);
    ADD_MESSAGE("You hear a sweet voice inside your head: \"Have fun, mortal!\"");
  }
}



void nefas::PrayBadEffect()
{
  ADD_MESSAGE("A potion drops on your head and shatters into small bits.");
  PLAYER->ReceiveDamage(0, 2 + RAND() % 7, PHYSICAL_DAMAGE, HEAD);
  PLAYER->GetStackUnder()->AddItem(brokenbottle::Spawn());
  PLAYER->CheckDeath(CONST_S("killed while enjoying the company of ") + GetName(), 0);
}
#endif
