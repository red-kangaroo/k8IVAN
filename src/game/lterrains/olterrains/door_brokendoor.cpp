#ifdef HEADER_PHASE
OLTERRAIN(brokendoor, door)
{
 public:
  virtual void BeKicked(character*, int, int);
  virtual void ReceiveDamage(character*, int, int);
  virtual void HasBeenHitByItem(character*, item*, int);
  virtual void Break() { olterrain::Break(); }
};


#else



void brokendoor::HasBeenHitByItem (character *Thrower, item *, int Damage) { ReceiveDamage(Thrower, Damage, PHYSICAL_DAMAGE); }



void brokendoor::BeKicked (character *Kicker, int KickDamage, int) {
  if (!Opened) {
    if (!IsLocked() && KickDamage > (RAND() & 3)) {
      if (CanBeSeenByPlayer()) ADD_MESSAGE("The broken door opens.");
      MakeWalkable();
      return;
    }
    if (IsLocked()) {
      int SV = Max(GetStrengthValue(), 1);
      if (KickDamage > SV && RAND()%(100*KickDamage/SV) >= 100) {
        if (RAND() & 1) {
          if (CanBeSeenByPlayer()) ADD_MESSAGE("The broken door opens from the force of the kick.");
          MakeWalkable();
        } else if (CanBeSeenByPlayer()) {
          ADD_MESSAGE("The lock breaks from the force of the kick.");
          if (GetRoom()) GetRoom()->DestroyTerrain(Kicker);
        }
        SetIsLocked(false);
        return;
      }
    }
    if (CanBeSeenByPlayer() && (Kicker->CanBeSeenByPlayer() || Kicker->IsPlayer())) ADD_MESSAGE("The broken door won't budge!");
  }
}



void brokendoor::ReceiveDamage (character *Villain, int Damage, int) {
  if (!Opened && !IsLocked() && Damage > (RAND() & 3)) {
    if (CanBeSeenByPlayer()) ADD_MESSAGE("The broken door opens.");
    MakeWalkable();
    return;
  }
  if (CanBeDestroyed() && Damage > GetStrengthValue()) {
    EditHP(GetStrengthValue()-Damage);
    int SV = Max(GetStrengthValue(), 1);
    truth LockBreaks = IsLocked() && RAND()%(100*Damage/SV) >= 100;
    if (LockBreaks) SetIsLocked(false);
    if (HP <= 0) {
      if (CanBeSeenByPlayer()) ADD_MESSAGE("The broken door is completely destroyed.");
      room *Room = GetRoom();
      Break();
      if (Room) Room->DestroyTerrain(Villain);
    } else if (LockBreaks && CanBeSeenByPlayer()) {
      ADD_MESSAGE("The broken door's lock is shattered.");
      if (GetRoom()) GetRoom()->DestroyTerrain(Villain);
    }
  }
}
#endif
