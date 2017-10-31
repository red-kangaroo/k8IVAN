#ifdef HEADER_PHASE
ITEM(superkawai, meleeweapon) {
public:
  virtual truth HitEffect (character *, character *, v2, int, int, truth) override;
  virtual truth AllowAlphaEverywhere () const override { return true; }
  virtual truth IsAppliable (ccharacter *Who) const override;
  virtual truth Apply (character *User) override;
  virtual truth IsZappable (ccharacter *Who) const override;
  virtual truth Zap (character *Zapper, v2 curpos, int Direction) override;
protected:
  virtual int GetClassAnimationFrames () const override { return 32; }
  virtual col16 GetOutlineColor (int) const override;
  virtual alpha GetOutlineAlpha (int) const override;
};


#else


col16 superkawai::GetOutlineColor (int) const { return MakeRGB16(255, 0, 0); }
alpha superkawai::GetOutlineAlpha (int Frame) const { Frame &= 31; return 50+(Frame*(31-Frame)>>1); }

truth superkawai::IsAppliable (ccharacter *Who) const { return Who->CanWield(); }
truth superkawai::IsZappable (ccharacter *Who) const { return true; }


truth superkawai::HitEffect (character *Enemy, character *Hitter, v2 HitPos, int BodyPartIndex, int Direction, truth BlockedByArmour) {
  truth BaseSuccess = meleeweapon::HitEffect(Enemy, Hitter, HitPos, BodyPartIndex, Direction, BlockedByArmour);
  if (!IsBroken() && Enemy->IsEnabled() && !(RAND()%5)) {
    //if (Hitter->IsPlayer()) game::DoEvilDeed(10);
    if (Enemy->IsPlayer() || Hitter->IsPlayer() || Enemy->CanBeSeenByPlayer() || Hitter->CanBeSeenByPlayer()) {
      ADD_MESSAGE("%s SuperKawai's life-draining energies swallow %s!", Hitter->CHAR_POSSESSIVE_PRONOUN, Enemy->CHAR_DESCRIPTION(DEFINITE));
    }
    return Enemy->ReceiveBodyPartDamage(Hitter, 10+(RAND()%11), DRAIN, BodyPartIndex, Direction) || BaseSuccess;
  }
  return BaseSuccess;
}


truth superkawai::Apply (character *User) {
  // cannot be broken
  //if (IsBroken()) { ADD_MESSAGE("%s is totally broken.", CHAR_NAME(DEFINITE)); return false; }

  int Dir = game::DirectionQuestion(CONST_S("What direction do you want to dig? [press a direction key]"), false);
  if (Dir == DIR_ERROR) return false;

  v2 Temp = game::GetMoveVector(Dir);
  v2 ToBeDug = User->GetPos()+Temp;
  if (!GetArea()->IsValidPos(ToBeDug)) return false;

  lsquare *Square = GetNearLSquare(ToBeDug);
  olterrain *Terrain = Square->GetOLTerrain();

  if (!Terrain) { ADD_MESSAGE("Nothing to dig there!"); return false; }

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


truth superkawai::Zap (character *Zapper, v2 curpos, int Direction) {
  Zapper->EditExperience(PERCEPTION, 150, 1<<10);

  beamdata Beam (
    Zapper,
    CONST_S("killed by ")+GetName(INDEFINITE)+" zapped @bk",
    Zapper->GetPos(),
    GetBeamColor(),
    GetBeamEffect(),
    Direction,
    GetBeamRange(),
    0/*GetSpecialParameters()*/
  );

  (GetLevel()->*level::GetBeam(GetBeamStyle()))(Beam);

  return true;
}

#endif
