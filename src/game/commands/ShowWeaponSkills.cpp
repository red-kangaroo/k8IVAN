COMMAND(ShowWeaponSkills) {
  felist List(CONST_S("Your experience in weapon categories"));
  List.AddDescription(CONST_S(""));
  List.AddDescription(CONST_S("Category name                 Level     Points    Needed    Battle bonus"));
  truth Something = false;
  festring Buffer;
  for (int c = 0; c < Char->GetAllowedWeaponSkillCategories(); ++c) {
    cweaponskill *Skill = Char->GetCWeaponSkill(c);
    if (Skill->GetHits()/100 || (Char->IsUsingWeaponOfCategory(c))) {
      Buffer = Skill->GetName(c);
      Buffer.Resize(30);
      Buffer << Skill->GetLevel();
      Buffer.Resize(40);
      Buffer << Skill->GetHits()/100;
      Buffer.Resize(50);
      if (Skill->GetLevel() != 20) Buffer << (Skill->GetLevelMap(Skill->GetLevel()+1)-Skill->GetHits())/100;
      else Buffer << '-';
      Buffer.Resize(60);
      Buffer << '+' << (Skill->GetBonus()-1000)/10;
      if (Skill->GetBonus() % 10) Buffer << '.' << Skill->GetBonus()%10;
      Buffer << '%';
      List.AddEntry(Buffer, Char->IsUsingWeaponOfCategory(c) ? WHITE : LIGHT_GRAY);
      Something = true;
    }
  }
  if (Char->AddSpecialSkillInfo(List)) Something = true;
  if (Something) {
    game::SetStandardListAttributes(List);
    List.Draw();
  } else {
    ADD_MESSAGE("You are not experienced in any weapon skill yet!");
  }
  return false;
};
