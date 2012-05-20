#ifdef HEADER_PHASE
CHARACTER(smith, humanoid)
{
 public:
  virtual void BeTalkedTo();
  virtual void GetAICommand() { StandIdleAI(); }
};


#else



void smith::BeTalkedTo()
{
  if(GetRelation(PLAYER) == HOSTILE)
  {
    ADD_MESSAGE("\"You talkin' to me? You talkin' to me? You talkin' to me? Then who the hell else are you talkin' to? You talkin' to me? Well I'm the only one here. Who do you think you're talking to? Oh yeah? Huh? Ok.\"");
    return;
  }

  if(!GetMainWielded() || !GetMainWielded()->CanBeUsedBySmith())
  {
    ADD_MESSAGE("\"Sorry, I need an intact hammer to practise the art of smithing.\"");
    return;
  }

  if(PLAYER->PossessesItem(&item::IsFixableBySmith))
  {
    item* Item = PLAYER->SelectFromPossessions(CONST_S("\"What do you want me to fix?\""), &item::IsFixableBySmith);

    if(!Item)
      return;

    if(!(Item->GetMainMaterial()->GetCategoryFlags() & IS_METAL))
    {
      ADD_MESSAGE("\"I only fix items made of metal.\"");
      return;
    }

    /** update messages */

    sLong FixPrice = Item->GetFixPrice();

    if(PLAYER->GetMoney() < FixPrice)
    {
      ADD_MESSAGE("\"Getting that fixed costs you %d gold pieces. Get the money and we'll talk.\"", FixPrice);
      return;
    }

    ADD_MESSAGE("\"I can fix your %s, but it'll cost you %d gold pieces.\"", Item->CHAR_NAME(UNARTICLED), FixPrice);

    if(game::TruthQuestion(CONST_S("Do you accept this deal? [y/N]")))
    {
      Item->RemoveRust();
      Item->Fix();
      PLAYER->EditMoney(-FixPrice);
      ADD_MESSAGE("%s fixes %s in no time.", CHAR_NAME(DEFINITE), Item->CHAR_NAME(DEFINITE));
    }
  }
  else
    ADD_MESSAGE("\"Come back when you have some weapons or armor I can fix.\"");
}
#endif
