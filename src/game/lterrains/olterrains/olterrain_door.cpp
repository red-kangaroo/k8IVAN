#ifdef HEADER_PHASE
OLTERRAIN(door, olterrain)
{
public:
  door () : Opened(false), Locked(false), BoobyTrap(0) {}

  virtual truth Open (character *);
  virtual truth Close (character *);
  virtual truth CanBeOpened () const;
  virtual void BeKicked (character *, int, int);
  virtual void SetIsOpened (truth What);
  virtual void Save (outputfile &) const;
  virtual void Load (inputfile &);
  virtual truth IsDoor () const;
  virtual void SetIsLocked (truth What);
  virtual truth IsLocked () const;
  virtual truth CanBeOpenedByAI ();
  virtual void ReceiveDamage (character *, int, int);
  virtual void CreateBoobyTrap ();
  virtual void ActivateBoobyTrap ();
  virtual truth TryKey (item *, character *);
  virtual void SetParameters (int);
  virtual void Lock () { SetIsLocked(true); }
  virtual void HasBeenHitByItem (character *, item *, int);
  virtual truth IsTransparent () const;
  virtual int GetWalkability () const;
  virtual int GetTheoreticalWalkability () const;
  virtual void BeDestroyed ();

protected:
  virtual void PostConstruct ();
  virtual truth AddAdjective (festring &, truth) const;
  virtual void Break ();
  virtual v2 GetBitmapPos (int) const;
  virtual void MakeWalkable ();
  virtual void MakeNotWalkable ();
  virtual int GetBoobyTrap ();
  virtual void SetBoobyTrap (int What);

protected:
  truth Opened;
  truth Locked;
  int BoobyTrap;
};


#else


truth door::IsDoor () const { return true; }
truth door::CanBeOpened () const { return !Opened; }
void door::SetIsOpened (truth What) { Opened = What; }
void door::SetIsLocked (truth What) { Locked = What; }
truth door::IsLocked () const { return Locked; }
int door::GetBoobyTrap () { return BoobyTrap; }
void door::SetBoobyTrap (int What) { BoobyTrap = What; }
truth door::CanBeOpenedByAI () { return !IsLocked() && CanBeOpened(); }
void door::HasBeenHitByItem (character *Thrower, item *, int Damage) { ReceiveDamage(Thrower, Damage, PHYSICAL_DAMAGE); }
v2 door::GetBitmapPos (int Frame) const { return Opened ? GetOpenBitmapPos(Frame) : olterrain::GetBitmapPos(Frame); }
int door::GetTheoreticalWalkability () const { return ANY_MOVE; }


truth door::Open (character *Opener) {
  if (!Opened) {
    if (IsLocked()) {
      if (Opener->IsPlayer()) ADD_MESSAGE("The door is locked.");
      return false;
    } else if (RAND()%20 < Opener->GetAttribute(ARM_STRENGTH)) {
      truth WasSeenByPlayer = CanBeSeenByPlayer(); // MakeWalkable() might destroy the door
      MakeWalkable();
      if (Opener->IsPlayer()) ADD_MESSAGE("You open the door.");
      else if (WasSeenByPlayer) {
        if (Opener->CanBeSeenByPlayer()) ADD_MESSAGE("%s opens the door.", Opener->CHAR_NAME(DEFINITE));
        else ADD_MESSAGE("Something opens the door.");
      }
    } else {
      if (Opener->IsPlayer()) ADD_MESSAGE("The door resists.");
      else if (CanBeSeenByPlayer() && Opener->CanBeSeenByPlayer()) ADD_MESSAGE("%s fails to open the door.", Opener->CHAR_NAME(DEFINITE));
      ActivateBoobyTrap();
    }
    Opener->DexterityAction(Opener->OpenMultiplier()*5);
    return true;
  } else {
    if (Opener->IsPlayer()) ADD_MESSAGE("The door is already open, %s.", game::Insult());
    return false;
  }
}


truth door::Close (character *Closer) {
  if (Closer->IsPlayer()) {
    if (Opened) {
      if (RAND()%20 < Closer->GetAttribute(ARM_STRENGTH)) {
        ADD_MESSAGE("You close the door.");
        MakeNotWalkable();
      } else {
        ADD_MESSAGE("The door resists!");
      }
    } else {
      ADD_MESSAGE("The door is already closed, %s.", game::Insult());
      return false;
    }
  }
  Closer->DexterityAction(Closer->OpenMultiplier() * 5);
  return true;
}


void door::BeKicked (character *Kicker, int KickDamage, int) {
  if (!Opened) {
    if (!IsLocked() && KickDamage > (RAND()&3)) {
      if (CanBeSeenByPlayer()) ADD_MESSAGE("The door opens.");
      MakeWalkable();
      return;
    }
    if (KickDamage <= GetStrengthValue()) {
      if (CanBeSeenByPlayer() && (Kicker->CanBeSeenByPlayer() || Kicker->IsPlayer()))
        ADD_MESSAGE("%s weak kick has no chance to affect this door.", Kicker->CHAR_POSSESSIVE_PRONOUN);
      return;
    }
    EditHP(GetStrengthValue()-KickDamage);
    int SV = Max(GetStrengthValue(), 1);
    truth LockBreaks = IsLocked() && RAND() % (100 * KickDamage / SV) >= 100;
    if (LockBreaks) SetIsLocked(false);
    if (GetHP() <= 0) {
      if (CanBeSeenByPlayer()) {
        if (LockBreaks) ADD_MESSAGE("The lock breaks and the door is damaged.");
        else ADD_MESSAGE("The door is damaged.");
      }
      room *Room = GetRoom();
      Break();
      if (Room) Room->DestroyTerrain(Kicker);
    } else if (CanBeSeenByPlayer()) {
      if (LockBreaks) {
        ADD_MESSAGE("The lock breaks.");
        if (GetRoom()) GetRoom()->DestroyTerrain(Kicker);
      } else if (Kicker->CanBeSeenByPlayer() || Kicker->IsPlayer()) ADD_MESSAGE("The door won't budge!");
    }
    // The door may have been destroyed here, so don't do anything!
  }
}


void door::Save (outputfile &SaveFile) const {
  olterrain::Save(SaveFile);
  SaveFile << Opened << Locked << BoobyTrap;
}


void door::Load (inputfile &SaveFile) {
  olterrain::Load(SaveFile);
  SaveFile >> Opened >> Locked >> BoobyTrap;
}


void door::MakeWalkable () {
  SetIsOpened(true);
  UpdatePictures();
  GetLSquareUnder()->CalculateIsTransparent();
  GetLSquareUnder()->SendNewDrawRequest();
  GetLSquareUnder()->SendMemorizedUpdateRequest();
  GetLevel()->ForceEmitterEmitation(GetLSquareUnder()->GetEmitter(), GetLSquareUnder()->GetSunEmitter());
  GetLSquareUnder()->CalculateLuminance();
  if (GetLSquareUnder()->GetLastSeen() == game::GetLOSTick()) game::SendLOSUpdateRequest();
  ActivateBoobyTrap();
}


void door::MakeNotWalkable () {
  emittervector EmitterBackup = GetLSquareUnder()->GetEmitter();
  GetLevel()->ForceEmitterNoxify(EmitterBackup);
  SetIsOpened(false);
  UpdatePictures();
  GetLSquareUnder()->CalculateIsTransparent();
  GetLSquareUnder()->SendNewDrawRequest();
  GetLSquareUnder()->SendMemorizedUpdateRequest();
  GetLevel()->ForceEmitterEmitation(EmitterBackup, GetLSquareUnder()->GetSunEmitter(), FORCE_ADD);
  GetLSquareUnder()->CalculateLuminance();
  if (GetLSquareUnder()->GetLastSeen() == game::GetLOSTick()) game::SendLOSUpdateRequest();
}


truth door::AddAdjective (festring &String, truth Articled) const {
  if (olterrain::AddAdjective(String, Articled)) Articled = false;
  if (Articled) String << (Opened ? "an open" : "a closed");
  else String << (Opened ? "open" : "closed");
  if (IsLocked()) String << " locked "; else String << ' ';
  return true;
}


void door::Break () {
  if (BoobyTrap) ActivateBoobyTrap();
  else {
    truth Open = Opened;
    door *Temp = brokendoor::Spawn(GetConfig(), NO_MATERIALS);
    Temp->InitMaterials(GetMainMaterial()->SpawnMore());
    Temp->SetIsLocked(IsLocked());
    Temp->SetBoobyTrap(0);
    GetLSquareUnder()->ChangeOLTerrainAndUpdateLights(Temp);
    if (Open) Temp->MakeWalkable(); else Temp->MakeNotWalkable();
  }
}


void door::ActivateBoobyTrap () {
  switch (BoobyTrap) {
    case 1: // Explosion
      if (LSquareUnder->CanBeSeenByPlayer(true)) ADD_MESSAGE("%s is booby trapped!", CHAR_NAME(DEFINITE));
      BoobyTrap = 0;
      GetLevel()->Explosion(0, "killed by an exploding booby trapped door", GetPos(), 20 + RAND() % 5 - RAND() % 5);
      break;
    case 0:
      break;
  }
}


void door::CreateBoobyTrap () {
  SetBoobyTrap(1);
}


void door::PostConstruct () {
  /* Terrible gum solution! */
  if (!(GetConfig() & LOCK_BITS)) {
    int NormalLockTypes = 0;
    const database *const *ConfigData = GetProtoType()->GetConfigData();
    int c, ConfigSize = GetProtoType()->GetConfigSize();
    for (c = 0; c < ConfigSize; ++c)
      if ((ConfigData[c]->Config & LOCK_BITS) &&
          (ConfigData[c]->Config & ~LOCK_BITS) == GetConfig() &&
          !(ConfigData[c]->Config & S_LOCK_ID)) ++NormalLockTypes;
      int ChosenLock = RAND()%NormalLockTypes;
      for (c = 0; c < ConfigSize; ++c)
        if ((ConfigData[c]->Config & LOCK_BITS) &&
            (ConfigData[c]->Config & ~LOCK_BITS) == GetConfig() &&
            !(ConfigData[c]->Config & S_LOCK_ID) &&
            !ChosenLock--) {
          SetConfig(ConfigData[c]->Config, NO_PIC_UPDATE);
          break;
        }
  }
  SetBoobyTrap(0);
  SetIsOpened(false);
  SetIsLocked(false);
}


void door::SetParameters (int Param) {
  SetIsLocked(Param & LOCKED);
}


truth door::TryKey (item *Thingy, character *Applier) {
  if (Opened) return false;
  if (Thingy->CanOpenLockType(GetConfig()&LOCK_BITS)) {
    if (Applier->IsPlayer()) {
      if (IsLocked()) ADD_MESSAGE("You unlock the door."); else ADD_MESSAGE("You lock the door.");
    } else if (Applier->CanBeSeenByPlayer()) {
      if (IsLocked()) ADD_MESSAGE("%s unlocks the door.", Applier->CHAR_NAME(DEFINITE));
      else ADD_MESSAGE("%s locks the door.", Applier->CHAR_NAME(DEFINITE));
    }
    SetIsLocked(!IsLocked());
    return true;
  }
  if (Applier->IsPlayer()) ADD_MESSAGE("%s doesn't fit into the lock.", Thingy->CHAR_NAME(DEFINITE));
  return false;
}


void door::ReceiveDamage (character *Villain, int Damage, int) {
  if (!Opened && !IsLocked() && Damage > (RAND() & 3)) {
    if (CanBeSeenByPlayer()) ADD_MESSAGE("The door opens.");
    MakeWalkable();
    return;
  }
  if (CanBeDestroyed() && Damage > GetStrengthValue()) {
    EditHP(GetStrengthValue()-Damage);
    int SV = Max(GetStrengthValue(), 1);
    truth LockBreaks = IsLocked() && RAND() % (100 * Damage / SV) >= 100;
    if (LockBreaks) SetIsLocked(false);
    if (HP <= 0) {
      if (CanBeSeenByPlayer()) {
        if (LockBreaks) ADD_MESSAGE("The door breaks and its lock is destroyed.");
        else ADD_MESSAGE("The door breaks.");
      }
      room *Room = GetRoom();
      Break();
      if (Room) Room->DestroyTerrain(Villain);
    } else if (LockBreaks && CanBeSeenByPlayer()) {
      ADD_MESSAGE("The door's lock is shattered.");
      if (GetRoom()) GetRoom()->DestroyTerrain(Villain);
    }
  }
}


int door::GetWalkability () const {
  return Opened ? ANY_MOVE : ANY_MOVE&~(WALK|FLY);
}


truth door::IsTransparent () const {
  return Opened || MainMaterial->IsTransparent() || DataBase->IsAlwaysTransparent;
}


void door::BeDestroyed () {
  olterrain::Break();
}


#endif
