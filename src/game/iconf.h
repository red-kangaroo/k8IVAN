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

#include "ivancommon.h"

#include "config.h"
#include "v2.h"


class ivanconfig {
public:
  static cfestring &GetDefaultName () { return DefaultName.Value; }
  static cfestring &GetDefaultPetName () { return DefaultPetName.Value; }
  static sLong GetAutoSaveInterval () { return AutoSaveInterval.Value; }
  static sLong GetContrast () { return Contrast.Value; }
  static truth GetWarnAboutDanger () { return WarnAboutDanger.Value; }
  static truth GetAutoDropLeftOvers () { return AutoDropLeftOvers.Value; }
  static truth GetAutoDropBottles () { return AutoDropBottles.Value; }
  static truth GetAutoDropCans () { return AutoDropCans.Value; }
  static truth GetLookZoom () { return LookZoom.Value; }
  static truth GetUseAlternativeKeys () { return UseAlternativeKeys.Value; }
  static truth GetBeNice () { return BeNice.Value; }
  static truth GetFullScreenMode () { return FullScreenMode.Value; }
  static void SwitchModeHandler ();
  static truth GetKickDownDoors () { return KickDownDoors.Value; }
  static truth GetAutoCenterMap () { return AutoCenterMap.Value; }
  static truth GetAutoCenterMapOnLook () { return AutoCenterMapOnLook.Value; }
  static truth GetPlaySounds () { return PlaySounds.Value; }
  static sLong GetSoundVolume () { return SoundVolume.Value; }
  static truth GetConfirmCorpses () { return ConfirmCorpses.Value; }
  static int GetGoingDelay () { return GoingDelay.Value; }
  static truth GetStopOnCorpses () { return StopOnCorpses.Value; }
  static truth GetStopOnSeenItems () { return StopOnSeenItems.Value; }
  static truth GetConfirmScrollReading () { return ConfirmScrollReading.Value; }
  static truth GetUseMaximumCompression () { return UseMaximumCompression.Value; }
  static truth GetShowFullItemDesc () { return ShowFullItemDesc.Value; }
  static sLong ApplyContrastTo (sLong);
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
  static void AutoSaveIntervalChanger (numberoption *, sLong);
  static void ContrastChanger (numberoption *, sLong);
  static void FullScreenModeChanger (truthoption *, truth);
  static void FastListChanger (truthoption *, truth);
  static void ContrastHandler (sLong);
  static void BackGroundDrawer ();

  static void SoundVolumeDisplayer (const numberoption *, festring &);
  static truth SoundVolumeChangeInterface (numberoption *);
  static void SoundVolumeChanger (numberoption *, sLong);
  static void SoundVolumeHandler (sLong);

  static void GoingDelayDisplayer (const numberoption *O, festring &Entry);
  static truth GoingDelayChangeInterface (numberoption *O);
  static void GoingDelayChanger (numberoption *O, sLong What);

private:
  static stringoption DefaultName;
  static stringoption DefaultPetName;
  static numberoption AutoSaveInterval;
  static scrollbaroption Contrast;
  static truthoption WarnAboutDanger;
  static truthoption AutoDropLeftOvers;
  static truthoption AutoDropBottles;
  static truthoption AutoDropCans;
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
  static truthoption ConfirmScrollReading;
  static truthoption UseMaximumCompression;
  static col24 ContrastLuminance;
  static truthoption ShowFullItemDesc;
};


inline sLong ivanconfig::ApplyContrastTo (sLong L) {
  sLong C = Contrast.Value;
  if (C == 100) return L;
  return MakeRGB24(41*GetRed24(L)*C>>12, 41*GetGreen24(L)*C>>12, 41*GetBlue24(L)*C>>12);
}


#endif
