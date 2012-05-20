COMMAND(OpenMondedr) {
  if (!game::GetMondedrPass()) {
    if (!game::IsInWilderness()) game::LoadWorldMap();
    v2 MondedrPos = game::GetWorldMap()->GetEntryPos(0, MONDEDR);
    game::GetWorldMap()->GetWSquare(MondedrPos)->ChangeOWTerrain(mondedr::Spawn());
    game::GetWorldMap()->RevealEnvironment(MondedrPos, 1);
    if (game::IsInWilderness()) game::GetWorldMap()->SendNewDrawRequest(); else game::SaveWorldMap();
    game::SetMondedrPass(1);
    game::Save();
    game::Save(game::GetAutoSaveFileName());
    ADD_MESSAGE("Mondedr opened.");
  } else {
    ADD_MESSAGE("Mondedr already opened.");
  }
  return false;
};
