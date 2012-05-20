#ifdef HEADER_PHASE
CHARACTER(elder, humanoid)
{
 protected:
  virtual void GetAICommand();
  virtual void CreateBodyParts(int);
};


#else



void elder::CreateBodyParts(int SpecialFlags)
{
  for(int c = 0; c < BodyParts; ++c)
    if(c != LEFT_LEG_INDEX)
      CreateBodyPart(c, SpecialFlags);
    else
      SetBodyPart(LEFT_LEG_INDEX, 0);
}



void elder::GetAICommand()
{
  /* Select a place to guide the tourists to */

  if(!(RAND() % 10))
    SetGoingTo(GetLevel()->GetRandomSquare());

  humanoid::GetAICommand();
}
#endif
