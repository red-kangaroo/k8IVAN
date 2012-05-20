#ifdef HEADER_PHASE
ACTION(unconsciousness, action)
{
public:
  unconsciousness () : Counter(0) {}

  virtual void Save (outputfile &SaveFile) const;
  virtual void Load (inputfile &SaveFile);
  virtual void Handle ();
  void SetCounter (int What) { Counter = What; }
  virtual truth IsVoluntary () const { return false; }
  virtual void Terminate (truth);
  virtual truth AllowUnconsciousness () const { return false; }
  virtual cchar *GetDescription () const;
  virtual cchar *GetDeathExplanation () const;
  virtual truth CanBeTalkedTo () const { return false; }
  virtual truth IsUnconsciousness () const { return true; }
  void RaiseCounterTo (int);

protected:
  int Counter;
};


#else


cchar *unconsciousness::GetDeathExplanation () const { return " unconscious"; }
cchar *unconsciousness::GetDescription () const { return "unconscious"; }


void unconsciousness::Save (outputfile &SaveFile) const {
  action::Save(SaveFile);
  SaveFile << Counter;
}


void unconsciousness::Load (inputfile &SaveFile) {
  action::Load(SaveFile);
  SaveFile >> Counter;
}


void unconsciousness::Handle () {
  if (!--Counter) {
    Terminate(true);
  } else
  {
    Actor->EditExperience(ARM_STRENGTH, -50, 1 << 2);
    Actor->EditExperience(LEG_STRENGTH, -50, 1 << 2);
  }
}


void unconsciousness::Terminate (truth Finished) {
  if (Flags & TERMINATING) return;
  Flags |= TERMINATING;
  if (GetActor()->IsPlayer()) ADD_MESSAGE("You wake up.");
  else if (GetActor()->CanBeSeenByPlayer()) ADD_MESSAGE("%s wakes up.", GetActor()->CHAR_NAME(DEFINITE));
  action::Terminate(Finished);
}


void unconsciousness::RaiseCounterTo (int What) {
  if (Counter < What) Counter = What;
}


#endif
