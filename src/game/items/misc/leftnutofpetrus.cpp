#ifdef HEADER_PHASE
ITEM(leftnutofpetrus, nut)
{
 public:
  virtual void Be() { }
  virtual truth IsPetrussNut() const { return true; }
  virtual truth IsConsumable() const { return false; }
};
#endif
