COMMAND(WizardBlow) {
  if (!game::Wish(PLAYER, "%s appears from nothing!", "Two %s appear from nothing!", true)) {
    ADD_MESSAGE("You changed your mind, didn't you, %s?", game::Insult());
  }
  return false;
};
