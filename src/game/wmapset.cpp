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
#define __FILE_OF_STATIC_WTERRAIN_PROTOTYPE_DEFINITIONS__

#include "proto.h"
#include "wterra.h"
#include "database.h"

static owterrain *owterrainSpawner (int Config, int SpecialFlags) {
  owterrain *res = new owterrain;
  res->Initialize(Config, SpecialFlags);
  return res;
}

static gwterrain *gwterrainSpawner (int Config, int SpecialFlags) {
  gwterrain *res = new gwterrain;
  res->Initialize(Config, SpecialFlags);
  return res;
}

EXTENDED_SYSTEM_SPECIALIZATIONS(gwterrain)(0, &gwterrainSpawner, "gwterrain");
EXTENDED_SYSTEM_SPECIALIZATIONS(owterrain)(0, &owterrainSpawner, "owterrain");


// stupid cache, as globals won't change anyway
#define MAKE_TYPE_CACHER(name, constname) \
  static int name () { \
    static int type = -666; \
    if (type == -666) type = game::GetGlobalConst(constname); \
    return type; \
  }

MAKE_TYPE_CACHER(DesertType, "DESERT")
MAKE_TYPE_CACHER(JungleType, "JUNGLE")
MAKE_TYPE_CACHER(SteppeType, "STEPPE")
MAKE_TYPE_CACHER(LForestType, "LEAFY_FOREST")
MAKE_TYPE_CACHER(EGForestType, "EVERGREEN_FOREST")
MAKE_TYPE_CACHER(SnowType, "TUNDRA")
MAKE_TYPE_CACHER(GlacierType, "GLACIER")
MAKE_TYPE_CACHER(OceanType, "OCEAN")

#undef MAKE_TYPE_CACHER


//#include "wterras.h"

#undef __FILE_OF_STATIC_WTERRAIN_PROTOTYPE_DEFINITIONS__

#include <algorithm>

#include "allocate.h"
#include "char.h"
#include "cont.h"
#include "game.h"
#include "cont.h"
#include "femath.h"
#include "iconf.h"
#include "graphics.h"
#include "whandler.h"
#include "message.h"
#include "igraph.h"
#include "bitmap.h"
#include "fesave.h"
#include "feparse.h"
#include "confdef.h"

#include "cont.cpp"
#include "worldmap.cpp"
#include "wsquare.cpp"
#include "wterra.cpp"
//#include "wterras.cpp"
