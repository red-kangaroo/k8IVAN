#ifdef HEADER_PHASE
GOD(sophos, god)
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



cchar* sophos::GetName() const { return "Sophos"; }



cchar* sophos::GetDescription() const { return "god of knowledge, magic and handicrafts"; }



int sophos::GetAlignment() const { return ALM; }



int sophos::GetBasicAlignment() const { return GOOD; }



col16 sophos::GetColor() const { return LAWFUL_BASIC_COLOR; }



col16 sophos::GetEliteColor() const { return LAWFUL_ELITE_COLOR; }



void sophos::PrayGoodEffect()
{
  ADD_MESSAGE("Suddenly, the fabric of space experiences an unnaturally powerful quantum displacement! You teleport away!");
  PLAYER->Move(game::GetCurrentLevel()->GetRandomSquare(PLAYER), true);
}



void sophos::PrayBadEffect()
{
  ADD_MESSAGE("Suddenly, the fabric of space experiences an unnaturally powerful quantum displacement!");
  PLAYER->TeleportSomePartsAway(1 + (RAND() & 1));
  PLAYER->CheckDeath(CONST_S("shattered to pieces by the wrath of ") + GetName(), 0);
}
#endif
