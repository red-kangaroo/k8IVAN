#ifdef HEADER_PHASE
ITEM(kleinbottle, materialcontainer)
{
 public:
  //virtual item *BetterVersion () const; // better version is a filled version of the klein bottle
  virtual void DipInto (liquid *, character *);
  virtual liquid *CreateDipLiquid ();
  virtual truth IsDippable (ccharacter *) const { return !SecondaryMaterial; }
  virtual truth IsDumpable (ccharacter *) const { return false; } // can't dump!
  virtual material *GetConsumeMaterial (ccharacter *, materialpredicate = TrueMaterialPredicate) const;
  //virtual void Break (character *, int); // cannot be broken, remove this
  //virtual truth IsDipDestination (ccharacter *) const; // cannot be dipped into
  //virtual truth IsExplosive () const; // cannot explode contents since the contents resides in another dimension
  //virtual truth ReceiveDamage (character *, int, int, int); // should not recieve damage
  //virtual truth HasBetterVersion () const { return !SecondaryMaterial; } // cannot be filled by mellis
  //virtual truth EffectIsGood () const;
  //virtual truth IsKamikazeWeapon (ccharacter *) const { return IsExplosive(); } // no explode, so false
  virtual truth IsKleinBottle () const { return true; }

protected:
  virtual void AddPostFix (festring &String, int) const { AddContainerPostFix(String); }
  virtual truth AddAdjective (festring &, truth) const;
};


#else


truth kleinbottle::AddAdjective (festring &String, truth Articled) const { return AddEmptyAdjective(String, Articled); }
liquid *kleinbottle::CreateDipLiquid () { return static_cast<liquid *>(GetSecondaryMaterial()->TakeDipVolumeAway()); }


void kleinbottle::DipInto (liquid *Liquid, character *Dipper) {
  /* Add alchemy */
  if (Dipper->IsPlayer()) ADD_MESSAGE("%s is now filled with %s.", CHAR_NAME(DEFINITE), Liquid->GetName(false, false).CStr());
  ChangeSecondaryMaterial(Liquid);
  Dipper->DexterityAction(100);
}


material *kleinbottle::GetConsumeMaterial (ccharacter *Consumer, materialpredicate Predicate) const {
  if (SecondaryMaterial && (SecondaryMaterial->*Predicate)() && Consumer->CanConsume(SecondaryMaterial) && Consumer->StateIsActivated(TELEPORT)) {
    return SecondaryMaterial;
  }
  return item::GetConsumeMaterial(Consumer, Predicate);
}


#endif
