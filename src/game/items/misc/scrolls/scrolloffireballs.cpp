#ifdef HEADER_PHASE
ITEM(scrolloffireballs, scroll)
{
public:
  virtual void FinishReading (character *);
  virtual truth IsExplosive () const { return true; }
};


#else


void scrolloffireballs::FinishReading (character *Reader) {
  v2 FireBallPos = ERROR_V2;
  //
  beamdata Beam(
    Reader,
    CONST_S("killed by the spells of ") + Reader->GetName(INDEFINITE),
    YOURSELF,
    0
  );
  ADD_MESSAGE("This could be loud...");
  //
  v2 Input = game::PositionQuestion(CONST_S("Where do you wish to send the fireball? [direction keys move cursor, space accepts]"), Reader->GetPos(), &game::TeleportHandler, 0, false);
  //
  if (Input == ERROR_V2) {
    // esc pressed
    ADD_MESSAGE("You choose not to summon a fireball... phew!");
    return;
  }
  //
  lsquare *Square = GetNearLSquare(Input);
  //
  if ((Input - Reader->GetPos()).GetLengthSquare() <= Reader->GetTeleportRangeSquare()) {
    if (!Reader->IsFreeForMe(Square) || (Input == Reader->GetPos())) {
      //Reader->EditExperience(INTELLIGENCE, 100, 1 << 10);
      FireBallPos = Input;
    } else {
      ADD_MESSAGE("The spell must target creatures, %s.", game::Insult());
      //FireBallPos = Reader->GetPos(); /*Better not. It's a little bit wtf for the player to understand*/
      return;
    }
  } else {
    ADD_MESSAGE("You cannot concentrate yourself enough to send a fireball that far.");
    //FireBallPos = ERROR_V2;
    //if (Reader->GetNearestEnemy()) FireBallPos = Reader->GetNearestEnemy()->GetPos(); else
    if (!(RAND()%3)) {
      ADD_MESSAGE("Something very wrong happens with the spell.");
      FireBallPos = Reader->GetPos();
      Square = GetNearLSquare(Reader->GetPos());
    } else {
      return;
    }
  }
  //
  if (FireBallPos == ERROR_V2) Square = GetNearLSquare(GetLevel()->GetRandomSquare(Reader));
  if (Square->GetPos() == Reader->GetPos()) {
    ADD_MESSAGE("The scroll explodes in your face!");
    //Square->DrawParticles(RED);
    //Square->FireBall(Beam);
    //if(Reader->GetAction() && Reader->GetAction()->IsVoluntary())
    //  Reader->GetAction()->Terminate(false);
    //RemoveFromSlot();
    //SendToHell();
    //Reader->EditExperience(INTELLIGENCE, 150, 1 << 12);
    //return;
  } else { //(FireBallPos != ERROR_V2)
    ADD_MESSAGE("A mighty fireball is called into existence.");
  }
  RemoveFromSlot();
  SendToHell();
  Reader->EditExperience(INTELLIGENCE, 150, 1 << 12);
  Square->DrawParticles(RED);
  Square->FireBall(Beam);
}


#endif
