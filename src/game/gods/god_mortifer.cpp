#ifdef HEADER_PHASE
GOD(mortifer, god)
{
public:
  virtual cchar *GetName () const;
  virtual cchar *GetDescription () const;
  virtual int GetAlignment () const;
  virtual void Pray ();
  virtual int GetBasicAlignment () const;
  virtual col16 GetColor () const;
  virtual col16 GetEliteColor () const;
  virtual int GetSex () const { return MALE; }

protected:
  virtual void PrayGoodEffect ();
  virtual void PrayBadEffect ();
};


#else


cchar *mortifer::GetName () const { return "Mortifer"; }
cchar *mortifer::GetDescription () const { return "Destroyer of Worlds"; }
int mortifer::GetAlignment () const { return ACMM; }
int mortifer::GetBasicAlignment () const { return EVIL; }
col16 mortifer::GetColor () const { return CHAOS_BASIC_COLOR; }
col16 mortifer::GetEliteColor () const { return CHAOS_ELITE_COLOR; }


void mortifer::PrayGoodEffect () {
  ADD_MESSAGE("The air vibrates violently around you. A terrible undead voice echoes through the caverns: \"SlAvE! ThOu HaSt PlAeSeD mE! lIfT tHy ReWaRd, ChAmPiOn!\" A heavy weapon of pure corruption materializes before you.");
  PLAYER->GetGiftStack()->AddItem(neercseulb::Spawn());
}


void mortifer::PrayBadEffect () {
  ADD_MESSAGE("A dark, booming voice shakes the area: \"PuNy MoRtAl! ThOu ArT nOt WoRtHy! I sHaLl dEsTrOy ThEe LiKe EvErYoNe ElSe!\" A bolt of black energy hits you.");
  PLAYER->ReceiveDamage(0, 1+RAND()%20, ENERGY, ALL);
  PLAYER->EditAttribute(AGILITY, -1);
  PLAYER->EditAttribute(ARM_STRENGTH, -1);
  PLAYER->EditAttribute(ENDURANCE, -1);
  PLAYER->CheckDeath(CONST_S("obliterated by the unholy power of ") + GetName(), 0);
}


void mortifer::Pray () {
  if (!Timer && Relation == 1000) {
    ADD_MESSAGE("You feel %s is very pleased.", GetName());
    PrayGoodEffect();
    AdjustTimer(100000);
    AdjustRelation(-500);
    game::ApplyDivineAlignmentBonuses(this, 100, true);
    PLAYER->EditExperience(WISDOM, 400, 1 << 11);
    if (Relation > 250 && !(RAND() % 2)) {
      character *Angel = CreateAngel(PLAYER->GetTeam());
      if (Angel) ADD_MESSAGE("%s seems to be very friendly towards you.", Angel->CHAR_DESCRIPTION(DEFINITE));
    }
  } else {
    ADD_MESSAGE("You feel you are not yet worthy enough for %s.", GetName());
    PrayBadEffect();
    AdjustTimer(50000);
    AdjustRelation(-100);
    game::ApplyDivineAlignmentBonuses(this, 20, false);
    PLAYER->EditExperience(WISDOM, -50, 1 << 10);
    if (Relation < -250 && !(RAND() % 3)) {
      character *Angel = CreateAngel(game::GetTeam(4), 10000);
      if (Angel) ADD_MESSAGE("%s seems to be hostile.", Angel->CHAR_DESCRIPTION(DEFINITE));
    }
  }
}


#endif
