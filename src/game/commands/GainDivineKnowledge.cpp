COMMAND(GainDivineKnowledge) {
  for (int c = 1; c <= GODS; ++c) game::LearnAbout(game::GetGod(c));
  return false;
};
