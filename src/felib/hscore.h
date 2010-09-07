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


#define HIGH_SCORE_FILENAME  "ivan-highscore.scores"

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

  truth Add (long NewScore, cfestring &NewEntry);
  truth LastAddFailed () const { return LastAdd == MAX_HIGHSCORES; }

  int Find (long AScore, cfestring &AEntry, time_t ATime, long ARandomID);

  ushort GetVersion () const { return Version; }
  cfestring &GetEntry (int I) const { return Entry[I]; }
  long GetScore (int I) const { return Score[I]; }
  long GetSize () const { return Entry.size(); }

  void Draw () const;

private:
  truth Add (long NewScore, cfestring &NewEntry, time_t NewTime, long NewRandomID);
  festring genFileName (const festring &fname) const;

private:
  std::vector<festring> Entry;
  std::vector<long> Score;
  std::vector<time_t> Time;
  std::vector<long> RandomID;
  int LastAdd;
  ushort Version;
};


#endif
