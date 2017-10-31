#ifdef HEADER_PHASE
CHARACTER(tourist, humanoid)
{
 public:
  tourist () : Said(0) {}
  virtual void BeTalkedTo ();
  virtual character* GetLeader() const;
 protected:
  virtual void GetAICommand();
 private:
  sLong Said;
};


#else


character *tourist::GetLeader () const {
  character *Guide = game::GetTeam(TOURIST_GUIDE_TEAM)->GetLeader();
  return (Guide && Guide->GetRelation(this) != HOSTILE ? Guide : GetTeam()->GetLeader());
}


void tourist::GetAICommand () {
  if (game::IsSumoWrestling() && !(RAND()%10)) {
    if (GetConfig() == HUSBAND) {
      if (RAND()&1)
        ADD_MESSAGE("%s shouts: \"Show that skinny wimp what you've got, Huang!\"", CHAR_DESCRIPTION(DEFINITE));
      else
        ADD_MESSAGE("%s screams: \"Go for it, Huang!\"", CHAR_DESCRIPTION(DEFINITE));
    } else if (GetConfig() == WIFE) {
      if (RAND()&1)
        ADD_MESSAGE("%s encourages you: \"Knock him out, %s!\"", CHAR_DESCRIPTION(DEFINITE), game::GetPlayerName().CStr());
      else
        ADD_MESSAGE("%s cheers you: \"A handsome guy like you can't lose to that banana ball!\"", CHAR_DESCRIPTION(DEFINITE));
    } else if (GetConfig() == CHILD) {
      if (RAND()&1)
        ADD_MESSAGE("%s yells: \"More blood on the ring!!!\"", CHAR_DESCRIPTION(DEFINITE));
      else
        ADD_MESSAGE("%s cries: \"Kill him, Pong!!!\"", CHAR_DESCRIPTION(DEFINITE));
    }
  }

  humanoid::GetAICommand();
}


void tourist::BeTalkedTo () {
  if (GetConfig() == CHILD) {
    character *Spider = 0;

    // check all enabled members of PLAYER_TEAM
    truth isLob = false;
    for (auto &tm : game::GetTeam(PLAYER_TEAM)->GetMember()) {
      if (tm->IsEnabled() && !tm->IsPlayer() && tm->IsSpider()) {
        if (tm->GetNameSingular() == "behemoth spider") {
          // lobh-se found
          Spider = tm;
          isLob = true;
          break;
        } else if (tm->GetConfig() == LARGE || tm->GetConfig() == GIANT) {
          Spider = tm;
        }
      }
    }

    if (GetRelation(PLAYER) == HOSTILE) { ADD_MESSAGE("\"Daddy!!! Hit this man!!! He teases me!!!\""); return; }

    // implement truthquestion + proper dialogue  // quest fulfilled
    if (Spider && !game::ChildTouristHasSpider()) {
      ADD_MESSAGE("\"Wow, what a cool spider!!! Can I have it mister? Can I?\"");
      festring GiveSpider = CONST_S("Will you give ")+Spider->CHAR_NAME(DEFINITE)+CONST_S(" to ")+CHAR_NAME(DEFINITE)+CONST_S("?");
      if (game::TruthQuestion(GiveSpider)) {
        ADD_MESSAGE("\"Thanks a lot mister!!! Here, you can have this.\"");
        item *Reward = 0; // create gift item
        // lobh-se?
        if (isLob) {
          Reward = scrollofwishing::Spawn();
        } else {
          // other spider
          Reward = stick::Spawn();
          Reward->InitMaterials(MAKE_MATERIAL(BALSA_WOOD)); // balsa stick
        }
        PLAYER->GetStack()->AddItem(Reward); // add gift to player's inventory
        ADD_MESSAGE("%s hands you %s.", CHAR_NAME(DEFINITE), Reward->CHAR_NAME(INDEFINITE));
        team *Team = game::GetTeam(TOURIST_TEAM);
        Spider->ChangeTeam(Team); // change spider to tourist team
        game::SetTouristHasSpider(); // sets game::TouristHasSpider to true
      } else {
        ADD_MESSAGE("\"Aw... you're no fun!!!\"");
      }
    } else if (!Spider && !game::ChildTouristHasSpider()) {
      // kid does not have spider; normal chat
      ProcessAndAddMessage(GetFriendlyReplies()[RandomizeReply(Said, GetFriendlyReplies().Size)]);
    } else if (game::ChildTouristHasSpider() && !(RAND()%4)) {
      ADD_MESSAGE("\"My friends back home will be so jealous of my new pet spider!!!\"");
    } else {
      // kid has spider; normal chat (no spider request)
      ProcessAndAddMessage(GetFriendlyReplies()[RandomizeReply(Said, GetFriendlyReplies().Size - 1)]);
    }
  } else {
    // not child tourist; normal chat
    character::BeTalkedTo();
  }
}


#endif
