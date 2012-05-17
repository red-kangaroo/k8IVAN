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


typedef void (*entrydrawer) (bitmap *Bitmap, v2 Pos, uInt);

class felist {
public:
  felist (cfestring &Topic, col16 TopicColor=WHITE, uInt Maximum=0);
  ~felist ();
  void AddEntry (cfestring &Str, col16 Color, uInt Marginal=0, uInt Key=NO_IMAGE, truth Selectable=true);
  void AddDescription (cfestring &Str, col16 Color=WHITE);
  uInt Draw ();
  void QuickDraw (bitmap *Bitmap, uInt PageLength) const;
  void Empty ();
  void EmptyDescription () { Description.resize(1); }
  void Load (inputfile &SaveFile);
  void Save (outputfile &SaveFile) const;
  uInt GetSelected () const { return Selected; }
  void SetSelected (uInt What) { Selected = What; }
  void EditSelected (int What) { Selected += What; }
  truth DrawPage (bitmap *Bitmap) const;
  void Pop ();
  static void CreateQuickDrawFontCaches (rawbitmap *Font, col16 Color, uInt PageLength);
  void PrintToFile (cfestring &SaveFile);
  void SetPos (v2 What) { Pos = What; }
  void SetWidth (uInt What) { Width = What; }
  void SetPageLength (uInt What) { PageLength = What; }
  void SetBackColor (col16 What) { BackColor = What; }
  void SetFlags (uInt What) { Flags = What; }
  void AddFlags (uInt What) { Flags |= What; }
  void RemoveFlags (uInt What) { Flags &= ~What; }
  void SetUpKey (uInt What) { UpKey = What; }
  void SetDownKey (uInt What) { DownKey = What; }
  void SetEntryDrawer (entrydrawer What) { EntryDrawer = What; }
  truth IsEmpty () const { return Entry.empty(); }
  uInt GetLength () const { return Entry.size(); }
  uInt GetLastEntryIndex () const { return Entry.size() - 1; }
  festring GetEntry (uInt I) const;
  col16 GetColor (uInt I) const;
  void SetColor (uInt I, col16 What);

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
  uInt PageBegin;
  uInt Maximum;
  uInt Selected;
  v2 Pos;
  uInt Width;
  uInt PageLength;
  col16 BackColor;
  uInt Flags;
  uInt UpKey;
  uInt DownKey;
  entrydrawer EntryDrawer;
};


#endif
