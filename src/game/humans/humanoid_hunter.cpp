#ifdef HEADER_PHASE
CHARACTER(hunter, humanoid)
{
 public:
  virtual void BeTalkedTo();
 protected:
  virtual void CreateBodyParts(int);
};


#else



void hunter::BeTalkedTo()
{
  if(GetRelation(PLAYER) != HOSTILE && GetMainWielded() && !(RAND() % 10))
    ADD_MESSAGE("\"This is my %s. There are many like it but this one is mine. My %s is my best friend.\"", GetMainWielded()->CHAR_NAME(UNARTICLED), GetMainWielded()->CHAR_NAME(UNARTICLED));
  else
    character::BeTalkedTo();
}



void hunter::CreateBodyParts(int SpecialFlags)
{
  for(int c = 0; c < BodyParts; ++c)
    if(c != LEFT_ARM_INDEX)
      CreateBodyPart(c, SpecialFlags);
    else
      SetBodyPart(LEFT_ARM_INDEX, 0);
}
#endif
