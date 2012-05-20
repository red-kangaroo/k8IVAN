COMMAND(LowerGodRelations) {
  for (int c = 1; c <= GODS; ++c) game::GetGod(c)->AdjustRelation(-50);
  return false;
};
