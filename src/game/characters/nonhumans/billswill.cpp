#ifdef HEADER_PHASE
CHARACTER(billswill, nonhumanoid)
{
 protected:
  virtual int GetBodyPartWobbleData(int) const;
  virtual cchar* FirstPersonBiteVerb() const;
  virtual cchar* FirstPersonCriticalBiteVerb() const;
  virtual cchar* ThirdPersonBiteVerb() const;
  virtual cchar* ThirdPersonCriticalBiteVerb() const;
  virtual truth AttackIsBlockable(int) const { return false; }
  virtual truth AttackMayDamageArmor() const { return false; }
};


#else



cchar* billswill::FirstPersonBiteVerb() const { return "emit psi waves at"; }



cchar* billswill::FirstPersonCriticalBiteVerb() const { return "emit powerful psi waves at"; }



cchar* billswill::ThirdPersonBiteVerb() const { return "emits psi waves at"; }



cchar* billswill::ThirdPersonCriticalBiteVerb() const { return "emits powerful psi waves at"; }



int billswill::GetBodyPartWobbleData(int) const { return WOBBLE_HORIZONTALLY|(2 << WOBBLE_FREQ_SHIFT); }
#endif
