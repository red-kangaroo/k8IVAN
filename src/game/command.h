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

#include "ivancommon.h"

#include <vector>

#include "ivandef.h"


typedef truth (item::*sorter) (ccharacter *) const;

class command {
public:
  command (cchar *name, truth (*LinkedFunction)(character *), cchar *Description,
    char Key1, truth UsableInWilderness=false, truth WizardModeFunction=false);

  truth (*GetLinkedFunction() const) (character *) { return LinkedFunction; }

  cchar *GetDescription () const { return Description; }
  const festring &GetName () const { return mName; }
  inline char GetKey () const { return Key1; }
  inline void SetKey (char k1) { Key1 = k1; }
  truth IsUsableInWilderness () const { return UsableInWilderness; }
  truth IsWizardModeFunction () const { return WizardModeFunction; }

private:
  festring mName;
  truth (*LinkedFunction) (character *);
  cchar *Description;
  char Key1;
  truth UsableInWilderness;
  truth WizardModeFunction;
};


class commandsystem {
public:
  commandsystem ();

  static void ConfigureKeys ();
  static void SaveKeys (truth forced=false);

  static command *GetCommand (int idx) { return (idx >= 0 && idx < (int)mCommands.size()) ? mCommands[idx] : 0; }

  static truth Consume (character*, cchar*, sorter);

  static void RegisterCommand (command *);

protected:
  //static command *Command[];
  static std::vector<command *> mCommands;
};


#endif
