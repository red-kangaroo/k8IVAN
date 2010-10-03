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
#ifndef __ICONF_H__
#define __ICONF_H__

#include "config.h"
#include "v2.h"


class ivanconfig {
public:
  static cfestring &GetDefaultName () { return DefaultName.Value; }
  static cfestring &GetDefaultPetName () { return DefaultPetName.Value; }
  static long GetAutoSaveInterval () { return AutoSaveInterval.Value; }
  static long GetContrast () { return Contrast.Value; }
  static truth GetWarnAboutDanger () { return WarnAboutDanger.Value; }
  static truth GetAutoDropLeftOvers () { return AutoDropLeftOvers.Value; }
  static truth GetLookZoom () { return LookZoom.Value; }
  static truth GetUseAlternativeKeys () { return UseAlternativeKeys.Value; }
  static truth GetBeNice () { return BeNice.Value; }
  static truth GetFullScreenMode () { return FullScreenMode.Value; }
  static void SwitchModeHandler ();
  static truth GetKickDownDoors () { return KickDownDoors.Value; }
  static truth GetAutoCenterMap () { return AutoCenterMap.Value; }
  static truth GetAutoCenterMapOnLook () { return AutoCenterMapOnLook.Value; }
  static truth GetPlaySounds () { return PlaySounds.Value; }
  static long GetSoundVolume () { return SoundVolume.Value; }
  static truth GetConfirmCorpses () { return ConfirmCorpses.Value; }
  static int GetGoingDelay () { return GoingDelay.Value; }
  static truth GetStopOnCorpses () { return StopOnCorpses.Value; }
  static truth GetStopOnSeenItems () { return StopOnSeenItems.Value; }
  static long ApplyContrastTo (long);
  static void Save () { configsystem::Save(); }
  static void Load () { configsystem::Load(); }
  static void CalculateContrastLuminance ();
  static col24 GetContrastLuminance () { return ContrastLuminance; }
  static void Initialize ();
  static void Show ();

  static festring GetMyDir ();

private:
  static v2 GetQuestionPos ();
  static void AutoSaveIntervalDisplayer (const numberoption *, festring &);
  static void ContrastDisplayer (const numberoption *, festring &);
  static truth DefaultNameChangeInterface (stringoption *);
  static truth DefaultPetNameChangeInterface (stringoption *);
  static truth AutoSaveIntervalChangeInterface (numberoption *);
  static truth ContrastChangeInterface (numberoption *);
  static void AutoSaveIntervalChanger (numberoption *, long);
  static void ContrastChanger (numberoption *, long);
  static void FullScreenModeChanger (truthoption *, truth);
  static void FastListChanger (truthoption *, truth);
  static void ContrastHandler (long);
  static void BackGroundDrawer ();

  static void SoundVolumeDisplayer (const numberoption *, festring &);
  static truth SoundVolumeChangeInterface (numberoption *);
  static void SoundVolumeChanger (numberoption *, long);
  static void SoundVolumeHandler (long);

  static void GoingDelayDisplayer (const numberoption *O, festring &Entry);
  static truth GoingDelayChangeInterface (numberoption *O);
  static void GoingDelayChanger (numberoption *O, long What);

private:
  static stringoption DefaultName;
  static stringoption DefaultPetName;
  static numberoption AutoSaveInterval;
  static scrollbaroption Contrast;
  static truthoption WarnAboutDanger;
  static truthoption AutoDropLeftOvers;
  static truthoption LookZoom;
  static truthoption UseAlternativeKeys;
  static truthoption BeNice;
  static truthoption FullScreenMode;
  /*k8*/
  static truthoption KickDownDoors;
  static truthoption AutoCenterMap;
  static truthoption AutoCenterMapOnLook;
  static truthoption FastListMode;
  static truthoption PlaySounds;
  static scrollbaroption SoundVolume;
  static truthoption ConfirmCorpses;
  static numberoption GoingDelay;
  static truthoption StopOnCorpses;
  static truthoption StopOnSeenItems;
  /*k8*/
  static col24 ContrastLuminance;
};


inline long ivanconfig::ApplyContrastTo (long L) {
  long C = Contrast.Value;
  if (C == 100) return L;
  return MakeRGB24(41*GetRed24(L)*C>>12, 41*GetGreen24(L)*C>>12, 41*GetBlue24(L)*C>>12);
}


#endif
