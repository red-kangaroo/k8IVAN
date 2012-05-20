COMMAND(GoUp) {
  if (!Char->TryToUnStickTraps(ZERO_V2)) return false;
  /*if(!game::IsInWilderness() && game::WizardModeIsActive() && game::GetCurrentLevelIndex() >= 1)
    if(game::TryTravel(game::GetCurrentDungeonIndex(), game::GetCurrentLevelIndex() - 1, RANDOM, true))
    return true;*/
  oterrain *Terrain = Char->GetSquareUnder()->GetOTerrain();
  if (!Terrain) {
    if (game::IsInWilderness()) {
      if (!Char->IsFlying()) ADD_MESSAGE("You jump into the air. For some reason you don't get too far above.");
      else ADD_MESSAGE("You fly around for some time.");
    } else ADD_MESSAGE("You can't go up.");
    return false;
  }
  if (Terrain->Enter(true)) {
    Char->EditExperience(LEG_STRENGTH, 150, 1<<6);
    Char->EditNP(-20);
    Char->EditAP(-100000/APBonus(Char->GetAttribute(AGILITY)));
    return true;
  }
  return false;
};
