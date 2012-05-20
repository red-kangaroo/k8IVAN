#ifdef HEADER_PHASE
ITEM(armor, item)
{
 public:
  virtual sLong GetPrice() const;
  virtual void AddInventoryEntry(ccharacter*, festring&, int, truth) const;
  virtual void Save(outputfile&) const;
  virtual void Load(inputfile&);
  virtual truth IsArmor(ccharacter*) const { return true; }
  virtual int GetEnchantment() const { return Enchantment; }
  virtual void SetEnchantment(int);
  virtual void EditEnchantment(int);
  virtual int GetStrengthValue() const;
  virtual truth CanBePiledWith(citem*, ccharacter*) const;
  virtual int GetInElasticityPenalty(int) const;
  virtual int GetCarryingBonus() const;
  virtual truth IsFixableBySmith(ccharacter*) const { return IsBroken() || IsRusted(); }
  virtual truth IsFixableByTailor(ccharacter*) const { return IsBroken(); }
  virtual truth AllowFluids() const { return true; }
  virtual void CalculateEnchantment();
  virtual double GetTHVBonus() const;
  virtual double GetDamageBonus() const;
 protected:
  virtual void AddPostFix(festring&, int) const;
  virtual void PostConstruct();
  int Enchantment;
};


#else



int armor::GetCarryingBonus() const { return Enchantment << 1; }



double armor::GetTHVBonus() const { return Enchantment * .5; }



double armor::GetDamageBonus() const { return Enchantment; }



sLong armor::GetPrice() const
{
  double StrengthValue = GetStrengthValue();
  return sLong(StrengthValue * StrengthValue * StrengthValue * 20 / sqrt(GetWeight()));
}



void armor::AddInventoryEntry(ccharacter*, festring& Entry, int Amount, truth ShowSpecialInfo) const
{
  if(Amount == 1)
    AddName(Entry, INDEFINITE);
  else
  {
    Entry << Amount << ' ';
    AddName(Entry, PLURAL);
  }

  if(ShowSpecialInfo)
    Entry << " [" << GetWeight() * Amount << "g, AV " << GetStrengthValue() << ']';
}



truth armor::CanBePiledWith(citem* Item, ccharacter* Viewer) const
{
  return item::CanBePiledWith(Item, Viewer) && Enchantment == static_cast<const armor*>(Item)->Enchantment;
}



void armor::Save(outputfile& SaveFile) const
{
  item::Save(SaveFile);
  SaveFile << Enchantment;
}



void armor::Load(inputfile& SaveFile)
{
  item::Load(SaveFile);
  SaveFile >> Enchantment;
}



void armor::AddPostFix(festring& String, int Case) const
{
  item::AddPostFix(String, Case);

  if(Fluid)
  {
    String << " covered with ";
    fluid::AddFluidInfo(Fluid[0], String);
  }

  if(Enchantment > 0)
    String << " +" << Enchantment;
  else if(Enchantment < 0)
    String << ' ' << Enchantment;
}



void armor::SetEnchantment(int Amount)
{
  Enchantment = Amount;
  SignalEnchantmentChange();
}



void armor::EditEnchantment(int Amount)
{
  Enchantment += Amount;
  SignalEnchantmentChange();
}



int armor::GetStrengthValue() const
{
  return Max<sLong>(sLong(GetStrengthModifier()) * GetMainMaterial()->GetStrengthValue() / 2000 + Enchantment, 0);
}



void armor::PostConstruct()
{
  Enchantment = GetBaseEnchantment();
}



int armor::GetInElasticityPenalty(int Attribute) const
{
  return Attribute * GetInElasticityPenaltyModifier() / (GetMainMaterial()->GetFlexibility() * 100);
}



void armor::CalculateEnchantment()
{
  Enchantment -= femath::LoopRoll(game::GetCurrentLevel()->GetEnchantmentMinusChance(), 5);
  Enchantment += femath::LoopRoll(game::GetCurrentLevel()->GetEnchantmentPlusChance(), 5);
  Enchantment -= femath::LoopRoll(GetEnchantmentMinusChance(), 5);
  Enchantment += femath::LoopRoll(GetEnchantmentPlusChance(), 5);
}
#endif
