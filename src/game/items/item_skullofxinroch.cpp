#ifdef HEADER_PHASE
ITEM(skullofxinroch, item)
{
 public:
  virtual truth IsASkull () const { return true; }
  virtual void Be () {}
  virtual bool SpecialOfferEffect (int);
  virtual truth AllowSpoil () const { return false; }
  virtual truth Spoils () const { return false; }
 protected:
  virtual int GetClassAnimationFrames () const { return 32; }
  virtual col16 GetOutlineColor (int) const;
  virtual alpha GetOutlineAlpha (int) const;
};


#else


col16 skullofxinroch::GetOutlineColor (int) const { return MakeRGB16(180, 0, 0); }


alpha skullofxinroch::GetOutlineAlpha (int Frame) const {
  Frame &= 31;
  return 50+(Frame*(31-Frame)>>1);
}


bool skullofxinroch::SpecialOfferEffect (int GodNumber) {
  if (GodNumber == INFUSCOR) {
    god *Receiver = game::GetGod(GodNumber);
    Receiver->AdjustRelation(500);
    ADD_MESSAGE("You sacrifice %s. %s appreciates your generous offer truly.", CHAR_NAME(DEFINITE), Receiver->GetName());
    return true;
  }
  return false;
}


#endif
