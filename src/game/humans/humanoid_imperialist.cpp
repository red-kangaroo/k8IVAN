#ifdef HEADER_PHASE
CHARACTER(imperialist, humanoid)
{
public:
  virtual void GetAICommand () { StandIdleAI(); }
  virtual void BeTalkedTo ();
  virtual void DisplayStethoscopeInfo (character *) const;
protected:
  virtual void CreateCorpse (lsquare *);
};


#else



void imperialist::BeTalkedTo () {
  decosadshirt *Shirt = static_cast<decosadshirt *>(PLAYER->SearchForItem(this, &item::IsDecosAdShirt));
  if (Shirt) {
    feuLong Reward = Shirt->GetEquippedTicks()/500;
    if (Reward) {
      ADD_MESSAGE("%s smiles. \"I see you have advertised our company diligently. Here's %dgp as a token of my gratitude.\"", CHAR_NAME(DEFINITE), Reward);
      PLAYER->EditMoney(Reward);
      Shirt->SetEquippedTicks(0);
    } else if (!(RAND()%5)) {
      ADD_MESSAGE("\"Come back when you've worn the shirt for some time and I'll reward you generously!\"");
    }
    return;
  }
  static sLong Said;
  if (GetRelation(PLAYER) == HOSTILE)
    ProcessAndAddMessage(GetHostileReplies()[RandomizeReply(Said, GetHostileReplies().Size)]);
  else if (!game::PlayerIsSumoChampion())
    ProcessAndAddMessage(GetFriendlyReplies()[RandomizeReply(Said, GetFriendlyReplies().Size)]);
  else
    ProcessAndAddMessage(GetFriendlyReplies()[RandomizeReply(Said, GetFriendlyReplies().Size-1)]);
}



void imperialist::DisplayStethoscopeInfo (character *) const {
  ADD_MESSAGE("You hear coins clinking inside.");
}



void imperialist::CreateCorpse (lsquare *Square) {
  if (!game::GetLiberator()) {
    game::SetLiberator(1);
    ADD_MESSAGE("You liberate citizens of New Attnam!");
  }
  imperialistsysbase::CreateCorpse(Square);
}
#endif
