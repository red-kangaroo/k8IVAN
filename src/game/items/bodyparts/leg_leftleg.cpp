#ifdef HEADER_PHASE
ITEM(leftleg, leg)
{
 public:
  leftleg();
  leftleg(const leftleg&);
  virtual int GetBodyPartIndex() const;
  virtual int GetSpecialFlags() const;
 protected:
};


#else



int leftleg::GetBodyPartIndex() const { return LEFT_LEG_INDEX; }



int leftleg::GetSpecialFlags() const { return SpecialFlags|ST_LEFT_LEG; }



leftleg::leftleg()
{
  BootSlot.Init(this, LEFT_BOOT_INDEX);
}



leftleg::leftleg(const leftleg& Leg) : mybase(Leg)
{
  BootSlot.Init(this, LEFT_BOOT_INDEX);
}
#endif
