COMMAND(SecretKnowledge) {
  felist List(CONST_S("Knowledge of the ancients"));
  List.AddEntry(CONST_S("Character attributes"), LIGHT_GRAY);
  List.AddEntry(CONST_S("Character attack info"), LIGHT_GRAY);
  List.AddEntry(CONST_S("Character defence info"), LIGHT_GRAY);
  List.AddEntry(CONST_S("Character danger values"), LIGHT_GRAY);
  List.AddEntry(CONST_S("Item attack info"), LIGHT_GRAY);
  List.AddEntry(CONST_S("Miscellaneous item info"), LIGHT_GRAY);
  List.AddEntry(CONST_S("Material info"), LIGHT_GRAY);
  game::SetStandardListAttributes(List);
  List.AddFlags(SELECTABLE);
  uInt c, Chosen = List.Draw();
  festring Entry;
  if (Chosen & FELIST_ERROR_BIT) return false;
  List.Empty();
  List.RemoveFlags(SELECTABLE);
  if (Chosen < 4) {
    charactervector &Character = game::GetCharacterDrawVector();
    int TeamSize = 0;
    for (std::list<character*>::const_iterator i = Char->GetTeam()->GetMember().begin(); i != Char->GetTeam()->GetMember().end(); ++i) {
      if ((*i)->IsEnabled()) {
        Character.push_back(*i);
        ++TeamSize;
      }
    }
    protosystem::CreateEveryCharacter(Character);
    List.SetEntryDrawer(game::CharacterEntryDrawer);
    switch (Chosen) {
      case 0:
        List.AddDescription(CONST_S("                                                AS LS DX AG EN PE IN WI CH MA"));
        for (c = 0; c < Character.size(); ++c) {
          Entry.Empty();
          Character[c]->AddName(Entry, UNARTICLED);
          Character[c]->AddAttributeInfo(Entry);
          List.AddEntry(Entry, LIGHT_GRAY, 0, c);
        }
        List.SetPageLength(15);
        break;
      case 1:
        List.AddDescription(CONST_S("                                                  BD        THV       APC"));
        for (c = 0; c < Character.size(); ++c) {
          Entry.Empty();
          Character[c]->AddName(Entry, UNARTICLED);
          List.AddEntry(Entry, LIGHT_GRAY, 0, c);
          Character[c]->AddAttackInfo(List);
        }
        List.SetPageLength(20);
        break;
      case 2:
        List.AddDescription(CONST_S("                                                  DV/BV     HP        AV/BS"));
        for (c = 0; c < Character.size(); ++c) {
          Entry.Empty();
          Character[c]->AddName(Entry, UNARTICLED);
          Entry.Resize(47);
          Entry << int(Character[c]->GetDodgeValue());
          Entry.Resize(57);
          Entry << Character[c]->GetMaxHP();
          List.AddEntry(Entry, LIGHT_GRAY, 0, c);
          Character[c]->AddDefenceInfo(List);
        }
        List.SetPageLength(25);
        break;
      case 3:
        List.AddDescription(CONST_S("                                                  Danger    NGM       EGM"));
        for (c = 0; c < Character.size(); ++c) {
          Entry.Empty();
          Character[c]->AddName(Entry, UNARTICLED);
          Entry.Resize(47);
          Entry << int(Character[c]->GetRelativeDanger(Char, true) * 1000);
          Entry.Resize(57);
          const dangerid& DI = game::GetDangerMap().find(configid(Character[c]->GetType(), Character[c]->GetConfig()))->second;
          Entry << int(DI.NakedDanger * 1000);
          Entry.Resize(67);
          Entry << int(DI.EquippedDanger * 1000);
          List.AddEntry(Entry, LIGHT_GRAY, 0, c);
        }
        List.SetPageLength(15);
        break;
    }
    List.Draw();
    for (c = TeamSize; c < Character.size(); ++c) delete Character[c];
    game::ClearCharacterDrawVector();
  } else if (Chosen < 6) {
    itemvectorvector &Item = game::GetItemDrawVector();
    protosystem::CreateEveryItem(Item);
    List.SetEntryDrawer(game::ItemEntryDrawer);
    List.SetPageLength(20);
    switch (Chosen) {
      case 4:
        List.AddDescription(CONST_S("                                        Weight    Size      SR        DAM"));
        for (c = 0; c < Item.size(); ++c) {
          Entry.Empty();
          Item[c][0]->AddName(Entry, UNARTICLED);
          List.AddEntry(Entry, LIGHT_GRAY, 0, c, true);
          Item[c][0]->AddAttackInfo(List);
        }
        break;
      case 5:
        List.AddDescription(CONST_S("                                        \177              OV             NV"));
        for (c = 0; c < Item.size(); ++c) {
          Entry.Empty();
          Item[c][0]->AddName(Entry, UNARTICLED);
          List.AddEntry(Entry, LIGHT_GRAY, 0, c, true);
          Item[c][0]->AddMiscellaneousInfo(List);
        }
        break;
    }
    List.Draw();
    for (c = 0; c < Item.size(); ++c) delete Item[c][0];
    game::ClearItemDrawVector();
  } else if (Chosen < 7) {
    std::vector<material *> Material;
    protosystem::CreateEveryMaterial(Material);
    List.SetPageLength(30);
    List.AddDescription(CONST_S("                                        Strength       Flexibility   Density"));
    for (c = 0; c < Material.size(); ++c) {
      Entry.Empty();
      Material[c]->AddName(Entry, false, false);
      Entry.Resize(40);
      Entry << Material[c]->GetStrengthValue();
      Entry.Resize(55);
      Entry << int(Material[c]->GetFlexibility());
      Entry.Resize(70);
      Entry << int(Material[c]->GetDensity());
      List.AddEntry(Entry, Material[c]->GetColor());
    }
    List.Draw();
    for (c = 0; c < Material.size(); ++c) delete Material[c];
  }
  List.PrintToFile(game::GetHomeDir()+"secret"+Chosen+".txt");
  ADD_MESSAGE("Info written also to %ssecret%d.txt.", game::GetHomeDir().CStr(), Chosen);
  return false;
};
