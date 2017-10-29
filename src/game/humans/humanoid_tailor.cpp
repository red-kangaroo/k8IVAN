#ifdef HEADER_PHASE
CHARACTER(tailor, humanoid)
{
 public:
  virtual void BeTalkedTo();
  virtual void GetAICommand() { StandIdleAI(); }
};


#else



void tailor::BeTalkedTo()
{
  if(GetRelation(PLAYER) == HOSTILE)
  {
    ADD_MESSAGE("\"You talkin' to me? You talkin' to me? You talkin' to me? Then who the hell else are you talkin' to? You talkin' to me? Well I'm the only one here. Who do you think you're talking to? Oh yeah? Huh? Ok.\"");
    return;
  }

  if(PLAYER->PossessesItem(&item::IsFixableByTailor))
  {
    item* Item = PLAYER->SelectFromPossessions(CONST_S("\"What do you want me to fix?\""), &item::IsFixableByTailor);

    if(!Item)
      return;

    if(!(Item->GetMainMaterial()->GetCategoryFlags() & CAN_BE_TAILORED))
    {
      ADD_MESSAGE("\"I can't work on %s.\"", Item->GetMainMaterial()->GetName(false, false).CStr());
      return;
    }

    /** update messages */

    sLong FixPrice = Item->GetFixPrice();

    if(PLAYER->GetMoney() < FixPrice)
    {
      ADD_MESSAGE("\"Getting that fixed costs you \1Y%d\2 gold pieces. Get the money and we'll talk.\"", FixPrice);
      return;
    }

    ADD_MESSAGE("\"I can fix your \1Y%s\2, but it'll cost you %d gold pieces.\"", Item->CHAR_NAME(UNARTICLED), FixPrice);

    if(game::TruthQuestion(CONST_S("Do you accept this deal?")))
    {
      Item->Fix();
      PLAYER->EditMoney(-FixPrice);
      ADD_MESSAGE("%s fixes %s in no time.", CHAR_NAME(DEFINITE), Item->CHAR_NAME(DEFINITE));
    }
  }
  else
    ADD_MESSAGE("\"Come back when you have some weapons or armor I can fix.\"");
}
#endif
