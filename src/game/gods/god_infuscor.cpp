#ifdef HEADER_PHASE
GOD(infuscor, god)
{
public:
  virtual cchar *GetName () const;
  virtual cchar *GetDescription () const;
  virtual int GetAlignment () const;
  virtual int GetBasicAlignment () const;
  virtual col16 GetColor () const;
  virtual col16 GetEliteColor () const;
  virtual int GetSex () const { return FEMALE; }

protected:
  virtual void PrayGoodEffect ();
  virtual void PrayBadEffect ();
};


#else


cchar *infuscor::GetName () const { return "Infuscor"; }
cchar *infuscor::GetDescription () const { return "goddess of wrong knowledge and vile magic"; }
int infuscor::GetAlignment () const { return ACM; }
int infuscor::GetBasicAlignment () const { return EVIL; }
col16 infuscor::GetColor () const { return CHAOS_BASIC_COLOR; }
col16 infuscor::GetEliteColor () const { return CHAOS_ELITE_COLOR; }


void infuscor::PrayBadEffect () {
  ADD_MESSAGE("Vile and evil knowledge pulps into your brain. It's too much for it to handle; you faint.");
  PLAYER->LoseConsciousness(1000+RAND_N(1000));
}


void infuscor::PrayGoodEffect () {
  ADD_MESSAGE("%s helps you.", GetName());
  //
  if (!PLAYER->StateIsActivated(ESP)) {
    PLAYER->BeginTemporaryState(ESP, 10000+RAND()%10000);
    return;
  }
  //
  if (!PLAYER->StateIsActivated(TELEPORT_CONTROL)) {
    PLAYER->BeginTemporaryState(TELEPORT_CONTROL, 10000+RAND()%10000);
    return;
  }
  //
  if (!PLAYER->StateIsActivated(POLYMORPH_CONTROL)) {
    PLAYER->BeginTemporaryState(POLYMORPH_CONTROL, 10000+RAND()%10000);
    return;
  }
  //
  ADD_MESSAGE("Suddenly three scrolls appear almost under your feet.");
  for (int c = 0; c < 3; ++c) PLAYER->GetGiftStack()->AddItem(scrollofteleportation::Spawn());
}


#endif
