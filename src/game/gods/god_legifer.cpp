#ifdef HEADER_PHASE
GOD(legifer, god)
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



cchar* legifer::GetName() const { return "Legifer"; }



cchar* legifer::GetDescription() const { return "god of law and order"; }



int legifer::GetAlignment() const { return ALP; }



int legifer::GetBasicAlignment() const { return GOOD; }



col16 legifer::GetColor() const { return LAWFUL_BASIC_COLOR; }



col16 legifer::GetEliteColor() const { return LAWFUL_ELITE_COLOR; }



void legifer::PrayGoodEffect () {
  if (!game::GetMasamune()) {
    if (GetRelation() == 1000) {
      ADD_MESSAGE("You hear a booming voice: \"Thou have mastered the arts of law, order, and honor. I grant thee the most sacred symbol of mastery, the Masamune!\" A weapon glowing in power appears from nothing.");
      PLAYER->GetGiftStack()->AddItem(masamune::Spawn());
      game::SetMasamune(1);
      return; //k8
    }
  }
  ADD_MESSAGE("A booming voice echoes: \"Inlux! Inlux! Save us!\" A huge firestorm engulfs everything around you.");
  game::GetCurrentLevel()->Explosion(PLAYER, CONST_S("killed by the holy fire of ") + GetName(), PLAYER->GetPos(), (300 + Max(GetRelation(), 0)) >> 3, false);
}



void legifer::PrayBadEffect()
{
  ADD_MESSAGE("%s casts a beam of horrible, yet righteous, fire on you.", GetName());
  PLAYER->ReceiveDamage(0, 50 + RAND() % 50, FIRE, ALL);
  PLAYER->CheckDeath(CONST_S("burned to death by the wrath of ") + GetName(), 0);
}
#endif
