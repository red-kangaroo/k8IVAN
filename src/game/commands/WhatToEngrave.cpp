COMMAND(WhatToEngrave) {
  if (!Char->CanRead()) {
    ADD_MESSAGE("You can't even read.");
    return false;
  }
  Char->GetNearLSquare(Char->GetPos())->Engrave(game::StringQuestion(CONST_S("What do you want to engrave here?"), WHITE, 0, 80, true));
  return false;
};
