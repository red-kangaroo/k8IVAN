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
#include <cstdarg>
#include <cctype>

#ifndef DISABLE_SOUND
# include <pcre.h>
# include <SDL/SDL_mixer.h>
#endif

#include "message.h"
#include "festring.h"
#include "felist.h"
#include "game.h"
#include "graphics.h"
#include "save.h"
#include "bitmap.h"
#include "igraph.h"
#include "iconf.h"


felist msgsystem::MessageHistory(CONST_S("Message history"), WHITE, 128);
festring msgsystem::LastMessage;
festring msgsystem::BigMessage;
int msgsystem::Times;
v2 msgsystem::Begin, msgsystem::End;
truth msgsystem::Enabled = true;
truth msgsystem::BigMessageMode = false;
truth msgsystem::MessagesChanged = true;
bitmap* msgsystem::QuickDrawCache = 0;
int msgsystem::LastMessageLines;


void msgsystem::AddMessage (cchar *Format, ...) {
  if (!Enabled) return;
  if (BigMessageMode && BigMessage.GetSize() >= 512) LeaveBigMessageMode();
  char Message[1024];

  va_list AP;
  va_start(AP, Format);
  vsnprintf(Message, sizeof(Message)-1, Format, AP);
  va_end(AP);

  festring Buffer(Message);

  if (!Buffer.GetSize()) ABORT("Empty message request!");

  soundsystem::playSound(Buffer);

  Buffer.Capitalize();

  /* Comment the first line and uncomment the second before the release! */
  if (isalpha(Buffer[Buffer.GetSize()-1]))
    //Buffer << " (this sentence isn't terminated correctly because Hex doesn't know grammar rules)";
    Buffer << '.';

  if (BigMessageMode) {
    if (BigMessage.GetSize()) BigMessage << ' ';
    BigMessage << Buffer;
    return;
  }

  ivantime Time;
  game::GetTime(Time);

  if (Buffer == LastMessage) {
    for (int c = 0; c < LastMessageLines; ++c) MessageHistory.Pop();
    ++Times;
    End = v2(Time.Hour, Time.Min);
  } else {
    Times = 1;
    Begin = End = v2(Time.Hour, Time.Min);
    LastMessage = Buffer;
    LastMessage.EnsureOwnsData();
  }

  festring Temp;
  Temp << Begin.X << ':';

  if(Begin.Y < 10)
    Temp << '0';

  Temp << Begin.Y;

  if(Begin != End)
  {
    Temp << '-' << End.X << ':';

    if(End.Y < 10)
      Temp << '0';

    Temp << End.Y;
  }

  if(Times != 1)
    Temp << " (" << Times << "x)";

  Temp << ' ';
  int Marginal = Temp.GetSize();
  Temp << Buffer;

  std::vector<festring> Chapter;
  festring::SplitString(Temp, Chapter, 78, Marginal);

  for(uint c = 0; c < Chapter.size(); ++c)
    MessageHistory.AddEntry(Chapter[c], WHITE);

  MessageHistory.SetSelected(MessageHistory.GetLastEntryIndex());
  LastMessageLines = Chapter.size();
  MessagesChanged = true;
}

void msgsystem::Draw()
{
  truth WasInBigMessageMode = BigMessageMode;
  LeaveBigMessageMode();

  if(MessagesChanged)
  {
    MessageHistory.QuickDraw(QuickDrawCache, 8);
    MessagesChanged = false;
  }

  v2 Size = QuickDrawCache->GetSize();
  int Y = RES.Y - 122;
  blitdata B = { DOUBLE_BUFFER,
     { 0, 0 },
     { 13, Y },
     { Size.X, Size.Y },
     { 0 },
     0,
     0 };

  QuickDrawCache->NormalBlit(B);
  igraph::BlitBackGround(v2(13, Y), v2(1, 1));
  igraph::BlitBackGround(v2(12 + Size.X, Y), v2(1, 1));
  igraph::BlitBackGround(v2(13, Y + Size.Y - 1), v2(1, 1));
  igraph::BlitBackGround(v2(12 + Size.X, Y + Size.Y - 1), v2(1, 1));

  if(WasInBigMessageMode)
    EnterBigMessageMode();
}

void msgsystem::DrawMessageHistory()
{
  MessageHistory.Draw();
}

void msgsystem::Format()
{
  MessageHistory.Empty();
  LastMessage.Empty();
  MessagesChanged = true;
  BigMessageMode = false;
}

void msgsystem::Save(outputfile& SaveFile)
{
  MessageHistory.Save(SaveFile);
  SaveFile << LastMessage << Times << Begin << End;
}

void msgsystem::Load(inputfile& SaveFile)
{
  MessageHistory.Load(SaveFile);
  SaveFile >> LastMessage >> Times >> Begin >> End;
}

void msgsystem::ScrollDown()
{
  if(MessageHistory.GetSelected() < MessageHistory.GetLastEntryIndex())
  {
    MessageHistory.EditSelected(1);
    MessagesChanged = true;
  }
}

void msgsystem::ScrollUp()
{
  if(MessageHistory.GetSelected())
  {
    MessageHistory.EditSelected(-1);
    MessagesChanged = true;
  }
}

void msgsystem::LeaveBigMessageMode()
{
  BigMessageMode = false;

  if(BigMessage.GetSize())
  {
    AddMessage("%s", BigMessage.CStr());
    BigMessage.Empty();
  }
}

void msgsystem::Init()
{
  QuickDrawCache = new bitmap(v2((game::GetScreenXSize() << 4) + 6, 106));
  QuickDrawCache->ActivateFastFlag();
  game::SetStandardListAttributes(MessageHistory);
  MessageHistory.AddFlags(INVERSE_MODE);
}

void msgsystem::ThyMessagesAreNowOld()
{
  if(MessageHistory.GetColor(MessageHistory.GetLastEntryIndex()) == WHITE)
    MessagesChanged = true;

  for(uint c = 0; c < MessageHistory.GetLength(); ++c)
    MessageHistory.SetColor(c, LIGHT_GRAY);
}


/* SOUND SYSTEM */
#ifndef DISABLE_SOUND
struct SoundFile {
  festring filename;
  Mix_Chunk *chunk;
  //Mix_Music *music;
};


struct SoundInfo {
  pcre *re;
  pcre_extra *extra;
  std::vector<int> sounds;
};

int soundsystem::SoundState = 0;

std::vector<SoundFile> soundsystem::files;
std::vector<SoundInfo> soundsystem::patterns;


int soundsystem::addFile (const festring &filename) {
  for (int i = 0; i < int(files.size()); i++) if (files[i].filename == filename) return i;
  SoundFile p;
  p.filename = filename;
  p.chunk = NULL;
  //p.music = NULL;
  files.push_back(p);
  return files.size()-1;
}

bool eol = false;

static festring getstr (FILE *f, truth word) {
  if (eol && word) return "";
  festring s;
  for (;;) {
    char c = fgetc(f);
    if (c == EOF) return s;
    if (c == 13) continue;
    if (c == 10 && s != "") return eol = true, s;
    if (c == 10) continue;
    if (c == ' ' && word && s != "") return s;
    s = s+c;
  }
}


void soundsystem::initSound () {
  const char *error;
  int erroffset;
  if (SoundState == 0) {
    //FILE *debf = fopen("snddebug.txt", "wt");
    FILE *debf = NULL;
    if (debf) fprintf(debf, "This file can be used to diagnose problems with sound.\n");
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 8000) != 0) {
      ADD_MESSAGE("Unable to initialize audio: %s", Mix_GetError());
      if (debf) fprintf(debf, "Unable to initialize audio: %s\n", Mix_GetError());
      SoundState = -1;
      return;
    }
    Mix_AllocateChannels(16);
    SoundState = -2;
    festring cfgfile = game::GetGameDir()+"Sound/config.txt";
    FILE *f = fopen(cfgfile.CStr(), "r");
    if (!f) SoundState = -1;
    else {
      festring Pattern, File;
      while ((Pattern = getstr(f, false)) != "") {
        SoundInfo si;
        si.re = pcre_compile(Pattern.CStr(), 0, &error, &erroffset, NULL);
        if (!si.re) {
          if (debf) fprintf(debf, "PCRE compilation failed at expression offset %d: %s\n", erroffset, error);
          ADD_MESSAGE("PCRE compilation failed at expression offset %d: %s", erroffset, error);
        } else {
          si.extra = pcre_study(si.re, 0, &error);
        }
        eol = false;
        while ((File = getstr(f, true)) != "") si.sounds.push_back(addFile(File));
        if (si.sounds.size() != 0) patterns.push_back(si);
      }
      fclose(f);
      SoundState = 1;
      //Mix_HookMusicFinished(changeMusic);
    }
    if (debf) fclose(debf);
  }
}


SoundFile *soundsystem::findMatchingSound (const festring &Buffer) {
  for (int i = patterns.size() - 1; i >= 0; i--)
  if (patterns[i].re)
    if (pcre_exec(patterns[i].re, patterns[i].extra, Buffer.CStr(), Buffer.GetSize(), 0, 0, NULL, 0) >= 0)
      return &files[patterns[i].sounds[rand()%patterns[i].sounds.size()]];
  return NULL;
}


void soundsystem::playSound (const festring &Buffer) {
  if (!ivanconfig::GetPlaySounds()) return;
  initSound();
  if (SoundState == 1) {
    SoundFile *sf = findMatchingSound(Buffer);
    if (!sf) return;
    if (!sf->chunk) {
      festring sndfile = game::GetGameDir()+"Sound/"+sf->filename;
      sf->chunk = Mix_LoadWAV(sndfile.CStr());
    }
    if (sf->chunk) {
      for (int i = 0; i < 16; i++) {
        if (!Mix_Playing(i)) {
          Mix_PlayChannel(i, sf->chunk, 0);
          //Mix_SetPosition(i, angle, dist);
          return;
        }
      }
    }
  }
}

#else

void soundsystem::initSound () {}
int soundsystem::addFile (const festring &filename) {}
SoundFile *soundsystem::findMatchingSound (const festring &Buffer) {}
void soundsystem::playSound (const festring &Buffer) {}

#endif
