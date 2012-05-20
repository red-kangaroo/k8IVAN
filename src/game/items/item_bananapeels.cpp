#ifdef HEADER_PHASE
ITEM(bananapeels, item)
{
 public:
  virtual item* BetterVersion() const;
  virtual truth HasBetterVersion() const { return true; }
  virtual void StepOnEffect(character*);
  virtual truth IsBananaPeel() const { return true; }
  virtual truth IsDangerous(ccharacter*) const;
  virtual truth RaiseTheDead(character*);
};


#else



truth bananapeels::IsDangerous(ccharacter* Stepper) const { return Stepper->HasALeg(); }



void bananapeels::StepOnEffect(character* Stepper)
{
  if(Stepper->HasALeg() && !(RAND() % 5))
  {
    if(Stepper->IsPlayer())
      ADD_MESSAGE("Ouch. Your feet slip on %s and you fall down.", CHAR_NAME(INDEFINITE));
    else if(Stepper->CanBeSeenByPlayer())
      ADD_MESSAGE("%s steps on %s and falls down.", Stepper->CHAR_NAME(DEFINITE), CHAR_NAME(INDEFINITE));

    /* Do damage against any random bodypart except legs */

    Stepper->ReceiveDamage(0, 1 + (RAND() & 1), PHYSICAL_DAMAGE, ALL&~LEGS);
    Stepper->CheckDeath(CONST_S("slipped on a banana peel"), 0);
    Stepper->EditAP(-500);
  }
}



item* bananapeels::BetterVersion() const
{
  return banana::Spawn();
}



truth bananapeels::RaiseTheDead(character*)
{
  GetSlot()->AddFriendItem(banana::Spawn());
  RemoveFromSlot();
  SendToHell();
  return true;
}
#endif
