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

#include "hscore.h"
#include "fesave.h"
#include "felist.h"
#include "feio.h"
#include "femath.h"


highscore::highscore (cfestring &File) : LastAdd(0xFF), Version(HIGH_SCORE_VERSION) {
  Load(File);
}


festring highscore::genFileName (const festring &fname) const {
  if (fname[0] != '/') {
#ifdef LOCAL_SAVES
    return inputfile::GetMyDir()+'/'+fname;
#else
    festring nname;
    nname << getenv("HOME") << '/' << fname;
    return nname;
#endif
  }
  return fname;
}


truth highscore::Add (sLong NewScore, cfestring &NewEntry, time_t NewTime, sLong NewRandomID) {
  for (uInt c = 0; c < Score.size(); ++c) {
    if (Score[c] < NewScore) {
      Entry.insert(Entry.begin()+c, NewEntry);
      Score.insert(Score.begin()+c, NewScore);
      Time.insert(Time.begin()+c, NewTime);
      RandomID.insert(RandomID.begin()+c, NewRandomID);
      if (Score.size() > MAX_HIGHSCORES) {
        Entry.resize(MAX_HIGHSCORES, festring());
        Score.resize(MAX_HIGHSCORES);
        Time.resize(MAX_HIGHSCORES);
        RandomID.resize(MAX_HIGHSCORES);
      }
      LastAdd = c;
      return true;
    }
  }
  if (Score.size() < MAX_HIGHSCORES) {
    LastAdd = Score.size();
    Entry.push_back(NewEntry);
    Score.push_back(NewScore);
    Time.push_back(NewTime);
    RandomID.push_back(NewRandomID);
    return true;
  }
  LastAdd = MAX_HIGHSCORES;
  return false;
}


void highscore::Draw () const {
  if (Score.empty()) {
    iosystem::TextScreen(CONST_S("There are no entries yet. Play a game to correct this."));
    return;
  }
  if (GetVersion() != HIGH_SCORE_VERSION) {
    iosystem::TextScreen(CONST_S("The highscore file is for another version of IVAN."));
    return;
  }
  felist List(CONST_S("Adventurers' Hall of Fame"));
  festring Desc;
  for (uInt c = 0; c < Score.size(); ++c) {
    Desc.Empty();
    Desc << c+1;
    Desc.Resize(5, ' ');
    Desc << Score[c];
    Desc.Resize(13, ' ');
    Desc << Entry[c];
    List.AddEntry(Desc, c == uInt(LastAdd) ? WHITE : LIGHT_GRAY, 13);
  }
  List.SetFlags(FADE);
  List.SetPageLength(26);
  List.Draw();
}


void highscore::Save (cfestring &File) const {
  outputfile HighScore(genFileName(File), true);
  if (!HighScore.IsOpen()) return;
  sLong CheckSum = HIGH_SCORE_VERSION+LastAdd;
  for (uShort c = 0; c < Score.size(); ++c) {
    CheckSum += Score[c]+Entry[c].GetCheckSum()+RandomID[c];
  }
  HighScore <<
    uShort(HIGH_SCORE_VERSION)
    << Score
    << Entry
    << Time
    << RandomID
    << LastAdd
    << CheckSum;
}


/* This function needs much more error handling */
void highscore::Load (cfestring &File) {
  {
    inputfile HighScore(genFileName(File), false);
    if (!HighScore.IsOpen()) return;
    HighScore.Get();
    if (HighScore.Eof()) return;
  }
  sLong cs1;
  inputfile HighScore(genFileName(File), false);
  HighScore >> Version;
  HighScore >> Score >> Entry >> Time >> RandomID >> LastAdd >> cs1;
  sLong CheckSum = HIGH_SCORE_VERSION+LastAdd;
  for (uShort c = 0; c < Score.size(); ++c) {
    CheckSum += Score[c]+Entry[c].GetCheckSum()+RandomID[c];
  }
  if (cs1 != CheckSum) Clear();
}


truth highscore::MergeToFile (highscore *To) const {
  truth MergedSomething = false;
  for (uInt c = 0; c < Score.size(); ++c) {
    if (!To->Find(Score[c], Entry[c], Time[c], RandomID[c])) {
      To->Add(Score[c], Entry[c], Time[c], RandomID[c]);
      MergedSomething = true;
    }
  }
  return MergedSomething;
}


truth highscore::Add (sLong NewScore, cfestring &NewEntry) {
  return Add(NewScore, NewEntry, time(0), RAND());
}


/* Because of major stupidity this return the number of NEXT from the right entry, 0 = not found */
int highscore::Find (sLong AScore, cfestring &AEntry, time_t ATime, sLong ARandomID) {
  for (uInt c = 0; c < Score.size(); ++c) {
    if (AScore == Score[c] && Entry[c] == AEntry && ATime == Time[c] && ARandomID == RandomID[c])
      return c+1;
  }
  return 0;
}


void highscore::Clear () {
  Entry.clear();
  Score.clear();
  Time.clear();
  RandomID.clear();
  Version = HIGH_SCORE_VERSION;
  LastAdd = 0xFF;
}
