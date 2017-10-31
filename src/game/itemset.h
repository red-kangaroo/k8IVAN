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
#ifndef __HEADER_ITEMSET_H__
#define __HEADER_ITEMSET_H__

#include <algorithm>
#include <ctime>

#include "fesave.h"

#include "proto.h"
#include "item.h"
#include "database.h"


class humanoid;
class sweaponskill;


struct scar {
  int Severity;
  mutable bitmap *PanelBitmap;
};


struct damageid {
  int SrcID, Amount;
};


outputfile &operator << (outputfile &, const scar &);
inputfile &operator >> (inputfile &, scar &);


RAW_SAVE_LOAD(damageid);


#define HEADER_PHASE
#include "items/0list.cpp"
#undef HEADER_PHASE

#endif
