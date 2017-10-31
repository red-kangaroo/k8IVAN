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

#define __FILE_OF_STATIC_ITEM_PROTOTYPE_DEFINITIONS__

#include <algorithm>
#include <ctime>

#include "feloops.h"
#include "fesave.h"

#include "proto.h"
#include "item.h"
#include "database.h"

EXTENDED_SYSTEM_SPECIALIZATIONS(item)(0, 0, 0, "item");

#include "itemset.h"

#undef __FILE_OF_STATIC_ITEM_PROTOTYPE_DEFINITIONS__

#include "char.h"
#include "message.h"
#include "stack.h"
#include "felist.h"
#include "confdef.h"
#include "room.h"
#include "game.h"
#include "materias.h"
#include "characters.h"
#include "team.h"
#include "god.h"
#include "team.h"
#include "smoke.h"
#include "fesave.h"
#include "feparse.h"
#include "whandler.h"
#include "bitmap.h"
#include "fluid.h"
#include "rawbit.h"
#include "balance.h"
#include "lterras.h"

#include "item.cpp"
#include "items/0list.cpp"


outputfile &operator << (outputfile &SaveFile, const scar &Scar) {
  SaveFile << Scar.Severity << Scar.PanelBitmap;
  return SaveFile;
}


inputfile &operator >> (inputfile &SaveFile, scar &Scar) {
  SaveFile >> Scar.Severity >> Scar.PanelBitmap;
  return SaveFile;
}
