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


#include "item_bodypart.cpp"
#include "bodypart_head.cpp"
#include "bodypart_torso.cpp"
#include "torso_normaltorso.cpp"
#include "torso_humanoidtorso.cpp"
#include "bodypart_arm.cpp"
#include "arm_rightarm.cpp"
#include "arm_leftarm.cpp"
#include "bodypart_groin.cpp"
#include "bodypart_leg.cpp"
#include "leg_rightleg.cpp"
#include "leg_leftleg.cpp"
#include "item_corpse.cpp"
#include "normaltorso_eddytorso.cpp"
#include "normaltorso_largetorso.cpp"
#include "corpse_largecorpse.cpp"
#include "head_ennerhead.cpp"
#include "head_playerkindhead.cpp"
#include "humanoidtorso_playerkindtorso.cpp"
#include "rightarm_playerkindrightarm.cpp"
#include "leftarm_playerkindleftarm.cpp"
#include "groin_playerkindgroin.cpp"
#include "rightleg_playerkindrightleg.cpp"
#include "leftleg_playerkindleftleg.cpp"
#include "normaltorso_magicmushroomtorso.cpp"
#include "largetorso_menatrixtorso.cpp"
#include "normaltorso_dogtorso.cpp"
#include "dogtorso_blinkdogtorso.cpp"
#include "normaltorso_mysticfrogtorso.cpp"
#include "largetorso_lobhsetorso.cpp"
#include "normaltorso_battorso.cpp"
#include "normaltorso_spidertorso.cpp"
