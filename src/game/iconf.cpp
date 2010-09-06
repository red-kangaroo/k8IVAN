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
#include "iconf.h"
#include "game.h"
#include "feio.h"
#include "area.h"
#include "graphics.h"
#include "bitmap.h"
#include "igraph.h"
#include "felist.h"


stringoption ivanconfig::DefaultName("DefaultName", "player's default name", "", &configsystem::NormalStringDisplayer, &DefaultNameChangeInterface);
stringoption ivanconfig::DefaultPetName("DefaultPetName", "starting pet's default name", CONST_S("Kenny"), &configsystem::NormalStringDisplayer, &DefaultPetNameChangeInterface);
numberoption ivanconfig::AutoSaveInterval("AutoSaveInterval", "autosave interval", 100, &AutoSaveIntervalDisplayer, &AutoSaveIntervalChangeInterface, &AutoSaveIntervalChanger);
scrollbaroption ivanconfig::Contrast("Contrast", "contrast", 100, &ContrastDisplayer, &ContrastChangeInterface, &ContrastChanger, &ContrastHandler);
truthoption ivanconfig::WarnAboutDanger("WarnAboutVeryDangerousMonsters", "Warn about very dangerous monsters", true);
truthoption ivanconfig::AutoDropLeftOvers("AutoDropLeftOvers", "drop food leftovers automatically", true);
truthoption ivanconfig::LookZoom("LookZoom", "zoom feature in look mode", false);
truthoption ivanconfig::UseAlternativeKeys("UseAlternativeKeys", "use alternative direction keys", false);
truthoption ivanconfig::BeNice("BeNice", "be nice to pets", true);
truthoption ivanconfig::FullScreenMode("FullScreenMode", "run the game in full screen mode", false, &configsystem::NormalTruthDisplayer, &configsystem::NormalTruthChangeInterface, &FullScreenModeChanger);
/*k8*/
truthoption ivanconfig::KickDownDoors("KickDownDoors", "Kick down doors by default", false);
truthoption ivanconfig::AutoCenterMap("AutoCenterMap", "Automatically center map when player moves", true);
truthoption ivanconfig::AutoCenterMapOnLook("AutoCenterMapOnLook", "Automatically center map when player looks", true);
truthoption ivanconfig::FastListMode("FastLists", "Instantly select list items with alpha keys", true, &configsystem::NormalTruthDisplayer, &configsystem::NormalTruthChangeInterface, &FastListChanger);
/*k8*/


col24 ivanconfig::ContrastLuminance = NORMAL_LUMINANCE;
v2 ivanconfig::GetQuestionPos () { return game::IsRunning() ? v2(16, 6) : v2(30, 30); }
void ivanconfig::BackGroundDrawer () { game::DrawEverythingNoBlit(); }


void ivanconfig::AutoSaveIntervalDisplayer (const numberoption *O, festring &Entry) {
  if (O->Value) {
    Entry << O->Value << " turn";
    if (O->Value != 1) Entry << 's';
  } else {
    Entry << "disabled";
  }
}


void ivanconfig::ContrastDisplayer (const numberoption *O, festring &Entry) {
  Entry << O->Value << "/100";
}


truth ivanconfig::DefaultNameChangeInterface (stringoption *O) {
  festring String;
  if (iosystem::StringQuestion(String, CONST_S("Set new default name (1-20 letters):"), GetQuestionPos(), WHITE, 0, 20, !game::IsRunning(), true) == NORMAL_EXIT)
    O->ChangeValue(String);
  if (game::IsRunning()) igraph::BlitBackGround(v2(16, 6), v2(game::GetScreenXSize() << 4, 23));
  return false;
}


truth ivanconfig::DefaultPetNameChangeInterface (stringoption *O) {
  festring String;
  if (iosystem::StringQuestion(String, CONST_S("Set new default name for the starting pet (1-20 letters):"), GetQuestionPos(), WHITE, 0, 20, !game::IsRunning(), true) == NORMAL_EXIT)
    O->ChangeValue(String);
  if (game::IsRunning()) igraph::BlitBackGround(v2(16, 6), v2(game::GetScreenXSize() << 4, 23));
  return false;
}


truth ivanconfig::AutoSaveIntervalChangeInterface (numberoption *O) {
  O->ChangeValue(iosystem::NumberQuestion(CONST_S("Set new autosave interval (1-50000 turns, 0 for never):"), GetQuestionPos(), WHITE, !game::IsRunning()));
  if (game::IsRunning()) igraph::BlitBackGround(v2(16, 6), v2(game::GetScreenXSize() << 4, 23));
  return false;
}


truth ivanconfig::ContrastChangeInterface (numberoption *O) {
  iosystem::ScrollBarQuestion(CONST_S("Set new contrast value (0-200, '<' and '>' move the slider):"), GetQuestionPos(), O->Value, 5, 0, 200, O->Value, WHITE, LIGHT_GRAY, DARK_GRAY, game::GetMoveCommandKey(KEY_LEFT_INDEX), game::GetMoveCommandKey(KEY_RIGHT_INDEX), !game::IsRunning(), static_cast<scrollbaroption*>(O)->BarHandler);
  if (game::IsRunning()) igraph::BlitBackGround(v2(16, 6), v2(game::GetScreenXSize() << 4, 23));
  return false;
}


void ivanconfig::AutoSaveIntervalChanger (numberoption *O, long What) {
  if (What < 0) What = 0;
  if (What > 50000) What = 50000;
  O->Value = What;
}


void ivanconfig::ContrastChanger (numberoption *O, long What) {
  if (What < 0) What = 0;
  if (What > 200) What = 200;
  O->Value = What;
  CalculateContrastLuminance();
}


void ivanconfig::FullScreenModeChanger (truthoption *, truth) {
  graphics::SwitchMode();
}


void ivanconfig::FastListChanger (truthoption *O, truth value) {
  //fprintf(stderr, "FLC: %s\n", value ? "TAN" : "ona");
  FastListMode.Value = !FastListMode.Value;
  felist::SetFastListMode(!FastListMode.Value);
}


void ivanconfig::Show () {
  configsystem::Show(&BackGroundDrawer, &game::SetStandardListAttributes, game::IsRunning());
}


void ivanconfig::ContrastHandler (long Value) {
  ContrastChanger(&Contrast, Value);
  if (game::IsRunning()) {
    game::GetCurrentArea()->SendNewDrawRequest();
    game::DrawEverythingNoBlit();
  }
}


void ivanconfig::SwitchModeHandler () {
  FullScreenMode.Value = !FullScreenMode.Value;
  Save();
}


void ivanconfig::CalculateContrastLuminance () {
  int Element = Min<long>((GetContrast() << 7) / 100, 255);
  ContrastLuminance = MakeRGB24(Element, Element, Element);
}


/* k8 */
static char *getMyDir (void) {
  static char buf[128], myDir[8192];
  pid_t mypid = getpid();
  memset(myDir, 0, sizeof(myDir));
  sprintf(buf, "/proc/%u/exe", (unsigned int)mypid);
  if (readlink(buf, myDir, sizeof(myDir)-1) < 0) strcpy(myDir, ".");
  else {
    char *p = (char *)strrchr(myDir, '/');
    if (!p) strcpy(myDir, "."); else *p = '\0';
  }
  if (myDir[strlen(myDir)-1] == '/') myDir[strlen(myDir)-1] = '\0';
  return myDir;
}
/* k8 */


static festring getConfigPath () {
  festring path;
#ifdef LOCAL_SAVES
  path << getMyDir();
#else
  path << getenv("HOME");
#endif
  return path;
}


void ivanconfig::Initialize () {
  configsystem::AddOption(&DefaultName);
  configsystem::AddOption(&DefaultPetName);
  configsystem::AddOption(&AutoSaveInterval);
  configsystem::AddOption(&Contrast);
  configsystem::AddOption(&WarnAboutDanger);
  configsystem::AddOption(&AutoDropLeftOvers);
  configsystem::AddOption(&LookZoom);
  configsystem::AddOption(&UseAlternativeKeys);
  configsystem::AddOption(&BeNice);
  configsystem::AddOption(&FullScreenMode);
/*k8*/
  configsystem::AddOption(&KickDownDoors);
  configsystem::AddOption(&AutoCenterMap);
  configsystem::AddOption(&AutoCenterMapOnLook);
  configsystem::AddOption(&FastListMode);
/*k8*/
  configsystem::SetConfigFileName(getConfigPath()+"/.ivan.conf");
  configsystem::Load();
  felist::SetFastListMode(!FastListMode.Value);
  CalculateContrastLuminance();
}
