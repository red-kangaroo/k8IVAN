#ifdef HEADER_PHASE
ITEM(normaltorso, torso)
{
 public:
  virtual int GetGraphicsContainerIndex() const;
  virtual int GetTotalResistance(int) const;
};


#else



int normaltorso::GetGraphicsContainerIndex() const { return GR_CHARACTER; }



int normaltorso::GetTotalResistance(int Type) const
{
  if(Master)
    return GetResistance(Type) + Master->GetGlobalResistance(Type);
  else
    return GetResistance(Type);
}
#endif
