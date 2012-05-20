#ifdef HEADER_PHASE
GOD(silva, god)
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


cchar *silva::GetName () const { return "Silva"; }
cchar *silva::GetDescription () const { return "goddess of nature"; }
int silva::GetAlignment () const { return ANP; }
int silva::GetBasicAlignment () const { return NEUTRAL; }
col16 silva::GetColor () const { return NEUTRAL_BASIC_COLOR; }
col16 silva::GetEliteColor () const { return NEUTRAL_ELITE_COLOR; }


void silva::PrayGoodEffect () {
  if (PLAYER->GetNP() < HUNGER_LEVEL) {
    ADD_MESSAGE("Your stomach feels full again.");
    PLAYER->SetNP(SATIATED_LEVEL);
  }
  //
  if (!game::GetCurrentLevel()->IsOnGround()) {
    ADD_MESSAGE("Suddenly a horrible earthquake shakes the level.");
    //
    int Tunnels = 2+RAND()%3;
    int ToEmpty = 10+RAND()%11;
    int ToGround = 20+RAND()%21;
    int BoulderNumber = 10+RAND()%10;
    //
    for (int c = 0; c < Tunnels; ++c) {
      game::GetCurrentLevel()->AttachPos(game::GetCurrentLevel()->GetRandomSquare(0, NOT_WALKABLE|ATTACHABLE));
    }
    //
    for (int c = 0; c < ToEmpty; ++c) {
      for (int i = 0; i < 50; ++i) {
        v2 Pos = game::GetCurrentLevel()->GetRandomSquare(0, NOT_WALKABLE);
        truth Correct = false;
        //
        for (int d = 0; d < 8; ++d) {
          lsquare *Square = game::GetCurrentLevel()->GetLSquare(Pos)->GetNeighbourLSquare(d);
          //
          if (Square && Square->IsFlyable()) { Correct = true; break; }
        }
        //
        if (Correct) {
          game::GetCurrentLevel()->GetLSquare(Pos)->ChangeOLTerrainAndUpdateLights(0);
          break;
        }
      }
    }
    //
    for (int c = 0; c < ToGround; ++c) {
      for (int i = 0; i < 50; ++i) {
        int Walkables = 0;
        v2 Pos = game::GetCurrentLevel()->GetRandomSquare(0, RAND() & 1 ? 0 : HAS_CHARACTER);
        //
        if (Pos == ERROR_V2) continue;
        //
        lsquare *Square = game::GetCurrentLevel()->GetLSquare(Pos);
        character *Char = Square->GetCharacter();
        //
        if (Square->GetOLTerrain() || (Char && (Char->IsPlayer() || PLAYER->GetRelation(Char) != HOSTILE))) continue;
        //
        for (int d = 0; d < 8; ++d) {
          lsquare *NearSquare = game::GetCurrentLevel()->GetLSquare(Pos)->GetNeighbourLSquare(d);
          //
          if (NearSquare && NearSquare->IsFlyable()) ++Walkables;
        }
        //
        if (Walkables > 6) {
          Square->ChangeOLTerrainAndUpdateLights(earth::Spawn());
          if (Char) {
            if (Char->CanBeSeenByPlayer()) {
              ADD_MESSAGE("%s is hit by a brick of earth falling from the roof!", Char->CHAR_NAME(DEFINITE));
            }
            Char->ReceiveDamage(0, 20 + RAND() % 21, PHYSICAL_DAMAGE, HEAD|TORSO, 8, true);
            Char->CheckDeath(CONST_S("killed by an earthquake"), 0);
          }
          Square->KickAnyoneStandingHereAway();
          Square->GetStack()->ReceiveDamage(0, 10 + RAND() % 41, PHYSICAL_DAMAGE);
          break;
        }
      }
    }
    // Generate a few boulders in the level
    for (int c = 0; c < BoulderNumber; ++c) {
      v2 Pos = game::GetCurrentLevel()->GetRandomSquare();
      lsquare *Square = game::GetCurrentLevel()->GetLSquare(Pos);
      character *MonsterHere = Square->GetCharacter();
      //
      if (!Square->GetOLTerrain() && (!MonsterHere || MonsterHere->GetRelation(PLAYER) == HOSTILE)) {
        Square->ChangeOLTerrainAndUpdateLights(boulder::Spawn(1+(RAND()&1)));
        if (MonsterHere) MonsterHere->ReceiveDamage(0, 10+RAND()%10, PHYSICAL_DAMAGE, HEAD|TORSO, 8, true);
        Square->GetStack()->ReceiveDamage(0, 10+RAND()%10, PHYSICAL_DAMAGE);
      }
    }
    // Damage to items in the level
    for (int x = 0; x < game::GetCurrentLevel()->GetXSize(); ++x) {
      for (int y = 0; y < game::GetCurrentLevel()->GetYSize(); ++y) {
        game::GetCurrentLevel()->GetLSquare(x, y)->ReceiveEarthQuakeDamage();
      }
    }
  } else {
    int TryToCreate = 1+RAND()%7;
    int Created = 0;
    //

    for (; Created < TryToCreate; ++Created) {
      wolf *Wolf = wolf::Spawn();
      v2 Pos = game::GetCurrentLevel()->GetNearestFreeSquare(Wolf, PLAYER->GetPos());
      //
      if (Pos == ERROR_V2) { delete Wolf; break; }
      Wolf->SetTeam(PLAYER->GetTeam());
      Wolf->PutTo(Pos);
    }
    //
    if (!Created && PLAYER->CanHear()) ADD_MESSAGE("You hear a sad howling of a wolf imprisoned in the earth.");
    if (Created == 1) ADD_MESSAGE("Suddenly a tame wolf materializes beside you.");
    if (Created > 1) ADD_MESSAGE("Suddenly some tame wolves materialize around you.");
  }
}


void silva::PrayBadEffect () {
  switch (RAND()%3) {
    case 0: PLAYER->Polymorph(largerat::Spawn(), 1000+RAND()%1000); break;
    case 1: PLAYER->Polymorph(ass::Spawn(), 1000+RAND()%1000); break;
    case 2: PLAYER->Polymorph(jackal::Spawn(), 1000+RAND()%1000); break;
  }
}


#endif
