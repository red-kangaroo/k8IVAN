#ifdef HEADER_PHASE
ITEM(gorovitscopyoflenin, item)
{
 protected:
  virtual col16 GetMaterialColorB(int) const;
};


#else



col16 gorovitscopyoflenin::GetMaterialColorB(int) const
{
  return MakeRGB16(30, 30, 30);
}
#endif
