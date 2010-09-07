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
#ifndef __FELIB_LIST_H__
#define __FELIB_LIST_H__

#include <vector>

#include "v2.h"
#include "festring.h"


class outputfile;
class inputfile;
class rawbitmap;
class bitmap;
class festring;
struct felistentry;
struct felistdescription;


typedef void (*entrydrawer) (bitmap *Bitmap, v2 Pos, uint);

class felist {
public:
  felist (cfestring &Topic, col16 TopicColor=WHITE, uint Maximum=0);
  ~felist ();
  void AddEntry (cfestring &Str, col16 Color, uint Marginal=0, uint Key=NO_IMAGE, truth Selectable=true);
  void AddDescription (cfestring &Str, col16 Color=WHITE);
  uint Draw ();
  void QuickDraw (bitmap *Bitmap, uint PageLength) const;
  void Empty ();
  void EmptyDescription () { Description.resize(1); }
  void Load (inputfile &SaveFile);
  void Save (outputfile &SaveFile) const;
  uint GetSelected () const { return Selected; }
  void SetSelected (uint What) { Selected = What; }
  void EditSelected (int What) { Selected += What; }
  truth DrawPage (bitmap *Bitmap) const;
  void Pop ();
  static void CreateQuickDrawFontCaches (rawbitmap *Font, col16 Color, uint PageLength);
  void PrintToFile (cfestring &SaveFile);
  void SetPos (v2 What) { Pos = What; }
  void SetWidth (uint What) { Width = What; }
  void SetPageLength (uint What) { PageLength = What; }
  void SetBackColor (col16 What) { BackColor = What; }
  void SetFlags (uint What) { Flags = What; }
  void AddFlags (uint What) { Flags |= What; }
  void RemoveFlags (uint What) { Flags &= ~What; }
  void SetUpKey (uint What) { UpKey = What; }
  void SetDownKey (uint What) { DownKey = What; }
  void SetEntryDrawer (entrydrawer What) { EntryDrawer = What; }
  truth IsEmpty () const { return Entry.empty(); }
  uint GetLength () const { return Entry.size(); }
  uint GetLastEntryIndex () const { return Entry.size() - 1; }
  festring GetEntry (uint I) const;
  col16 GetColor (uint I) const;
  void SetColor (uint I, col16 What);

  static truth GetFastListMode ();
  static void SetFastListMode (truth modeon);

  truth isSaveSelector () const { return mSaveSelector; }
  void ResetSaveSelector () { mSaveSelector = false; }
  void SetSaveSelector (cfestring &dir) { mSaveDir = dir; mSaveSelector = true; }

private:
  void DrawDescription (bitmap *Bitmap) const;

private:
  truth mSaveSelector;
  festring mSaveDir;
  std::vector<felistentry *> Entry;
  std::vector<felistdescription *> Description;
  uint PageBegin;
  uint Maximum;
  uint Selected;
  v2 Pos;
  uint Width;
  uint PageLength;
  col16 BackColor;
  uint Flags;
  uint UpKey;
  uint DownKey;
  entrydrawer EntryDrawer;
};


#endif
