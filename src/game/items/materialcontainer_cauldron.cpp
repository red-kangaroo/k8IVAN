#ifdef HEADER_PHASE
ITEM(cauldron, materialcontainer)
{
public:
  virtual item *BetterVersion () const;
  virtual void DipInto (liquid *, character *);
  virtual liquid *CreateDipLiquid ();
  virtual truth IsDippable (ccharacter *) const { return !SecondaryMaterial; }
  virtual truth IsDumpable (ccharacter *) const { return SecondaryMaterial != 0; }
  //virtual void Break (character *, int);
  virtual truth IsDipDestination (ccharacter *) const;
  virtual truth IsExplosive () const;
  //virtual truth ReceiveDamage (character *, int, int, int); // can't break ... unless it turns into a lump of copper?? hmmm. Maybe can have a cracked cauldron.
  virtual truth HasBetterVersion () const { return !SecondaryMaterial; }
  virtual truth EffectIsGood () const;
  virtual truth IsKamikazeWeapon (ccharacter *) const { return IsExplosive(); }

protected:
  virtual void AddPostFix (festring &String, int) const { AddContainerPostFix(String); }
  virtual truth AddAdjective (festring &, truth) const;
};


#else


truth cauldron::IsExplosive () const { return GetSecondaryMaterial() && GetSecondaryMaterial()->IsExplosive(); }
truth cauldron::AddAdjective (festring & String, truth Articled) const { return AddEmptyAdjective(String, Articled); }
truth cauldron::EffectIsGood () const { return GetSecondaryMaterial() && GetSecondaryMaterial()->GetInteractionFlags() & EFFECT_IS_GOOD; }
truth cauldron::IsDipDestination (ccharacter *) const { return SecondaryMaterial && SecondaryMaterial->IsLiquid(); }
liquid *cauldron::CreateDipLiquid () { return static_cast<liquid *>(GetSecondaryMaterial()->TakeDipVolumeAway()); }


void cauldron::DipInto (liquid *Liquid, character *Dipper) {
  /* Add alchemy */
  if (Dipper->IsPlayer()) ADD_MESSAGE("%s is now filled with %s.", CHAR_NAME(DEFINITE), Liquid->GetName(false, false).CStr());
  ChangeSecondaryMaterial(Liquid);
  Dipper->DexterityAction(10);
}


item *cauldron::BetterVersion () const {
  if (!GetSecondaryMaterial()) return cauldron::Spawn();
  return 0;
}


#endif
