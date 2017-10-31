#ifdef HEADER_PHASE
ITEM(loaf, item)
{
 protected:
  virtual void AddPostFix(festring& String, int) const { AddLumpyPostFix(String); }
  virtual truth ShowMaterial() const { return false; }
};
#endif
