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
#include "fesave.h"
#include "feio.h"
#include "area.h"
#include "graphics.h"
#include "bitmap.h"
#include "igraph.h"
#include "felist.h"

#include "message.h"
#include "command.h"


stringoption ivanconfig::DefaultName("DefaultName", "player's default name", "", &configsystem::NormalStringDisplayer, &DefaultNameChangeInterface);
stringoption ivanconfig::DefaultPetName("DefaultPetName", "starting pet's default name", CONST_S("Kenny"), &configsystem::NormalStringDisplayer, &DefaultPetNameChangeInterface);
numberoption ivanconfig::AutoSaveInterval("AutoSaveInterval", "autosave interval", 100, &AutoSaveIntervalDisplayer, &AutoSaveIntervalChangeInterface, &AutoSaveIntervalChanger);
scrollbaroption ivanconfig::Contrast("Contrast", "contrast", 100, &ContrastDisplayer, &ContrastChangeInterface, &ContrastChanger, &ContrastHandler);
truthoption ivanconfig::WarnAboutDanger("WarnAboutVeryDangerousMonsters", "Warn about very dangerous monsters", true);
truthoption ivanconfig::AutoDropLeftOvers("AutoDropLeftOvers", "drop food leftovers automatically", true);
truthoption ivanconfig::AutoDropBottles("AutoDropBottles", "automatically drop empty bottles after drinking", true);
truthoption ivanconfig::AutoDropCans("AutoDropCans", "automatically drop empty cans after eating/drinking", false);
truthoption ivanconfig::LookZoom("LookZoom", "zoom feature in look mode", false);
truthoption ivanconfig::UseAlternativeKeys("UseAlternativeKeys", "use alternative direction keys", false);
truthoption ivanconfig::BeNice("BeNice", "be nice to pets", true);
truthoption ivanconfig::FullScreenMode("FullScreenMode", "run the game in full screen mode", false, &configsystem::NormalTruthDisplayer, &configsystem::NormalTruthChangeInterface, &FullScreenModeChanger);
/*k8*/
truthoption ivanconfig::KickDownDoors("KickDownDoors", "kick down doors by default", false);
truthoption ivanconfig::AutoCenterMap("AutoCenterMap", "automatically center map when player moves", true);
truthoption ivanconfig::AutoCenterMapOnLook("AutoCenterMapOnLook", "automatically center map when player looks", true);
truthoption ivanconfig::FastListMode("FastLists", "instantly select list items with alpha keys", true, &configsystem::NormalTruthDisplayer, &configsystem::NormalTruthChangeInterface, &FastListChanger);
truthoption ivanconfig::PlaySounds("PlaySounds", "use sounds", false);
scrollbaroption ivanconfig::SoundVolume("SoundVolume", "sound volume", 128, &SoundVolumeDisplayer, &SoundVolumeChangeInterface, &ContrastChanger, &SoundVolumeHandler);
truthoption ivanconfig::ConfirmCorpses("ConfirmCorpses", "confirm corpse pickup", true);
truthoption ivanconfig::StopOnCorpses("StopOnCorpses", "abort going on corpses", false);
truthoption ivanconfig::StopOnSeenItems("StopOnSeenItems", "abort going on seen items", false);
truthoption ivanconfig::ConfirmScrollReading("ConfirmScrollReading", "confirm scroll reading", false);
numberoption ivanconfig::GoingDelay("GoingDelay", "delay betwen steps in 'go' command", 100, &GoingDelayDisplayer, &GoingDelayChangeInterface, &GoingDelayChanger);
truthoption ivanconfig::UseMaximumCompression("UseMaximumCompression", "use maximum compression", false);
truthoption ivanconfig::ShowFullItemDesc("ShowFullItemDesc", "show detailed description of lying item", false);


void ivanconfig::SoundVolumeDisplayer (const numberoption *O, festring &Entry) {
  Entry << O->Value << "/128";
}


truth ivanconfig::SoundVolumeChangeInterface (numberoption *O) {
  iosystem::ScrollBarQuestion(CONST_S("Set new sound volume (0-128, '<' and '>' move the slider):"),
    GetQuestionPos(), O->Value, 16, 0, 128, O->Value, WHITE, LIGHT_GRAY, DARK_GRAY, game::GetMoveCommandKey(KEY_LEFT_INDEX), game::GetMoveCommandKey(KEY_RIGHT_INDEX), !game::IsRunning(), static_cast<scrollbaroption*>(O)->BarHandler);
  if (game::IsRunning()) igraph::BlitBackGround(v2(16, 6), v2(game::GetScreenXSize() << 4, 23));
  return false;
}


void ivanconfig::SoundVolumeChanger (numberoption *O, sLong What) {
  if (What < 0) What = 0;
  if (What > 128) What = 128;
  O->Value = What;
  soundsystem::setVolume(What);
}


void ivanconfig::SoundVolumeHandler (sLong Value) {
  SoundVolumeChanger(&SoundVolume, Value);
}
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


void ivanconfig::AutoSaveIntervalChanger (numberoption *O, sLong What) {
  if (What < 0) What = 0;
  if (What > 50000) What = 50000;
  O->Value = What;
}


void ivanconfig::ContrastChanger (numberoption *O, sLong What) {
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


void ivanconfig::ContrastHandler (sLong Value) {
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
  int Element = Min<sLong>((GetContrast() << 7) / 100, 255);
  ContrastLuminance = MakeRGB24(Element, Element, Element);
}


void ivanconfig::GoingDelayDisplayer (const numberoption *O, festring &Entry) {
  if (O->Value) {
    Entry << O->Value << " ms";
  } else {
    Entry << "disabled";
  }
}


truth ivanconfig::GoingDelayChangeInterface (numberoption *O) {
  O->ChangeValue(iosystem::NumberQuestion(CONST_S("Set new 'go' delay (0-2000):"), GetQuestionPos(), WHITE, !game::IsRunning()));
  if (game::IsRunning()) igraph::BlitBackGround(v2(16, 6), v2(game::GetScreenXSize() << 4, 23));
  return false;
}


void ivanconfig::GoingDelayChanger (numberoption *O, sLong What) {
  if (What < 0) What = 0;
  if (What > 2000) What = 2000;
  O->Value = What;
}


festring ivanconfig::GetMyDir (void) { return inputfile::GetMyDir(); }


static festring getConfigPath () {
  festring path;
#ifdef LOCAL_SAVES
  path << ivanconfig::GetMyDir();
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
  configsystem::AddOption(&AutoDropBottles);
  configsystem::AddOption(&AutoDropCans);
  configsystem::AddOption(&LookZoom);
  configsystem::AddOption(&UseAlternativeKeys);
  configsystem::AddOption(&BeNice);
  configsystem::AddOption(&FullScreenMode);
/*k8*/
  configsystem::AddOption(&KickDownDoors);
  configsystem::AddOption(&AutoCenterMap);
  configsystem::AddOption(&AutoCenterMapOnLook);
  configsystem::AddOption(&FastListMode);
  configsystem::AddOption(&PlaySounds);
  configsystem::AddOption(&SoundVolume);
  configsystem::AddOption(&ConfirmCorpses);
  configsystem::AddOption(&StopOnCorpses);
  configsystem::AddOption(&StopOnSeenItems);
  configsystem::AddOption(&ConfirmScrollReading);
  configsystem::AddOption(&GoingDelay);
  configsystem::AddOption(&UseMaximumCompression);
  configsystem::AddOption(&ShowFullItemDesc);
/*k8*/
  configsystem::SetConfigFileName(getConfigPath()+"/.ivan.rc");
  configsystem::Load();
  felist::SetFastListMode(!FastListMode.Value);
  CalculateContrastLuminance();

  commandsystem::ConfigureKeys();
  commandsystem::SaveKeys(false);
}
