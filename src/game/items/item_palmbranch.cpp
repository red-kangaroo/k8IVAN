#ifdef HEADER_PHASE
ITEM(palmbranch, item)
{
 public:
  virtual truth IsShield(ccharacter*) const { return true; }
};
#endif
