/* *
 *  Iter Vehemens ad Necem (IVAN)
 *  Copyright (C) Timo Kiviluoto
 *  Released under the GNU General
 *  Public License
 *
 *  See LICENSING which should be included
 *  along with this file for more details
 *
 */
#include <string.h>
#include <stdio.h>

#include "fesave.h"
#include "command.h"
#include "char.h"
#include "message.h"
#include "game.h"
#include "stack.h"
#include "room.h"
#include "god.h"
#include "felist.h"
#include "iconf.h"
#include "bitmap.h"
#include "actions.h"
#include "miscitem.h"
#include "worldmap.h"
#include "wsquare.h"
#include "wterras.h"
#include "materia.h"
#include "database.h"
#include "team.h"

#ifdef WIZARD
#include "proto.h"
#endif


#define COMMAND(name)  static truth name (character *Char)


////////////////////////////////////////////////////////////////////////////////
/* k8 */
/* SLOOOOWWW! */
static void cstrTrim (char *buf) {
  char *p = buf, *t;
  while (*p && *((unsigned char *)(p)) <= ' ') p++;
  if (!p[0]) { buf[0] = '\0'; return; } /* empty line */
  if (p > buf) { t = buf; while (*p) *t++ = *p++; }
  p = buf+strlen(buf)-1;
  while (p >= buf && *((unsigned char *)(p)) <= ' ') p--;
  if (p < buf) { buf[0] = '\0'; return; } /* empty line */
  *(++p) = '\0';
}


////////////////////////////////////////////////////////////////////////////////
command::command(cchar *name, truth (*LinkedFunction)(character *), cchar *Description, char Key1, char Key2, truth UsableInWilderness, truth WizardModeFunction) :
  mName(name),
  LinkedFunction(LinkedFunction),
  Description(Description),
  Key1(Key1), Key2(Key2),
  UsableInWilderness(UsableInWilderness),
  WizardModeFunction(WizardModeFunction)
{
}


char command::GetKey () const { return !ivanconfig::GetUseAlternativeKeys() ? Key1 : Key2; }


////////////////////////////////////////////////////////////////////////////////
static const char *adirNames[9] = {
  "dir-left-up",
  "dir-up",
  "dir-right-up",
  "dir-left",
  "dir-right",
  "dir-left-down",
  "dir-down",
  "dir-right-down",
  "dir-none"
};


/* k8 */
void commandsystem::ConfigureKeys () {
  command *cmd;
  char buf[512];
  festring fname;
#ifdef LOCAL_SAVES
  fname << ivanconfig::GetMyDir();
#else
  fname << getenv("HOME");
#endif
  fname << "/.keys.rc";
  FILE *fl = fopen(fname.CStr(), "r");
  if (!fl) return;
  while (fgets(buf, 510, fl)) {
    cstrTrim(buf);
    char k1 = 0, k2 = 0, i1;
    if (buf[0] == '=') {
      i1 = 1;
    } else if (buf[2] != '=') {
      if (buf[1] != '=') continue;
      i1 = 2;
      k1 = buf[0];
    } else {
      i1 = 3;
      k1 = buf[0];
      k2 = buf[1];
    }
    for (int f = 0; (cmd = commandsystem::GetCommand(f)) != 0; ++f) {
      if (!strcmp(buf+i1, cmd->GetName().CStr())) cmd->SetKeys(k1, k2);
    }
    for (int f = 0; f <= 8; f++) if (!strcmp(buf+i1, adirNames[f])) game::SetAbnormalMoveKey(f, k1);
  }
  fclose(fl);
}


void commandsystem::SaveKeys (truth forced) {
  command *cmd;
  festring fname;
  int isWizard = 0;
#ifdef LOCAL_SAVES
  fname << ivanconfig::GetMyDir();
#else
  fname << getenv("HOME");
#endif
  fname << "/.keys.rc";
  if (!forced) {
    if (inputfile::fileExists(fname)) return;
  }
  FILE *fl = fopen(fname.CStr(), "w");
  if (!fl) return;
  for (; isWizard < 2; isWizard++) {
    fputs(isWizard ? "\n# wizard actions\n" : "# actions\n", fl);
    for (int f = 0; (cmd = commandsystem::GetCommand(f)) != 0; ++f) {
      if (cmd->IsWizardModeFunction()) {
        if (!isWizard) continue;
      } else {
        if (isWizard) continue;
      }
      cchar *dsc = cmd->GetName().CStr();
      char k1 = cmd->GetKey1(), k2 = cmd->GetKey2();
      if (!dsc || !dsc[0]) continue;
      if (k1 && k2 && k1 != k2) fprintf(fl, "%c%c", k1, k2);
      else if (k1) fprintf(fl, "%c", k1);
      else if (k2) fprintf(fl, "%c", k2);
      fprintf(fl, "=%s\n", dsc);
    }
  }
  fputs("\n# alternate movement\n", fl);
  for (int f = 0; f <= 8; f++) {
    char ch = game::GetAbnormalMoveKey(f);
    if (ch) fputc(ch, fl);
    fprintf(fl, "=%s\n", adirNames[f]);
  }
  fclose(fl);
}
/* k8 */


////////////////////////////////////////////////////////////////////////////////
std::vector<command *> commandsystem::mCommands;


void commandsystem::RegisterCommand (command *cmd) {
  for (unsigned int f = 0; f < mCommands.size(); ++f) {
    if (mCommands[f]->GetName() == cmd->GetName()) {
      // replace command
      delete mCommands[f];
      mCommands[f] = cmd;
      return;
    }
  }
  mCommands.push_back(cmd);
}


////////////////////////////////////////////////////////////////////////////////
static truth Consume (character* Char, cchar *ConsumeVerb, sorter Sorter) {
  lsquare *Square = Char->GetLSquareUnder();
  stack *Inventory = Char->GetStack();
  stack *StackUnder = Square->GetStack();
  if ((game::IsInWilderness() || !StackUnder->SortedItems(Char, Sorter)) && !Inventory->SortedItems(Char, Sorter)) {
    ADD_MESSAGE("You have nothing to %s!", ConsumeVerb);
    return false;
  }
  itemvector Item;
  festring Question = CONST_S("What do you wish to ")+ConsumeVerb+'?';
  if (!game::IsInWilderness() && StackUnder->SortedItems(Char, Sorter))
    Inventory->DrawContents(Item, StackUnder, Char, Question, CONST_S("Items in your inventory"), CONST_S("Items on the ground"), CONST_S(""), 0, NO_MULTI_SELECT, Sorter);
  else
    Inventory->DrawContents(Item, Char, Question, NO_MULTI_SELECT, Sorter);
  return !Item.empty() ? Char->ConsumeItem(Item[0], ConsumeVerb+CONST_S("ing")) : false;
}


////////////////////////////////////////////////////////////////////////////////
#include "commands/0list.cpp"


commandsystem::commandsystem () {
  RegisterCommand(new command("Apply", &Apply, "apply", 'a', 'a', false));
  RegisterCommand(new command("Talk", &Talk, "chat", 'C', 'C', false));
  RegisterCommand(new command("Close", &Close, "close", 'c', 'c', false));
  RegisterCommand(new command("Dip", &Dip, "dip", '!', '!', false));
  RegisterCommand(new command("Dump", &Dump, "dump potion/can contents", 'U', 'U', false));
  RegisterCommand(new command("Drink", &Drink, "drink", 'D', 'D', true));
  RegisterCommand(new command("Drop", &Drop, "drop", 'd', 'd', true));
  RegisterCommand(new command("Eat", &Eat, "eat", 'e', 'e', true));
  RegisterCommand(new command("WhatToEngrave", &WhatToEngrave, "engrave", 'G', 'G', false));
  RegisterCommand(new command("EquipmentScreen", &EquipmentScreen, "equipment menu", 'E', 'E', true));
  RegisterCommand(new command("Go", &Go, "go", 'g', 'g', false));
  RegisterCommand(new command("GoDown", &GoDown, "go down/enter area", '>', '>', true));
  RegisterCommand(new command("GoUp", &GoUp, "go up", '<', '<', true));
  RegisterCommand(new command("IssueCommand", &IssueCommand, "issue command(s) to team member(s)", 'I', 'I', false));
  RegisterCommand(new command("Kick", &Kick, "kick", 'k', 'K', false));
  RegisterCommand(new command("Look", &Look, "look", 'l', 'L', true));
  RegisterCommand(new command("AssignName", &AssignName, "name", 'n', 'n', false));
  RegisterCommand(new command("Offer", &Offer, "offer", 'O', 'f', false));
  RegisterCommand(new command("Open", &Open, "open", 'o', 'O', false));
  RegisterCommand(new command("PickUp", &PickUp, "pick up", ',', ',', false));
  RegisterCommand(new command("Pray", &Pray, "pray", 'p', 'p', false));
  RegisterCommand(new command("Quit", &Quit, "quit", 'Q', 'Q', true));
  RegisterCommand(new command("Read", &Read, "read", 'r', 'r', false));
  RegisterCommand(new command("Rest", &Rest, "rest/heal", 'h', 'h', true));
  RegisterCommand(new command("Save", &Save, "save game", 'S', 'S', true));
  RegisterCommand(new command("ScrollMessagesDown", &ScrollMessagesDown, "scroll messages down", '+', '+', true));
  RegisterCommand(new command("ScrollMessagesUp", &ScrollMessagesUp, "scroll messages up", '-', '-', true));
  RegisterCommand(new command("ShowConfigScreen", &ShowConfigScreen, "show config screen", '\\', '\\', true));
  RegisterCommand(new command("ShowInventory", &ShowInventory, "show inventory", 'i', 'i', true));
  RegisterCommand(new command("ShowKeyLayout", &ShowKeyLayout, "show key layout", '?', '?', true));
  RegisterCommand(new command("DrawMessageHistory", &DrawMessageHistory, "show message history", 'M', 'M', true));
  RegisterCommand(new command("ShowWeaponSkills", &ShowWeaponSkills, "show weapon skills", '@', '@', true));
  RegisterCommand(new command("Search", &Search, "search", 's', 's', false));
  RegisterCommand(new command("Sit", &Sit, "sit", '_', '_', false));
  RegisterCommand(new command("Throw", &Throw, "throw", 't', 't', false));
  RegisterCommand(new command("ToggleRunning", &ToggleRunning, "toggle running", 'u', 'U', true));
  RegisterCommand(new command("ForceVomit", &ForceVomit, "vomit", 'V', 'V', false));
  RegisterCommand(new command("NOP", &NOP, "wait", '.', '.', true));
  RegisterCommand(new command("WieldInRightArm", &WieldInRightArm, "wield in right arm", 'w', 'w', true));
  RegisterCommand(new command("WieldInLeftArm", &WieldInLeftArm, "wield in left arm", 'W', 'W', true));
  RegisterCommand(new command("Burn", &Burn, "burn", 'B', 'B', false));
#ifdef WIZARD
  RegisterCommand(new command("WizardMode", &WizardMode, "wizard mode activation", 'X', 'X', true));
#endif
  RegisterCommand(new command("Zap", &Zap, "zap", 'z', 'z', false));
#ifdef WIZARD
  /* Sort according to key */
  RegisterCommand(new command("RaiseStats", &RaiseStats, "raise stats", '1', '1', true, true));
  RegisterCommand(new command("LowerStats", &LowerStats, "lower stats", '2', '2', true, true));
  RegisterCommand(new command("SeeWholeMap", &SeeWholeMap, "see whole map", '3', '3', true, true));
  RegisterCommand(new command("WalkThroughWalls", &WalkThroughWalls, "toggle walk through walls mode", '4', '4', true, true));
  RegisterCommand(new command("RaiseGodRelations", &RaiseGodRelations, "raise your relations to the gods", '5', '5', true, true));
  RegisterCommand(new command("LowerGodRelations", &LowerGodRelations, "lower your relations to the gods", '6', '6', true, true));
  RegisterCommand(new command("WizardWish", &WizardWish, "wish something", '=', '=', true, true));
  RegisterCommand(new command("GainDivineKnowledge", &GainDivineKnowledge, "gain knowledge of all gods", '\"', '\"', true, true));
  RegisterCommand(new command("GainAllItems", &GainAllItems, "gain all items", '$', '$', true, true));
  RegisterCommand(new command("SecretKnowledge", &SecretKnowledge, "reveal secret knowledge", '*', '*', true, true));
  RegisterCommand(new command("DetachBodyPart", &DetachBodyPart, "detach a limb", '0', '0', true, true));
  RegisterCommand(new command("SummonMonster", &SummonMonster, "summon monster", '&', '&', false, true));
  RegisterCommand(new command("LevelTeleport", &LevelTeleport, "level teleport", '|', '|', false, true));
  RegisterCommand(new command("Possess", &Possess, "possess creature", '{', '{', false, true));
  RegisterCommand(new command("Polymorph", &Polymorph, "polymorph", '[', '[', true, true));
  RegisterCommand(new command("GetScroll", &GetScroll, "get scroll", 'R', 'R', true, true));
  RegisterCommand(new command("OpenMondedr", &OpenMondedr, "open Mondedr", 'm', 'm', true, true));
  RegisterCommand(new command("ShowCoords", &ShowCoords, "show current coordinates", '(', '(', true, true));
  RegisterCommand(new command("WizardHeal", &WizardHeal, "wizard healing", 'H', 'H', true, true));
  RegisterCommand(new command("WizardBlow", &WizardBlow, "wizard blowing", '%', '%', true, true));
#endif
}


static commandsystem cmdsystem;
