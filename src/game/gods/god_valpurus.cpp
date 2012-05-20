#ifdef HEADER_PHASE
GOD(valpurus, god)
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


cchar *valpurus::GetName () const { return "Valpurus"; }
cchar *valpurus::GetDescription () const { return "King of Gods"; }
int valpurus::GetAlignment () const { return ALPP; }
int valpurus::GetBasicAlignment () const { return GOOD; }
col16 valpurus::GetColor () const { return LAWFUL_BASIC_COLOR; }
col16 valpurus::GetEliteColor () const { return LAWFUL_ELITE_COLOR; }


void valpurus::PrayGoodEffect () {
  if (RAND()&1) {
    shield *Shield = shield::Spawn();
    //
    Shield->InitMaterials(MAKE_MATERIAL(VALPURIUM));
    PLAYER->GetGiftStack()->AddItem(Shield);
    ADD_MESSAGE("You hear booming voice: \"THIS WILL PROTECT THEE FROM MORTIFER, MY PALADIN!\" A shield glittering with holy might appears from nothing.");
  } else {
    meleeweapon *Weapon = meleeweapon::Spawn(TWO_HANDED_SWORD);
    //
    Weapon->InitMaterials(MAKE_MATERIAL(VALPURIUM), MAKE_MATERIAL(VALPURIUM), true);
    PLAYER->GetGiftStack()->AddItem(Weapon);
    ADD_MESSAGE("You hear booming voice: \"DEFEAT MORTIFER WITH THIS, MY PALADIN!\" A sword glittering with holy might appears from nothing.");
  }
}


void valpurus::PrayBadEffect () {
  ADD_MESSAGE("Valpurus smites you with a small hammer.");
  PLAYER->ReceiveDamage(0, 10, PHYSICAL_DAMAGE, HEAD, RAND() & 7);
  PLAYER->CheckDeath(CONST_S("faced the hammer of Justice from the hand of ") + GetName(), 0);
}


void valpurus::Pray () {
  if (!Timer && Relation == 1000) {
    ADD_MESSAGE("You feel %s is very pleased.", GetName());
    PrayGoodEffect();
    AdjustTimer(100000);
    AdjustRelation(-500);
    game::ApplyDivineAlignmentBonuses(this, 100, true);
    PLAYER->EditExperience(WISDOM, 400, 1 << 11);
    if (Relation > 250 && !(RAND()%2)) {
      character *Angel = CreateAngel(PLAYER->GetTeam());
      //
      if (Angel) ADD_MESSAGE("%s seems to be very friendly towards you.", Angel->CHAR_DESCRIPTION(DEFINITE));
    }
  } else if (Relation < 0 || (!TryToAttachBodyPart(PLAYER) && !TryToHardenBodyPart(PLAYER))) {
    ADD_MESSAGE("You feel you are not yet worthy enough for %s.", GetName());
    PrayBadEffect();
    AdjustTimer(50000);
    AdjustRelation(-100);
    game::ApplyDivineAlignmentBonuses(this, 20, false);
    PLAYER->EditExperience(WISDOM, -50, 1 << 10);
    if (Relation < -250 && !(RAND()%3)) {
      character *Angel = CreateAngel(game::GetTeam(VALPURUS_ANGEL_TEAM), 10000);
      //
      if (Angel) ADD_MESSAGE("%s seems to be hostile.", Angel->CHAR_DESCRIPTION(DEFINITE));
    }
  }
}


#endif
