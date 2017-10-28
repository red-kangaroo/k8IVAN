COMMAND(OpenMondedr) {
  if (!game::GetMondedrPass()) {
    game::RevealPOI(game::mondedrPOI());
    game::SetMondedrPass(1);
    game::ScheduleImmediateSave();
    ADD_MESSAGE("Mondedr opened.");
  } else {
    ADD_MESSAGE("Mondedr already opened.");
  }
  return false;
};
