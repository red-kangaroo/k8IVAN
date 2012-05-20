COMMAND(GoDown) {
  if (!Char->TryToUnStickTraps(ZERO_V2)) return false;
  /*if(!game::IsInWilderness() && game::WizardModeIsActive() && game::GetCurrentLevelIndex() < game::GetLevels() - 1)
    if(game::TryTravel(game::GetCurrentDungeonIndex(), game::GetCurrentLevelIndex() + 1, RANDOM, true))
    return true;*/
  oterrain *Terrain = Char->GetSquareUnder()->GetOTerrain();
  if (!Terrain) {
    if (game::IsInWilderness()) ADD_MESSAGE("There seems to be nothing of interest here.");
    else ADD_MESSAGE("You can't go down.");
    return false;
  }
  if (Terrain->Enter(false)) {
    Char->EditExperience(AGILITY, 150, 1<<6);
    Char->EditNP(-10);
    Char->EditAP(-100000/APBonus(Char->GetAttribute(AGILITY)));
    return true;
  }
  return false;
};
