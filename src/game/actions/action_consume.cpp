#ifdef HEADER_PHASE
ACTION(consume, action)
{
public:
  virtual void Save (outputfile &SaveFile) const;
  virtual void Load (inputfile &SaveFile);
  virtual void Handle ();
  virtual void Terminate (truth);
  void SetConsumingID (feuLong What) { ConsumingID = What; }
  virtual truth AllowUnconsciousness () const { return false; }
  virtual truth AllowFoodConsumption () const { return false; }
  virtual cchar* GetDescription () const;
  virtual void SetDescription (cfestring &);

protected:
  festring Description;
  feuLong ConsumingID;
};


#else



cchar *consume::GetDescription () const { return Description.CStr(); }



void consume::SetDescription (cfestring &What) { Description = What; }



void consume::Save (outputfile &SaveFile) const {
  action::Save(SaveFile);
  SaveFile << ConsumingID << Description;
}



void consume::Load (inputfile &SaveFile) {
  action::Load(SaveFile);
  SaveFile >> ConsumingID >> Description;
}



void consume::Handle () {
  item *Consuming = game::SearchItem(ConsumingID);
  if (!Consuming || !Consuming->Exists() || !Actor->IsOver(Consuming)) {
    Terminate(false);
    return;
  }
  character *Actor = GetActor();
  if (!InDNDMode() && Actor->GetHungerState() >= BLOATED) {
    if (Actor->IsPlayer()) {
      ADD_MESSAGE("You have a really hard time getting all this down your throat.");
      if (game::TruthQuestion(CONST_S("Continue ") + GetDescription() + "? [y/N]")) ActivateInDNDMode();
      else {
        Terminate(false);
        return;
      }
    } else {
      Terminate(false);
      return;
    }
  }
  if (!Actor->IsPlayer() && !Consuming->CanBeEatenByAI(Actor)) {
    // item may be spoiled after action was started
    Terminate(false);
    return;
  }
  /* Note: if backupped Actor has died of food effect,
     Action is deleted automatically, so we mustn't Terminate it */
  if (Consuming->Consume(Actor, 500) && Actor->GetAction() == this && Actor->IsEnabled()) {
    Terminate(true);
  } else if (Actor->GetHungerState() == OVER_FED) {
    Actor->DeActivateVoluntaryAction(CONST_S("You are about to choke on this stuff."));
    Actor->Vomit(Actor->GetPos(), 500 + RAND() % 500);
  }
}



void consume::Terminate (truth Finished) {
  if (Flags & TERMINATING) return;
  Flags |= TERMINATING;
  item *Consuming = game::SearchItem(ConsumingID);
  character *Actor = GetActor();
  if (Actor->IsPlayer()) ADD_MESSAGE("You %s %s.", Finished ? "finish" : "stop", Description.CStr());
  else if (Actor->CanBeSeenByPlayer()) ADD_MESSAGE("%s %s %s.", Actor->CHAR_NAME(DEFINITE), Finished ? "finishes" : "stops", Description.CStr());
  if (Finished) {
    if (Consuming->Exists() && !game::IsInWilderness() && (!Actor->IsPlayer() || game::CheckDropLeftover(Consuming))) {
      if (Actor->IsPlayer()) ADD_MESSAGE("%s dropped.", Consuming->GetName(INDEFINITE, 1).CStr());
      else if (Actor->CanBeSeenByPlayer()) ADD_MESSAGE("%s dropped %s.", Actor->CHAR_NAME(DEFINITE), Consuming->GetName(INDEFINITE, 1).CStr());
      Consuming->RemoveFromSlot();
      Actor->GetStackUnder()->AddItem(Consuming);
      Actor->DexterityAction(2);
    }
  } else if (Consuming && Consuming->Exists()) {
    material *ConsumeMaterial = Consuming->GetConsumeMaterial(Actor);
    if (ConsumeMaterial) ConsumeMaterial->FinishConsuming(Actor);
  }
  action::Terminate(Finished);
}
#endif
