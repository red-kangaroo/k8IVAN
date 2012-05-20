#ifdef HEADER_PHASE
ITEM(rightarm, arm)
{
 public:
  rightarm();
  rightarm(const rightarm&);
  virtual int GetBodyPartIndex() const;
  virtual arm* GetPairArm() const;
  virtual truth IsRightArm() const { return true; }
  virtual int GetSpecialFlags() const;
 protected:
  virtual sweaponskill** GetCurrentSWeaponSkill() const;
};


#else



int rightarm::GetBodyPartIndex() const { return RIGHT_ARM_INDEX; }



int rightarm::GetSpecialFlags() const { return SpecialFlags|ST_RIGHT_ARM; }



rightarm::rightarm()
{
  WieldedSlot.Init(this, RIGHT_WIELDED_INDEX);
  GauntletSlot.Init(this, RIGHT_GAUNTLET_INDEX);
  RingSlot.Init(this, RIGHT_RING_INDEX);
}



arm* rightarm::GetPairArm() const
{
  return GetHumanoidMaster() ? GetHumanoidMaster()->GetLeftArm() : 0;
}



sweaponskill** rightarm::GetCurrentSWeaponSkill() const
{
  return &GetHumanoidMaster()->CurrentRightSWeaponSkill;
}



rightarm::rightarm(const rightarm& Arm) : mybase(Arm)
{
  WieldedSlot.Init(this, RIGHT_WIELDED_INDEX);
  GauntletSlot.Init(this, RIGHT_GAUNTLET_INDEX);
  RingSlot.Init(this, RIGHT_RING_INDEX);
}
#endif
