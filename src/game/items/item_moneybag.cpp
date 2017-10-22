#ifdef HEADER_PHASE
ITEM(moneybag, item)
{
public:
  moneybag () : moneyAmount(1+RAND_N(300)) {
    if (RAND_N(100) < 10) moneyAmount += 1000;
  }
  virtual truth Apply (character *beggar);
  virtual void Save (outputfile &) const;
  virtual void Load (inputfile &);
  virtual truth IsConsumable () const { return false; }
  virtual truth IsAppliable (ccharacter *beggar) const;
  virtual truth CanBeHardened (ccharacter *) const { return false; }
  virtual truth IsMoneyBag () const { return true; }

  virtual sLong GetTruePrice () const { return moneyAmount; }

protected:
  int moneyAmount;
};


#else



truth moneybag::IsAppliable (ccharacter *beggar) const {
  return beggar->IsPlayer();
}



truth moneybag::Apply (character *beggar) {
  if (!beggar->IsPlayer()) return false;
  if (!game::TruthQuestion(CONST_S("Are you sure you want to open ")+GetName(DEFINITE)+"?")) return false;
  ADD_MESSAGE("You opens %s and found %i gold coins", CHAR_NAME(DEFINITE), moneyAmount);
  beggar->EditMoney(moneyAmount);
  RemoveFromSlot();
  SendToHell();
  return true;
}



void moneybag::Save (outputfile &saveFile) const {
  item::Save(saveFile);
  saveFile << moneyAmount;
}



void moneybag::Load (inputfile &saveFile) {
  item::Load(saveFile);
  saveFile >> moneyAmount;
}
#endif
