#ifdef HEADER_PHASE
CHARACTER(genie, humanoid)
{
 public:
  virtual truth BodyPartIsVital(int) const;
  virtual int GetAttribute(int, truth = true) const;
  virtual truth CanCreateBodyPart(int) const;
  virtual cfestring& GetStandVerb() const { return character::GetStandVerb(); }
};


#else



truth genie::BodyPartIsVital(int I) const { return I == TORSO_INDEX || I == HEAD_INDEX; }



int genie::GetAttribute(int Identifier, truth AllowBonus) const // temporary until someone invents a better way of doing this
{
  if(Identifier == LEG_STRENGTH)
    return GetDefaultLegStrength();
  else if(Identifier == AGILITY)
    return GetDefaultAgility();
  else
    return humanoid::GetAttribute(Identifier, AllowBonus);
}



truth genie::CanCreateBodyPart(int I) const
{
  return I == TORSO_INDEX || I == HEAD_INDEX || I == RIGHT_ARM_INDEX || I == LEFT_ARM_INDEX;
}
#endif
