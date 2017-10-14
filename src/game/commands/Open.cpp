COMMAND(Open) {
  if (Char->CanOpen()) {
    int Key;
    truth openableItems = Char->GetStack()->SortedItems(Char, &item::IsOpenable);

    int openableSquares = 0;
    v2 lastOpenableDir = v2(0, 0);

    for (int dy = -1; dy < 2; ++dy) {
      for (int dx = -1; dx < 2; ++dx) {
        v2 dirVect = v2(dx, dy);
        if (dirVect != ERROR_V2 && Char->GetArea()->IsValidPos(Char->GetPos()+dirVect)) {
          olterrain *Terrain = Char->GetNearLSquare(Char->GetPos()+dirVect)->GetOLTerrain();
          if (Terrain && Terrain->CanBeOpened()) {
            lastOpenableDir = dirVect;
            ++openableSquares;
          } else if (Char->GetNearLSquare(Char->GetPos()+dirVect)->GetStack()->SortedItems(Char, &item::IsOpenable)) {
            lastOpenableDir = dirVect;
            ++openableSquares;
          }
        }
      }
    }

    if (!openableItems && !openableSquares) {
      ADD_MESSAGE("There is nothing to open around.");
      return false;
    }

    v2 dirVect;

    if (openableItems || openableSquares > 1) {
      if (openableItems) {
        // only inventory, skip query
        if (openableSquares == 0) {
          item *Item = Char->GetStack()->DrawContents(Char, CONST_S("What do you want to open?"), 0, &item::IsOpenable);
          return (Item && Item->Open(Char));
        }
        Key = game::AskForKeyPress(CONST_S("What do you wish to open? [press a direction key, space or i]"));
      } else {
        Key = game::AskForKeyPress(CONST_S("What do you wish to open? [press a direction key or space]"));
      }

      if (openableItems && (Key == 'i' || Key == 'I')) {
        item *Item = Char->GetStack()->DrawContents(Char, CONST_S("What do you want to open?"), 0, &item::IsOpenable);
        return (Item && Item->Open(Char));
      }

      dirVect = game::GetDirectionVectorForKey(Key);
    } else {
      dirVect = lastOpenableDir;
    }

    if (dirVect != ERROR_V2 && Char->GetArea()->IsValidPos(Char->GetPos()+dirVect)) return Char->GetNearLSquare(Char->GetPos()+dirVect)->Open(Char);
  } else {
    ADD_MESSAGE("This monster type cannot open anything.");
  }
  return false;
};
