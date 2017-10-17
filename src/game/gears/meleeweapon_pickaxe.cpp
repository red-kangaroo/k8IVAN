#ifdef HEADER_PHASE
ITEM(pickaxe, meleeweapon)
{
 public:
  virtual truth Apply(character*);
  virtual truth IsAppliable(ccharacter*) const;
};


#else


truth pickaxe::Apply (character *User) {
  if (IsBroken()) {
    ADD_MESSAGE("%s is totally broken.",CHAR_NAME(DEFINITE));
    return false;
  }

  int Dir = game::DirectionQuestion(CONST_S("What direction do you want to dig? [press a direction key]"), false);
  if (Dir == DIR_ERROR) return false;

  v2 Temp = game::GetMoveVector(Dir);
  v2 ToBeDug = User->GetPos()+Temp;
  if (!GetArea()->IsValidPos(ToBeDug)) return false;

  lsquare *Square = GetNearLSquare(ToBeDug);
  olterrain *Terrain = Square->GetOLTerrain();

  if (!Terrain) {
    ADD_MESSAGE("Nothing to dig there!");
    return false;
  }

  if (Square->CanBeDug()) {
    if (Terrain->CanBeDestroyed()) {
      if (Terrain->GetMainMaterial()->CanBeDug(GetMainMaterial())) {
        int RoomNumber = Square->GetRoomIndex();
        if (!RoomNumber || Square->GetLevel()->GetRoom(RoomNumber)->CheckDestroyTerrain(User)) {
          User->SwitchToDig(this, ToBeDug);
          User->DexterityAction(5);
          return true;
        }
        return false;
      } else {
        ADD_MESSAGE("%s is too hard to dig with %s.", Square->GetOLTerrain()->CHAR_NAME(DEFINITE), CHAR_NAME(INDEFINITE));
      }
    } else {
      ADD_MESSAGE("%s", Terrain->GetDigMessage().CStr());
    }
  }

  return false;
}


truth pickaxe::IsAppliable (ccharacter *Who) const {
  return Who->CanWield();
}

#endif
