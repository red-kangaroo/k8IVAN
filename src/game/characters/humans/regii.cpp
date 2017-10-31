#ifdef HEADER_PHASE
CHARACTER(regii, humanoid)
{
public:
  virtual void GetAICommand () { StandIdleAI(); }
  virtual void BeTalkedTo ();
  //virtual void DisplayStethoscopeInfo (character*) const;
};


#else


void regii::BeTalkedTo () {
  static sLong Said;
  //
  if (GetRelation(PLAYER) == HOSTILE) {
    ProcessAndAddMessage(GetHostileReplies()[RandomizeReply(Said, GetHostileReplies().Size)]);
  }
  if (!game::GetRegiiTalkState()) {
    game::TextScreen(CONST_S(
      "\"I am Regii, of the Unite Tweraif Forest Army, or UTFA for short. And you are?\"\n"
      "\n"
      "You tell Regii your name and explain that you have escaped from New Attnam.\n"));
    game::TextScreen(CONST_S("\"Do you really hate the imperialists?\"\n"));
    game::TextScreen(CONST_S("\"A lot\" You reply.\n\n"));
    game::TextScreen(CONST_S(
      "\"Alright then. From now on, you shall be called\n'")+PLAYER->GetPanelName().CStr()+CONST_S("'\n"
      " that is called '")+PLAYER->GetPanelName().CStr()+CONST_S("'.\"\n"
      "Regii saluts you after the fashion of the UTFA.\n\n\n"
      "\"Now, listen up! We have an upcoming mission, we will be launching a\n"
      "reconaissance to free some prisoners held in the forest prison.\n"
      "The prison is operated by goblins and guarded by manifold beasts and\n"
      "orcs. They have no political reason to operate the prison, other than\n"
      "the fact that it spins straw into gold.\n"
      "We have reason to believe that the imperialists pay the goblins to\n"
      "imprison our brothers and sisters of the formerly independant Tweraif.\""));
    game::TextScreen(CONST_S(
      "\"Now the mission is to free the prisoners, but, our intelligence suspects\n"
      "that important constitutional documents are held within the prison also.\n"
      "If it is within our power to do so, these should be retrieved and safeguarded.\n"
      "The assualt on the prison will be achieved by means of a haphazard underground\n"
      "tunnel whose entry point is located beyond the river. The tunnel terminates\n"
      "more or less underneath the prison, so that is where the assault will begin.\n"
      "Free the prisoners, arm them with weapons or otherwise, and fight your way out\n"
      "of the vicinity of the prison. Members will regroup here after the mission\n"
      "for a de-brief.\""));
    GetArea()->SendNewDrawRequest();
    game::SetRegiiTalkState(1);
    return;
  } else {
    //ADD_MESSAGE("\"How goes your preparation for the mission brother ") + PLAYER->GetPanelName().CStr() + CONST_S("?\"");
    ProcessAndAddMessage(GetFriendlyReplies()[RandomizeReply(Said, GetFriendlyReplies().Size)]);
  }
}


#endif
