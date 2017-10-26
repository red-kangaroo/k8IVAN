#ifdef HEADER_PHASE
ITEM(hellsdevice, item) {
public:
  virtual truth IsAppliable (ccharacter *User) const override;
  virtual truth Apply (character *User) override;
  //virtual truth IsZappable (ccharacter *User) const override;
  //virtual truth Zap (character *User, v2 curpos, int Direction) override;

private:
  void explode (character *User, v2 pos);
};


#else

truth hellsdevice::IsAppliable (ccharacter *User) const { return true; }
//truth hellsdevice::IsZappable (ccharacter *User) const { return true; }


truth hellsdevice::Apply (character *User) {
  // cannot be broken
  //if (IsBroken()) { ADD_MESSAGE("%s is totally broken.", CHAR_NAME(DEFINITE)); return false; }

  v2 FireBallPos = ERROR_V2;
  beamdata Beam(
    User,
    CONST_S("killed by the spells of ")+User->GetName(INDEFINITE),
    YOURSELF,
    0
  );
  ADD_MESSAGE("This could be loud...");

  v2 Input = game::PositionQuestion(CONST_S("Where do you wish to send the fireball? [direction keys move cursor, space accepts]"), User->GetPos(), &game::TeleportHandler, 0, false);

  if (Input == ERROR_V2) {
    // esc pressed
    ADD_MESSAGE("You choose not to summon a fireball... phew!");
    return false;
  }

  lsquare *Square = GetNearLSquare(Input);

  /*
  if ((Input-User->GetPos()).GetLengthSquare() <= User->GetTeleportRangeSquare()) {
    if (!User->IsFreeForMe(Square) || (Input == User->GetPos())) {
      //User->EditExperience(INTELLIGENCE, 100, 1 << 10);
      FireBallPos = Input;
    } else {
      ADD_MESSAGE("The spell must target creatures, %s.", game::Insult());
      //FireBallPos = User->GetPos(); // Better not. It's a little bit wtf for the player to understand
      return;
    }
  } else {
    ADD_MESSAGE("You cannot concentrate yourself enough to send a fireball that far.");
    //FireBallPos = ERROR_V2;
    //if (User->GetNearestEnemy()) FireBallPos = User->GetNearestEnemy()->GetPos(); else
    if (!(RAND()%3)) {
      ADD_MESSAGE("Something very wrong happens with the spell.");
      FireBallPos = User->GetPos();
      Square = GetNearLSquare(User->GetPos());
    } else {
      return;
    }
  }
  */

  FireBallPos = Input;

  if (FireBallPos == ERROR_V2) {
    //Square = GetNearLSquare(GetLevel()->GetRandomSquare(User));
    return false;
  }

  if (Square->GetPos() == User->GetPos()) ADD_MESSAGE("The scroll explodes in your face!");

  User->EditExperience(INTELLIGENCE, 150, 1<<12);
  //Square->DrawParticles(RED);
  //!Square->FireBall(Beam);
  //GetLevel()->Explosion(User, Beam.DeathMsg, FireBallPos, /*75+RAND()%25-RAND()%25*/400+RAND()%100);
  explode(User, FireBallPos);

  return true;
}


/*
truth hellsdevice::Zap (character *User, v2 curpos, int Direction) {
  User->EditExperience(PERCEPTION, 150, 1<<10);

  beamdata Beam (
    User,
    CONST_S("killed by ")+GetName(INDEFINITE)+" zapped @bk",
    User->GetPos(),
    YELLOW, // beam color
    BEAM_FIRE_BALL,
    Direction,
    400,
    0//GetSpecialParameters()
  );

  (GetLevel()->*level::GetBeam(GetBeamStyle()))(Beam);

  return true;
}
*/


void hellsdevice::explode (character *User, v2 pos) {
  festring dmsg = CONST_S("killed by the spells of ")+User->GetName(INDEFINITE);
  lsquare *Square = GetNearLSquare(pos);
  if (Square) {
    Square->DrawParticles(RED);
    GetLevel()->Explosion(User, dmsg, pos, /*75+RAND()%25-RAND()%25*/400+RAND()%100);
  }
  for (int f = 0; f < 8; ++f) {
    if (RAND()%100 > 30) {
      v2 dest = pos+game::GetMoveVector(f);
      Square = GetNearLSquare(dest);
      if (Square) {
        Square->DrawParticles(RED);
        GetLevel()->Explosion(User, dmsg, dest, /*75+RAND()%25-RAND()%25*/400+RAND()%100);
      }
    }
  }
}


#endif
