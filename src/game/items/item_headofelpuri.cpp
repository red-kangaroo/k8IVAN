#ifdef HEADER_PHASE
ITEM(headofelpuri, item) // can't wear equipment, so not "head"
{
 public:
  virtual truth IsHeadOfElpuri() const { return true; }
  virtual truth IsConsumable() const { return false; }
  virtual void Be() { }
};
#endif
