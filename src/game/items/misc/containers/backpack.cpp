#ifdef HEADER_PHASE
ITEM(backpack, materialcontainer)
{
 public:
  virtual truth Apply(character*);
  virtual truth IsAppliable(ccharacter*) const { return true; }
  virtual truth ReceiveDamage(character*, int, int, int);
  virtual truth IsExplosive() const;
  virtual sLong GetTotalExplosivePower() const;
  virtual void SpillFluid(character*, liquid*, int = 0);
 protected:
  virtual void AddPostFix(festring& String, int) const { AddContainerPostFix(String); }
};


#else



truth backpack::IsExplosive() const { return GetSecondaryMaterial() && GetSecondaryMaterial()->IsExplosive(); }



sLong backpack::GetTotalExplosivePower() const { return GetSecondaryMaterial() ? GetSecondaryMaterial()->GetTotalExplosivePower() : 0; }



truth backpack::Apply(character* Terrorist)
{
  if(IsExplosive())
  {
    if(Terrorist->IsPlayer())
      ADD_MESSAGE("You light your %s. It explodes!", CHAR_NAME(UNARTICLED));
    else if(Terrorist->CanBeSeenByPlayer())
      ADD_MESSAGE("%s lights %s. It explodes!", Terrorist->CHAR_NAME(DEFINITE), CHAR_NAME(INDEFINITE));
    else if(GetSquareUnder()->CanBeSeenByPlayer(true))
      ADD_MESSAGE("Something explodes!");

    RemoveFromSlot();
    SendToHell();
    Terrorist->DexterityAction(5);
    Terrorist->GetLevel()->Explosion(Terrorist, CONST_S("kamikazed @k"), Terrorist->GetLSquareUnder()->GetPos(), GetSecondaryMaterial()->GetTotalExplosivePower());
    return true;
  }
  else if(Terrorist->IsPlayer())
    ADD_MESSAGE("You are not able to explode yourself with this crummy %s!", CHAR_NAME(UNARTICLED));

  return false;
}



truth backpack::ReceiveDamage(character* Damager, int Damage, int Type, int)
{
  if(Type & (FIRE|ENERGY) && Damage && IsExplosive() && (Damage > 25 || !(RAND() % (50 / Damage))))
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



void backpack::SpillFluid(character* Spiller, liquid* Liquid, int SquareIndex)
{
  if(!Liquid->IsExplosive())
  {
    GetSecondaryMaterial()->AddWetness(Liquid->GetVolume() * 25);

    if(CanBeSeenByPlayer())
      ADD_MESSAGE("%s gets wet.", CHAR_NAME(DEFINITE));
  }

  item::SpillFluid(Spiller, Liquid, SquareIndex);
}
#endif
