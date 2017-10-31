#ifdef HEADER_PHASE
ITEM(scrollofearthquake, scroll)
{
 public:
  virtual void FinishReading(character*);
};


#else


void scrollofearthquake::FinishReading(character* Reader)
{
  if(!game::GetCurrentLevel()->IsOnGround())
  {
    ADD_MESSAGE("Suddenly a horrible earthquake shakes the level!");
    int c, Tunnels = 2 + RAND() % 3;
    if(!game::GetCurrentLevel()->EarthquakesAffectTunnels())
      Tunnels = 0;

    for(c = 0; c < Tunnels; ++c)
      game::GetCurrentLevel()->AttachPos(game::GetCurrentLevel()->GetRandomSquare(0, NOT_WALKABLE|ATTACHABLE));

    int ToEmpty = 10 + RAND() % 11;

    for(c = 0; c < ToEmpty; ++c)
      for(int i = 0; i < 50; ++i)
      {
        v2 Pos = game::GetCurrentLevel()->GetRandomSquare(0, NOT_WALKABLE);
        truth Correct = false;

        for(int d = 0; d < 8; ++d)
        {
          lsquare* Square = game::GetCurrentLevel()->GetLSquare(Pos)->GetNeighbourLSquare(d);

          if(Square && Square->IsFlyable())
          {
            Correct = true;
            break;
          }
        }

        if(Correct)
        {
          game::GetCurrentLevel()->GetLSquare(Pos)->ChangeOLTerrainAndUpdateLights(0);
          break;
        }
      }

    int ToGround = 20 + RAND() % 21;

    for(c = 0; c < ToGround; ++c)
      for(int i = 0; i < 50; ++i)
      {
        v2 Pos = game::GetCurrentLevel()->GetRandomSquare(0, RAND() & 1 ? 0 : HAS_CHARACTER);

        if(Pos == ERROR_V2)
          continue;

        lsquare* Square = game::GetCurrentLevel()->GetLSquare(Pos);
        character* Char = Square->GetCharacter();

        if(Square->GetOLTerrain() || (Char && (Char->IsPlayer() || PLAYER->GetRelation(Char) != HOSTILE)))
          continue;

        int Walkables = 0;

        for(int d = 0; d < 8; ++d)
        {
          lsquare* NearSquare = game::GetCurrentLevel()->GetLSquare(Pos)->GetNeighbourLSquare(d);

          if(NearSquare && NearSquare->IsFlyable())
            ++Walkables;
        }

        if(Walkables > 6)
        {
          Square->ChangeOLTerrainAndUpdateLights(earth::Spawn());

          if(Char)
          {
            if(Char->CanBeSeenByPlayer())
              ADD_MESSAGE("%s is hit by a rock falling from the ceiling!", Char->CHAR_NAME(DEFINITE));

            Char->ReceiveDamage(0, 20 + RAND() % 21, PHYSICAL_DAMAGE, HEAD|TORSO, 8, true);
            Char->CheckDeath(CONST_S("killed by an earthquake"), 0);
          }

          Square->KickAnyoneStandingHereAway();
          Square->GetStack()->ReceiveDamage(0, 10 + RAND() % 41, PHYSICAL_DAMAGE);
          break;
        }
      }

    // Generate a few boulders in the level

    int BoulderNumber = 10 + RAND() % 10;

    for(c = 0; c < BoulderNumber; ++c)
    {
      v2 Pos = game::GetCurrentLevel()->GetRandomSquare();
      lsquare* Square = game::GetCurrentLevel()->GetLSquare(Pos);
      character* MonsterHere = Square->GetCharacter();

      if(!Square->GetOLTerrain() && (!MonsterHere || MonsterHere->GetRelation(PLAYER) == HOSTILE))
      {
        Square->ChangeOLTerrainAndUpdateLights(boulder::Spawn(1 + (RAND() & 1)));

        if(MonsterHere)
          MonsterHere->ReceiveDamage(0, 10 + RAND() % 10, PHYSICAL_DAMAGE, HEAD|TORSO, 8, true);

        Square->GetStack()->ReceiveDamage(0, 10 + RAND() % 10, PHYSICAL_DAMAGE);
      }
    }

    // Damage to items in the level

    for(int x = 0; x < game::GetCurrentLevel()->GetXSize(); ++x)
      for(int y = 0; y < game::GetCurrentLevel()->GetYSize(); ++y)
        game::GetCurrentLevel()->GetLSquare(x, y)->ReceiveEarthQuakeDamage();
  }
  else
  {
    ADD_MESSAGE("The ground shakes slightly.");
  }

  RemoveFromSlot();
  SendToHell();
  Reader->EditExperience(INTELLIGENCE, 150, 1 << 12);
}


#endif
