#ifdef HEADER_PHASE
ITEM(skull, item)
{
  virtual truth IsBodyPart () const { return true; }
  virtual truth IsASkull () const { return true; }
};
#endif
