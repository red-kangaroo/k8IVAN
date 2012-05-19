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
#ifndef __COMMAND_H__
#define __COMMAND_H__

#include "ivandef.h"


typedef truth (item::*sorter) (ccharacter *) const;

class command {
public:
  command (truth (*LinkedFunction)(character *), cchar *Description,
    char Key1, char Key2, truth UsableInWilderness, truth WizardModeFunction=false);
  truth (*GetLinkedFunction() const) (character *) { return LinkedFunction; }
  cchar *GetDescription () const { return Description; }
  char GetKey () const;
  char GetKey1 () const { return Key1; }
  char GetKey2 () const { return Key2; }
  void SetKeys (char k1, char k2) { Key1 = k1; Key2 = k2; }
  truth IsUsableInWilderness () const { return UsableInWilderness; }
  truth IsWizardModeFunction () const { return WizardModeFunction; }

private:
  truth (*LinkedFunction) (character *);
  cchar *Description;
  char Key1;
  char Key2;
  truth UsableInWilderness;
  truth WizardModeFunction;
};


class commandsystem {
public:
  static command *GetCommand (int I) { return Command[I]; }
/*k8:private:*/
  static void ConfigureKeys ();
  static void SaveKeys (truth forced=false);
  static truth Apply (character *);
  static truth Close (character *);
  static truth Eat (character *);
  static truth Drink (character *);
  static truth Dip (character *);
  static truth DrawMessageHistory (character *);
  static truth Drop (character *);
  static truth ForceVomit (character *);
  static truth GoDown (character *);
  static truth GoUp (character *);
  static truth Kick (character *);
  static truth Look (character *);
  static truth NOP (character *);
  static truth Offer (character *);
  static truth Open (character *);
  static truth PickUp (character *);
  static truth Pray (character *);
  static truth Quit (character *);
  static truth Read (character *);
  static truth Save (character *);
  static truth ShowInventory (character *);
  static truth ShowKeyLayout (character *);
  static truth ShowWeaponSkills (character *);
  static truth Talk (character *);
  static truth Throw (character *);
  static truth EquipmentScreen (character *);
  static truth WhatToEngrave (character *);
  static truth Zap (character *);
  static truth Rest (character *);
  static truth Sit (character *);
  static truth Go (character *);
  static truth ShowConfigScreen (character *);
  static truth ScrollMessagesDown (character *);
  static truth ScrollMessagesUp (character *);
  static truth WieldInRightArm (character *);
  static truth WieldInLeftArm (character *);
  static truth AssignName (character *);
  static truth Search (character *);
  static truth Consume (character*, cchar*, sorter);
  static truth Burn (character *);
  static truth Dump (character *);
#ifdef WIZARD
  static truth WizardMode (character *);
  static truth RaiseStats (character *);
  static truth LowerStats (character *);
  static truth SeeWholeMap (character *);
  static truth WalkThroughWalls (character *);
  static truth RaiseGodRelations (character *);
  static truth LowerGodRelations (character *);
  static truth GainDivineKnowledge (character *);
  static truth GainAllItems (character *);
  static truth SecretKnowledge (character *);
  static truth DetachBodyPart (character *);
  static truth SummonMonster (character *);
  static truth LevelTeleport (character *);
  static truth Possess (character *);
  static truth Polymorph (character *);
  static truth GetScroll (character *);
  static truth WizardWish (character *);
  static truth OpenMondedr (character *);
  static truth ShowCoords (character *);
  static truth WizardHeal (character *);
#endif
  static truth ToggleRunning (character *);
  static truth IssueCommand (character *);

protected:
  static command *Command[];
};


#endif
