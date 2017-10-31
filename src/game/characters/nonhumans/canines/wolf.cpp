#ifdef HEADER_PHASE
CHARACTER(wolf, canine)
{
 protected:
  virtual col16 GetSkinColor() const;
};


#else



col16 wolf::GetSkinColor() const
{
  int Element = 40 + RAND() % 50;
  return MakeRGB16(Element, Element, Element);
}
#endif
