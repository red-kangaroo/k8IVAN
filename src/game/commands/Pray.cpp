COMMAND(Pray) {
  felist Panthenon(CONST_S("To Whom you want to address your prayers?"));
  Panthenon.SetEntryDrawer(game::GodEntryDrawer);
  int Known[GODS];
  int Index = 0;
  int DivineMaster = Char->GetLSquareUnder()->GetDivineMaster();
  if (DivineMaster == ATHEIST) {
    ADD_MESSAGE("Somehow you feel that no god will help you here.");
    return false;
  }
  if (!DivineMaster) {
    for (int c = 1; c <= GODS; ++c) {
      if (game::GetGod(c)->IsKnown()) {
        Panthenon.AddEntry(game::GetGod(c)->GetCompleteDescription(), LIGHT_GRAY, 20, c);
        Known[Index++] = c;
      }
    }
  } else if (game::GetGod(DivineMaster)->IsKnown()) {
    Panthenon.AddEntry(game::GetGod(DivineMaster)->GetCompleteDescription(), LIGHT_GRAY, 20, DivineMaster);
    Known[0] = DivineMaster;
  } else {
    ADD_MESSAGE("Somehow you feel that no deity you know can hear your prayers from this place.");
    return false;
  }
  game::SetStandardListAttributes(Panthenon);
  Panthenon.AddFlags(SELECTABLE);
  int Select = Panthenon.Draw();
  if (Select == LIST_WAS_EMPTY) {
    ADD_MESSAGE("You do not know any gods.");
    return false;
  }
  if (Select & FELIST_ERROR_BIT) return false;
  if (Char->GetLSquareUnder()->GetDivineMaster()) {
    if (!Select) {
      if (game::TruthQuestion(CONST_S("Do you really wish to pray to ")+game::GetGod(Char->GetLSquareUnder()->GetDivineMaster())->GetName()+"? [y/N]"))
        game::GetGod(Char->GetLSquareUnder()->GetDivineMaster())->Pray();
      else
        return false;
    } else return false;
  } else {
    if (game::TruthQuestion(CONST_S("Do you really wish to pray to ")+game::GetGod(Known[Select])->GetName()+"? [y/N]")) {
      if (Char->StateIsActivated(CONFUSED) && !(RAND() & 7)) {
        int Index;
        for (Index = 1+RAND()%GODS; Index == Known[Select]; Index = 1+RAND()%GODS);
        if (game::GetGod(Index)->IsKnown())
          ADD_MESSAGE("You feel something went wrong in the rituals. You have accidentally prayed to %s!", game::GetGod(Index)->GetName());
        else
          ADD_MESSAGE("Your rituals were seriously incorrect. You have accidentally prayed to an unknown god, but have no idea how!");
        game::GetGod(Index)->Pray();
      } else {
        game::GetGod(Known[Select])->Pray();
      }
    } else {
      return false;
    }
  }
  Char->EditAP(-1000);
  return true;
};
