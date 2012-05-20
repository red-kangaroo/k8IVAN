#ifdef HEADER_PHASE
ACTION(dig, action)
{
public:
  dig () : RightBackupID(0), LeftBackupID(0), SquareDug(0, 0), MoveDigger(false) {}

  virtual void Save (outputfile &SaveFile) const;
  virtual void Load (inputfile &SaveFile);
  virtual void Handle ();
  void SetSquareDug (v2 What) { SquareDug = What; }
  virtual void Terminate (truth);
  void SetRightBackupID (feuLong What) { RightBackupID = What; }
  void SetLeftBackupID (feuLong What) { LeftBackupID = What; }
  virtual truth TryDisplace () { return false; }
  virtual cchar *GetDescription () const;
  virtual truth ShowEnvironment () const { return false; }
  void SetMoveDigger (truth What) { MoveDigger = What; }

protected:
  feuLong RightBackupID;
  feuLong LeftBackupID;
  v2 SquareDug;
  truth MoveDigger;
};


#else


cchar *dig::GetDescription () const { return "digging"; }


void dig::Save (outputfile &SaveFile) const {
  action::Save(SaveFile);
  SaveFile << SquareDug << MoveDigger << RightBackupID << LeftBackupID;
}


void dig::Load (inputfile &SaveFile) {
  action::Load(SaveFile);
  SaveFile >> SquareDug >> MoveDigger >> RightBackupID >> LeftBackupID;
}


void dig::Handle () {
  character *Actor = GetActor();
  item *Digger = Actor->GetMainWielded();
  if (!Digger) {
    Terminate(false);
    return;
  }
  lsquare *Square = Actor->GetNearLSquare(SquareDug);
  olterrain *Terrain = Square->GetOLTerrain();
  if (!Terrain || !Terrain->CanBeDestroyed() || !Terrain->GetMainMaterial()->CanBeDug(Digger->GetMainMaterial())) {
    Terminate(false);
    return;
  }
  int Damage = Actor->GetAttribute(ARM_STRENGTH) * Digger->GetMainMaterial()->GetStrengthValue() / 500;
  Terrain->EditHP(-Max(Damage, 1));
  Actor->EditExperience(ARM_STRENGTH, 200, 1 << 5);
  Actor->EditAP(-200000 / APBonus(Actor->GetAttribute(DEXTERITY)));
  Actor->EditNP(-500);
  if (Terrain->GetHP() <= 0) {
    if (Square->CanBeSeenByPlayer()) ADD_MESSAGE("%s", Terrain->GetDigMessage().CStr());
    Terrain->Break();
    /* If the door was boobytrapped etc. and the character is dead, Action has already been deleted */
    if (!Actor->IsEnabled()) return;
    if (MoveDigger && Actor->GetMainWielded()) Actor->GetMainWielded()->MoveTo(Actor->GetStack());
    item *RightBackup = game::SearchItem(RightBackupID);
    if (RightBackup && RightBackup->Exists() && Actor->IsOver(RightBackup)) {
      RightBackup->RemoveFromSlot();
      Actor->SetRightWielded(RightBackup);
    }
    item *LeftBackup = game::SearchItem(LeftBackupID);
    if (LeftBackup && LeftBackup->Exists() && Actor->IsOver(LeftBackup)) {
      LeftBackup->RemoveFromSlot();
      Actor->SetLeftWielded(LeftBackup);
    }
    Terminate(true);
  } else {
    game::DrawEverything();
  }
}


void dig::Terminate (truth Finished) {
  if (Flags & TERMINATING) return;
  Flags |= TERMINATING;
  if (!Finished) {
    if (GetActor()->IsPlayer()) ADD_MESSAGE("You stop digging.");
    else if (GetActor()->CanBeSeenByPlayer()) ADD_MESSAGE("%s stops digging.", GetActor()->CHAR_NAME(DEFINITE));
  }
  action::Terminate(Finished);
}


#endif
