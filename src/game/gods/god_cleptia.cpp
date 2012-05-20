#ifdef HEADER_PHASE
GOD(cleptia, god)
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



cchar* cleptia::GetName() const { return "Cleptia"; }



cchar* cleptia::GetDescription() const { return "goddess of assassins and thieves"; }



int cleptia::GetAlignment() const { return ACP; }



int cleptia::GetBasicAlignment() const { return EVIL; }



col16 cleptia::GetColor() const { return CHAOS_BASIC_COLOR; }



col16 cleptia::GetEliteColor() const { return CHAOS_ELITE_COLOR; }



void cleptia::PrayGoodEffect()
{
  PLAYER->RestoreStamina();

  if(!PLAYER->StateIsActivated(HASTE))
  {
    ADD_MESSAGE("%s gives you the talent for speed.", GetName());
    PLAYER->BeginTemporaryState(HASTE, 2500);
    return;
  }

  if(!PLAYER->StateIsActivated(INVISIBLE))
  {
    ADD_MESSAGE("%s helps you to avoid your enemies by making you invisible.", GetName());
    PLAYER->BeginTemporaryState(INVISIBLE, 2500);
    return;
  }

  ADD_MESSAGE("Cleptia helps you, but you really don't know how.");
  int StateToActivate = RAND() & 1 ? HASTE : INVISIBLE;
  PLAYER->BeginTemporaryState(StateToActivate, 2500);
}



void cleptia::PrayBadEffect()
{
  ADD_MESSAGE("%s slows you down.", GetName());
  PLAYER->BeginTemporaryState(SLOW, 250);
}
#endif
