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

SYSTEM_SPECIALIZATIONS(gwterrain)
EXTENDED_SYSTEM_SPECIALIZATIONS(owterrain)(0, &owterrainSpawner, "owterrain");

#include "wterras.h"

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
#include "wterras.cpp"
