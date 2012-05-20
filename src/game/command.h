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

#include <vector>

#include "ivandef.h"


typedef truth (item::*sorter) (ccharacter *) const;

class command {
public:
  command (cchar *name, truth (*LinkedFunction)(character *), cchar *Description,
    char Key1, char Key2, truth UsableInWilderness, truth WizardModeFunction=false);

  truth (*GetLinkedFunction() const) (character *) { return LinkedFunction; }

  cchar *GetDescription () const { return Description; }
  const festring &GetName () const { return mName; }
  char GetKey () const;
  char GetKey1 () const { return Key1; }
  char GetKey2 () const { return Key2; }
  void SetKeys (char k1, char k2) { Key1 = k1; Key2 = k2; }
  truth IsUsableInWilderness () const { return UsableInWilderness; }
  truth IsWizardModeFunction () const { return WizardModeFunction; }

private:
  festring mName;
  truth (*LinkedFunction) (character *);
  cchar *Description;
  char Key1;
  char Key2;
  truth UsableInWilderness;
  truth WizardModeFunction;
};


class commandsystem {
public:
  commandsystem ();

  static void ConfigureKeys ();
  static void SaveKeys (truth forced=false);

  static command *GetCommand (int I) { return mCommands[I]; }

  static truth Consume (character*, cchar*, sorter);

  static void RegisterCommand (command *);

protected:
  //static command *Command[];
  static std::vector<command *> mCommands;
};


#endif
