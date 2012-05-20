COMMAND(LevelTeleport) {
  sLong Level = game::NumberQuestion(CONST_S("To which level?"), WHITE);
  if (Level <= 0 || Level > game::GetLevels()) {
    ADD_MESSAGE("There is no level %d in this dungeon, %s!", Level, game::Insult());
    return false;
  }
  if (Level == game::GetCurrentLevelIndex()+1) {
    ADD_MESSAGE("You are already here, %s!", game::Insult());
    return false;
  }
  return game::TryTravel(game::GetCurrentDungeonIndex(), Level-1, RANDOM, true);
};
