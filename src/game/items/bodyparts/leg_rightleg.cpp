#ifdef HEADER_PHASE
ITEM(rightleg, leg)
{
 public:
  rightleg();
  rightleg(const rightleg&);
  virtual int GetBodyPartIndex() const;
  virtual int GetSpecialFlags() const;
 protected:
};


#else



int rightleg::GetBodyPartIndex() const { return RIGHT_LEG_INDEX; }



int rightleg::GetSpecialFlags() const { return SpecialFlags|ST_RIGHT_LEG; }



rightleg::rightleg()
{
  BootSlot.Init(this, RIGHT_BOOT_INDEX);
}



rightleg::rightleg(const rightleg& Leg) : mybase(Leg)
{
  BootSlot.Init(this, RIGHT_BOOT_INDEX);
}
#endif
