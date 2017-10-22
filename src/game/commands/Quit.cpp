COMMAND(Quit) {
  if (game::TruthQuestion(CONST_S("Your quest is not yet compeleted! Really quit?"))) {
    Char->ShowAdventureInfo();
    festring Msg = CONST_S("cowardly quit the game");
    Char->AddScoreEntry(Msg, 0.75);
    game::End(Msg, !game::WizardModeIsActive() || game::TruthQuestion(CONST_S("Remove saves?")));
    return true;
  }
  return false;
};
