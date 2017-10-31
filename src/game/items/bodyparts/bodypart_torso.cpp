#ifdef HEADER_PHASE
ITEM(torso, bodypart)
{
 public:
  virtual int GetBodyPartIndex() const;
  virtual double GetRoughChanceToHit(double, double) const;
};


#else



int torso::GetBodyPartIndex() const { return TORSO_INDEX; }



double torso::GetRoughChanceToHit(double ToHitValue, double DodgeValue) const
{
  return 1 / (DodgeValue / ToHitValue + 1);
}
#endif
