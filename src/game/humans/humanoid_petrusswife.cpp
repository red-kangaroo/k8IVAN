#ifdef HEADER_PHASE
CHARACTER(petrusswife, humanoid)
{
 public:
  petrusswife() : GiftTotal(0) { }
  virtual truth MoveRandomly() { return MoveRandomlyInRoom(); }
  virtual truth SpecialEnemySightedReaction(character*);
  virtual void Save(outputfile&) const;
  virtual void Load(inputfile&);
  virtual void BeTalkedTo();
 protected:
  int GiftTotal;
};


#else



truth petrusswife::SpecialEnemySightedReaction(character* Char)
{
  item* Weapon = Char->GetMainWielded();

  if(Weapon && Weapon->IsWeapon(Char) && !(RAND() % 20))
    ADD_MESSAGE("%s screams: \"Oh my Frog, %s's got %s %s!\"", CHAR_DESCRIPTION(DEFINITE), Char->CHAR_PERSONAL_PRONOUN_THIRD_PERSON_VIEW, Weapon->GetArticle(), Weapon->GetNameSingular().CStr());

  return false;
}



void petrusswife::Save(outputfile& SaveFile) const
{
  humanoid::Save(SaveFile);
  SaveFile << GiftTotal;
}



void petrusswife::Load(inputfile& SaveFile)
{
  humanoid::Load(SaveFile);
  SaveFile >> GiftTotal;
}



void petrusswife::BeTalkedTo()
{
  itemvector Item;

  if(!PLAYER->SelectFromPossessions(Item, CONST_S("Do you have something to give me?"), 0, &item::IsLuxuryItem)
     || Item.empty())
    humanoid::BeTalkedTo();

  int Accepted = 0;
  truth RefusedSomething = false;

  for(unsigned int c = 0; c < Item.size(); ++c)
    if(!MakesBurdened(GetCarriedWeight() + Item[c]->GetWeight()))
    {
      ++Accepted;
      GiftTotal += Item[c]->GetTruePrice();
      Item[c]->RemoveFromSlot();
      GetStack()->AddItem(Item[c]);
    }
    else
    {
      RefusedSomething = true;
      break;
    }

  if(Accepted)
    ADD_MESSAGE("\"I thank you for your little gift%s.\"", Accepted == 1 ? "" : "s");

  if(RefusedSomething)
    ADD_MESSAGE("\"Unfortunately I cannot carry any more of your gifts. I'm a delicate woman, you see.\"");
}
#endif
