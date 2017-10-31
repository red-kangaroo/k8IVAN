#ifdef HEADER_PHASE
CHARACTER(raven, humanoid)
{
public:
  virtual void BeTalkedTo();
protected:
  virtual void GetAICommand();
};


#else



void raven::GetAICommand () {
  int Enemies = 0;
  for (int c = 0; c < game::GetTeams(); ++c) {
    if (GetTeam()->GetRelation(game::GetTeam(c)) == HOSTILE) Enemies += game::GetTeam(c)->GetEnabledMembers();
  }
  StandIdleAI();
}



void raven::BeTalkedTo () {
  if (GetRelation(PLAYER) == HOSTILE) {
    ADD_MESSAGE("I bet you are with those filthy Attnamanese bastards!");
    return;
  }
  if (!game::GetRingOfThieves()) {
    if (PLAYER->RemoveRingOfThieves()) {
      game::TextScreen(CONST_S(
        "You hand over the Ring of Thieves, Raven's eyes glow in amazement.\n"
        "\"May Cleptia bless this mighty warrior, whom vanquished Vulcan-Loki!\n"
        "Retrieving the ring is no easy task, Vulcan is a powerful adversary.\n"
        "The importance of the Ring of Thieves is great. It is used to channel\n"
        "Cleptia's power to help Mondedr avoid it's enemies, including Petrus.\n"
        "Without it we would have been raided by every nation that hates us.\n"
        "Not a mere mortal like Vulcan-Loki could channel the ring's power for\n"
        "himself so he decided to contract a deal with Oree to assist him.\n"
        "To reward your efforts I will give you the artifact whip, Gleipnir.\"\n\n"
        "\"I pray to Cleptia that you will use it well..\"\n\n"""));
      game::GetGod(CLEPTIA)->AdjustRelation(500);
      game::GetGod(NEFAS)->AdjustRelation(50);
      game::GetGod(SCABIES)->AdjustRelation(50);
      game::GetGod(INFUSCOR)->AdjustRelation(50);
      game::GetGod(CRUENTUS)->AdjustRelation(50);
      game::GetGod(MORTIFER)->AdjustRelation(50);
      meleeweapon *Weapon = whipofthievery::Spawn();
      Weapon->InitMaterials(MAKE_MATERIAL(SPIDER_SILK), MAKE_MATERIAL(EBONY_WOOD), true);
      PLAYER->GetGiftStack()->AddItem(Weapon);
      GetArea()->SendNewDrawRequest();
      ADD_MESSAGE("A whip materializes near your feet.");
      game::SetRingOfThieves(1);
    } else {
      ADD_MESSAGE("\"Mondedr's most important artifact, the Ring of Thieves, has been stolen by Vulcan-Loki; residing at the deepest floor of the underground temple, he awaits Oree to receive it and reward him with incredible power.\"");
    }
  } else {
    /* StoryState == 100 */
    ADD_MESSAGE("\"I must thank you again, I hope you make good use of that whip.\"");
  }
}
#endif
