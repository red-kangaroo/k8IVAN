#ifdef HEADER_PHASE
ITEM(daggerofvenom, meleeweapon)
{
public:
  daggerofvenom ();

  virtual void Be ();

protected:
  virtual truth CalculateHasBe () const { return true; }
};


#else


daggerofvenom::daggerofvenom () {
  Enable();
}


void daggerofvenom::Be () {
  meleeweapon::Be();
  //
  if (Exists() && !IsBroken() && Slot[0] && Slot[0]->IsGearSlot() && !RAND_N(10)) {
    fluidvector FluidVector;
    uInt Volume = 0;
    //
    FillFluidVector(FluidVector);
    for (uInt c = 0; c < FluidVector.size(); ++c) {
      liquid *L = FluidVector[c]->GetLiquid();
      //
      Volume += L->GetVolume(); // I imagine that there is a function I don't know to do this...
    }
    if (Volume < 90) SpillFluid(0, liquid::Spawn(POISON_LIQUID, 10));
  }
}


#endif
