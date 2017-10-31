#ifdef HEADER_PHASE
ITEM(stone, item)
{
 public:
  virtual sLong GetTruePrice() const;
  virtual truth IsLuxuryItem(ccharacter*) const { return GetTruePrice() > 0; }
 protected:
  virtual truth WeightIsIrrelevant() const { return true; }
};


#else



sLong stone::GetTruePrice() const { return item::GetTruePrice() << 1; }
#endif
