#ifdef HEADER_PHASE
ACTION(study, action)
{
public:
  virtual void Save (outputfile &SaveFile) const;
  virtual void Load (inputfile &SaveFile);
  virtual void Handle ();
  virtual void Terminate (truth);
  void SetLiteratureID (feuLong What) { LiteratureID = What; }
  virtual cchar *GetDescription() const;
  void SetCounter (int What) { Counter = What; }

protected:
  feuLong LiteratureID;
  int Counter;
};


#else



cchar *study::GetDescription () const { return "reading"; }



void study::Handle () {
  item *Literature = game::SearchItem(LiteratureID);
  if (!Literature || !Literature->Exists() || !Actor->IsOver(Literature)) {
    Terminate(false);
    return;
  }
  if (GetActor()->GetLSquareUnder()->IsDark() && !game::GetSeeWholeMapCheatMode()) {
    ADD_MESSAGE("It is too dark to read now.");
    Terminate(false);
    return;
  }
  if (game::CompareLightToInt(GetActor()->GetLSquareUnder()->GetLuminance(), 115) < 0) GetActor()->EditExperience(PERCEPTION, -50, 1 << 1);
  if (!Counter) {
    Terminate(true);
    return;
  }
  if (GetActor()->GetAttribute(INTELLIGENCE) >= Counter) Counter = 0;
  else Counter -= GetActor()->GetAttribute(INTELLIGENCE);
}



void study::Terminate (truth Finished) {
  if (Flags & TERMINATING) return;
  Flags |= TERMINATING;
  item *Literature = game::SearchItem(LiteratureID);
  if (Finished) {
    if (GetActor()->IsPlayer())
      ADD_MESSAGE("You finish reading %s.", Literature->CHAR_NAME(DEFINITE));
    else if(GetActor()->CanBeSeenByPlayer())
      ADD_MESSAGE("%s finishes reading %s.", GetActor()->CHAR_NAME(DEFINITE), Literature->CHAR_NAME(DEFINITE));
    character *Actor = GetActor();
    Literature->FinishReading(Actor);
    if (!Actor->IsEnabled()) return;
  } else if (Literature && Literature->Exists()) {
    if (GetActor()->IsPlayer())
      ADD_MESSAGE("You stop reading %s.", Literature->CHAR_NAME(DEFINITE));
    else if(GetActor()->CanBeSeenByPlayer())
      ADD_MESSAGE("%s stops reading %s.", GetActor()->CHAR_NAME(DEFINITE), Literature->CHAR_NAME(DEFINITE));
  } else {
    if (GetActor()->IsPlayer()) ADD_MESSAGE("You stop reading.");
    else if (GetActor()->CanBeSeenByPlayer()) ADD_MESSAGE("%s stops reading.", GetActor()->CHAR_NAME(DEFINITE));
  }
  action::Terminate(Finished);
}



void study::Save (outputfile &SaveFile) const {
  action::Save(SaveFile);
  SaveFile << Counter << LiteratureID;
}



void study::Load (inputfile &SaveFile) {
  action::Load(SaveFile);
  SaveFile >> Counter >> LiteratureID;
}
#endif
