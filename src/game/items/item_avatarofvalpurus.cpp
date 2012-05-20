#ifdef HEADER_PHASE
ITEM(avatarofvalpurus, item)
{
 public:
  virtual void Be() { }
  virtual truth IsTheAvatar() const { return true; }
  virtual truth IsConsumable() const { return false; }
};
#endif
