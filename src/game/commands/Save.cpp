COMMAND(Save) {
  if (game::TruthQuestion(CONST_S("Do you truly wish to save and flee?"))) {
    game::Save();
    if (game::WizardModeIsActive()) {
      if (!game::TruthQuestion(CONST_S("Do you want to exit, cheater?"))) return false;
    }
    game::End("", false);
    return true;
  }
  return false;
};
