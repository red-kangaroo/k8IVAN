COMMAND(WizardMode) {
  if (!game::WizardModeIsActive()) {
    if (game::TruthQuestion(CONST_S("Do you want to cheat, cheater? This action cannot be undone."))) {
      game::ActivateWizardMode();
      ADD_MESSAGE("Wizard mode activated.");
      game::RevealPOI(game::elpuricavePOI());
      //game::RevealPOI(game::underwatertunnelexitPOI());
      game::ScheduleImmediateSave();
    }
    return false;
  }
  // wizard mode
  if (game::GetPlayerName() == "_k8_" && game::TruthQuestion(CONST_S("Do you want to uncheat, cheater?"))) {
    game::DeactivateWizardMode();
    ADD_MESSAGE("You are the ordinary mortal again.");
  } else {
    if (PLAYER->GetVirtualHead()) {
      ADD_MESSAGE("spillfluid.");
      //PLAYER->GetVirtualHead()->SpillFluid(0, liquid::Spawn(GetBloodMaterial(), 1000));
      PLAYER->GetVirtualHead()->SpillBlood(1000);
    }
  }
  return false;
};
