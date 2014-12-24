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
#ifndef __FELIB_HSCORE_H__
#define __FELIB_HSCORE_H__

#include <vector>
#include <ctime>

#include "festring.h"


#define HIGH_SCORE_FILENAME  ".ivan.scores"

/* Increment this if changes make highscores incompatible */
#define HIGH_SCORE_VERSION 121


class highscore {
public:
  highscore (cfestring &File=HIGH_SCORE_FILENAME);

  void Clear ();

  truth CheckVersion () const { return Version == HIGH_SCORE_VERSION; }

  void Save (cfestring &File=HIGH_SCORE_FILENAME) const;
  void Load (cfestring &File=HIGH_SCORE_FILENAME);
  //void AddToFile (highscore *) const;
  truth MergeToFile (highscore *To) const;

  truth Add (sLong NewScore, cfestring &NewEntry);
  truth LastAddFailed () const { return LastAdd == MAX_HIGHSCORES; }

  int Find (sLong AScore, cfestring &AEntry, time_t ATime, sLong ARandomID);

  uShort GetVersion () const { return Version; }
  cfestring &GetEntry (int I) const { return Entry[I]; }
  sLong GetScore (int I) const { return Score[I]; }
  sLong GetSize () const { return Entry.size(); }

  void Draw () const;

private:
  truth Add (sLong NewScore, cfestring &NewEntry, time_t NewTime, sLong NewRandomID);
  festring genFileName (const festring &fname) const;

private:
  std::vector<festring> Entry;
  std::vector<sLong> Score;
  std::vector<time_t> Time;
  std::vector<sLong> RandomID;
  int LastAdd;
  uShort Version;
};


#endif
