#ifdef HEADER_PHASE
GOD(mellis, god)
{
public:
  virtual cchar *GetName () const;
  virtual cchar *GetDescription () const;
  virtual int GetAlignment () const;
  virtual int GetBasicAlignment () const;
  virtual col16 GetColor () const;
  virtual col16 GetEliteColor () const;
  virtual int GetSex () const { return MALE; }

protected:
  virtual void PrayGoodEffect ();
  virtual void PrayBadEffect ();
};


#else


cchar *mellis::GetName () const { return "Mellis"; }
cchar *mellis::GetDescription () const { return "god of money, trade and politics"; }
int mellis::GetAlignment () const { return ANM; }
int mellis::GetBasicAlignment () const { return NEUTRAL; }
col16 mellis::GetColor () const { return NEUTRAL_BASIC_COLOR; }
col16 mellis::GetEliteColor () const { return NEUTRAL_ELITE_COLOR; }


void mellis::PrayGoodEffect () {
  truth Success = false;
  itemvector OKItems;
  //
  for (stackiterator i = PLAYER->GetStack()->GetBottom(); i.HasItem(); ++i) {
    if (!i->HasBetterVersion()) continue;
    OKItems.push_back(*i);
    Success = true;
  }
  //
  for (int c = 0; !OKItems.empty() && c < 5; ++c) {
    item *ToBeDeleted = OKItems[RAND()%OKItems.size()];
    item *NewVersion = ToBeDeleted->BetterVersion();
    //
    ADD_MESSAGE("%s manages to trade %s into %s.", GetName(), ToBeDeleted->CHAR_NAME(DEFINITE), NewVersion->CHAR_NAME(INDEFINITE));
    PLAYER->GetStack()->AddItem(NewVersion);
    ToBeDeleted->RemoveFromSlot();
    ToBeDeleted->SendToHell();
    OKItems.erase(std::find(OKItems.begin(), OKItems.end(), ToBeDeleted));
  }
  //
  if ((Success && !(RAND()%5)) || (!Success && !(RAND()%3))) {
    int Possible[GODS];
    int PossibleSize = 0;
    //
    for (int c = 1; c <= GODS; ++c) if (!game::GetGod(c)->IsKnown()) Possible[PossibleSize++] = c;
    //
    if (PossibleSize) {
      int NewKnownGod = Possible[RAND() % PossibleSize];
      //
      game::LearnAbout(game::GetGod(NewKnownGod));
      ADD_MESSAGE("%s shares his knowledge of %s, the %s.", GetName(), game::GetGod(NewKnownGod)->GetName(), game::GetGod(NewKnownGod)->GetDescription());
      return;
    }
  }
  //
  if (!Success) ADD_MESSAGE("Nothing happens.");
}


void mellis::PrayBadEffect () {
  for (int c = 1; c <= GODS; ++c) if (c != GetType()) game::GetGod(c)->AdjustRelation(-150+(RAND()%100));
  ADD_MESSAGE("%s spreads bad rumours about you to other gods.", GetName());
}


#endif
