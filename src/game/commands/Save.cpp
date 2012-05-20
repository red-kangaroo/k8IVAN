COMMAND(Save) {
  if (game::TruthQuestion(CONST_S("Do you truly wish to save and flee? [y/N]"))) {
    game::Save();
    if (game::WizardModeIsActive()) {
      if (!game::TruthQuestion(CONST_S("Do you want to exit, cheater? [y/N]"))) return false;
    }
    game::End("", false);
    return true;
  }
  return false;
};
