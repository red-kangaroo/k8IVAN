COMMAND(GainAllItems) {
  itemvectorvector AllItems;
  protosystem::CreateEveryItem(AllItems);
  stack *Stack = game::IsInWilderness() ? Char->GetStack() : Char->GetStackUnder();
  for (uInt c = 0; c < AllItems.size(); ++c) Stack->AddItem(AllItems[c][0]);
  return false;
};
