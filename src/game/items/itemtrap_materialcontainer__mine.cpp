#ifdef HEADER_PHASE
ITEM(mine, itemtrap<materialcontainer>)
{
 public:
  virtual void StepOnEffect(character*);
  virtual truth ReceiveDamage(character*, int, int, int);
  virtual truth Apply(character* User);
  virtual truth IsDangerous(ccharacter* Stepper) const { return WillExplode(Stepper); }
  virtual truth WillExplode(ccharacter*) const;
  virtual truth CheckPickUpEffect(character*);
 protected:
  virtual truth AddAdjective(festring&, truth) const;
};


#else



truth mine::AddAdjective(festring& String, truth Articled) const { return IsActive() && AddActiveAdjective(String, Articled); }



truth mine::ReceiveDamage(character* Damager, int Damage, int Type, int)
{
  if((Type & (FIRE|ENERGY) && Damage && (Damage > 50 || !(RAND() % (100 / Damage)))) || (Type & (PHYSICAL_DAMAGE|SOUND) && WillExplode(0)))
  {
    festring DeathMsg = CONST_S("killed by an explosion of ");
    AddName(DeathMsg, INDEFINITE);

    if(Damager)
      DeathMsg << " caused @bk";

    if(GetSquareUnder()->CanBeSeenByPlayer(true))
      ADD_MESSAGE("%s explodes!", GetExtendedDescription().CStr());

    lsquare* Square = GetLSquareUnder();
    RemoveFromSlot();
    SendToHell();
    Square->GetLevel()->Explosion(Damager, DeathMsg, Square->GetPos(), GetSecondaryMaterial()->GetTotalExplosivePower());
    return true;
  }

  return false;
}



void mine::StepOnEffect(character* Stepper)
{
  if(!WillExplode(Stepper))
    return;

  if(Stepper->IsPlayer())
  {
    cchar* SenseVerb = Stepper->CanHear() ? "hear" : "sense";

    if(GetLSquareUnder()->IsDark())
      ADD_MESSAGE("You %s a faint thump. You try to look down, but it is too dark to see anything.", SenseVerb);
    else
      ADD_MESSAGE("You %s a faint thump. You look down. You see %s.", SenseVerb, CHAR_NAME(INDEFINITE));
  }
  else if(Stepper->CanBeSeenByPlayer())
    ADD_MESSAGE("%s steps on %s.", Stepper->CHAR_NAME(DEFINITE), CHAR_NAME(INDEFINITE));
  else if(GetSquareUnder()->CanBeSeenByPlayer(true))
    ADD_MESSAGE("Something explodes!");

  SetIsActive(false);
  SendNewDrawAndMemorizedUpdateRequest();

  if (Stepper->IsPlayer()) game::AskForEscPress(CONST_S("Trap activated!"));

  lsquare* Square = GetLSquareUnder();
  RemoveFromSlot();
  SendToHell();
  Square->GetLevel()->Explosion(0, "killed by a land mine", Square->GetPos(), GetSecondaryMaterial()->GetTotalExplosivePower());
}



truth mine::Apply(character* User)
{
  if(User->IsPlayer() && !game::TruthQuestion(CONST_S("Are you sure you want to plant ") + GetName(DEFINITE) + "? [y/N]"))
    return false;

  room* Room = GetRoom();

  if(Room)
    Room->HostileAction(User);

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

  return true;
}



truth mine::WillExplode(ccharacter* Stepper) const
{
  return IsActive() && GetSecondaryMaterial() && GetSecondaryMaterial()->IsExplosive() && (!Stepper || Stepper->GetWeight() > 5000);
}



truth mine::CheckPickUpEffect(character*)
{
  if(WillExplode(0))
  {
    lsquare* Square = GetLSquareUnder();

    if(Square->CanBeSeenByPlayer(true))
      ADD_MESSAGE("%s explodes!", GetExtendedDescription().CStr());

    RemoveFromSlot();
    SendToHell();
    Square->GetLevel()->Explosion(0, "killed by a land mine", Square->GetPos(), GetSecondaryMaterial()->GetTotalExplosivePower());
    return false;
  }

  return true;
}
#endif
