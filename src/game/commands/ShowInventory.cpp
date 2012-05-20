COMMAND(ShowInventory) {
  festring Title("Your inventory (total weight: ");
  Title << Char->GetStack()->GetWeight();
  Title << "g)";
  Char->GetStack()->DrawContents(Char, Title, NO_SELECT);
  return false;
};
