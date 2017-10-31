#ifdef HEADER_PHASE
ITEM(can, materialcontainer)
{
public:
  virtual item *BetterVersion () const;
  virtual void DipInto (liquid *, character *);
  virtual truth IsDippable (ccharacter *) const { return !SecondaryMaterial; }
  virtual truth IsDipDestination (ccharacter *) const;
  virtual truth IsDumpable (ccharacter *) const { return SecondaryMaterial != 0; }
  virtual liquid *CreateDipLiquid ();
  virtual truth AllowSpoil () const { return false; } // temporary
  virtual truth Spoils () const { return false; } // temporary
  virtual truth HasBetterVersion () const { return !SecondaryMaterial; }
  virtual truth IsCan () const { return true; }
 protected:
  virtual void AddPostFix (festring &String, int) const { AddContainerPostFix(String); }
  virtual truth AddAdjective (festring &, truth) const;
  virtual v2 GetBitmapPos (int) const;
};


#else


truth can::AddAdjective (festring &String, truth Articled) const { return AddEmptyAdjective(String, Articled); }
v2 can::GetBitmapPos (int) const { return v2(16, SecondaryMaterial ? 288 : 304); }
truth can::IsDipDestination (ccharacter *) const { return SecondaryMaterial && SecondaryMaterial->IsLiquid(); }
liquid *can::CreateDipLiquid () { return static_cast<liquid *>(GetSecondaryMaterial()->TakeDipVolumeAway()); }



item *can::BetterVersion () const {
  if (!GetSecondaryMaterial()) return can::Spawn();
  return 0;
}


void can::DipInto (liquid *Liquid, character *Dipper) {
  /* Add alchemy */
  if (Dipper->IsPlayer()) ADD_MESSAGE("%s is now filled with %s.", CHAR_NAME(DEFINITE), Liquid->GetName(false, false).CStr());
  ChangeSecondaryMaterial(Liquid);
  Dipper->DexterityAction(10);
}


#endif
