#ifdef HEADER_PHASE
CHARACTER(librarian, humanoid)
{
 public:
  virtual void BeTalkedTo();
 protected:
  virtual void GetAICommand() { StandIdleAI(); }
};


#else



void librarian::BeTalkedTo()
{
  if(GetRelation(PLAYER) == HOSTILE)
  {
    ADD_MESSAGE("\"The pen is mightier than the sword! Fall, unlearned one!\"");
    return;
  }

  static sLong Said;

  switch(RandomizeReply(Said, 12))
  {
   case 0:
    if(game::GetPetrus() && !game::GetStoryState())
      ADD_MESSAGE("\"Thou shouldst visit Petrus if thou art in need of further adventures.\"");
    else
      ADD_MESSAGE("\"They say a wand of polymorph hath dozens of uses.\"");

    break;
   case 1:
    if(game::GetPetrus() && game::GetStoryState() == 1)
      ADD_MESSAGE("\"Thou art going to fight Elpuri? Beware! It is a powerful enemy. Other monsters are very vulnerable if surrounded by thy party, but not that beast, for it may slay a horde of thy friends at once with its horrendous tail attack.\"");
    else
      ADD_MESSAGE("\"Thou shalt remember: Scientia est potentia.\"");

    break;
   case 2:
    if(game::GetPetrus() && game::GetStoryState() == 1)
      ADD_MESSAGE("\"Elpuri the Dark Frog abhors light and resides in a level of eternal darkness.\"");
    else
      ADD_MESSAGE("\"Shh! Thou shalt be silent in the library.\"");

    break;
   case 3:
    if(game::GetPetrus() && game::GetStoryState() == 1)
      ADD_MESSAGE("\"Elpuri's attacks are so strong that they may shatter many of thy precious items.\"");
    else
      ADD_MESSAGE("\"Dost thou not smell all the knowledge floating around here?\"");

    break;
   case 4:
    ADD_MESSAGE("\"It is said that Loricatus, the god of smithing, can upgrade thy weapons' materials.\"");
    break;
   case 5:
    if(game::GetPetrus() && game::GetStoryState() == 1)
      ADD_MESSAGE("\"The Shirt of the Golden Eagle is a legendary artifact. Thou canst not find a better armor.\"");
    else
      ADD_MESSAGE("\"In this book they talk about Mortifer, the great chaos god. He hates us mortals more than anything and will respond only to Champions of Evil.\"");

    break;
   case 6:
    ADD_MESSAGE("\"Attnam is traditionally ruled by the high priest of the Great Frog. He holds the Shirt of the Golden Eagle and has always killed his predecessor.\"");
    break;
   case 7:
    ADD_MESSAGE("\"They say thou shouldst keep all the artifacts thou findst. They shall make thee famous after thy retirement.\"");
    break;
   case 8:
    ADD_MESSAGE("\"If thou wilt ever encounter an enner beast, know this: It is a horrible foe. It may shatter thy items and armor with its scream that penetrates iron and stone. Thou shouldst not engage it in melee but rather kill it from afar.\"");
    break;
   case 9:
    if(game::GetPetrus() && game::GetStoryState() == 1)
      ADD_MESSAGE("\"Thou art not alone in thy attempt to defeat Elpuri. A brave adventurer called Ivan also diveth into its cave not long ago.\"");
    else
      ADD_MESSAGE("\"It is said that chaotic gods offer great power to their followers. But thou must remember: unlike lawfuls, they shall not help thee when things go bad.\"");

    break;
   case 10:
    ADD_MESSAGE("\"If a man cannot choose, he ceases to be a man.\"");
    break;
   case 11:
    ADD_MESSAGE("%s sighs: \"The censorship laws in this town are really too strict...\"", CHAR_DESCRIPTION(DEFINITE));
    break;
  }
}
#endif
