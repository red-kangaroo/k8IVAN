#ifdef HEADER_PHASE
ITEM(carrot, item)
{
 public:
  virtual truth BunnyWillCatchAndConsume(ccharacter*) const;
 protected:
  virtual col16 GetMaterialColorB(int) const;
};


#else



col16 carrot::GetMaterialColorB(int) const { return MakeRGB16(80, 100, 16); }



truth carrot::BunnyWillCatchAndConsume(ccharacter* Bunny) const
{
  return GetConsumeMaterial(Bunny)->GetConfig() == CARROT_FLESH
             && !GetConsumeMaterial(Bunny)->GetSpoilLevel();
}
#endif
