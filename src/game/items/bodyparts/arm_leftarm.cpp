#ifdef HEADER_PHASE
ITEM(leftarm, arm)
{
 public:
  leftarm();
  leftarm(const leftarm&);
  virtual int GetBodyPartIndex() const;
  virtual arm* GetPairArm() const;
  virtual truth IsRightArm() const { return false; }
  virtual int GetSpecialFlags() const;
 protected:
  virtual sweaponskill** GetCurrentSWeaponSkill() const;
};


#else



int leftarm::GetBodyPartIndex() const { return LEFT_ARM_INDEX; }



int leftarm::GetSpecialFlags() const { return SpecialFlags|ST_LEFT_ARM; }



leftarm::leftarm()
{
  WieldedSlot.Init(this, LEFT_WIELDED_INDEX);
  GauntletSlot.Init(this, LEFT_GAUNTLET_INDEX);
  RingSlot.Init(this, LEFT_RING_INDEX);
}



arm* leftarm::GetPairArm() const
{
  return GetHumanoidMaster() ? GetHumanoidMaster()->GetRightArm() : 0;
}



sweaponskill** leftarm::GetCurrentSWeaponSkill() const
{
  return &GetHumanoidMaster()->CurrentLeftSWeaponSkill;
}



leftarm::leftarm(const leftarm& Arm) : mybase(Arm)
{
  WieldedSlot.Init(this, LEFT_WIELDED_INDEX);
  GauntletSlot.Init(this, LEFT_GAUNTLET_INDEX);
  RingSlot.Init(this, LEFT_RING_INDEX);
}
#endif
