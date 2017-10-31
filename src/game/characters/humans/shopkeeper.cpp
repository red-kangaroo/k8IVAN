#ifdef HEADER_PHASE
CHARACTER(shopkeeper, humanoid)
{
 public:
  shopkeeper();
 protected:
  virtual void GetAICommand() { StandIdleAI(); }
};


#else



shopkeeper::shopkeeper()
{
  if(!game::IsLoading())
    Money = GetMoney() + RAND() % 2001;
}
#endif
