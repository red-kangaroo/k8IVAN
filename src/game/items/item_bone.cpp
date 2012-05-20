#ifdef HEADER_PHASE
ITEM(bone, item)
{
public:
  virtual truth DogWillCatchAndConsume(ccharacter*) const;
  virtual truth isBone () const { return true; }
  virtual truth IsBodyPart () const { return true; }
};


#else



truth bone::DogWillCatchAndConsume(ccharacter* Doggie) const
{
  return GetConsumeMaterial(Doggie)->GetConfig() == BONE
              && !GetConsumeMaterial(Doggie)->GetSpoilLevel();
}
#endif
