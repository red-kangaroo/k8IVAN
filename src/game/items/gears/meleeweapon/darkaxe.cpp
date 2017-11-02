#ifdef HEADER_PHASE
ITEM(darkaxe, meleeweapon)
{
 public:
  darkaxe () { Enable(); }
  virtual void Be () override;
protected:
  virtual truth CalculateHasBe () const override { return true; }
};


#else


void darkaxe::Be () {
  meleeweapon::Be();
  if (Exists() && !IsBroken() && (*Slot)->IsGearSlot() && !RAND_N(10)) {
    fluidvector FluidVector;
    FillFluidVector(FluidVector);
    uInt Volume = 0;

    for (uInt c = 0; c < FluidVector.size(); ++c) {
      liquid *L = FluidVector[c]->GetLiquid();
      Volume += L->GetVolume();
    }

    if (Volume < 90) SpillFluid(0, liquid::Spawn(LIQUID_DARKNESS, 10));
  }
}


#endif
