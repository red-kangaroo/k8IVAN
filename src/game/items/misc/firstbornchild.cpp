#ifdef HEADER_PHASE
ITEM(firstbornchild, item)
{
 public:
  virtual bool SpecialOfferEffect(int);
  virtual truth AllowSpoil() const { return false; } // temporary
  virtual truth Spoils() const { return false; } // temporary
 protected:
  virtual col16 GetMaterialColorB(int) const;
};


#else



bool firstbornchild::SpecialOfferEffect(int GodNumber) {
  god* Receiver = game::GetGod(GodNumber);
  Receiver->AdjustRelation(2000); // to the max

  int AmountOfAngelAppears = false;
  for(int c = 0; c < 10; ++c) {
    character* Angel = Receiver->CreateAngel(PLAYER->GetTeam(), 1000);
    if(Angel)
    {
      ++AmountOfAngelAppears;
    }
  }

  if(AmountOfAngelAppears == 0) {
    ADD_MESSAGE("You sacrifice %s. %s is very pleased.",
    CHAR_NAME(DEFINITE), Receiver->GetName());
  }
  else if(AmountOfAngelAppears == 1) {
    ADD_MESSAGE("You sacrifice %s. %s is very pleased. An angel appears! ",
    CHAR_NAME(DEFINITE), Receiver->GetName());
  }
  else {
    ADD_MESSAGE("You sacrifice %s. %s is very pleased. An army of angels appears! ",
    CHAR_NAME(DEFINITE), Receiver->GetName());
  }

  return true;
}



col16 firstbornchild::GetMaterialColorB(int) const { return MakeRGB16(160, 160, 160); }
#endif
