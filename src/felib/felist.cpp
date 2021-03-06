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
#include <fstream>

#include "felist.h"
#include "graphics.h"
#include "bitmap.h"
#include "whandler.h"
#include "rawbit.h"
#include "fesave.h"
#include "festring.h"


const felist *FelistCurrentlyDrawn = 0;
truth fastListMode = 0;


truth felist::GetFastListMode () { return fastListMode; }
void felist::SetFastListMode (truth modeon) { fastListMode = modeon; }


truth FelistDrawController () {
  FelistCurrentlyDrawn->DrawPage(DOUBLE_BUFFER);
  return true;
}


struct felistentry {
  felistentry () : ImageKey(NO_IMAGE) {}
  felistentry (cfestring &String, col16 Color, uInt Marginal, uInt ImageKey, truth Selectable, feuLong udata, void *uptr) :
    String(String),
    Color(Color),
    Marginal(Marginal),
    ImageKey(ImageKey),
    Selectable(Selectable),
    UData(udata),
    UPtr(uptr) {}

  festring String;
  col16 Color;
  uInt Marginal;
  uInt ImageKey;
  truth Selectable;
  feuLong UData;
  void *UPtr;
};


outputfile &operator << (outputfile &SaveFile, const felistentry *Entry) {
  SaveFile << Entry->String << Entry->Color << Entry->Marginal << Entry->Selectable;
  return SaveFile;
}


inputfile &operator >> (inputfile &SaveFile, felistentry *&Entry) {
  Entry = new felistentry;
  SaveFile >> Entry->String >> Entry->Color >> Entry->Marginal >> Entry->Selectable;
  return SaveFile;
}


struct felistdescription {
  felistdescription () {}
  felistdescription (cfestring &String, col16 Color)
  : String(String), Color(Color) {}
  festring String;
  col16 Color;
};


felist::felist (cfestring &Topic, col16 TopicColor, uInt Maximum) :
  mSaveSelector(false),
  mSaveDir(""),
  Maximum(Maximum),
  Selected(0),
  Pos(10, 10),
  Width(780),
  PageLength(26+10),
  BackColor(0),
  Flags(SELECTABLE|FADE),
  UpKey(KEY_UP),
  DownKey(KEY_DOWN),
  EntryDrawer(0)
{
  AddDescription(Topic, TopicColor);
}


felist::~felist () {
  Empty();
  for (uInt c = 0; c < Description.size(); ++c) delete Description[c];
}


truth felist::IsEntrySelectable (uInt idx) const { return (idx < Entry.size() ? Entry[idx]->Selectable : false); }

feuLong felist::GetEntryUData (uInt idx) const { return (idx < Entry.size() ? Entry[idx]->UData : 0); }
void felist::SetEntryUData (uInt idx, feuLong udata) { if (idx < Entry.size()) Entry[idx]->UData = udata; }

void *felist::GetEntryUPtr (uInt idx) const { return (idx < Entry.size() ? Entry[idx]->UPtr : 0); }
void felist::SetEntryUPtr (uInt idx, void *uptr) { if (idx < Entry.size()) Entry[idx]->UPtr = uptr; }


void felist::Pop () {
  delete Entry[GetLastEntryIndex()];
  Entry.pop_back();
}


uInt felist::Draw () {
  while (Entry.size() && Entry[GetLastEntryIndex()]->String.IsEmpty()) Pop();
  if (Entry.empty()) return LIST_WAS_EMPTY;

  FelistCurrentlyDrawn = this;
  if (globalwindowhandler::ControlLoopsInstalled()) globalwindowhandler::InstallControlLoop(FelistDrawController);

  bitmap BackGround(RES);
  bitmap*Buffer;

  BackGround.ActivateFastFlag();
  if (Flags&FADE) {
    Buffer = new bitmap(RES, 0);
    Buffer->ActivateFastFlag();
    BackGround.ClearToColor(0);
  } else {
    Buffer = DOUBLE_BUFFER;
    Buffer->FastBlit(&BackGround);
  }

  uInt c;
  uInt Return, Selectables = 0;
  truth JustSelectMove = false;
  for (c = 0; c < Entry.size(); ++c) if (Entry[c]->Selectable) ++Selectables;
  if (Selected >= Selectables) Selected = Selectables-1;

       if (Flags&SELECTABLE) PageBegin = Selected-Selected%PageLength;
  else if (Flags&INVERSE_MODE) PageBegin = GetLastEntryIndex()-GetLastEntryIndex()%PageLength;
  else PageBegin = 0;

  for (;;) {
    truth AtTheEnd = DrawPage(Buffer);
    if (Flags&FADE) {
      if (JustSelectMove) {
        Buffer->FastBlit(DOUBLE_BUFFER);
        graphics::BlitDBToScreen();
      } else {
        Buffer->FadeToScreen();
      }
      JustSelectMove = false;
    } else {
      graphics::BlitDBToScreen();
    }

    uInt Pressed = GET_KEY(false);

    if (Flags&SELECTABLE) {
      // list movement and selections
      int prs = -1;

           if (Pressed >= 'a' && Pressed <= 'z') prs = Pressed-'a';
      else if (Pressed >= 'A' && Pressed <= 'Z') prs = Pressed-'A';
      else if (Pressed >= '0' && Pressed <= '9') prs = Pressed-'0'+26;

      if (prs >= 0) {
        if ((uInt)prs < PageLength && (uInt)prs+PageBegin < Selectables) {
          Return = Selected = (uInt)prs+PageBegin;
          if (fastListMode) {
            JustSelectMove = true;
            continue;
          } else {
            break;
          }
        }
        continue;
      }

      if (Pressed == UpKey) {
        if (Selected) {
          --Selected;
          if (Selected < PageBegin) {
            BackGround.FastBlit(Buffer);
            PageBegin -= PageLength;
          } else {
            JustSelectMove = true;
          }
        } else {
          for (c = 0, Selected = 0; c < Entry.size(); ++c) if (Entry[c]->Selectable) ++Selected;
          --Selected;
          if (PageBegin == Selected-Selected%PageLength) JustSelectMove = true;
          else {
            BackGround.FastBlit(Buffer);
            PageBegin = Selected-Selected%PageLength;
          }
        }
        continue;
      }

      if (Pressed == DownKey) {
        if (!AtTheEnd || Selected != Selectables-1) {
          ++Selected;
          if (Selected > PageBegin+PageLength-1) {
            PageBegin += PageLength;
            BackGround.FastBlit(Buffer);
          } else {
            JustSelectMove = true;
          }
        } else {
          if (!PageBegin) JustSelectMove = true; else BackGround.FastBlit(Buffer);
          Selected = PageBegin = 0;
        }
        continue;
      }

      if (Pressed == KEY_HOME) {
        Selected = PageBegin;
        JustSelectMove = true;
        continue;
      }

      if (Pressed == KEY_END) {
        //fprintf(stderr, "sel: %u; pb: %u; pl: %u; ss: %u; ae: %s\n", Selected, PageBegin, PageLength, Selectables, AtTheEnd ? "end" : "not end");
        if (Selectables) {
          Selected = PageBegin+PageLength-1;
          if (Selected >= Selectables) Selected = Selectables-1;
        }
        JustSelectMove = true;
        continue;
      }

      if (Pressed == KEY_LEFT) {
        // previous page
        if (Selectables && PageBegin > 0) {
          PageBegin = (PageBegin < PageLength ? 0 : PageBegin-PageLength);
          Selected = PageBegin;
          BackGround.FastBlit(Buffer);
        }
        continue;
      }

      if (Pressed == KEY_RIGHT) {
        // next page
        if (Selectables) {
          uInt pgend = PageBegin+PageLength;
          if (pgend < Selectables) {
            PageBegin += PageLength;
            Selected = PageBegin;
            BackGround.FastBlit(Buffer);
          }
        }
        continue;
      }

      if (Pressed == KEY_ENTER) {
        Return = Selected;
        break;
      }
    } else {
      // not selectable: allow navigation with left/right
      if (Pressed == KEY_LEFT) {
        if (PageBegin > 0) {
          PageBegin -= (PageBegin < PageLength ? PageBegin : PageLength);
          BackGround.FastBlit(Buffer);
        }
        continue;
      }

      if (Pressed == KEY_RIGHT) {
        if (!AtTheEnd) {
          PageBegin += PageLength;
          BackGround.FastBlit(Buffer);
        }
        continue;
      }
    }

    if (Pressed == KEY_ESC) {
      Return = ESCAPED;
      break;
    }

    if ((AtTheEnd && !(Flags&INVERSE_MODE)) || (!PageBegin && (Flags&INVERSE_MODE))) {
      if (Flags&FELIST_NO_BADKEY_EXIT) continue;
      Return = NOTHING_SELECTED;
      break;
    } else {
      BackGround.FastBlit(Buffer);
      if (Flags&INVERSE_MODE) PageBegin -= PageLength; else PageBegin += PageLength;
      if (Flags&SELECTABLE) Selected = PageBegin;
    }
  }

  if (!(Flags&FADE)) {
    if (Flags&DRAW_BACKGROUND_AFTERWARDS) BackGround.FastBlit(DOUBLE_BUFFER);
    if (Flags&BLIT_AFTERWARDS) graphics::BlitDBToScreen();
  } else {
    delete Buffer;
  }

  globalwindowhandler::DeInstallControlLoop(FelistDrawController);
  return Return;
}


//static festring Str;

truth felist::DrawPage (bitmap *Buffer) const {
  uInt LastFillBottom = Pos.Y+23+Description.size()*10;
  festring Str;

  DrawDescription(Buffer);
  uInt c, i; // c == entry index, i == selectable index
  int selIdx = -1;
  for (c = 0, i = 0; i != PageBegin; ++c) if (Entry[c]->Selectable) ++i;
  while (!Entry[c]->Selectable && Entry[c]->String.IsEmpty()) ++c;
  std::vector<festring> Chapter;

  //col16 selBack = BackColor^0xFFFF, selText = BackColor;
  //col16 selBack = LIGHT_GRAY, selText = BLACK;
  col16 selBack = SEL_BLUE, selText = WHITE;

  for (;;) {
    Str.Empty();
    uInt Marginal = Entry[c]->Marginal;
    if ((Flags&SELECTABLE) && Entry[c]->Selectable) {
      if (i-PageBegin <= 25) Str << char('A'+(i-PageBegin)); else Str << char('0'+(i-PageBegin-26));
      Str << ": ";
      Marginal += 3;
    }
    Str << Entry[c]->String;
    bool selected = (Flags&SELECTABLE && Entry[c]->Selectable && Selected == i);
    if (selected) selIdx = (int)c;
    col16 ecolor = Entry[c]->Color;
    if (selected) {
      if (ecolor != RED && ecolor != GREEN && ecolor != PINK && ecolor != ORANGE) ecolor = selText;
    }
    if (Entry[c]->ImageKey != NO_IMAGE) {
      if (Str.rawLength() <= (Width-50)>>3) {
        Buffer->Fill(Pos.X+3, LastFillBottom, Width-6, 20, (!selected ? BackColor : selBack));
        if (EntryDrawer) EntryDrawer(Buffer, v2(Pos.X+13, LastFillBottom), Entry[c]->ImageKey);
        if (selected) {
          FONT->PrintfUnshadedColored(Buffer, v2(Pos.X+37, LastFillBottom+4), ecolor, "%s", Str.CStr());
        } else {
          FONT->PrintfColored(Buffer, v2(Pos.X+37, LastFillBottom+4), ecolor, "%s", Str.CStr());
        }
        LastFillBottom += 20;
      } else {
        uInt ChapterSize = festring::SplitStringColored(Str, Chapter, (Width-50)>>3, Marginal);
        uInt PictureTop = LastFillBottom+ChapterSize*5-9;
        for (uInt l = 0; l < ChapterSize; ++l) {
          Buffer->Fill(Pos.X+3, LastFillBottom, Width-6, 10, !selected ? BackColor : selBack);
          if (selected) {
            FONT->PrintfUnshadedColored(Buffer, v2(Pos.X+37, LastFillBottom), ecolor, "%s", Chapter[l].CStr());
          } else {
            FONT->PrintfColored(Buffer, v2(Pos.X+37, LastFillBottom), ecolor, "%s", Chapter[l].CStr());
          }
          LastFillBottom += 10;
        }
        if (EntryDrawer) EntryDrawer(Buffer, v2(Pos.X+13, PictureTop), Entry[c]->ImageKey);
      }
    } else {
      uInt ChapterSize = festring::SplitStringColored(Str, Chapter, (Width-26)>>3, Marginal);
      for (uInt l = 0; l < ChapterSize; ++l) {
        Buffer->Fill(Pos.X+3, LastFillBottom, Width-6, 10, !selected ? BackColor : selBack);
        if (selected) {
          FONT->PrintfUnshadedColored(Buffer, v2(Pos.X+13, LastFillBottom), ecolor, "%s", Chapter[l].CStr());
        } else {
          FONT->PrintfColored(Buffer, v2(Pos.X+13, LastFillBottom), ecolor, "%s", Chapter[l].CStr());
        }
        LastFillBottom += 10;
      }
    }
    if ((i-PageBegin == PageLength-1 && Entry[c]->Selectable) || c == Entry.size()-1) {
      if ((!(Flags&INVERSE_MODE) && c != Entry.size()-1) || (Flags&INVERSE_MODE && PageBegin)) {
        Buffer->Fill(Pos.X+3, LastFillBottom, Width-6, 30, BackColor);
        FONT->PrintfColored(Buffer, v2(Pos.X+13, LastFillBottom+10), WHITE, "- Press SPACE to continue, ESC to exit -");
        LastFillBottom += 30;
      } else {
        Buffer->Fill(Pos.X+3, LastFillBottom, Width-6, 10, BackColor);
        LastFillBottom += 10;
      }
      Buffer->DrawRectangle(Pos.X+1, Pos.Y+1, Pos.X+Width-2, LastFillBottom+1, DARK_GRAY, true);
      break;
    }
    if (Entry[c++]->Selectable) ++i;
  }
  if (selIdx != -1 && mSaveSelector) {
    bitmap bmp(ZERO_V2);
    festring imgName = Entry[selIdx]->String;
    festring::sizetype pos = imgName.FindLast(".sav");
    if (pos != festring::NPos) {
      imgName.Erase(pos, 4); // remove '.sav'
#if defined(SGAME_SHOTS_IPU) || (!defined(HAVE_IMLIB2) && !defined(HAVE_LIBPNG))
      imgName = mSaveDir+imgName+".ipu";
#else
      imgName = mSaveDir+imgName+".png";
#endif
      //fprintf(stderr, "sel=%d; img=%s\n", selIdx, imgName.CStr());
#if defined(SGAME_SHOTS_IPU) || (!defined(HAVE_IMLIB2) && !defined(HAVE_LIBPNG))
      if (bmp.LoadIPU(imgName))
#else
      if (bmp.LoadPNG(imgName))
#endif
      {
        //fprintf(stderr, " LOADED! %dx%d\n", bmp.GetSize().X, bmp.GetSize().Y);
        int x = Buffer->GetSize().X-bmp.GetSize().X-2;
        int y = Buffer->GetSize().Y-bmp.GetSize().Y-2;
        int w = bmp.GetSize().X;
        int h = bmp.GetSize().Y;
        blitdata bd = {
          Buffer,
          {0, 0}, //src
          {x, y}, //dest
          {w, h}, //border
          {0}, // luminance/flags
          0xDEAD, // mask color
          0 // custom data
        };
        bmp.NormalBlit(bd);
        Buffer->DrawRectangle(x-2, y-2, x+w, y+h, DARK_GRAY, true);
      }
    }
  }
  return c == Entry.size()-1;
}


void felist::DrawDescription (bitmap *Buffer) const {
  Buffer->Fill(Pos.X+3, Pos.Y+3, Width-6, 20, BackColor);
  for (uInt c = 0; c < Description.size(); ++c) {
    Buffer->Fill(Pos.X+3, Pos.Y+13+c*10, Width-6, 10, BackColor);
    FONT->PrintfColored(Buffer, v2(Pos.X+13, Pos.Y+13+c*10), Description[c]->Color, "%s", Description[c]->String.CStr());
  }
  Buffer->Fill(Pos.X+3, Pos.Y+13+Description.size()*10, Width-6, 10, BackColor);
}


/* We suppose InverseMode != false here */
void felist::QuickDraw (bitmap *Bitmap, uInt PageLength) const {
  static std::vector<festring> Chapter;
  uInt Width = Bitmap->GetSize().X;
  Bitmap->Fill(3, 3, Width-6, 20+PageLength*10, 0);
  Bitmap->DrawRectangle(1, 1, Width-2, 24+PageLength*10, DARK_GRAY, true);
  uInt LineSize = (Width-26)>>3;
  uInt Index = 0;
  uInt Bottom = PageLength*10+3;
  for (uInt c1 = 0; c1 <= Selected; ++c1) {
    const felistentry* CurrentEntry = Entry[Selected-c1];
    uInt ChapterSize = festring::SplitStringColored(CurrentEntry->String, Chapter, LineSize, CurrentEntry->Marginal);
    for (uInt c2 = 0; c2 < ChapterSize; ++c2) {
      col16 Color = CurrentEntry->Color;
      Color = MakeRGB16(
        GetRed16(Color)-((GetRed16(Color)*3*Index/PageLength)>>2),
        GetGreen16(Color)-((GetGreen16(Color)*3*Index/PageLength)>>2),
        GetBlue16(Color)-((GetBlue16(Color)*3*Index/PageLength)>>2));
      FONT->PrintfColored(Bitmap, v2(13, Bottom), Color, "%s", Chapter[ChapterSize-c2-1].CStr());
      Bottom -= 10;
      if (++Index == PageLength) return;
    }
  }
}


void felist::CreateQuickDrawFontCaches (rawbitmap *Font, col16 Color, uInt PageLength) {
  if (PageLength < 2) return;
  for (uInt c = 0; c < PageLength; ++c)
    Font->CreateFontCache(
      MakeRGB16(
        GetRed16(Color)-((GetRed16(Color)*3*c/PageLength)>>2),
        GetGreen16(Color)-((GetGreen16(Color)*3*c/PageLength)>>2),
        GetBlue16(Color)-((GetBlue16(Color)*3*c/PageLength)>>2)));
}


void felist::Empty () {
  for (uInt c = 0; c < Entry.size(); ++c) delete Entry[c];
  Entry.clear();
}


void felist::AddEntry (cfestring &Str, col16 Color, uInt Marginal, uInt Key, truth Selectable, feuLong udata, void *uptr) {
  Entry.push_back(new felistentry(Str, Color, Marginal, Key, Selectable, udata, uptr));
  if (Maximum && Entry.size() > feuLong(Maximum)) {
    delete Entry[0];
    Entry.erase(Entry.begin());
  }
}


void felist::Save (outputfile &SaveFile) const {
  SaveFile << Entry << Maximum << Selected;
}


void felist::Load (inputfile& SaveFile) {
  SaveFile >> Entry >> Maximum >> Selected;
}


void felist::PrintToFile (cfestring& FileName) {
  std::ofstream SaveFile(FileName.CStr(), std::ios::out);
  if (!SaveFile.is_open()) return;
  uInt c;
  for (c = 0; c < Description.size(); ++c) SaveFile << Description[c]->String.CStr() << std::endl;
  SaveFile << std::endl;
  for (c = 0; c < Entry.size(); ++c) {
    if (Entry[c]->ImageKey != NO_IMAGE) SaveFile << "   ";
    SaveFile << Entry[c]->String.CStr() << std::endl;
  }
}


void felist::AddDescription (cfestring &Str, col16 Color) {
  Description.push_back(new felistdescription(Str, Color));
}


festring felist::GetEntry (uInt I) const {
  return Entry[I]->String;
}


col16 felist::GetColor (uInt I) const {
  return Entry[I]->Color;
}


void felist::SetColor (uInt I, col16 What) {
  Entry[I]->Color = What;
}
