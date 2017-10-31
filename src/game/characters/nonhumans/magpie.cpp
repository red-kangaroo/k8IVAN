#ifdef HEADER_PHASE
CHARACTER(magpie, nonhumanoid)
{
 public:
  virtual void GetAICommand();
  virtual truth IsRetreating() const;
 protected:
  virtual cchar* FirstPersonBiteVerb() const;
  virtual cchar* FirstPersonCriticalBiteVerb() const;
  virtual cchar* ThirdPersonBiteVerb() const;
  virtual cchar* ThirdPersonCriticalBiteVerb() const;
};


#else



cchar* magpie::FirstPersonBiteVerb() const { return "peck"; }



cchar* magpie::FirstPersonCriticalBiteVerb() const { return "critically peck"; }



cchar* magpie::ThirdPersonBiteVerb() const { return "pecks"; }



cchar* magpie::ThirdPersonCriticalBiteVerb() const { return "critically pecks"; }



truth magpie::IsRetreating() const
{
  if(nonhumanoid::IsRetreating())
    return true;

  for(stackiterator i = GetStack()->GetBottom(); i.HasItem(); ++i)
    if((*i)->GetSparkleFlags())
      return true;

  return false;
}



void magpie::GetAICommand()
{
  if(!IsRetreating())
  {
    character* Char = GetRandomNeighbour();

    if(Char)
    {
      itemvector Sparkling;

      for(stackiterator i = Char->GetStack()->GetBottom(); i.HasItem(); ++i)
      {
  if((*i)->GetSparkleFlags() && !MakesBurdened((*i)->GetWeight()))
    Sparkling.push_back(*i);
      }

      if(!Sparkling.empty())
      {
  item* ToSteal = Sparkling[RAND() % Sparkling.size()];
  ToSteal->RemoveFromSlot();
  GetStack()->AddItem(ToSteal);

  if(Char->IsPlayer())
    ADD_MESSAGE("%s steals your %s.", CHAR_NAME(DEFINITE), ToSteal->CHAR_NAME(UNARTICLED));

  EditAP(-500);
  return;
      }
    }
  }

  nonhumanoid::GetAICommand();
}
#endif
