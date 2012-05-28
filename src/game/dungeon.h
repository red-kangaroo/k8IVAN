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
#ifndef __DUNGEON_H__
#define __DUNGEON_H__

#include "ivancommon.h"

#include "v2.h"


class level;
class outputfile;
class inputfile;
class dungeonscript;
class levelscript;
class festring;


class dungeon {
public:
  dungeon ();
  dungeon (int Index);
  virtual ~dungeon ();

  truth PrepareLevel (int Index, truth Visual=true); // returns whether the level has been visited before
  void SaveLevel (cfestring &SaveName, int Number, truth DeleteAfterwards=true);
  level *LoadLevel (cfestring &SaveName, int Number);
  inline level *GetLevel (int I) const { return Level[I]; }
  int GetLevels () const;
  void Save (outputfile &SaveFile) const;
  void Load (inputfile &SaveFile);
  inline void SetIndex (int What) { Index = What; }
  inline int GetIndex () const { return Index; }
  const levelscript *GetLevelScript (int I);
  v2 GetWorldMapPos () { return WorldMapPos; }
  inline void SetWorldMapPos (v2 What) { WorldMapPos = What; }
  festring GetLevelDescription (int I);
  festring GetShortLevelDescription (int I);
  level *LoadLevel (inputfile &SaveFile, int Number);
  inline truth IsGenerated (int I) const { return Generated[I]; }
  inline void SetIsGenerated (int I, truth What) { Generated[I] = What; }
  int GetLevelTeleportDestination (int From) const;

private:
  void Initialize ();

private:
  const dungeonscript *DungeonScript;
  level **Level;
  int Index;
  truth *Generated;
  v2 WorldMapPos;
};


outputfile &operator << (outputfile &SaveFile, const dungeon *Dungeon);
inputfile &operator >> (inputfile &SaveFile, dungeon *&Dungeon);


#endif
