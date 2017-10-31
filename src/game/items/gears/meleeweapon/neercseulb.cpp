#ifdef HEADER_PHASE
ITEM(neercseulb, meleeweapon)
{
 public:
  virtual truth HitEffect(character*, character*, v2, int, int, truth);
  virtual truth AllowAlphaEverywhere() const { return true; }
 protected:
  virtual int GetClassAnimationFrames() const { return 32; }
  virtual col16 GetOutlineColor(int) const;
  virtual alpha GetOutlineAlpha(int) const;
};


#else



col16 neercseulb::GetOutlineColor(int) const { return MakeRGB16(255, 0, 0); }



truth neercseulb::HitEffect(character* Enemy, character* Hitter, v2 HitPos, int BodyPartIndex, int Direction, truth BlockedByArmour)
{
  truth BaseSuccess = meleeweapon::HitEffect(Enemy, Hitter, HitPos, BodyPartIndex, Direction, BlockedByArmour);

  if(!IsBroken() && Enemy->IsEnabled() && !(RAND() % 5))
  {
    if(Hitter->IsPlayer())
      game::DoEvilDeed(10);

    if(Enemy->IsPlayer() || Hitter->IsPlayer() || Enemy->CanBeSeenByPlayer() || Hitter->CanBeSeenByPlayer())
      ADD_MESSAGE("%s Neerc Se-ulb's life-draining energies swallow %s!", Hitter->CHAR_POSSESSIVE_PRONOUN, Enemy->CHAR_DESCRIPTION(DEFINITE));

    return Enemy->ReceiveBodyPartDamage(Hitter, 10 + (RAND() % 11), DRAIN, BodyPartIndex, Direction) || BaseSuccess;
  }
  else
    return BaseSuccess;
}



alpha neercseulb::GetOutlineAlpha(int Frame) const
{
  Frame &= 31;
  return 50 + (Frame * (31 - Frame) >> 1);
}
#endif
