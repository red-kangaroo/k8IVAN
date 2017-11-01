/*
 *
 *  Iter Vehemens ad Necem (IVAN)
 *  Copyright (C) Timo Kiviluoto
 *  Released under the GNU General
 *  Public License
 *
 *  See LICENSING which should be included
 *  along with this file for more details
 *
 */

/* Compiled through wmapset.cpp */

// ////////////////////////////////////////////////////////////////////////// //
/*
#define MAX_TEMPERATURE   27    //increase for a warmer world
#define LATITUDE_EFFECT   40    //increase for more effect
#define ALTITUDE_EFFECT   0.02

#define COLD    10
#define MEDIUM  12
#define WARM    17
#define HOT     19
*/

static int MAX_TEMPERATURE = 27;
static int LATITUDE_EFFECT = 40;
static float ALTITUDE_EFFECT = 0.02;
static int COLD = 10;
static int MEDIUM = 12;
static int WARM = 17;
static int HOT = 19;


// ////////////////////////////////////////////////////////////////////////// //
static const int DirX[8] = { -1, -1, -1, 0, 0, 1, 1, 1 };
static const int DirY[8] = { -1, 0, 1, -1, 1, -1, 0, 1 };


// ////////////////////////////////////////////////////////////////////////// //
worldmap::worldmap () {}
continent *worldmap::GetContinentUnder (v2 Pos) const { return Continent[ContinentBuffer[Pos.X][Pos.Y]]; }
v2 worldmap::GetEntryPos (ccharacter *, int I) const { return EntryMap.find(I)->second; }
continent *worldmap::GetContinent (int I) const { return Continent[I]; }
int worldmap::GetAltitude (v2 Pos) { return AltitudeBuffer[Pos.X][Pos.Y]; }
charactervector &worldmap::GetPlayerGroup () { return PlayerGroup; }
character *worldmap::GetPlayerGroupMember (int c) { return PlayerGroup[c]; }


worldmap::worldmap (int XSize, int YSize) : area(XSize, YSize) {
  Map = reinterpret_cast<wsquare ***>(area::Map);
  for (int x = 0; x < XSize; ++x) {
    for (int y = 0; y < YSize; ++y) {
      //Map[x][y] = new wsquare(this, v2(x, y));
      //Map[x][y]->SetGWTerrain(ocean::Spawn());
      Map[x][y] = nullptr;
    }
  }
  TypeBuffer = nullptr;
  AltitudeBuffer = nullptr;
  ContinentBuffer = nullptr;
  continent::TypeBuffer = nullptr;
  continent::AltitudeBuffer = nullptr;
  continent::ContinentBuffer = nullptr;
}


worldmap::~worldmap () {
  resetItAll(false);
  delete [] TypeBuffer;
  delete [] AltitudeBuffer;
  delete [] ContinentBuffer;
  for (uInt c = 1; c < Continent.size(); ++c) delete Continent[c];
  for (uInt c = 0; c < PlayerGroup.size(); ++c) delete PlayerGroup[c];
  Continent.resize(1, 0);
  PlayerGroup.clear();
  continent::TypeBuffer = nullptr;
  continent::AltitudeBuffer = nullptr;
  continent::ContinentBuffer = nullptr;
}


void worldmap::resetItAll (truth recreate) {
  Map = reinterpret_cast<wsquare ***>(area::Map);
  for (int x = 0; x < XSize; ++x) {
    for (int y = 0; y < YSize; ++y) {
      if (Map[x][y]) delete Map[x][y];
      if (recreate) {
        Map[x][y] = new wsquare(this, v2(x, y));
        Map[x][y]->SetGWTerrain(GWSpawn(OCEAN));
      } else {
        Map[x][y] = nullptr;
      }
    }
  }
  ClearEntryPoints();
  if (recreate) {
    if (TypeBuffer) delete [] TypeBuffer;
    if (AltitudeBuffer) delete [] AltitudeBuffer;
    if (ContinentBuffer) delete [] ContinentBuffer;
    Alloc2D(TypeBuffer, XSize, YSize);
    Alloc2D(AltitudeBuffer, XSize, YSize);
    Alloc2D(ContinentBuffer, XSize, YSize);
    continent::TypeBuffer = TypeBuffer;
    continent::AltitudeBuffer = AltitudeBuffer;
    continent::ContinentBuffer = ContinentBuffer;
  }
}


void worldmap::Save (outputfile &SaveFile) const {
  area::Save(SaveFile);
  SaveFile.Write(reinterpret_cast<char*>(TypeBuffer[0]), XSizeTimesYSize*sizeof(uChar));
  SaveFile.Write(reinterpret_cast<char*>(AltitudeBuffer[0]), XSizeTimesYSize*sizeof(short));
  SaveFile.Write(reinterpret_cast<char*>(ContinentBuffer[0]), XSizeTimesYSize*sizeof(uChar));
  for (feuLong c = 0; c < XSizeTimesYSize; ++c) Map[0][c]->Save(SaveFile);
  SaveFile << Continent << PlayerGroup;
}


void worldmap::Load (inputfile &SaveFile) {
  area::Load(SaveFile);
  Map = reinterpret_cast<wsquare ***>(area::Map);
  Alloc2D(TypeBuffer, XSize, YSize);
  Alloc2D(AltitudeBuffer, XSize, YSize);
  Alloc2D(ContinentBuffer, XSize, YSize);
  SaveFile.Read(reinterpret_cast<char*>(TypeBuffer[0]), XSizeTimesYSize*sizeof(uChar));
  SaveFile.Read(reinterpret_cast<char*>(AltitudeBuffer[0]), XSizeTimesYSize*sizeof(short));
  SaveFile.Read(reinterpret_cast<char*>(ContinentBuffer[0]), XSizeTimesYSize*sizeof(uChar));
  continent::TypeBuffer = TypeBuffer;
  continent::AltitudeBuffer = AltitudeBuffer;
  continent::ContinentBuffer = ContinentBuffer;
  for (int x = 0; x < XSize; ++x) {
    for (int y = 0; y < YSize; ++y) {
      delete Map[x][y];
      Map[x][y] = new wsquare(this, v2(x, y));
    }
  }
  for (int x = 0; x < XSize; ++x) {
    for (int y = 0; y < YSize; ++y) {
      game::SetSquareInLoad(Map[x][y]);
      Map[x][y]->Load(SaveFile);
    }
  }
  CalculateNeighbourBitmapPoses();
  SaveFile >> Continent >> PlayerGroup;
}


void worldmap::poiReset () {
  for (int f = 0; f < game::poiCount(); ++f) {
    auto terra = game::poiByIndex(f);
    terra->SetRevealed(false);
    terra->SetPlaced(false);
    terra->SetGenerated(false);
    auto cfg = terra->GetConfig();
    // fix obvious scripting bugs
    if (cfg == NEW_ATTNAM || cfg == UNDER_WATER_TUNNEL || cfg == UNDER_WATER_TUNNEL_EXIT || cfg == ELPURI_CAVE) {
      terra->MustBeSkipped = false;
    } else {
      //fprintf(stderr, "terra #%d prob is %d\n", terra->GetConfig(), terra->GetProbability());
      terra->MustBeSkipped = (terra->GetProbability() < RAND()%100+1);
      //if (terra->MustBeSkipped) fprintf(stderr, "worldmap::poiReset: skipped POI with config #%d\n", terra->GetConfig());
    }
  }
}


void worldmap::poiPlaceAtMap (owterrain *terra, truth forceReveal) {
  if (!terra) ABORT("cannot place nothing on worldmap!");
  if (!terra->IsGenerated()) ABORT("cannot place ungenerated something on worldmap!");
  if (terra->MustBeSkipped) ABORT("cannot place skipped something on worldmap!");
  // place it, if it is not placed yet
  if (!terra->IsPlaced()) {
    terra->SetPlaced(true);
    GetWSquare(terra->GetPosition())->ChangeOWTerrain(terra->Clone());
    SetEntryPos(terra->GetConfig(), terra->GetPosition());
    //fprintf(stderr, "POI #%d placed at (%d,%d); attached dungeon is %d\n", terra->GetConfig(), terra->GetPosition().X, terra->GetPosition().Y, terra->GetAttachedDungeon());
  } else {
    //fprintf(stderr, "already placed...\n");
  }
  if (!IsValidPos(terra->GetPosition())) ABORT("cannot place something on invalid worldmap position!");
  if (forceReveal || (terra->RevealEnvironmentInitially() && !terra->IsRevealed())) {
    //if (terra->IsRevealed()) fprintf(stderr, "re-revealing...\n");
    terra->SetRevealed(true);
    RevealEnvironment(terra->GetPosition(), 1);
  }
}


ContinentVector worldmap::poiFindPlacesFor (owterrain *terra, truth shuffle) {
  ContinentVector list;
  if (terra) {
    for (uInt c = 1; c < Continent.size(); ++c) {
      if (terra->MustBeSkipped) continue;
      //fprintf(stderr, " trying continent %u for %d...\n", c, terra->GetConfig());
      if (terra->IsSuitableContinent(Continent[c])) {
        //fprintf(stderr, "  FOUND!\n");
        list.push_back(Continent[c]);
      }
    }
    if (shuffle && list.size() > 1) {
      for (uInt f = 0; f < list.size(); ++f) {
        uInt swp = (uInt)RAND()%list.size();
        if (swp != f) {
          continent *tmp = list[f];
          list[f] = list[swp];
          list[swp] = tmp;
        }
      }
    }
  }
  //fprintf(stderr, ">>>%u continent(s) for %d...\n", list.size(), terra->GetConfig());
  return list;
}


static ContinentVector createShuffledContinentList (const ContinentVector &list) {
  ContinentVector res;
  for (auto &cont : list) res.push_back(cont);
  if (res.size() > 1) {
    for (uInt f = 0; f < res.size(); ++f) {
      uInt swp = (uInt)RAND()%res.size();
      if (swp != f) {
        continent *tmp = res[f];
        res[f] = res[swp];
        res[swp] = tmp;
      }
    }
  }
  return res;
}


truth worldmap::poiIsOccupied (v2 pos) {
  //fprintf(stderr, "checking...\n");
  for (int f = 0; f < game::poiCount(); ++f) {
    auto terra = game::poiByIndex(f);
    if (!terra) ABORT("Somone stole our terrain!");
    if (!terra->IsGenerated()) continue;
    //fprintf(stderr, " checking #%d...\n", f);
    if (terra->GetPosition() == pos) {
      //fprintf(stderr, "  CONFLICT!\n");
      return true;
    }
  }
  //fprintf(stderr, "checking complete!\n");
  return false;
}


truth worldmap::poiPlaceAttnamsAndUT (continent *PetrusLikes) {
  if (!PetrusLikes) return false; // oops
  if (PetrusLikes->GetSize() < 8) return false; // oops

  if (!attnam) ABORT("Who stole my Attnam?!");
  if (!newattnam) ABORT("Who stole my New Attnam?!");
  if (!underwatertunnel) ABORT("Who stole my UC Entry?!");
  if (!underwatertunnelexit) ABORT("Who stole my UC Exit?!");

  v2 newattnamPos = ERROR_V2, tunnelEntryPos = ERROR_V2, tunnelExitPos = ERROR_V2;

  // place tunnel exit
  truth Correct = false;
  for (int c1 = 0; c1 < 25; ++c1) {
    game::BusyAnimation();
    for (int c2 = 1; c2 < 50; ++c2) {
      tunnelExitPos = PetrusLikes->GetRandomMember(-1, &Correct);
      if (!Correct) return false; // no room, oops
      Correct = false;
      for (int d1 = 0; d1 < 8; ++d1) {
        v2 Pos = tunnelExitPos+game::GetMoveVector(d1);
        if (IsValidPos(Pos) && AltitudeBuffer[Pos.X][Pos.Y] <= 0) {
          int Distance = 3+(RAND()&3);
          truth Error = false;
          int x, y;
          int Counter = 0;

          tunnelEntryPos = Pos;
          for (int c2 = 0; c2 < Distance; ++c2) {
            tunnelEntryPos += game::GetMoveVector(d1);
            if (!IsValidPos(tunnelEntryPos) || AltitudeBuffer[tunnelEntryPos.X][tunnelEntryPos.Y] > 0) { Error = true; break; }
          }
          if (Error) continue;

          for (x = tunnelEntryPos.X-3; x <= tunnelEntryPos.X+3; ++x) {
            for (y = tunnelEntryPos.Y-3; y <= tunnelEntryPos.Y+3; ++y, ++Counter) {
              if (Counter != 0 && Counter != 6 && Counter != 42 && Counter != 48 &&
                  (!IsValidPos(x, y) || AltitudeBuffer[x][y] > 0 || AltitudeBuffer[x][y] < -350)) {
                Error = true;
                break;
              }
            }
            if (Error) break;
          }
          if (Error) continue;

          Error = true;
          for (x = 0; x < XSize; ++x) if (TypeBuffer[x][tunnelEntryPos.Y] == JungleType()) { Error = false; break; }
          if (Error) continue;

          Counter = 0;
          for (x = tunnelEntryPos.X - 2; x <= tunnelEntryPos.X+2; ++x) {
            for (y = tunnelEntryPos.Y - 2; y <= tunnelEntryPos.Y+2; ++y, ++Counter) {
              if (Counter != 0 && Counter != 4 && Counter != 20 && Counter != 24) AltitudeBuffer[x][y] /= 2;
            }
          }

          AltitudeBuffer[tunnelEntryPos.X][tunnelEntryPos.Y] = 1+RAND()%50;
          TypeBuffer[tunnelEntryPos.X][tunnelEntryPos.Y] = JungleType();
          GetWSquare(tunnelEntryPos)->ChangeGWTerrain(GWSpawn(JUNGLE));

          int NewAttnamIndex;
          for (NewAttnamIndex = RAND()&7; NewAttnamIndex == 7-d1; NewAttnamIndex = RAND()&7) {}
          newattnamPos = tunnelEntryPos+game::GetMoveVector(NewAttnamIndex);

          static const int DiagonalDir[4] = { 0, 2, 5, 7 };
          static const int NotDiagonalDir[4] = { 1, 3, 4, 6 };
          static const int AdjacentDir[4][2] = { { 0, 1 }, { 0, 2 }, { 1, 3 }, { 2, 3 } };
          truth Raised[] = { false, false, false, false };

          for (int d2 = 0; d2 < 4; ++d2) {
            if (NotDiagonalDir[d2] != 7-d1 && (NotDiagonalDir[d2] == NewAttnamIndex || !(RAND()&2))) {
              v2 Pos = tunnelEntryPos+game::GetMoveVector(NotDiagonalDir[d2]);
              AltitudeBuffer[Pos.X][Pos.Y] = 1+RAND()%50;
              TypeBuffer[Pos.X][Pos.Y] = JungleType();
              GetWSquare(Pos)->ChangeGWTerrain(GWSpawn(JUNGLE));
              Raised[d2] = true;
            }
          }

          for (int d2 = 0; d2 < 4; ++d2) {
            if (DiagonalDir[d2] != 7-d1 &&
                (DiagonalDir[d2] == NewAttnamIndex ||
                 (Raised[AdjacentDir[d2][0]] && Raised[AdjacentDir[d2][1]] && !(RAND()&2)))) {
              v2 Pos = tunnelEntryPos+game::GetMoveVector(DiagonalDir[d2]);

              AltitudeBuffer[Pos.X][Pos.Y] = 1+RAND()%50;
              TypeBuffer[Pos.X][Pos.Y] = JungleType();
              GetWSquare(Pos)->ChangeGWTerrain(GWSpawn(JUNGLE));
            }
          }
          Correct = true;
          break;
        }
      }
      if (Correct) break;
    }
    if (Correct) break;
  }
  if (!Correct) return false;
  if (newattnamPos == ERROR_V2 || tunnelEntryPos == ERROR_V2 || tunnelExitPos == ERROR_V2) return false;

  //fprintf(stderr, "UC and New Attnam were successfully placed...\n");
  // tunnel entry, tunnel exit and New Attnam are ok, find a place for Attnam
  game::BusyAnimation();

  // spawn and reveal all special places
  for (int f = 0; f < game::poiCount(); ++f) {
    auto terra = game::poiByIndex(f);
         if (terra->GetConfig() == NEW_ATTNAM) { terra->SetGenerated(true); terra->SetPosition(newattnamPos); poiPlaceAtMap(terra); }
    else if (terra->GetConfig() == UNDER_WATER_TUNNEL) { terra->SetGenerated(true); terra->SetPosition(tunnelEntryPos); poiPlaceAtMap(terra); }
    else if (terra->GetConfig() == UNDER_WATER_TUNNEL_EXIT) { terra->SetGenerated(true); terra->SetPosition(tunnelExitPos); poiPlaceAtMap(terra); }
  }

  // done
  return true;
}


static const gwterraindatabase *findWorldMapOptionsConfig () {
  auto xtype = protocontainer<gwterrain>::SearchCodeName("gwterrain");
  if (!xtype) ABORT("Your worldmap is dull and empty.");
  auto proto = protocontainer<gwterrain>::GetProto(xtype);
  if (!proto) ABORT("wtf?!");
  auto configs = proto->GetConfigData();
  if (!configs) ABORT("wtf?!");
  int cfgcount = proto->GetConfigSize();
  // search for overriden config
  for (int f = 0; f < cfgcount; ++f) {
    auto cfg = configs[f];
    //if (cfg->Config == 0) continue;
    //fprintf(stderr, "<%s>\n", getCPPTypeName<decltype(cfg->Config)>().c_str());
    if (cfg->Config == WorldMapOptionsCfg()) return cfg;
  }
  // search for base config
  for (int f = 0; f < cfgcount; ++f) {
    auto cfg = configs[f];
    if (cfg->Config == 0) return cfg;
  }
  ABORT("Worldmap is without options!");
  return nullptr;
}


void worldmap::Generate () {
  Alloc2D(OldAltitudeBuffer, XSize, YSize);
  Alloc2D(OldTypeBuffer, XSize, YSize);

  {
    auto cfg = findWorldMapOptionsConfig();
    /*
    fprintf(stderr, "=== FOUND! ===\n");
    fprintf(stderr, "  max temperature: %d\n", cfg->MaxTemperature);
    fprintf(stderr, "  latitude effect: %d\n", cfg->LatitudeEffect);
    fprintf(stderr, "  altitude effect: %f\n", (double)cfg->AltitudeEffect);
    fprintf(stderr, "  cold  : %d\n", cfg->TemperatureCold);
    fprintf(stderr, "  medium: %d\n", cfg->TemperatureMedium);
    fprintf(stderr, "  warm  : %d\n", cfg->TemperatureWarm);
    fprintf(stderr, "  hot   : %d\n", cfg->TemperatureHot);
    */
    MAX_TEMPERATURE = cfg->MaxTemperature;
    LATITUDE_EFFECT = cfg->LatitudeEffect;
    ALTITUDE_EFFECT = cfg->AltitudeEffect;
    COLD = cfg->TemperatureCold;
    MEDIUM = cfg->TemperatureMedium;
    WARM = cfg->TemperatureWarm;
    HOT = cfg->TemperatureHot;
  }

  /*
  fprintf(stderr, "ocean survive: <%s>\n", GWSpawn(OceanType())->GetSurviveMessage().CStr());
  fprintf(stderr, "ocean survive: <%s>\n", GWSpawn(OceanType())->SurviveMessage());
  fprintf(stderr, "ocean walkability: 0x%04x (0x%08x)\n", GWSpawn(OceanType())->GetWalkability(), ANY_MOVE&~WALK);
  */

  //continent* poiContinents[CONFIG_TABLE_SIZE]; // max number of configs

  for (;;) {
    //fprintf(stderr, "generating new planet...\n");
    resetItAll(true); // recreate

    RandomizeAltitude();
    SmoothAltitude();
    GenerateClimate();
    SmoothClimate();
    CalculateContinents();

    if (Continent.size() < 2) ABORT("Strange things happens in Universe...");
    //fprintf(stderr, "%u continents generated...\n", Continent.size());

    //fprintf(stderr, "resetting POI info...\n");
    poiReset(); // this also sets `owterrain::MustBeSkipped`, using spawn probabilities
    //memset(poiContinents, 0, sizeof(poiContinents));

    // find place for attnam
    if (!attnam) ABORT("Who stole my Attnam?!");

    game::BusyAnimation();
    auto PerfectForAttnam = poiFindPlacesFor(attnam, true); // shuffle results
    if (PerfectForAttnam.size() == 0) {
      //fprintf(stderr, "no country for old man...\n");
      continue;
    }

    //fprintf(stderr, "trying to find room for other POIs...\n");
    // try to place all initial places, and check if other places are (roughly) ok
    continent *PetrusLikes = nullptr;
    truth success = false;
    for (auto &cont : PerfectForAttnam) {
      //fprintf(stderr, "trying to place special pois...\n");
      if (!poiPlaceAttnamsAndUT(cont)) continue; // alas
      // WARNING! from here, we cannot continue checking continents on failure!
      //fprintf(stderr, "checking other pois...\n");
      success = true;
      for (int f = 0; f < game::poiCount(); ++f) {
        auto terra = game::poiByIndex(f);
        if (terra->MustBeSkipped) continue;
        if (terra->IsGenerated()) continue;
        if (terra->GetWantContinentWith() != attnam->GetConfig()) continue;
        //fprintf(stderr, " f=%d; cidx=%d (%d)\n", f, terra->GetWantContinentWith(), attnam->GetConfig());
        if (!terra->IsSuitableContinent(cont)) {
          //fprintf(stderr, "  OOPS!(0)\n");
          if (!terra->CanBeSkipped()) {
            //fprintf(stderr, "   OOPS!(1)\n");
            success = false;
            break;
          }
          terra->MustBeSkipped = true;
        } else {
          //poiContinents[f] = cont;
        }
      }
      // we can't continue looping here
      if (success) PetrusLikes = cont; // ok, this continent can be used for Petrus' needs
      break;
    }
    if (!PetrusLikes) continue; // alas

    //TODO: check and assign other continents
    // here, we should sort POI list in order of placement, pick continents, and so on
    // but i'll leave that for some indefinite future
    /*
    for (;;) {
      success = false;
      for (int f = 0; f < game::poiCount(); ++f) {
        auto terra = game::poiByIndex(f);
        if (terra->MustBeSkipped) continue;
        if (terra->IsGenerated()) continue;
        auto wantc = terra->GetWantContinentWith();
        // Attnam continent?
        if (wantc == attnam->GetConfig()) {
          // yep, already checked
          poiContinents[f] = PetrusLikes;
          continue;
        }
        // New Attnam or UC Entry island is too small to place anything
        if (wantc == newattnam->GetConfig() || wantc == underwatertunnel->GetConfig()) {
          ABORT("Cannot place anything near New Attnam!");
        }
        // list of suitable continents
        ContinentVector clist;
        // 0: pick random continent
        if (wantc == 0) {
          clist = createShuffledContinentList(Continent);
        } else {
          // check if we
        }
      }
    }
    */

    //fprintf(stderr, "spawining other pois...\n");
    // spawn others
    for (int f = 0; f < game::poiCount(); ++f) {
      auto terra = game::poiByIndex(f);
      if (terra->MustBeSkipped) continue;
      if (terra->IsGenerated()) continue;
      //fprintf(stderr, "trying poicfg #%d...\n", terra->GetConfig());
      //FIXME: find continent for this place
      continent *cont = nullptr;
      auto wantc = terra->GetWantContinentWith();
      // Attnam continent?
      if (wantc == attnam->GetConfig() || wantc == underwatertunnel->GetConfig()) {
        // yep, already checked
        cont = PetrusLikes;
      } else if (wantc == 0) {
        // pick random continent
        auto clist = createShuffledContinentList(Continent);
        for (auto &cc : clist) if (terra->IsSuitableContinent(cc)) { cont = cc; break; }
      } else if (wantc == newattnam->GetConfig() || wantc == underwatertunnel->GetConfig()) {
        // New Attnam or UC Entry island is too small to place anything
        ABORT("Cannot place anything near New Attnam!");
      }
      // found something?
      if (!cont) {
        // if we can skip this dungeon, then skip it, otherwise signal failure
        if (!terra->CanBeSkipped()) { success = false; break; }
        terra->MustBeSkipped = true;
        success = true; // in case this is last POI
        continue;
      }
      // get random position for this poi
      v2 poipos;
      game::BusyAnimation();
      //fprintf(stderr, "POI config #%d(%s): anyterra=%d; nativeg=%d\n", terra->GetConfig(), terra->GetNameSingular().CStr(), (terra->CanBeOnAnyTerrain() ? 1 : 0), terra->GetNativeGTerrainType());
      auto possiblePlaces = cont->GetShuffledMembers(terra->CanBeOnAnyTerrain() ? -1 : terra->GetNativeGTerrainType());
      success = false;
      for (auto &ppos : possiblePlaces) {
        if (!poiIsOccupied(ppos)) { poipos = ppos; success = true; break; } // ok
      }
      if (!success) {
        // if we can skip this dungeon, then skip it, otherwise signal failure
        if (!terra->CanBeSkipped()) break;
        terra->MustBeSkipped = true;
        success = true; // in case this is last POI
        continue;
      }
      // ok, we can place it
      //fprintf(stderr, "place poicfg #%d at (%d,%d)...\n", terra->GetConfig(), poipos.X, poipos.Y);
      terra->SetGenerated(true);
      terra->SetPosition(poipos);
      if (terra->PlaceInitially()) poiPlaceAtMap(terra);
    }

    // if something's failed, do it all again
    if (!success) continue;

    // player just exited new attnam
    PLAYER->PutTo(newattnam->GetPosition());

    CalculateLuminances();
    CalculateNeighbourBitmapPoses();
    // break infinite loop, we're done
    break;
  }

  // done
  delete [] OldAltitudeBuffer;
  delete [] OldTypeBuffer;
}


void worldmap::RandomizeAltitude () {
  game::BusyAnimation();
  for (int x = 0; x < XSize; ++x) {
    for (int y = 0; y < YSize; ++y) {
      AltitudeBuffer[x][y] = 4000-RAND()%8000;
    }
  }
}


void worldmap::SmoothAltitude () {
  for (int c = 0; c < 10; ++c) {
    game::BusyAnimation();
    int x, y;
    for (y = 0; y < YSize; ++y) SafeSmooth(0, y);
    for (x = 1; x < XSize - 1; ++x) {
      SafeSmooth(x, 0);
      for (y = 1; y < YSize - 1; ++y) FastSmooth(x, y);
      SafeSmooth(x, YSize - 1);
    }
    for (y = 0; y < YSize; ++y) SafeSmooth(XSize - 1, y);
  }
}


void worldmap::FastSmooth (int x, int y) {
  sLong HeightNear = 0;
  int d;
  for (d = 0; d < 4; ++d) HeightNear += OldAltitudeBuffer[x + DirX[d]][y + DirY[d]];
  for (d = 4; d < 8; ++d) HeightNear += AltitudeBuffer[x + DirX[d]][y + DirY[d]];
  OldAltitudeBuffer[x][y] = AltitudeBuffer[x][y];
  AltitudeBuffer[x][y] = HeightNear >> 3;
}


void worldmap::SafeSmooth (int x, int y) {
  sLong HeightNear = 0;
  int d, SquaresNear = 0;
  for (d = 0; d < 4; ++d) {
    int X = x + DirX[d];
    int Y = y + DirY[d];
    if (IsValidPos(X, Y)) {
      HeightNear += OldAltitudeBuffer[X][Y];
      ++SquaresNear;
    }
  }
  for (d = 4; d < 8; ++d) {
    int X = x + DirX[d];
    int Y = y + DirY[d];
    if (IsValidPos(X, Y)) {
      HeightNear += AltitudeBuffer[X][Y];
      ++SquaresNear;
    }
  }
  OldAltitudeBuffer[x][y] = AltitudeBuffer[x][y];
  AltitudeBuffer[x][y] = HeightNear / SquaresNear;
}


void worldmap::GenerateClimate () {
  game::BusyAnimation();
  for (int y = 0; y < YSize; ++y) {
    double DistanceFromEquator = fabs(double(y) / YSize - 0.5);
    truth LatitudeRainy = DistanceFromEquator <= 0.05 || (DistanceFromEquator > 0.25 && DistanceFromEquator <= 0.45);
    for (int x = 0; x < XSize; ++x) {
      if (AltitudeBuffer[x][y] <= 0) {
        TypeBuffer[x][y] = OceanType();
        continue;
      }
      truth Rainy = LatitudeRainy;
      if (!Rainy) {
        for(int d = 0; d < 8; ++d) {
          v2 Pos = v2(x, y) + game::GetMoveVector(d);
          if (IsValidPos(Pos) && AltitudeBuffer[Pos.X][Pos.Y] <= 0) {
            Rainy = true;
            break;
          }
        }
      }
      int Temperature = int(MAX_TEMPERATURE-DistanceFromEquator*LATITUDE_EFFECT-AltitudeBuffer[x][y]*ALTITUDE_EFFECT);
      int Type = 0;
           if (Temperature <= COLD) Type = (Rainy ? SnowType() : GlacierType());
      else if (Temperature <= MEDIUM) Type = (Rainy ? EGForestType() : SnowType());
      else if (Temperature <= WARM) Type = (Rainy ? LForestType() : SteppeType());
      else if (Temperature <= HOT) Type = (Rainy ? LForestType() : DesertType());
      else Type = (Rainy ? JungleType() : DesertType());
      TypeBuffer[x][y] = Type;
    }
  }
}


void worldmap::SmoothClimate () {
  for (int c = 0; c < 3; ++c) {
    game::BusyAnimation();
    for (int x = 0; x < XSize; ++x) {
      for (int y = 0; y < YSize; ++y) {
        if ((OldTypeBuffer[x][y] = TypeBuffer[x][y]) != OceanType()) {
          TypeBuffer[x][y] = WhatTerrainIsMostCommonAroundCurrentTerritorySquareIncludingTheSquareItself(x, y);
        }
      }
    }
  }
  game::BusyAnimation();
  for (int x = 0; x < XSize; ++x) {
    for (int y = 0; y < YSize; ++y) {
      //auto terraProto = protocontainer<gwterrain>::GetProto(TypeBuffer[x][y]);
      //if (!terraProto) ABORT("Oops! No gwterrain prototype for type #%d!\n", TypeBuffer[x][y]);
      //Map[x][y]->ChangeGWTerrain(terraProto->Spawn());
      Map[x][y]->ChangeGWTerrain(GWSpawn(TypeBuffer[x][y]));
    }
  }
}


/* Evil... */
#define ANALYZE_TYPE(type) {\
  int T = type;\
  for (c = 0; c < u; ++c) if (T == UsedType[c]) { ++TypeAmount[c]; break; }\
  if (c == u) { UsedType[u] = T; TypeAmount[u++] = 1; }\
}


/* k8: WOW! */
int worldmap::WhatTerrainIsMostCommonAroundCurrentTerritorySquareIncludingTheSquareItself (int x, int y) {
  int UsedType[9];
  int TypeAmount[9];
  int c, d, u = 1;
  UsedType[0] = TypeBuffer[x][y];
  TypeAmount[0] = 1;
  for (d = 0; d < 4; ++d) {
    int X = x+DirX[d];
    int Y = y+DirY[d];
    if (IsValidPos(X, Y)) ANALYZE_TYPE(OldTypeBuffer[X][Y]);
  }
  for (d = 4; d < 8; ++d) {
    int X = x+DirX[d];
    int Y = y+DirY[d];
    if (IsValidPos(X, Y)) ANALYZE_TYPE(TypeBuffer[X][Y]);
  }
  int MostCommon = 0;
  for (c = 1; c < u; ++c) if (TypeAmount[c] > TypeAmount[MostCommon] && UsedType[c] != OceanType()) MostCommon = c;
  return UsedType[MostCommon];
}


void worldmap::CalculateContinents () {
  for (uInt c = 1; c < Continent.size(); ++c) delete Continent[c];
  Continent.resize(1, 0);
  memset(ContinentBuffer[0], 0, XSizeTimesYSize*sizeof(uChar));
  game::BusyAnimation();
  for (int x = 0; x < XSize; ++x) {
    for (int y = 0; y < YSize; ++y) {
      if (AltitudeBuffer[x][y] > 0) {
        truth Attached = false;
        for (int d = 0; d < 8; ++d) {
          v2 Pos = v2(x, y)+game::GetMoveVector(d);
          if (IsValidPos(Pos)) {
            cint NearCont = ContinentBuffer[Pos.X][Pos.Y];
            if (NearCont) {
              cint ThisCont = ContinentBuffer[x][y];
              if (ThisCont) {
                if (ThisCont != NearCont) {
                  if (Continent[ThisCont]->GetSize() < Continent[NearCont]->GetSize()) {
                    Continent[ThisCont]->AttachTo(Continent[NearCont]);
                  } else {
                    Continent[NearCont]->AttachTo(Continent[ThisCont]);
                  }
                }
              } else {
                Continent[NearCont]->Add(v2(x, y));
              }
              Attached = true;
            }
          }
        }
        if (!Attached) {
          if (Continent.size() == 255) {
            RemoveEmptyContinents();
            if (Continent.size() == 255) ABORT("Valpurus shall not carry more continents!");
          }
          continent *NewContinent = new continent(Continent.size());
          NewContinent->Add(v2(x, y));
          Continent.push_back(NewContinent);
        }
      }
    }
  }
  RemoveEmptyContinents();
  for (uInt c = 1; c < Continent.size(); ++c) Continent[c]->GenerateInfo();
}


void worldmap::RemoveEmptyContinents () {
  for (uInt c = 1; c < Continent.size(); ++c) {
    if (!Continent[c]->GetSize()) {
      for (uInt i = Continent.size()-1; i >= c; --i) {
        if (Continent[i]->GetSize()) {
          Continent[i]->AttachTo(Continent[c]);
          delete Continent[i];
          Continent.pop_back();
          break;
        } else {
          delete Continent[i];
          Continent.pop_back();
        }
      }
    }
  }
}


void worldmap::Draw (truth) const {
  cint XMin = Max(game::GetCamera().X, 0);
  cint YMin = Max(game::GetCamera().Y, 0);
  cint XMax = Min(XSize, game::GetCamera().X+game::GetScreenXSize());
  cint YMax = Min(YSize, game::GetCamera().Y+game::GetScreenYSize());
  blitdata BlitData = {
    DOUBLE_BUFFER,
    { 0, 0 },
    { 0, 0 },
    { TILE_SIZE, TILE_SIZE },
    { 0 },
    TRANSPARENT_COLOR,
    ALLOW_ANIMATE|ALLOW_ALPHA
  };
  if (!game::GetSeeWholeMapCheatMode()) {
    for (int x = XMin; x < XMax; ++x) {
      BlitData.Dest = game::CalculateScreenCoordinates(v2(x, YMin));
      wsquare **Square = &Map[x][YMin];
      for (int y = YMin; y < YMax; ++y, ++Square, BlitData.Dest.Y += TILE_SIZE) {
        if ((*Square)->LastSeen) (*Square)->Draw(BlitData);
      }
    }
  } else {
    for (int x = XMin; x < XMax; ++x) {
      BlitData.Dest = game::CalculateScreenCoordinates(v2(x, YMin));
      wsquare **Square = &Map[x][YMin];
      for (int y = YMin; y < YMax; ++y, ++Square, BlitData.Dest.Y += TILE_SIZE) (*Square)->Draw(BlitData);
    }
  }
}


void worldmap::CalculateLuminances () {
  for (feuLong c = 0; c < XSizeTimesYSize; ++c) Map[0][c]->CalculateLuminance();
}


void worldmap::CalculateNeighbourBitmapPoses () {
  for (feuLong c = 0; c < XSizeTimesYSize; ++c) Map[0][c]->GetGWTerrain()->CalculateNeighbourBitmapPoses();
}


wsquare *worldmap::GetNeighbourWSquare (v2 Pos, int I) const {
  Pos += game::GetMoveVector(I);
  if (Pos.X >= 0 && Pos.Y >= 0 && Pos.X < XSize && Pos.Y < YSize) return Map[Pos.X][Pos.Y];
  return 0;
}


void worldmap::RevealEnvironment (v2 Pos, int Radius) {
  rect Rect;
  femath::CalculateEnvironmentRectangle(Rect, Border, Pos, Radius);
  for (int x = Rect.X1; x <= Rect.X2; ++x)
    for (int y = Rect.Y1; y <= Rect.Y2; ++y)
      Map[x][y]->SignalSeen();
}


outputfile &operator << (outputfile &SaveFile, const worldmap *WorldMap) {
  WorldMap->Save(SaveFile);
  return SaveFile;
}


inputfile &operator >> (inputfile &SaveFile, worldmap *&WorldMap) {
  WorldMap = new worldmap;
  WorldMap->Load(SaveFile);
  return SaveFile;
}


void worldmap::UpdateLOS () {
  game::RemoveLOSUpdateRequest();
  int Radius = PLAYER->GetLOSRange();
  sLong RadiusSquare = Radius*Radius;
  v2 Pos = PLAYER->GetPos();
  rect Rect;
  femath::CalculateEnvironmentRectangle(Rect, Border, Pos, Radius);
  for (int x = Rect.X1; x <= Rect.X2; ++x)
    for (int y = Rect.Y1; y <= Rect.Y2; ++y)
      if (sLong(HypotSquare(Pos.X-x, Pos.Y-y)) <= RadiusSquare) Map[x][y]->SignalSeen();
}
