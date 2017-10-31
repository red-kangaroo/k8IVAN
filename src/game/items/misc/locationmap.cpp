#ifdef HEADER_PHASE
ITEM(locationmap, item)
{
public:
  virtual truth CanBeRead (character *) const;
  virtual truth IsReadable (ccharacter *) const { return true; }
  virtual truth ReceiveDamage (character *, int, int, int);
};


#else


truth locationmap::CanBeRead (character *Reader) const {
  return Reader->CanRead() || game::GetSeeWholeMapCheatMode();
}


truth locationmap::ReceiveDamage (character *, int Damage, int Type, int) {
  if (Type & FIRE && Damage && GetMainMaterial()->GetInteractionFlags() & CAN_BURN && (Damage > 125 || !(RAND()%(250/Damage)))) {
    if (CanBeSeenByPlayer()) ADD_MESSAGE("%s catches fire!", GetExtendedDescription().CStr());
    RemoveFromSlot();
    SendToHell();
    return true;
  }
  return false;
}


#endif
