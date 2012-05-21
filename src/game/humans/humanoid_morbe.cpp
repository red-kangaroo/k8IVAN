#ifdef HEADER_PHASE
CHARACTER(morbe, humanoid)
{
public:
  virtual void BeTalkedTo ();
  //virtual void FinalProcessForBone ();
  //virtual truth MoveTowardsHomePos (); // may need this in times to come

protected:
  virtual void CreateCorpse (lsquare *);
  virtual void GetAICommand ();

protected:
  //ulong LastHealed;
};


#else


void morbe::GetAICommand () {
  StandIdleAI();
}


void morbe::CreateCorpse (lsquare *Square) {
  angel *Angel;
  int Seen = 0;
  //
  for (int c = 0; c < 3; ++c) {
    if (!c) Angel = archangel::Spawn(SCABIES); else Angel = angel::Spawn(SCABIES);
    //
    v2 Where = game::GetCurrentLevel()->GetRandomSquare(Angel);
    //
    Angel->SetTeam(game::GetTeam(4));
    Angel->PutTo(Where);
    if (Angel->CanBeSeenByPlayer()) ++Seen;
  }
  //
  if (Seen == 1) ADD_MESSAGE("%s materializes.", Angel->CHAR_NAME(INDEFINITE));
  else if (Seen == 2) ADD_MESSAGE("Two %s materialize.", Angel->CHAR_NAME(PLURAL));
  else if (Seen == 3) ADD_MESSAGE("Three %s materialize.", Angel->CHAR_NAME(PLURAL));
  ADD_MESSAGE("\"We will defend the Unholy Order!\"");
  SendToHell();
}


void morbe::BeTalkedTo () {
  if (GetRelation(PLAYER) == HOSTILE) {
    ADD_MESSAGE("A plague on all your household!");
    return;
  }
  //
  if (PLAYER->HasCurdledBlood()) {
    if (PLAYER->RemoveCurdledOmmelBlood()) {
      game::TextScreen(CONST_S(
        "You hand over the vial filled with curdled ommel blood. Morbe is delighted.\n"
        "\"You have retrieved the curdled ommel blood!\n"
        "I will share a little of my knowledge with you, as a gift.\""));
      game::TextScreen(CONST_S(
        "Morbe begins to relate a dark secret to you from the depths of her knowledge:\n"
        "\"There is a place not far from here, a place of utter darkness\n"
        "where dwells the soul of the ancient Warlord Xinroch.\n"
        "His bones have long since departed that place, but his soul still dwells beneath.\n"
        "It is stirring as we speak.\""));
      /*
      game::LoadWorldMap();
      v2 XinrochTombPos = game::GetWorldMap()->GetEntryPos(0, XINROCH_TOMB);
      game::GetWorldMap()->GetWSquare(XinrochTombPos)->ChangeOWTerrain(xinrochtomb::Spawn());
      game::GetWorldMap()->RevealEnvironment(XinrochTombPos, 1);
      game::SaveWorldMap();
      */
      GetArea()->SendNewDrawRequest();
      game::SetOmmelBloodMission(2);
      ADD_MESSAGE("Morbe hands you a map revealing the location of the tomb. \"There are untold riches in the Tomb of Xinroch, but beware, his many minions protect him and will kill all who attempt to enter that place!\"");
      //
      maptotombofxinroch *MapToTombOfXinroch = maptotombofxinroch::Spawn();
      //
      MapToTombOfXinroch->MoveTo(PLAYER->GetStack());
      return;
    }
  }
  //
  if (game::GetOmmelBloodMission() != 2) {
    if (!game::GetOmmelBloodMission()) {
      game::TextScreen(CONST_S(
        "You approach the acolyte standing by the cauldron, she speaks to you.\n\n"
        "\"Welcome, ")+PLAYER->GetPanelName().CStr()+CONST_S(".\n"
        "Doubtless I have seen you from afar and by the powers of the goddess,\n"
        "I have drawn you here to me. Can you not feel the presence of the\n"
        "unholy Scabies here in my abode? Indeed it is I, Morbe, high\n"
        "priestess of Scabies, who channels the powers of the goddess\n"
        "into the world above Valpurus!\"\n\n"
        "You feel your stomach churn as you realise where you are, the source of all illness and\n"
        "pestillence in the world. Morbe speaks further: \n\n"));
      game::TextScreen(CONST_S(
        "\"I have brought you here because I have a use for you. I need you to\n"
        "curdle a measure of Ommel Blood with the scream of the Enner Beast!\n"
        "Here is a klein bottle, it may be dipped, but not drunk from. It will\n"
        "not break due to the cry of the Enner Beast. I have filled it with\n"
        "the Ommel blood from this cauldron. Wield it before you as you encounter\n"
        "the enner beast in person; the liquid should change noticably.\n"
        "I promise you a great gift in exchange for this favour. Return\n"
        "to me with this very vial, filled with curdled Ommel blood!\""));
      //
      kleinbottle *KleinBottle = kleinbottle::Spawn();
      //
      KleinBottle->ChangeSecondaryMaterial(MAKE_MATERIAL(OMMEL_BLOOD));
      KleinBottle->MoveTo(PLAYER->GetStack());
      GetArea()->SendNewDrawRequest();
      ADD_MESSAGE("\"Do not fail me, or the goddess will be unhappy!\"");
      game::SetOmmelBloodMission(1);
    } else {
      /* StoryState == 1 */
      ADD_MESSAGE("Morbe says: \"Bring me the vial of curdled Ommel Blood and I'll tell you my secrets.\"");
    }
  } else {
    /* StoryState == 2 */
    ADD_MESSAGE("Morbe says: \"I have all I need from you right now. Be gone!\"");
  }
}


#endif
