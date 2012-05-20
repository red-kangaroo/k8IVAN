#ifdef HEADER_PHASE
GOD(atavus, god)
{
 public:
  virtual cchar* GetName() const;
  virtual cchar* GetDescription() const;
  virtual int GetAlignment() const;
  virtual int GetBasicAlignment() const;
  virtual col16 GetColor() const;
  virtual col16 GetEliteColor() const;
  virtual truth LikesMaterial(const materialdatabase*, ccharacter*) const;
  virtual int GetSex() const { return MALE; }
 protected:
  virtual void PrayGoodEffect();
  virtual void PrayBadEffect();
};


#else



cchar* atavus::GetName() const { return "Atavus"; }



cchar* atavus::GetDescription() const { return "god of charity and munificence"; }



int atavus::GetAlignment() const { return ALP; }



int atavus::GetBasicAlignment() const { return GOOD; }



col16 atavus::GetColor() const { return LAWFUL_BASIC_COLOR; }



col16 atavus::GetEliteColor() const { return LAWFUL_ELITE_COLOR; }



void atavus::PrayGoodEffect()
{
  if(!Timer && Relation > 500 + RAND_N(500))
  {
    item* Reward = bodyarmor::Spawn(PLATE_MAIL, NO_MATERIALS);
    Reward->InitMaterials(MAKE_MATERIAL(ARCANITE));
    ADD_MESSAGE("%s materializes before you.", Reward->CHAR_NAME(INDEFINITE));
    PLAYER->GetGiftStack()->AddItem(Reward);
    AdjustTimer(45000);
    AdjustRelation(-300);
  }
  else
    ADD_MESSAGE("Nothing happens.");
}



void atavus::PrayBadEffect()
{
  ADD_MESSAGE("You have not been good the whole year.");

  if(PLAYER->GetStack()->GetItems())
  {
    int ToBeDeleted = RAND() % PLAYER->GetStack()->GetItems();
    item* Disappearing = PLAYER->GetStack()->GetItem(ToBeDeleted);

    if(Disappearing->IsDestroyable(0))
    {
      ADD_MESSAGE("Your %s disappears.", Disappearing->CHAR_NAME(UNARTICLED));
      Disappearing->RemoveFromSlot();
      Disappearing->SendToHell();
    }
    else
    {
      ADD_MESSAGE("%s tries to remove your %s, but fails. You feel you are not so gifted anymore.", GetName(), Disappearing->CHAR_NAME(UNARTICLED));
      PLAYER->EditAttribute(AGILITY, -1);
      PLAYER->EditAttribute(ARM_STRENGTH, -1);
      PLAYER->EditAttribute(ENDURANCE, -1);
    }
  }
  else
  {
    ADD_MESSAGE("You feel you are not so gifted anymore.");
    PLAYER->EditAttribute(AGILITY, -1);
    PLAYER->EditAttribute(ARM_STRENGTH, -1);
    PLAYER->EditAttribute(ENDURANCE, -1);
  }

  PLAYER->CheckDeath(CONST_S("killed by Atavus's humour"));
}



truth atavus::LikesMaterial(const materialdatabase* MDB, ccharacter* Char) const
{
  return Char->GetTorso()->GetMainMaterial()->GetConfig() == MDB->Config;
}
#endif
