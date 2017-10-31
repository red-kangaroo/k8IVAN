#ifdef HEADER_PHASE
ITEM(scrollofbodyswitch, scroll)
{
 public:
  virtual void FinishReading(character*);
};


#else


void scrollofbodyswitch::FinishReading(character* Reader)
{
  int Dir = game::DirectionQuestion(CONST_S("Choose a creature to possess. [press a direction key]"), false);

  if(Dir == DIR_ERROR || !Reader->GetArea()->IsValidPos(Reader->GetPos() + game::GetMoveVector(Dir)))
  {
    ADD_MESSAGE("You fumble with the magical gestures. Nothing happens.");
    return;
  }

  character* ToPossess = Reader->GetNearLSquare(Reader->GetPos() + game::GetMoveVector(Dir))->GetCharacter();

  if(ToPossess)
  {
    if(ToPossess->CanTameWithScroll(Reader))
    {
      ToPossess->ChangeTeam(Reader->GetTeam());
      Reader->RemoveFlags(C_PLAYER);
      game::SetPlayer(ToPossess);
      ToPossess->EditExperience(INTELLIGENCE, 150, 1 << 12);

      ADD_MESSAGE("Everything goes dark as you are torn from your body and dragged screaming "
                  "soundlessly through the endless Void beyond all worlds. Your soul weakens "
                  "and starts to fray, piece by piece dissolving into nothingness. Then suddenly, "
                  "you stand beside yourself.");
    }
    else
    {
      ADD_MESSAGE("Your mind is not strong enough for the transfer! The scroll turns to dust.");
    }
    RemoveFromSlot();
    SendToHell();
  }
  else
  {
    ADD_MESSAGE("There's no one to possess, %s!", game::Insult());
    return;
  }
}


#endif
