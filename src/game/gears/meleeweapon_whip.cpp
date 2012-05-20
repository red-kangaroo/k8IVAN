#ifdef HEADER_PHASE
ITEM(whip, meleeweapon)
{
 public:
  virtual truth IsWhip() const { return true; }
 protected:
  virtual int GetFormModifier() const;
};


#else



int whip::GetFormModifier() const
{
  return item::GetFormModifier() * GetMainMaterial()->GetFlexibility();
}
#endif
