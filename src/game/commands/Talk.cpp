COMMAND(Talk) {
  if (!Char->CheckTalk()) return false;
  character *ToTalk = 0;
  int Characters = 0;
  for (int d = 0; d < 8; ++d) {
    lsquare *Square = Char->GetNaturalNeighbourLSquare(d);
    if (Square) {
      character *Dude = Square->GetCharacter();
      if (Dude) {
        ToTalk = Dude;
        ++Characters;
      }
    }
  }
  if (!Characters) {
    ADD_MESSAGE("Find yourself someone to talk to first!");
    return false;
  }
  if (Characters == 1) return ToTalk->ChatMenu();
  int Dir = game::DirectionQuestion(CONST_S("To whom do you wish to talk to? [press a direction key]"), false, true);
  if (Dir == DIR_ERROR || !Char->GetArea()->IsValidPos(Char->GetPos()+game::GetMoveVector(Dir))) return false;
  character *Dude = Char->GetNearSquare(Char->GetPos()+game::GetMoveVector(Dir))->GetCharacter();
  if (Dude == Char) {
    ADD_MESSAGE("You talk to yourself for some time.");
    Char->EditExperience(WISDOM, -50, 1<<7);
    Char->EditAP(-1000);
    return true;
  }
  if (Dude) return Dude->ChatMenu();
  ADD_MESSAGE("You get no response.");
  return false;
};
