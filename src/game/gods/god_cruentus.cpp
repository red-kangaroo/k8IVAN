#ifdef HEADER_PHASE
GOD(cruentus, god)
{
 public:
  virtual cchar* GetName() const;
  virtual cchar* GetDescription() const;
  virtual int GetAlignment() const;
  virtual int GetBasicAlignment() const;
  virtual col16 GetColor() const;
  virtual col16 GetEliteColor() const;
  virtual int GetSex() const { return MALE; }
 protected:
  virtual void PrayGoodEffect();
  virtual void PrayBadEffect();
};


#else



cchar* cruentus::GetName() const { return "Cruentus"; }



cchar* cruentus::GetDescription() const { return "god of war and blood"; }



int cruentus::GetAlignment() const { return ACM; }



int cruentus::GetBasicAlignment() const { return EVIL; }



col16 cruentus::GetColor() const { return CHAOS_BASIC_COLOR; }



col16 cruentus::GetEliteColor() const { return CHAOS_ELITE_COLOR; }



void cruentus::PrayGoodEffect()
{
  if (!game::GetMuramasa()) {
    if (GetRelation() == 1000) {
      ADD_MESSAGE("The air around you becomes as warm as blood. A voice booms: \"Thou have pleased me greatly, lift and behold: thy most sacred reward, the Muramasa\" A weapon of pure corruption materializes before you.");
      PLAYER->GetGiftStack()->AddItem(muramasa::Spawn());
      game::SetMuramasa(1);
    }
  }

  rect Rect;
  femath::CalculateEnvironmentRectangle(Rect, game::GetCurrentLevel()->GetBorder(), PLAYER->GetPos(), 10);
  truth AudiencePresent = false;

  for(int x = Rect.X1; x <= Rect.X2; ++x)
  {
    for(int y = Rect.Y1; y <= Rect.Y2; ++y)
    {
      character* Audience = game::GetCurrentLevel()->GetSquare(x, y)->GetCharacter();

      if(Audience && Audience->CanBeSeenByPlayer() && !Audience->TemporaryStateIsActivated(PANIC) && PLAYER->GetRelation(Audience) == HOSTILE)
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
    ADD_MESSAGE("The thundering voice of a godly battle drum shakes everything around you.");

    for(int x = Rect.X1; x <= Rect.X2; ++x)
      for(int y = Rect.Y1; y <= Rect.Y2; ++y)
      {
  character* Audience = game::GetCurrentLevel()->GetSquare(x, y)->GetCharacter();

  if(Audience
     && !Audience->TemporaryStateIsActivated(PANIC)
     && PLAYER->GetRelation(Audience) == HOSTILE
     && Audience->GetPanicLevel() > RAND() % 33)
    Audience->BeginTemporaryState(PANIC, 500 + RAND() % 500);
      }

    return;
  }

  item* Weapon = PLAYER->GetMainWielded();

  if(!Weapon || !Weapon->IsWeapon(PLAYER))
    Weapon = PLAYER->GetSecondaryWielded();

  if(Weapon && Weapon->IsWeapon(PLAYER) && Weapon->CanBeEnchanted()
     && Weapon->GetEnchantment() < 5 && !(RAND() % 10))
  {
    ADD_MESSAGE("Your %s glows briefly red. It feels very warm now.", Weapon->CHAR_NAME(UNARTICLED));
    Weapon->EditEnchantment(1);
  }
  else if(RAND() & 3)
  {
    potion* Bottle = potion::Spawn(0, NO_MATERIALS);
    Bottle->InitMaterials(MAKE_MATERIAL(GLASS), MAKE_MATERIAL(TROLL_BLOOD));
    PLAYER->GetGiftStack()->AddItem(Bottle);
    ADD_MESSAGE("%s drops from nowhere.", Bottle->CHAR_DESCRIPTION(DEFINITE));
  }
  else
  {
    ADD_MESSAGE("Cruentus recommends you to his master, Mortifer.");
    game::GetGod(MORTIFER)->AdjustRelation(100);
  }
}



void cruentus::PrayBadEffect()
{
  item* ToBe = PLAYER->GetMainWielded();

  if(ToBe)
  {
    if(!ToBe->IsDestroyable(0))
    {
      ToBe = PLAYER->GetSecondaryWielded();

      if(!ToBe || !ToBe->IsDestroyable(0))
  ADD_MESSAGE("%s tries to destroy your %s, but fails.", GetName(), PLAYER->GetMainWielded()->CHAR_NAME(UNARTICLED));
    }
  }
  else
  {
    ToBe = PLAYER->GetSecondaryWielded();

    if(ToBe && !ToBe->IsDestroyable(0))
      ADD_MESSAGE("%s tries to destroy your %s, but fails.", GetName(), ToBe->CHAR_NAME(UNARTICLED));
  }

  if(ToBe && ToBe->IsDestroyable(0))
  {
    ADD_MESSAGE("%s destroys your weapon.", GetName());
    ToBe->RemoveFromSlot();
    ToBe->SendToHell();
  }
  else
  {
    ADD_MESSAGE("%s gets mad and hits you!", GetName());
    PLAYER->ReceiveDamage(0, 1 + RAND() % 30, PHYSICAL_DAMAGE, ALL, RAND() & 7);
    PLAYER->CheckDeath(CONST_S("destroyed by ") + GetName(), 0);
  }
}
#endif
