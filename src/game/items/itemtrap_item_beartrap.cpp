#ifdef HEADER_PHASE
ITEM(beartrap, itemtrap<item>)
{
 public:
  beartrap();
  beartrap(const beartrap&);
  virtual ~beartrap();
  virtual void Load(inputfile&);
  virtual void Save(outputfile&) const;
  virtual void StepOnEffect(character*);
  virtual truth CheckPickUpEffect(character*);
  virtual truth IsPickable(character*) const;
  virtual truth Apply(character*);
  virtual v2 GetBitmapPos(int) const;
  virtual truth IsDangerous(ccharacter*) const { return Active; }
  virtual truth ReceiveDamage(character*, int, int, int);
  virtual truth NeedDangerSymbol() const { return IsActive(); }
  virtual void Fly(character*, int, int);
  virtual feuLong GetTrapID() const { return TrapData.TrapID; }
  virtual feuLong GetVictimID() const { return TrapData.VictimID; }
  virtual void UnStick() { TrapData.VictimID = 0; }
  virtual void UnStick(int I) { TrapData.BodyParts &= ~(1 << I); }
  virtual truth TryToUnStick(character*, v2);
  virtual void RemoveFromSlot();
  virtual int GetTrapType() const { return GetType() | ITEM_TRAP; }
  virtual void PreProcessForBone();
  virtual void PostProcessForBone();
  virtual void DonateSlotTo(item*);
 protected:
  virtual truth AddAdjective(festring&, truth) const;
  truth IsStuck() const { return TrapData.VictimID; }
  int GetBaseTrapDamage() const;
  trapdata TrapData;
};


#else



truth beartrap::AddAdjective(festring& String, truth Articled) const { return (IsActive() && AddActiveAdjective(String, Articled)) || (!IsActive() && item::AddAdjective(String, Articled)); }



beartrap::beartrap(const beartrap& Trap) : mybase(Trap)
{
  TrapData.TrapID = game::CreateNewTrapID(this);
  TrapData.VictimID = 0;
}



truth beartrap::TryToUnStick(character* Victim, v2)
{
  feuLong TrapID = GetTrapID();
  int Modifier = GetBaseTrapDamage() * 40 / Max(Victim->GetAttribute(DEXTERITY) + Victim->GetAttribute(ARM_STRENGTH), 1);

  if(!RAND_N(Max(Modifier, 2)))
  {
    Victim->RemoveTrap(TrapID);
    TrapData.VictimID = 0;

    if(Victim->IsPlayer())
      ADD_MESSAGE("You manage to free yourself from %s.", CHAR_NAME(DEFINITE));
    else if(Victim->CanBeSeenByPlayer())
      ADD_MESSAGE("%s manages to free %sself from %s.", Victim->CHAR_NAME(DEFINITE), Victim->CHAR_OBJECT_PRONOUN, CHAR_NAME(DEFINITE));

    Victim->EditAP(-500);
    return true;
  }

  if(!RAND_N(Max(Modifier << 1, 2)))
  {
    Victim->RemoveTrap(TrapID);
    TrapData.VictimID = 0;
    Break(Victim);

    if(Victim->IsPlayer())
      ADD_MESSAGE("You are freed.");
    else if(Victim->CanBeSeenByPlayer())
      ADD_MESSAGE("%s is freed.", Victim->CHAR_NAME(DEFINITE));

    Victim->EditAP(-500);
    return true;
  }

  Modifier = Victim->GetAttribute(DEXTERITY) + Victim->GetAttribute(ARM_STRENGTH) * 3 / 20;

  if(!RAND_N(Max(Modifier, 2)))
  {
    int BodyPart = Victim->RandomizeHurtBodyPart(TrapData.BodyParts);

    if(Victim->IsPlayer())
      ADD_MESSAGE("You manage to hurt your %s even more.", Victim->GetBodyPartName(BodyPart).CStr());
    else if(Victim->CanBeSeenByPlayer())
      ADD_MESSAGE("%s hurts %s %s more with %s.", Victim->CHAR_NAME(DEFINITE), Victim->GetPossessivePronoun().CStr(), Victim->GetBodyPartName(BodyPart).CStr(), CHAR_NAME(DEFINITE));

    Victim->ReceiveBodyPartDamage(0, GetBaseTrapDamage(), PHYSICAL_DAMAGE, BodyPart, YOURSELF, false, false, false);
    Victim->CheckDeath(CONST_S("died while trying to escape from ") + GetName(INDEFINITE), 0, IGNORE_TRAPS);
    Victim->EditAP(-1000);
    return false;
  }

  if(!RAND_N(Max(Modifier << 1, 2)))
  {
    int VictimBodyPart = Victim->RandomizeTryToUnStickBodyPart(ALL_BODYPART_FLAGS&~TrapData.BodyParts);

    if(VictimBodyPart != NONE_INDEX)
    {
      TrapData.BodyParts |= 1 << VictimBodyPart;
      Victim->AddTrap(GetTrapID(), 1 << VictimBodyPart);

      if(Victim->IsPlayer())
  ADD_MESSAGE("You fail to free yourself from %s and your %s is stuck in it in the attempt.", CHAR_NAME(DEFINITE), Victim->GetBodyPartName(VictimBodyPart).CStr());
      else if(Victim->CanBeSeenByPlayer())
  ADD_MESSAGE("%s tries to free %sself from %s but is stuck more tightly in it in the attempt.", Victim->CHAR_NAME(DEFINITE), Victim->CHAR_OBJECT_PRONOUN, CHAR_NAME(DEFINITE));

      Victim->ReceiveBodyPartDamage(0, GetBaseTrapDamage() << 1, PHYSICAL_DAMAGE, VictimBodyPart, YOURSELF, false, false, false);
      Victim->CheckDeath(CONST_S("died while trying to escape from ") + GetName(INDEFINITE), 0, IGNORE_TRAPS);
      Victim->EditAP(-1000);
      return true;
    }
  }

  if(Victim->IsPlayer())
    ADD_MESSAGE("You are unable to escape from %s.", CHAR_NAME(DEFINITE));

  Victim->EditAP(-1000);
  return false;
}



void beartrap::Load(inputfile& SaveFile)
{
  mybase::Load(SaveFile);
  SaveFile >> TrapData;
  game::AddTrapID(this, TrapData.TrapID);
}



void beartrap::Save(outputfile& SaveFile) const
{
  mybase::Save(SaveFile);
  SaveFile << TrapData;
}



beartrap::beartrap()
{
  if(!game::IsLoading())
  {
    TrapData.TrapID = game::CreateNewTrapID(this);
    TrapData.VictimID = 0;
  }
}



beartrap::~beartrap()
{
  game::RemoveTrapID(TrapData.TrapID);
}



void beartrap::StepOnEffect(character* Stepper)
{
  if(IsActive() && !IsBroken())
  {
    int StepperBodyPart = Stepper->GetRandomStepperBodyPart();

    if(StepperBodyPart == NONE_INDEX)
      return;

    TrapData.VictimID = Stepper->GetID();
    TrapData.BodyParts = 1 << StepperBodyPart;
    Stepper->AddTrap(GetTrapID(), 1 << StepperBodyPart);

    if(Stepper->IsPlayer())
      ADD_MESSAGE("You step in %s and it traps your %s.", CHAR_NAME(INDEFINITE), Stepper->GetBodyPartName(StepperBodyPart).CStr());
    else if(Stepper->CanBeSeenByPlayer())
      ADD_MESSAGE("%s is trapped in %s.", Stepper->CHAR_NAME(DEFINITE), CHAR_NAME(INDEFINITE));

    SetIsActive(false);
    SendNewDrawAndMemorizedUpdateRequest();

    if(Stepper->IsPlayer())
      game::AskForEscPress(CONST_S("Trap activated!"));

    Stepper->ReceiveBodyPartDamage(0, GetBaseTrapDamage() << 1, PHYSICAL_DAMAGE, StepperBodyPart, YOURSELF, false, false, false);
    Stepper->CheckDeath(CONST_S("died by stepping to ") + GetName(INDEFINITE), 0, IGNORE_TRAPS);
  }
}



truth beartrap::CheckPickUpEffect(character* Picker)
{
  if(Picker->IsStuckToTrap(GetTrapID()))
  {
    if(Picker->IsPlayer())
      ADD_MESSAGE("You are tightly stuck in %s.", CHAR_NAME(DEFINITE));

    return false;
  }

  SetIsActive(false);
  return true;
}



truth beartrap::Apply(character* User)
{
  if(IsBroken())
  {
    if(User->IsPlayer())
      ADD_MESSAGE("%s is useless.", CHAR_NAME(DEFINITE));

    return false;
  }

  if(User->IsPlayer()
     && !game::TruthQuestion(CONST_S("Are you sure you want to plant ") + GetName(DEFINITE) + "? [y/N]"))
    return false;

  room* Room = GetRoom();

  if(Room)
    Room->HostileAction(User);

  if(User->GetAttribute(DEXTERITY) < femath::LoopRoll(90, 1000))
  {
    int UserBodyPart = User->GetRandomApplyBodyPart();

    if(User->IsPlayer())
      ADD_MESSAGE("Somehow you manage to trap your %s in %s.", User->GetBodyPartName(UserBodyPart).CStr(), CHAR_NAME(DEFINITE));
    else if(User->CanBeSeenByPlayer())
      ADD_MESSAGE("%s somehow traps %sself in %s.", User->CHAR_NAME(DEFINITE), User->CHAR_OBJECT_PRONOUN, CHAR_NAME(DEFINITE));

    RemoveFromSlot();
    User->GetStackUnder()->AddItem(this);
    TrapData.VictimID = User->GetID();
    TrapData.BodyParts = 1 << UserBodyPart;
    User->AddTrap(GetTrapID(), 1 << UserBodyPart);
    SendNewDrawAndMemorizedUpdateRequest();

    if(User->IsPlayer())
      game::AskForEscPress(CONST_S("Trap activated!"));

    User->ReceiveBodyPartDamage(0, 1 + (RAND() & 1), PHYSICAL_DAMAGE, UserBodyPart, YOURSELF, false, false, false);
    User->CheckDeath(CONST_S("died failing to set ") + GetName(INDEFINITE), 0, IGNORE_TRAPS);
  }
  else
  {
    if(User->IsPlayer())
      ADD_MESSAGE("%s is now %sactive.", CHAR_NAME(DEFINITE), IsActive() ? "in" : "");

    SetIsActive(!IsActive());
    User->DexterityAction(10);

    if(IsActive())
    {
      Team = User->GetTeam()->GetID();
      RemoveFromSlot();
      User->GetStackUnder()->AddItem(this);
    }
  }

  return true;
}



v2 beartrap::GetBitmapPos(int Frame) const
{
  if(!IsBroken())
    return IsActive() ? v2(32, 304) : v2(32, 320);
  else
    return item::GetBitmapPos(Frame);
}



truth beartrap::IsPickable(character* Picker) const
{
  return !IsActive() && !Picker->IsStuckToTrap(GetTrapID());
}



truth beartrap::ReceiveDamage(character* Damager, int Damage, int Type, int)
{
  if(!IsBroken() && Type & PHYSICAL_DAMAGE && Damage)
  {
    if(Damage > 125 || !(RAND() % (250 / Damage)))
    {
      SetIsActive(false);
      Break(Damager);
      return true;
    }
    else
    {
      if(IsActive())
      {
  if(CanBeSeenByPlayer())
    ADD_MESSAGE("%s snaps shut.", CHAR_NAME(DEFINITE));

  SetIsActive(false);
  SendNewDrawAndMemorizedUpdateRequest();
  return true;
      }
    }
  }

  return false;
}



void beartrap::Fly(character* Thrower, int Direction, int Force)
{
  if(!IsStuck())
    item::Fly(Thrower, Direction, Force);
}



void beartrap::PreProcessForBone()
{
  mybase::PreProcessForBone();
  game::RemoveTrapID(TrapData.TrapID);
  TrapData.TrapID = 0;
}



void beartrap::PostProcessForBone()
{
  mybase::PostProcessForBone();
  TrapData.TrapID = game::CreateNewTrapID(this);
}



int beartrap::GetBaseTrapDamage() const
{
  int Modifier = GetMainMaterial()->GetStrengthValue() / 50;
  Modifier *= Modifier;
  Modifier >>= 1;
  return Modifier ? Modifier + RAND_N(Modifier) : 1;
}



void beartrap::RemoveFromSlot()
{
  character* Char = game::SearchCharacter(GetVictimID());

  if(Char)
    Char->RemoveTrap(GetTrapID());

  TrapData.VictimID = 0;
  item::RemoveFromSlot();
}



void beartrap::DonateSlotTo(item* Item)
{
  character* Char = game::SearchCharacter(GetVictimID());

  if(Char)
    Char->RemoveTrap(GetTrapID());

  TrapData.VictimID = 0;
  item::DonateSlotTo(Item);
}
#endif
