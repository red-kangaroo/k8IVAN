#ifdef HEADER_PHASE
ITEM(bodyarmor, armor)
{
 public:
  virtual sLong GetPrice() const;
  virtual truth IsBodyArmor(ccharacter*) const { return true; }
  virtual truth IsInCorrectSlot(int) const;
 protected:
  virtual cchar* GetBreakVerb() const;
  virtual truth AddAdjective(festring&, truth) const;
  virtual cfestring& GetNameSingular() const;
};


#else



sLong bodyarmor::GetPrice() const { return (armor::GetPrice() << 3) + GetEnchantedPrice(Enchantment); }



truth bodyarmor::IsInCorrectSlot(int I) const { return I == BODY_ARMOR_INDEX; }



cfestring& bodyarmor::GetNameSingular() const { return GetMainMaterial()->GetFlexibility() >= 5 ? item::GetFlexibleNameSingular() : item::GetNameSingular(); }



cchar* bodyarmor::GetBreakVerb() const { return GetMainMaterial()->GetFlexibility() >= 5 ? "is torn apart" : "breaks"; }



truth bodyarmor::AddAdjective(festring& String, truth Articled) const
{
  if(IsBroken())
  {
    if(Articled)
      String << "a ";

    if(GetMainMaterial()->GetFlexibility() >= 5)
      String << "torn";
    else
      String << "broken";

    String << ' ';
    return true;
  }
  else
    return false;
}
#endif
