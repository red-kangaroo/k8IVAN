#ifdef HEADER_PHASE
OLTERRAIN(stairs, olterrain)
{
public:
  stairs () : AttachedArea(0), AttachedEntry(0) {}

  virtual void Save(outputfile&) const;
  virtual void Load(inputfile&);
  virtual truth Enter(truth) const;
  virtual void StepOn(character*);
  virtual void SetAttachedArea (int What);
  virtual void SetAttachedEntry (int What);
  virtual int GetAttachedArea () const;
  virtual int GetAttachedEntry () const;
  virtual void AddSpecialCursors ();

protected:
  virtual void PostConstruct ();

protected:
  int AttachedArea;
  int AttachedEntry;
};


#else


void stairs::SetAttachedArea (int What) { AttachedArea = What; }
void stairs::SetAttachedEntry (int What) { AttachedEntry = What; }
int stairs::GetAttachedArea () const { return AttachedArea; }
int stairs::GetAttachedEntry () const { return AttachedEntry; }


void stairs::Save (outputfile &SaveFile) const {
  olterrain::Save(SaveFile);
  SaveFile << AttachedArea << AttachedEntry;
}


void stairs::Load (inputfile &SaveFile) {
  olterrain::Load(SaveFile);
  SaveFile >> AttachedArea >> AttachedEntry;
}


truth stairs::Enter (truth DirectionUp) const {
  if (!DirectionUp != !IsUpLink()) return olterrain::Enter(DirectionUp);
  /* "Temporary" gum solutions */
  //
  if (GetConfig() == XINROCH_TOMB_ENTRANCE) {
    if ((game::GetXinrochTombStoryState() == 2) || (PLAYER->GetMoveType() & ETHEREAL)) {
      ADD_MESSAGE("This dark gate seems to be a one-way portal. You sense something distant but extremely "
                  "dangerous on the other side. You feel you should think twice before entering.");
      if (!game::TruthQuestion(CONST_S("Continue anyway? [y/N]"))) return false;
    } else {
      ADD_MESSAGE("An unknown magical force pushes you back.");
      PLAYER->EditAP(-1000);
      return true;
    }
  }
  //
  if (GetConfig() == XINROCH_TOMB_EXIT) {
    if (PLAYER->HasLostRubyFlamingSword()) {
      ADD_MESSAGE("Somehow you get the feeling you cannot return.");
      if (!game::TruthQuestion(CONST_S("Continue anyway? [y/N]"))) return false;
    } else {
      ADD_MESSAGE("An unknown magical force pushes you back.");
      PLAYER->EditAP(-1000);
      return true;
    }
  }
  //
  if (GetConfig() == OREE_LAIR_ENTRY) {
    ADD_MESSAGE("You sense terrible evil trembling very near under your feet. You feel you shouldn't wander any further. On the other hand you have little choice.");
    if (!game::TruthQuestion(CONST_S("Continue? [y/N]"))) return false;
  }
  //
  if (GetConfig() == OREE_LAIR_EXIT) {
    if (PLAYER->HasGoldenEagleShirt()) {
      ADD_MESSAGE("Somehow you get the feeling you cannot return.");
      if (!game::TruthQuestion(CONST_S("Continue anyway? [y/N]"))) return false;
    } else {
      ADD_MESSAGE("An unknown magical force pushes you back.");
      PLAYER->EditAP(-1000);
      return true;
    }
  }
  //
  if (GetConfig() == DARK_LEVEL) {
    ADD_MESSAGE("This dark gate seems to be a one-way portal. You sense something distant but extremely dangerous on the other side. You feel you should think twice before entering.");
    if (!game::TruthQuestion(CONST_S("Continue? [y/N]"))) return false;
  }
  //
  if (GetConfig() == SUMO_ARENA_ENTRY && !game::TryToEnterSumoArena()) return false;
  //
  if (GetConfig() == SUMO_ARENA_EXIT && !game::TryToExitSumoArena()) return false;
  //
  return game::TryTravel(game::GetCurrentDungeonIndex(), GetAttachedArea(), GetAttachedEntry(), GetAttachedArea() != WORLD_MAP);
}


void stairs::StepOn (character *Stepper) {
  if (Stepper->IsPlayer()) ADD_MESSAGE("There is %s here.", CHAR_NAME(INDEFINITE));
}


void stairs::PostConstruct () {
  if (GetConfig() == STAIRS_UP) {
    if (game::GetCurrentLevelIndex()) {
      AttachedArea = game::GetCurrentLevelIndex()-1;
      AttachedEntry = STAIRS_DOWN;
    } else {
      AttachedArea = WORLD_MAP;
      AttachedEntry = game::GetCurrentDungeonIndex();
    }
  } else if (GetConfig() == STAIRS_DOWN) {
    AttachedArea = game::GetCurrentLevelIndex()+1;
    AttachedEntry = STAIRS_UP;
  }
}


void stairs::AddSpecialCursors () {
  game::AddSpecialCursor(GetPos(), YELLOW_CURSOR|CURSOR_TARGET);
}


#endif
