#ifdef HEADER_PHASE
CHARACTER(ghost, nonhumanoid)
{
 public:
  ghost() : Active(true) { }
  virtual void AddName(festring&, int) const;
  virtual void Save(outputfile&) const;
  virtual void Load(inputfile&);
  void SetOwnerSoul(cfestring& What) { OwnerSoul = What; }
  virtual truth IsNameable() const { return OwnerSoul.IsEmpty(); }
  virtual truth RaiseTheDead(character*);
  virtual int ReceiveBodyPartDamage(character*, int, int, int, int = 8, truth = false, truth = false, truth = true, truth = false);
  virtual truth SpecialEnemySightedReaction(character*);
  void SetIsActive(truth What) { Active = What; }
  virtual truth IsPolymorphable() const { return MaxHP < 100; }
 protected:
  virtual int GetBodyPartWobbleData(int) const;
  virtual cchar* FirstPersonBiteVerb() const;
  virtual cchar* FirstPersonCriticalBiteVerb() const;
  virtual cchar* ThirdPersonBiteVerb() const;
  virtual cchar* ThirdPersonCriticalBiteVerb() const;
  virtual truth AttackIsBlockable(int) const { return false; }
  virtual truth AttackMayDamageArmor() const { return false; }
  virtual void GetAICommand();
  festring OwnerSoul;
  truth Active;
};


#else



cchar* ghost::FirstPersonBiteVerb() const { return "touch"; }



cchar* ghost::FirstPersonCriticalBiteVerb() const { return "awfully touch"; }



cchar* ghost::ThirdPersonBiteVerb() const { return "touches"; }



cchar* ghost::ThirdPersonCriticalBiteVerb() const { return "awfully touches"; }



truth ghost::SpecialEnemySightedReaction(character*) { return !(Active = true); }



int ghost::GetBodyPartWobbleData(int) const { return WOBBLE_HORIZONTALLY|(2 << WOBBLE_FREQ_SHIFT); }



void ghost::AddName(festring& String, int Case) const
{
  if(OwnerSoul.IsEmpty() || Case & PLURAL)
    character::AddName(String, Case);
  else
  {
    character::AddName(String, (Case|ARTICLE_BIT)&~INDEFINE_BIT);
    String << " of " << OwnerSoul;
  }
}



void ghost::Save(outputfile& SaveFile) const
{
  nonhumanoid::Save(SaveFile);
  SaveFile << OwnerSoul << Active;
}



void ghost::Load(inputfile& SaveFile)
{
  nonhumanoid::Load(SaveFile);
  SaveFile >> OwnerSoul >> Active;
}



truth ghost::RaiseTheDead(character* Summoner)
{
  itemvector ItemVector;
  GetStackUnder()->FillItemVector(ItemVector);

  for(uInt c = 0; c < ItemVector.size(); ++c)
    if(ItemVector[c]->SuckSoul(this, Summoner))
      return true;

  if(IsPlayer())
    ADD_MESSAGE("You shudder.");
  else if(CanBeSeenByPlayer())
    ADD_MESSAGE("%s shudders.", CHAR_NAME(DEFINITE));

  return false;
}



int ghost::ReceiveBodyPartDamage(character* Damager, int Damage, int Type, int BodyPartIndex, int Direction, truth PenetrateResistance, truth Critical, truth ShowNoDamageMsg, truth CaptureBodyPart)
{
  if(Type != SOUND)
  {
    Active = true;
    return character::ReceiveBodyPartDamage(Damager, Damage, Type, BodyPartIndex, Direction, PenetrateResistance, Critical, ShowNoDamageMsg, CaptureBodyPart);
  }
  else
    return 0;
}



void ghost::GetAICommand()
{
  if(Active)
    character::GetAICommand();
  else
  {
    if(CheckForEnemies(false, false, false))
      return;

    EditAP(-1000);
  }
}
#endif
