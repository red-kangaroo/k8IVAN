#ifdef HEADER_PHASE
ITEM(holybook, item)
{
 public:
  virtual truth CanBeRead(character*) const;
  virtual truth IsReadable(ccharacter*) const { return true; }
  virtual truth ReceiveDamage(character*, int, int, int);
  virtual void FinishReading(character*);
 protected:
  virtual col16 GetMaterialColorA(int) const;
  virtual truth ShowMaterial() const { return false; }
};


#else



truth holybook::CanBeRead(character* Reader) const
{
  return Reader->CanRead() || game::GetSeeWholeMapCheatMode();
}



void holybook::FinishReading(character* Reader)
{
  if(Reader->IsPlayer())
  {
    PLAYER->EditExperience(INTELLIGENCE, 75, 1 << 12);
    PLAYER->EditExperience(WISDOM, 150, 1 << 12);

    if(GetMasterGod()->IsKnown())
    {
      ADD_MESSAGE("The book reveals many divine secrets of %s to you.", GetMasterGod()->GetName());
      GetMasterGod()->AdjustRelation(75);
      game::ApplyDivineAlignmentBonuses(GetMasterGod(), 15, true);

      if(!(RAND() % 3))
      {
  ADD_MESSAGE("But then it disappears.");
  RemoveFromSlot();
  SendToHell();
      }
    }
    else
    {
      game::LearnAbout(GetMasterGod());
      game::LearnAbout(GetMasterGod());
      ADD_MESSAGE("You feel you master the magical rites of %s.", GetMasterGod()->GetName());
    }
  }
}



truth holybook::ReceiveDamage(character*, int Damage, int Type, int)
{
  if(Type & FIRE && Damage
     && GetMainMaterial()->GetInteractionFlags() & CAN_BURN
     && (Damage > 125 || !(RAND() % (250 / Damage))))
  {
    if(CanBeSeenByPlayer())
      ADD_MESSAGE("%s catches fire!", GetExtendedDescription().CStr());

    RemoveFromSlot();
    SendToHell();
    return true;
  }

  return false;
}



col16 holybook::GetMaterialColorA(int) const
{
  return GetMasterGod()->GetColor();
}
#endif
