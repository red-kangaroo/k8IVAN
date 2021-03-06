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

#ifndef __CONT_H__
#define __CONT_H__

#include "ivancommon.h"

#include <vector>

#include "v2.h"
#include "festring.h"

class outputfile;
class inputfile;


class continent {
  friend class worldmap;

public:
  continent ();
  continent (int);
  virtual ~continent ();

  void AttachTo (continent *);
  void Add (v2);
  void Save (outputfile &) const;
  void Load (inputfile &);
  sLong GetSize () const;
  int GetIndex () const { return Index; }
  void GenerateInfo ();
  festring GetName () const { return Name; }
  int GetGTerrainAmount (int) const;
  v2 GetRandomMember (int Type, truth* success); // Type == -1: any terrain type
  std::vector<v2> GetShuffledMembers (int Type); // Type == -1: any terrain type
  v2 GetMember (int) const;

private:
  static uChar **TypeBuffer;
  static short **AltitudeBuffer;
  static uChar **ContinentBuffer;

private:
  enum { TerrainTypeLimit = 256 };
  sLong ttypeCount[TerrainTypeLimit];
  int Index;
  festring Name;
  std::vector<v2> Member;
};

outputfile &operator << (outputfile &, const continent *);
inputfile &operator >> (inputfile &, continent *&);


#endif
