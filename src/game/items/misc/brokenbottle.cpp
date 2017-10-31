#ifdef HEADER_PHASE
ITEM(brokenbottle, item)
{
 public:
  virtual truth IsBroken() const { return true; }
  virtual item* BetterVersion() const;
  virtual truth HasBetterVersion() const { return true; }
  virtual void StepOnEffect(character*);
  virtual item* Fix();
  virtual truth IsDangerous(ccharacter*) const;
  virtual truth IsBottle () const { return true; }
};


#else



truth brokenbottle::IsDangerous(ccharacter* Stepper) const { return Stepper->HasALeg(); }



item* brokenbottle::BetterVersion() const
{
  return potion::Spawn();
}



void brokenbottle::StepOnEffect(character* Stepper)
{
  if(Stepper->HasALeg() && !(RAND() % 5))
  {
    if(Stepper->IsPlayer())
      ADD_MESSAGE("Ouch. You step on sharp glass splinters.");
    else if(Stepper->CanBeSeenByPlayer())
      ADD_MESSAGE("%s steps on sharp glass splinters.", Stepper->CHAR_NAME(DEFINITE));

    Stepper->ReceiveDamage(0, 1 + RAND() % 3, PHYSICAL_DAMAGE, LEGS);
    Stepper->CheckDeath(CONST_S("stepped on a broken bottle"), 0);
  }
}



item* brokenbottle::Fix()
{
  potion* Potion = potion::Spawn(0, NO_MATERIALS);
  Potion->InitMaterials(GetMainMaterial(), 0);
  DonateFluidsTo(Potion);
  DonateIDTo(Potion);
  DonateSlotTo(Potion);
  SetMainMaterial(0, NO_PIC_UPDATE|NO_SIGNALS);
  SendToHell();
  return Potion;
}
#endif
