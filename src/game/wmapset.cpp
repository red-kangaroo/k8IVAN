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

SYSTEM_SPECIALIZATIONS(gwterrain) SYSTEM_SPECIALIZATIONS(owterrain)

#include "wterras.h"

#undef __FILE_OF_STATIC_WTERRAIN_PROTOTYPE_DEFINITIONS__

cint OceanType = ocean::ProtoType.GetIndex();
cint SnowType = snow::ProtoType.GetIndex();
cint GlacierType = glacier::ProtoType.GetIndex();
cint EGForestType = evergreenforest::ProtoType.GetIndex();
cint LForestType = leafyforest::ProtoType.GetIndex();
cint SteppeType = steppe::ProtoType.GetIndex();
cint DesertType = desert::ProtoType.GetIndex();
cint JungleType = jungle::ProtoType.GetIndex();

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


//#define REGISTER_PLACE(name, ttype, spawner)  worldmap::RegisterPlace(name::Spawn(), ttype, spawner)
#define REGISTER_PLACE(name, ttype, area)  worldmap::RegisterPlace(name::Spawn(), ttype, area, (PlaceSpawner)name::Spawn)


// gum solution
void InitPlaces () {
  static truth initialized = false;
  //
  if (!initialized) {
    initialized = true;
    //
    REGISTER_PLACE(newattnam, LForestType, NEW_ATTNAM);
    REGISTER_PLACE(underwatertunnel, LForestType, UNDER_WATER_TUNNEL);
    REGISTER_PLACE(underwatertunnelexit, LForestType, UNDER_WATER_TUNNEL_EXIT);
    //
    REGISTER_PLACE(attnam, EGForestType, ATTNAM);
    REGISTER_PLACE(elpuricave, SnowType, ELPURI_CAVE);
    REGISTER_PLACE(mondedr, SteppeType, MONDEDR);
    REGISTER_PLACE(muntuo, LForestType, MUNTUO);
    REGISTER_PLACE(dragontower, SteppeType, DRAGON_TOWER);
    REGISTER_PLACE(darkforest, LForestType, DARK_FOREST);
    REGISTER_PLACE(xinrochtomb, SnowType, XINROCH_TOMB);
    // alien mod
    REGISTER_PLACE(alienvessel, JungleType, ALIEN_VESSEL);
  }
}


#if 0
    REGISTER_PLACE(newattnam, LForestType, NEW_ATTNAM, newattnam::Spawn());
    REGISTER_PLACE(underwatertunnel, LForestType, UNDER_WATER_TUNNEL, underwatertunnel::Spawn());
    REGISTER_PLACE(underwatertunnelexit, LForestType, UNDER_WATER_TUNNEL_EXIT, underwatertunnelexit::Spawn());
    //
    REGISTER_PLACE(attnam, EGForestType, ATTNAM, attnam::Spawn());
    REGISTER_PLACE(elpuricave, SnowType, ELPURI_CAVE, elpuricave::Spawn());
    REGISTER_PLACE(mondedr, SteppeType, MONDEDR, mondedr::Spawn());
    REGISTER_PLACE(muntuo, LForestType, MUNTUO, muntuo::Spawn());
    REGISTER_PLACE(dragontower, SteppeType, DRAGON_TOWER, dragontower::Spawn());
    REGISTER_PLACE(darkforest, LForestType, DARK_FOREST, darkforest::Spawn());
    REGISTER_PLACE(xinrochtomb, SnowType, XINROCH_TOMB, xinrochtomb::Spawn());
#endif
