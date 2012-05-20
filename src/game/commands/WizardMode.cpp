COMMAND(WizardMode) {
  if (!game::WizardModeIsActive()) {
    if (game::TruthQuestion(CONST_S("Do you want to cheat, cheater? This action cannot be undone. [y/N]"))) {
      game::ActivateWizardMode();
      ADD_MESSAGE("Wizard mode activated.");
      if (!game::IsInWilderness()) game::LoadWorldMap();
      v2 ElpuriCavePos = game::GetWorldMap()->GetEntryPos(0, ELPURI_CAVE);
      game::GetWorldMap()->GetWSquare(ElpuriCavePos)->ChangeOWTerrain(elpuricave::Spawn());
      game::GetWorldMap()->RevealEnvironment(ElpuriCavePos, 1);
      if (game::IsInWilderness()) game::GetWorldMap()->SendNewDrawRequest(); else game::SaveWorldMap();
      game::Save();
      game::Save(game::GetAutoSaveFileName());
    }
    return false;
  }
  // wizard mode
  if (game::GetPlayerName() == "_k8_" && game::TruthQuestion(CONST_S("Do you want to uncheat, cheater? [y/N]"))) {
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
