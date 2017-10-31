#ifdef HEADER_PHASE
GOD(scabies, god)
{
public:
  virtual cchar *GetName () const;
  virtual cchar *GetDescription () const;
  virtual int GetAlignment () const;
  virtual int GetBasicAlignment () const;
  virtual truth PlayerVomitedOnAltar (liquid *);
  virtual col16 GetColor () const;
  virtual col16 GetEliteColor () const;
  virtual truth LikesMaterial (const materialdatabase *, ccharacter *) const;
  virtual truth MutatesBodyParts () const { return true; }
  virtual int GetSex () const { return FEMALE; }
  virtual truth LikesVomit () const { return true; }

protected:
  virtual void PrayGoodEffect ();
  virtual void PrayBadEffect ();
};


#else


cchar *scabies::GetName () const { return "Scabies"; }
cchar *scabies::GetDescription () const { return "goddess of mutations, disease and famine"; }
int scabies::GetAlignment () const { return AC; }
int scabies::GetBasicAlignment () const { return EVIL; }
col16 scabies::GetColor () const { return CHAOS_BASIC_COLOR; }
col16 scabies::GetEliteColor () const { return CHAOS_ELITE_COLOR; }


void scabies::PrayGoodEffect () {
  if (!RAND_N(10)) {
    for (int c = 0; c < game::GetTeams(); ++c) {
      if (PLAYER->GetTeam()->GetRelation(game::GetTeam(c)) == HOSTILE) {
        for (std::list<character *>::const_iterator i = game::GetTeam(c)->GetMember().begin(); i != game::GetTeam(c)->GetMember().end(); ++i) {
          character *Char = *i;
          //
          if ((*i)->IsEnabled() && !Char->IsImmuneToLeprosy()) Char->GainIntrinsic(LEPROSY);
        }
      }
    }
    ADD_MESSAGE("You feel a a horrible disease spreading.");
    return;
  }
  //
  if (!(RAND()%50)) {
    ADD_MESSAGE("Five cans full of school food drop from somewhere above!");
    for (int c = 0; c < 5; ++c) {
      can* Reward = can::Spawn(0, NO_MATERIALS);
      Reward->InitMaterials(MAKE_MATERIAL(IRON), MAKE_MATERIAL(SCHOOL_FOOD));
      PLAYER->GetGiftStack()->AddItem(Reward);
    }
    return;
  }
  //
  truth Success = false;
  //
  for (int d = 0; d < PLAYER->GetNeighbourSquares(); ++d) {
    lsquare *Square = PLAYER->GetNeighbourLSquare(d);
    //
    if (Square && Square->GetCharacter() && Square->GetCharacter()->GetRelation(PLAYER) == HOSTILE) {
      ADD_MESSAGE("%s throws poison on %s!", GetName(), Square->GetCharacter()->CHAR_DESCRIPTION(DEFINITE));
      Square->SpillFluid(PLAYER, liquid::Spawn(POISON_LIQUID, 500));
      Success = true;
    }
  }
  //
  if (!Success) PLAYER->PolymorphRandomly(2500, 10000, 1000+RAND()%1000);
}


void scabies::PrayBadEffect () {
  if (!(RAND() % 50)) {
    ADD_MESSAGE("%s makes you eat a LOT of school food.", GetName());
    material* SchoolFood = MAKE_MATERIAL(SCHOOL_FOOD, 2000);
    SchoolFood->EatEffect(PLAYER, 1000);
    delete SchoolFood;
    ADD_MESSAGE("You feel your muscles softening terribly...");
    PLAYER->EditAttribute(ARM_STRENGTH, -1);
    PLAYER->EditAttribute(DEXTERITY, -1);
  } else if (RAND_2) {
    ADD_MESSAGE("%s unleashes all her fury upon you!", GetName());
    PLAYER->BeginTemporaryState(POISONED, 600 + RAND() % 400);
  } else {
    ADD_MESSAGE("%s unleashes a horrible sickness upon you!", GetName());
    PLAYER->GainIntrinsic(LEPROSY);
  }
}


truth scabies::PlayerVomitedOnAltar (liquid *) {
  ADD_MESSAGE("%s feels that you are indeed her follower.", GetName());
  AdjustRelation(1+(RAND()%3));
  return false;
}


truth scabies::LikesMaterial (const materialdatabase *MDB, ccharacter *Char) const {
  return MDB->BodyFlags & IS_ALIVE && Char->GetTorso()->GetMainMaterial()->GetConfig() == MDB->Config;
}


#endif
