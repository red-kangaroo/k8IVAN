#ifdef HEADER_PHASE
ITEM(potion, materialcontainer)
{
 public:
  virtual item* BetterVersion() const;
  virtual void DipInto(liquid*, character*);
  virtual liquid* CreateDipLiquid();
  virtual truth IsDippable(ccharacter*) const { return !SecondaryMaterial; }
  virtual void Break(character*, int);
  virtual truth IsDipDestination(ccharacter*) const;
  virtual truth IsDumpable(ccharacter*) const { return SecondaryMaterial != 0; }
  virtual truth IsExplosive() const;
  virtual truth ReceiveDamage(character*, int, int, int);
  virtual truth HasBetterVersion() const { return !SecondaryMaterial; }
  virtual truth EffectIsGood() const;
  virtual truth IsKamikazeWeapon(ccharacter*) const { return IsExplosive(); }
  virtual truth IsBottle () const { return true; }
 protected:
  virtual void AddPostFix(festring& String, int) const { AddContainerPostFix(String); }
  virtual truth AddAdjective(festring&, truth) const;
};


#else



truth potion::IsExplosive() const { return GetSecondaryMaterial() && GetSecondaryMaterial()->IsExplosive(); }



truth potion::AddAdjective(festring& String, truth Articled) const { return AddEmptyAdjective(String, Articled); }



truth potion::EffectIsGood() const { return GetSecondaryMaterial() && GetSecondaryMaterial()->GetInteractionFlags() & EFFECT_IS_GOOD; }



truth potion::IsDipDestination(ccharacter*) const { return SecondaryMaterial && SecondaryMaterial->IsLiquid(); }



liquid* potion::CreateDipLiquid()
{
  return static_cast<liquid*>(GetSecondaryMaterial()->TakeDipVolumeAway());
}



void potion::DipInto(liquid* Liquid, character* Dipper)
{
  /* Add alchemy */

  if(Dipper->IsPlayer())
    ADD_MESSAGE("%s is now filled with %s.", CHAR_NAME(DEFINITE), Liquid->GetName(false, false).CStr());

  ChangeSecondaryMaterial(Liquid);
  Dipper->DexterityAction(10);
}



item* potion::BetterVersion() const
{
  if(!GetSecondaryMaterial())
    return potion::Spawn();
  else
    return 0;
}



void potion::Break(character* Breaker, int Dir)
{
  if(CanBeSeenByPlayer())
    ADD_MESSAGE("%s shatters to pieces.", GetExtendedDescription().CStr());
  else if(PLAYER->CanHear())
    ADD_MESSAGE("You hear something shattering.");

  if(Breaker && IsOnGround())
  {
    room* Room = GetRoom();

    if(Room)
      Room->HostileAction(Breaker);
  }

  item* Remains = brokenbottle::Spawn(0, NO_MATERIALS);
  Remains->InitMaterials(GetMainMaterial()->SpawnMore());
  DonateFluidsTo(Remains);
  DonateIDTo(Remains);
  DonateSlotTo(Remains);
  SendToHell();

  if(GetSecondaryMaterial() && GetSecondaryMaterial()->IsLiquid())
  {
    liquid* Liquid = static_cast<liquid*>(GetSecondaryMaterial());

    if(Dir != YOURSELF)
    {
      v2 Pos = Remains->GetPos() + game::GetMoveVector(Dir);

      if(Remains->GetLevel()->IsValidPos(Pos))
      {
  sLong HalfVolume = GetSecondaryMaterial()->GetVolume() >> 1;
  Liquid->EditVolume(-HalfVolume);
  Remains->GetNearLSquare(Pos)->SpillFluid(Breaker, Liquid->SpawnMoreLiquid(HalfVolume));
      }
    }

    if(Remains->Exists())
      Remains->GetLSquareUnder()->SpillFluid(Breaker, Liquid->SpawnMoreLiquid(Liquid->GetVolume()));
  }

  if(PLAYER->Equips(Remains))
    game::AskForEscPress(CONST_S("Equipment broken!"));
}



truth potion::ReceiveDamage(character* Damager, int Damage, int Type, int Dir)
{
  if(Type & FIRE && Damage && IsExplosive() && (Damage > 50 || !(RAND() % (100 / Damage))))
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

  if(Type & THROW)
  {
    int StrengthValue = GetStrengthValue();

    if(!StrengthValue)
      StrengthValue = 1;

    if(Damage > StrengthValue << 2 && RAND() % (50 * Damage / StrengthValue) >= 100)
    {
      Break(Damager, Dir);
      return true;
    }
  }

  return item::ReceiveDamage(Damager, Damage, Type, Dir);
}
#endif
