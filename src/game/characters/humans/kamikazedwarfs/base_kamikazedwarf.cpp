#ifdef HEADER_PHASE
CHARACTER(kamikazedwarf, humanoid)
{
public:
  virtual truth Hit (character *, v2, int, int = 0);
  virtual truth CheckForUsefulItemsOnGround (truth = true) { return false; }
  virtual void GetAICommand ();
  virtual void CreateInitialEquipment (int);
  virtual truth IsKamikazeDwarf () const { return true; }
  virtual void SingRandomSong ();

protected:
  virtual int GetTorsoMainColor () const;
  virtual int GetGauntletColor () const;
  virtual int GetLegMainColor () const;
  virtual v2 GetDrawDisplacement (int) const;
  virtual int GetWSkillHits () const { return 10000; }
  virtual truth IsElite () const { return false; }
  virtual truth IsRookie () const { return false; }
  virtual truth IsGrenadier () const { return false; }
};


#else


void kamikazedwarf::CreateInitialEquipment (int SpecialFlags) {
  if (IsRookie()) {
    humanoid::CreateInitialEquipment(SpecialFlags);
    SetRightWielded(holybook::Spawn(GetConfig(), SpecialFlags));
    SetLeftWielded(meleeweapon::Spawn(AXE));
    for (int k = 0; k < 6; ++k) GetStack()->AddItem(meleeweapon::Spawn(AXE));
    GetStack()->AddItem(lantern::Spawn());
    GetCWeaponSkill(AXES)->AddHit(GetWSkillHits());
    GetCurrentRightSWeaponSkill()->AddHit(GetWSkillHits());
    return;
  }
  //
  if (IsGrenadier()) {
    humanoid::CreateInitialEquipment(SpecialFlags);
    SetLeftWielded(holybook::Spawn(GetConfig(), SpecialFlags));
    SetRightWielded(meleeweapon::Spawn(GREAT_AXE, SpecialFlags));
    for (int k = 0; k < 3; ++k) GetStack()->AddItem(gasgrenade::Spawn());
    GetStack()->AddItem(lantern::Spawn());
    GetCWeaponSkill(AXES)->AddHit(GetWSkillHits());
    GetCurrentRightSWeaponSkill()->AddHit(GetWSkillHits());
    return;
  }
  //
  humanoid::CreateInitialEquipment(SpecialFlags);
  SetRightWielded(holybook::Spawn(GetConfig(), SpecialFlags));
  GetCWeaponSkill(UNCATEGORIZED)->AddHit(GetWSkillHits());
  GetCurrentRightSWeaponSkill()->AddHit(GetWSkillHits());
}


truth kamikazedwarf::Hit (character *Enemy, v2 HitPos, int Direction, int Flags) {
  if (!IsPlayer()) {
    itemvector KamikazeWeapon;
    sortdata SortData(KamikazeWeapon, this, false, &item::IsKamikazeWeapon);
    //
    SortAllItems(SortData);
    //
    if (IsRookie() && (RAND()&3)) return humanoid::Hit(Enemy, HitPos, Direction, Flags);

    if (!KamikazeWeapon.empty()) {
      if (IsElite() && (RAND()&1)) ADD_MESSAGE("%s shouts: \"This time I won't fail, O Great %s!\"", CHAR_DESCRIPTION(DEFINITE), GetMasterGod()->GetName());
      else if (IsGrenadier() && (RAND()&1)) ADD_MESSAGE("%s shouts: \"What the hell, it'll be quick. Here goes nothing!\"", CHAR_DESCRIPTION(DEFINITE));
      else if (IsRookie() && (RAND()&1)) ADD_MESSAGE("%s sobs: \"War is hell!\"", CHAR_DESCRIPTION(DEFINITE));
      else if (RAND()&1) ADD_MESSAGE("%s shouts: \"For %s!\"", CHAR_DESCRIPTION(DEFINITE), GetMasterGod()->GetName());
      else ADD_MESSAGE("%s screams: \"%s, here I come!\"", CHAR_DESCRIPTION(DEFINITE), GetMasterGod()->GetName());
      if (KamikazeWeapon[RAND_N(KamikazeWeapon.size())]->Apply(this)) return true;
    }
  }

  return humanoid::Hit(Enemy, HitPos, Direction, Flags);
}


void kamikazedwarf::GetAICommand () {
  if (GetHomeRoom()) {
    StandIdleAI();
  } else {
    if (!RAND_N(50)) {
      SingRandomSong();
      return;
    }
    character::GetAICommand();
  }
}


v2 kamikazedwarf::GetDrawDisplacement (int I) const {
  static const v2 DrawDisplacement[] = { v2(0, 0), v2(0, 1), v2(0, -1), v2(0, -1), v2(0, -1), v2(0, 0), v2(0, 0) };
  //
  return DrawDisplacement[I];
}


col16 kamikazedwarf::GetTorsoMainColor () const {
  return GetMasterGod()->GetColor();
}


col16 kamikazedwarf::GetGauntletColor () const {
  return GetMasterGod()->GetColor();
}


col16 kamikazedwarf::GetLegMainColor () const {
  return GetMasterGod()->GetColor();
}


void kamikazedwarf::SingRandomSong () {
  festring Song;
  festring God = GetMasterGod()->GetName();
  festring Bodypart;
  //

  switch (RAND_N(9)) {
    case 0:
      switch (RAND_N(3)) {
        case 0: Bodypart = "palm"; break;
        case 1: Bodypart = "forehead"; break;
        default: Bodypart = "tongue"; break;
      }
      Song = festring("On the ") + Bodypart + festring(" of ") + God + " everybody fears everything";
      break;
    case 1: {
       festring Title = GetMasterGod()->GetSex() == MALE ? "King" : "Queen";
       //
       Song = festring("Joy to the world, ")+God+" is come! Let all above Valpurus receive her "+Title;
       } break;
    case 2:
      Song = festring("Hark the herald angels sing. Glory to ")+God+"!";
      break;
    case 3:
      Song = festring("O ")+God+", You are so big, So absolutely huge, Gosh, we're all really impressed down here, I can tell You.";
      break;
    case 4:
      Song = festring("Forgive us, O ")+God+" for this, our dreadful toadying and barefaced flattery";
      break;
    case 5:
      Song = festring("But you, ")+God+", are so strong and, well, just so super fantastic. Amen.";
      break;
    case 6:
      Song = festring("O ")+God+", please don't burn us";
      break;
    case 7:
      Song = festring("O ")+God+", please don't grill or toast your flock";
      break;
    case 8:
      Song = festring("O ")+God+", please don't simmer us in stock";
      break;
  }
  EditAP(-1000);
  if (CanBeSeenByPlayer()) ADD_MESSAGE("%s sings: \"%s\"", CHAR_DESCRIPTION(DEFINITE), Song.CStr());
  else ADD_MESSAGE("You hear someone sing: \"%s\"", Song.CStr());
}


#endif
