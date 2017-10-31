#ifdef HEADER_PHASE
ITEM(lump, item)
{
 protected:
  virtual void AddPostFix(festring& String, int) const { AddLumpyPostFix(String); }
  virtual truth ShowMaterial() const { return false; }
  virtual truth WeightIsIrrelevant() const { return true; }
};
#endif
