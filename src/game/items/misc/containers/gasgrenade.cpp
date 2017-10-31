#ifdef HEADER_PHASE
ITEM(gasgrenade, materialcontainer)
{
 protected:
  virtual void AddPostFix(festring& String, int) const { AddContainerPostFix(String); }
  truth ReceiveDamage (character *Damager, int Damage, int Type, int Dir);
};


#else


truth gasgrenade::ReceiveDamage (character *Damager, int Damage, int Type, int Dir) {
  if ((Type&THROW) || ((Type&(PHYSICAL_DAMAGE|FIRE|ENERGY)) && Damage && (!(RAND_N(10/Damage+1))))) {
    if (GetSquareUnder()->CanBeSeenByPlayer(true)) ADD_MESSAGE("%s explodes!", GetExtendedDescription().CStr());
    material *GasMaterial = GetSecondaryMaterial();
    GetLevel()->GasExplosion(static_cast<gas*>(GasMaterial), GetLSquareUnder(), Damager);
    RemoveFromSlot();
    SendToHell();
    return true;
  }
  return false;
}


#endif
