#ifdef HEADER_PHASE
CHARACTER(housewife, humanoid)
{
 public:
  virtual truth SpecialEnemySightedReaction(character*);
 protected:
  virtual int GetHairColor() const;
  virtual v2 GetHeadBitmapPos() const;
};


#else



v2 housewife::GetHeadBitmapPos() const { return v2(112, (RAND() % 6) << 4); }



col16 housewife::GetHairColor() const
{
  static col16 HouseWifeHairColor[] = { MakeRGB16(48, 40, 8), MakeRGB16(60, 48, 24),  MakeRGB16(200, 0, 0) };
  return HouseWifeHairColor[RAND() % 3];
}



truth housewife::SpecialEnemySightedReaction(character* Char)
{
  item* Weapon = Char->GetMainWielded();

  if(Weapon && Weapon->IsWeapon(Char) && !(RAND() % 5))
    ADD_MESSAGE("%s screams: \"Oh my Frog, %s's got %s %s!\"", CHAR_DESCRIPTION(DEFINITE), Char->CHAR_PERSONAL_PRONOUN_THIRD_PERSON_VIEW, Weapon->GetArticle(), Weapon->GetNameSingular().CStr());

  return false;
}
#endif
