#ifdef HEADER_PHASE
class humanoid;
class sweaponskill;


struct scar {
  int Severity;
  mutable bitmap *PanelBitmap;
};


struct damageid {
  int SrcID, Amount;
};


outputfile &operator << (outputfile &, const scar &);
inputfile &operator >> (inputfile &, scar &);


RAW_SAVE_LOAD(damageid);

#else

outputfile &operator << (outputfile &SaveFile, const scar &Scar) {
  SaveFile << Scar.Severity << Scar.PanelBitmap;
  return SaveFile;
}


inputfile &operator >> (inputfile &SaveFile, scar &Scar) {
  SaveFile >> Scar.Severity >> Scar.PanelBitmap;
  return SaveFile;
}
#endif


#include "bodyparts/item_bodypart.cpp"
#include "bodyparts/item_corpse.cpp"

#include "bodyparts/bodypart_arm.cpp"
#include "bodyparts/bodypart_groin.cpp"
#include "bodyparts/bodypart_head.cpp"
#include "bodyparts/bodypart_leg.cpp"
#include "bodyparts/bodypart_torso.cpp"

#include "bodyparts/head_ennerhead.cpp"
#include "bodyparts/head_playerkindhead.cpp"

#include "bodyparts/arm_leftarm.cpp"
#include "bodyparts/arm_rightarm.cpp"
#include "bodyparts/leftarm_playerkindleftarm.cpp"
#include "bodyparts/rightarm_playerkindrightarm.cpp"

#include "bodyparts/leg_leftleg.cpp"
#include "bodyparts/leg_rightleg.cpp"
#include "bodyparts/rightleg_playerkindrightleg.cpp"
#include "bodyparts/leftleg_playerkindleftleg.cpp"

#include "bodyparts/groin_playerkindgroin.cpp"

#include "bodyparts/torso_normaltorso.cpp"
#include "bodyparts/torso_humanoidtorso.cpp"
#include "bodyparts/humanoidtorso_playerkindtorso.cpp"

#include "bodyparts/normaltorso_battorso.cpp"
#include "bodyparts/normaltorso_dogtorso.cpp"
#include "bodyparts/normaltorso_eddytorso.cpp"
#include "bodyparts/normaltorso_largetorso.cpp"
#include "bodyparts/normaltorso_magicmushroomtorso.cpp"
#include "bodyparts/normaltorso_mysticfrogtorso.cpp"
#include "bodyparts/normaltorso_spidertorso.cpp"

#include "bodyparts/corpse_largecorpse.cpp"

#include "bodyparts/dogtorso_blinkdogtorso.cpp"

#include "bodyparts/largetorso_lobhsetorso.cpp"
#include "bodyparts/largetorso_menatrixtorso.cpp"
