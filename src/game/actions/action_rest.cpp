#ifdef HEADER_PHASE
ACTION(rest, action)
{
public:
  rest () : GoalHP(0), MinToStop(0) {}

  virtual void Save (outputfile &SaveFile) const;
  virtual void Load (inputfile &SaveFile);
  virtual void Handle ();
  void SetGoalHP (int What) { GoalHP = What; }
  virtual void Terminate (truth);
  virtual truth IsRest () const { return true; }
  virtual cchar *GetDescription () const;
  void SetMinToStop (int What) { MinToStop = What; }

protected:
  int GoalHP;
  int MinToStop;
};


#else


cchar *rest::GetDescription () const { return "resting"; }


void rest::Save (outputfile &SaveFile) const {
  action::Save(SaveFile);
  SaveFile << GoalHP << MinToStop;
}


void rest::Load (inputfile &SaveFile) {
  action::Load(SaveFile);
  SaveFile >> GoalHP >> MinToStop;
}


void rest::Handle () {
  if ((GoalHP && (GetActor()->GetHP() >= GoalHP ||
      GetActor()->GetHP() == GetActor()->GetMaxHP() ||
      !GetActor()->CanHeal())) ||
      (MinToStop && game::GetTotalMinutes() >= MinToStop)) {
    Terminate(true);
  } else {
    GetActor()->EditExperience(DEXTERITY, -25, 1 << 1);
    GetActor()->EditExperience(AGILITY, -25, 1 << 1);
  }
}


void rest::Terminate (truth Finished) {
  if (Flags & TERMINATING) return;
  Flags |= TERMINATING;
  if (Finished) {
    if (GetActor()->IsPlayer()) ADD_MESSAGE("You finish resting.");
    else if (GetActor()->CanBeSeenByPlayer()) ADD_MESSAGE("%s finishes resting.", GetActor()->CHAR_NAME(DEFINITE));
  } else {
    if (GetActor()->IsPlayer()) ADD_MESSAGE("You stop resting.");
    else if (GetActor()->CanBeSeenByPlayer()) ADD_MESSAGE("%s stops resting.", GetActor()->CHAR_NAME(DEFINITE));
  }
  action::Terminate(Finished);
}


#endif
