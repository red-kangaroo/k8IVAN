COMMAND(OpenXinTomb) {
  if (!game::GetXinrochTombStoryState()) {
    game::RevealPOI(game::xinrochtombPOI());
    game::SetXinrochTombStoryState(1);
    game::ScheduleImmediateSave();
    ADD_MESSAGE("Xinroch Tomb opened.");
  } else {
    ADD_MESSAGE("Xinroch Tomb already opened.");
  }
  return false;
};
