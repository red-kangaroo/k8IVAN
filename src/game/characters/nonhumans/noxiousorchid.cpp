#ifdef HEADER_PHASE
CHARACTER(noxiousorchid, nonhumanoid)
{
public:
  virtual truth Hit(character*, v2, int, int = 0);
protected:
  virtual col16 GetTorsoSpecialColor() const;
  virtual void GetAICommand();
  virtual void CreateCorpse(lsquare*);
  virtual void PostConstruct();
};


#else


col16 noxiousorchid::GetTorsoSpecialColor () const {
  if (!GetConfig()) return MakeRGB16(125+RAND()%100, RAND()%125, RAND()%100);
  if (GetConfig() == GREATER) return MakeRGB16(100+RAND()%100, RAND()%100, 155+RAND()%100);
  // giant
  return MakeRGB16(200+RAND()%55, RAND()%60, 150);
}


void noxiousorchid::PostConstruct () {
  //GetTorso()->GetMainMaterial()->SetSpoilCounter(200+RAND_N(100));
}


void noxiousorchid::CreateCorpse (lsquare *Square) {
  //int Amount = !GetConfig() ? (RAND() % 7 ? 0 : 1) : GetConfig() == GREATER ? (RAND() & 1 ? 0 : (RAND() % 5 ? 1 : (RAND() % 5 ? 2 : 3))) : (!(RAND() % 3) ? 0 : (RAND() % 3 ? 1 : (RAND() % 3 ? 2 : 3)));
  //for(int c = 0; c < Amount; ++c) Square->AddItem(kiwi::Spawn());
  nonhumanoid::CreateCorpse(Square);
}


truth noxiousorchid::Hit (character *Enemy, v2 HitPos, int Direction, int Flags) {
  if (!(RAND()&2)) {
    liquid *Fluid = 0; // = liquid::Spawn(WATER, 25+RAND()%25);
    //
    if (IsPlayer()) {
      ADD_MESSAGE("You hit %s.", Enemy->CHAR_DESCRIPTION(DEFINITE));
    } else if (Enemy->IsPlayer() || CanBeSeenByPlayer() || Enemy->CanBeSeenByPlayer()) {
      ADD_MESSAGE("%s hits %s.", CHAR_DESCRIPTION(DEFINITE), Enemy->CHAR_DESCRIPTION(DEFINITE));
    }
    //
    int lqConfig = -1;
    auto cfg = GetConfig();
    if (cfg == 0) {
      switch (RAND()%48) {
        case 0: lqConfig = ANTIDOTE_LIQUID; break;
        case 1: case 2: case 3: case 4: lqConfig = POISON_LIQUID; break;
        case 5: lqConfig = LIQUID_HORROR; break;
        case 6: case 7: case 8: lqConfig = SULPHURIC_ACID; break;
        default: break;
      }
    } else if (cfg == GREATER) {
      switch (RAND()%24) {
        case 0: lqConfig = ANTIDOTE_LIQUID; break;
        case 1: case 2: case 3: case 4: lqConfig = POISON_LIQUID; break;
        case 5: lqConfig = LIQUID_HORROR; break;
        case 6: case 7: case 8: lqConfig = SULPHURIC_ACID; break;
        default: break;
      }
    } else if (cfg == GIANTIC) {
      switch (RAND()%24) {
        case 0: lqConfig = ANTIDOTE_LIQUID; break;
        case 1: lqConfig = YELLOW_SLIME; break;
        case 2: case 3: case 4: lqConfig = POISON_LIQUID; break;
        case 5: lqConfig = LIQUID_HORROR; break;
        case 6: case 7: case 8: lqConfig = SULPHURIC_ACID; break;
        case 9: lqConfig = MUSTARD_GAS_LIQUID; break;
        default: break;
      }
    } else {
      //Fluid = liquid::Spawn(WATER, 25+RAND()%25); break;
    }
    //
    if (lqConfig > 0) {
      int lqVolume = 0;
           if (cfg == 0) lqVolume = 15+RAND()%25;
      else if (cfg == GREATER) lqVolume = 25+RAND()%25;
      else if (cfg == GIANTIC) lqVolume = 50+RAND()%50;
      if (lqVolume > 0) {
        //fprintf(stderr, "SPILL: cfg=%d; volume=%d\n", lqConfig, lqVolume);
        Fluid = liquid::Spawn(lqConfig, lqVolume);
        //fprintf(stderr, "SPILL: <%s:%d>\n", Fluid->GetTypeID(), Fluid->GetConfig());
        //fprintf(stderr, "  0: GetName:<%s>\n", Fluid->GetName(false, false).CStr());
      }
    }
    //
    if (Fluid) {
      Enemy->SpillFluid(Enemy, Fluid);
      //fprintf(stderr, "  1: GetName:<%s>\n", Fluid->GetName(false, false).CStr());
      if (IsPlayer()) {
        ADD_MESSAGE("You spill %s on %s.", Fluid->GetName(false, false).CStr(), Enemy->CHAR_DESCRIPTION(DEFINITE));
      } else if (Enemy->IsPlayer() || CanBeSeenByPlayer() || Enemy->CanBeSeenByPlayer()) {
        ADD_MESSAGE("%s spills %s on %s.", CHAR_DESCRIPTION(DEFINITE), Fluid->GetName(false, false).CStr(), Enemy->CHAR_DESCRIPTION(DEFINITE));
      }
    }
  } else if (nonhumanoid::Hit(Enemy, HitPos, Direction, Flags)) {
    return true;
  } else if (IsPlayer()) {
    ADD_MESSAGE("You miss %s.", Enemy->CHAR_DESCRIPTION(DEFINITE));
  }
  EditAP(-1000);
  return true;
}


void noxiousorchid::GetAICommand () {
  SeekLeader(GetLeader());
  if (FollowLeader(GetLeader())) return;
  if (AttackAdjacentEnemyAI()) return;
  if (CheckForUsefulItemsOnGround()) return;
  if (MoveRandomly()) return;
  EditAP(-1000);
}


#endif
