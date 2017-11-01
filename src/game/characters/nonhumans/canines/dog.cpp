#ifdef HEADER_PHASE
CHARACTER(dog, canine)
{
 public:
  virtual truth Catches(item*);
  virtual void BeTalkedTo();
 protected:
  virtual bodypart* MakeBodyPart(int) const;
  virtual void GetAICommand();
};


#else



bodypart* dog::MakeBodyPart(int) const { return dogtorso::Spawn(0, NO_MATERIALS); }



truth dog::Catches (item *Thingy) {
  if (Thingy->DogWillCatchAndConsume(this)) {
    if (ConsumeItem(Thingy, CONST_S("eating"))) {
      if (IsPlayer()) {
        ADD_MESSAGE("You catch %s in mid-air and consume it.", Thingy->CHAR_NAME(DEFINITE));
      } else {
        if (CanBeSeenByPlayer()) ADD_MESSAGE("%s catches %s and eats it.", CHAR_NAME(DEFINITE), Thingy->CHAR_NAME(DEFINITE));
        ChangeTeam(PLAYER->GetTeam());
      }
    } else if (IsPlayer()) {
      ADD_MESSAGE("You catch %s in mid-air.", Thingy->CHAR_NAME(DEFINITE));
    } else if (CanBeSeenByPlayer()) {
      ADD_MESSAGE("%s catches %s.", CHAR_NAME(DEFINITE), Thingy->CHAR_NAME(DEFINITE));
    }
    return true;
  }
  return false;
}



void dog::BeTalkedTo () {
  if (RAND_N(5)) {
    if (GetRelation(PLAYER) != HOSTILE) {
      static truth Last;
      cchar *Reply;
      if (GetHP()<< 1 > GetMaxHP()) Reply = Last ? "barks happily" : "wags its tail happily";
      else Reply = Last ? "yelps" : "howls";
      ADD_MESSAGE("%s %s.", CHAR_NAME(DEFINITE), Reply);
      Last = !Last;
    } else {
      character::BeTalkedTo();
    }
  } else if (RAND_N(5)) {
    ADD_MESSAGE("\"Can't you understand I can't speak?\"");
  } else {
    ADD_MESSAGE("\"Meow.\"");
  }
}



void dog::GetAICommand () {
  if (!game::IsInWilderness() && !(RAND()&7)) {
    GetLSquareUnder()->SpillFluid(this, liquid::Spawn(DOG_DROOL, 25+RAND()%50), false, false);
  }
  character::GetAICommand();
}
#endif
