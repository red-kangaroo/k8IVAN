COMMAND(GetScroll) {
  for (;;) {
    int sel = game::ListSelector(-1, CONST_S("Select scroll to add"),
      "Scroll of Wishing",
      "Scroll of Charging",
      "Scroll of Repair",
      "Scroll of Change Material",
      "Scroll of Enchant Weapon",
      "Scroll of Enchant Armor",
      "Scroll of Taming",
      "Scroll of Teleportation",
      "Scroll of Detect Material",
      "Scroll of Harden Material",
      "Scroll of Golem Creation",
      NULL
    );
    if (sel < 0) break;
    festring sname;
    sLong amount = game::NumberQuestion(CONST_S("How many scrolls do you want?"), WHITE, true);
    if (amount < 1) break;
    for (sLong f = amount; f > 0; --f) {
      switch (sel) {
        case 0: sname = "Wishing"; Char->GetStack()->AddItem(scrollofwishing::Spawn()); break;
        case 1: sname = "Charging"; Char->GetStack()->AddItem(scrollofcharging::Spawn()); break;
        case 2: sname = "Repair"; Char->GetStack()->AddItem(scrollofrepair::Spawn()); break;
        case 3: sname = "Change Material"; Char->GetStack()->AddItem(scrollofchangematerial::Spawn()); break;
        case 4: sname = "Enchant Weapon"; Char->GetStack()->AddItem(scrollofenchantweapon::Spawn()); break;
        case 5: sname = "Enchant Armor"; Char->GetStack()->AddItem(scrollofenchantarmor::Spawn()); break;
        case 6: sname = "Taming"; Char->GetStack()->AddItem(scrolloftaming::Spawn()); break;
        case 7: sname = "Teleportation"; Char->GetStack()->AddItem(scrollofteleportation::Spawn()); break;
        case 8: sname = "Detect Material"; Char->GetStack()->AddItem(scrollofdetectmaterial::Spawn()); break;
        case 9: sname = "Harden Material"; Char->GetStack()->AddItem(scrollofhardenmaterial::Spawn()); break;
        case 10: sname = "Golem Creation"; Char->GetStack()->AddItem(scrollofgolemcreation::Spawn()); break;
        default: sel = 666; break;
      }
    }
    if (sel != 666) {
      if (amount == 1) {
        ADD_MESSAGE("Got Scroll of %s.", sname.CStr());
      } else {
        ADD_MESSAGE("Got %d Scrolls of %s.", amount, sname.CStr());
      }
      break;
    }
  }
  return false;
};
