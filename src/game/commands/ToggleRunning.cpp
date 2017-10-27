COMMAND(ToggleRunning) {
  if (game::PlayerIsRunning() && PLAYER->StateIsActivated(PANIC) && PLAYER->GetTirednessState() != FAINTING) {
    ADD_MESSAGE("You are too scared to move at a normal pace.");
    return false;
  }
  if (!Char->CanMove()) {
    ADD_MESSAGE("Well, well, aren't we funny today, eh?");
    return false;
  }
  game::SetPlayerIsRunning(!game::PlayerIsRunning());
  return false;
};
