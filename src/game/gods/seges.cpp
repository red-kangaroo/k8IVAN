#ifdef HEADER_PHASE
GOD(seges, god)
{
public:
  virtual cchar *GetName () const;
  virtual cchar *GetDescription () const;
  virtual int GetAlignment () const;
  virtual int GetBasicAlignment () const;
  virtual col16 GetColor () const;
  virtual col16 GetEliteColor () const;
  virtual truth ForceGiveBodyPart () const { return true; }
  virtual truth HealRegeneratingBodyParts () const { return true; }
  virtual truth LikesMaterial (const materialdatabase *, ccharacter *) const;
  virtual int GetSex () const { return FEMALE; }

protected:
  virtual void PrayGoodEffect ();
  virtual void PrayBadEffect ();
};


#else


cchar *seges::GetName () const { return "Seges"; }
cchar *seges::GetDescription () const { return "goddess of health and nutrition"; }
int seges::GetAlignment () const { return AL; }
int seges::GetBasicAlignment () const { return GOOD; }
col16 seges::GetColor () const { return LAWFUL_BASIC_COLOR; }
col16 seges::GetEliteColor () const { return LAWFUL_ELITE_COLOR; }


void seges::PrayGoodEffect () {
  if (PLAYER->IsInBadCondition()) {
    ADD_MESSAGE("%s cures your wounds.", GetName());
    PLAYER->RestoreLivingHP();
    return;
  }
  //
  if (PLAYER->TemporaryStateIsActivated(POISONED)) {
    ADD_MESSAGE("%s removes the foul liquid in your veins.", GetName());
    PLAYER->DeActivateTemporaryState(POISONED);
    return;
  }
  //
  if (PLAYER->StateIsActivated(LEPROSY)) {
    ADD_MESSAGE("%s cures your leprosy.", GetName());
    PLAYER->DeActivateTemporaryState(LEPROSY);
    return;
  }
  //
  if (PLAYER->GetNP() < SATIATED_LEVEL) {
    ADD_MESSAGE("Your stomach feels full again.");
    PLAYER->SetNP(BLOATED_LEVEL);
    return;
  }
  //
  if (PLAYER->GetStamina() < PLAYER->GetMaxStamina() >> 1) {
    ADD_MESSAGE("You don't feel a bit tired anymore.");
    PLAYER->RestoreStamina();
    return;
  }
}


void seges::PrayBadEffect () {
  if (PLAYER->UsesNutrition()) {
    ADD_MESSAGE("You feel Seges altering the contents of your stomach in an eerie way.");
    PLAYER->EditNP(-10000);
    PLAYER->CheckStarvationDeath(CONST_S("starved by ") + GetName());
  } else {
    ADD_MESSAGE("Seges tries to alter the contents of your stomach, but fails.");
  }
}


truth seges::LikesMaterial (const materialdatabase *MDB, ccharacter *Char) const {
  return MDB->BodyFlags & IS_ALIVE && Char->GetTorso()->GetMainMaterial()->GetConfig() == MDB->Config;
}


#endif
