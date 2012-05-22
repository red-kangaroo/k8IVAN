#ifdef HEADER_PHASE
CHARACTER(angel, humanoid)
{
public:
  angel() : LastHealed(0) {}

  virtual void Load (inputfile &);
  virtual void Save (outputfile &) const;
  virtual truth AttachBodyPartsOfFriendsNear ();
  virtual truth BodyPartIsVital (int) const;
  virtual int GetAttribute (int, truth = true) const;
  virtual col24 GetBaseEmitation () const;
  virtual truth CanCreateBodyPart (int) const;
  virtual cfestring& GetStandVerb () const { return character::GetStandVerb(); }
  virtual void FinalProcessForBone ();
  virtual void CreateInitialEquipment (int);

protected:
  virtual int GetTorsoMainColor () const;
  virtual int GetArmMainColor () const;
  virtual void GetAICommand ();

protected:
  feuLong LastHealed;
};


#else


truth angel::BodyPartIsVital (int I) const { return I == TORSO_INDEX || I == HEAD_INDEX; }
col16 angel::GetTorsoMainColor () const { return GetMasterGod()->GetColor(); }
col16 angel::GetArmMainColor () const { return GetMasterGod()->GetColor(); }


void angel::Save (outputfile &SaveFile) const {
  humanoid::Save(SaveFile);
  SaveFile << LastHealed;
}


void angel::Load (inputfile &SaveFile) {
  humanoid::Load(SaveFile);
  SaveFile >> LastHealed;
}


void angel::FinalProcessForBone () {
  humanoid::FinalProcessForBone();
  LastHealed = 0;
}


void angel::CreateInitialEquipment (int SpecialFlags) {
  humanoid::CreateInitialEquipment(SpecialFlags);
  GetStack()->AddItem(holybook::Spawn(GetConfig(), SpecialFlags));
}


void angel::GetAICommand () {
  if ((LastHealed || game::GetTick() - LastHealed > 10000) && AttachBodyPartsOfFriendsNear()) return;
  humanoid::GetAICommand();
}


/* Returns true if the angel finds somebody near to heal else false */
truth angel::AttachBodyPartsOfFriendsNear () {
  character *HurtOne = 0;
  bodypart *SeveredOne = 0;

  for(int d = 0; d < GetNeighbourSquares(); ++d) {
    square *Square = GetNeighbourSquare(d);
    //
    if (Square) {
      character *Char = Square->GetCharacter();
      //
      if (Char && (!HurtOne || Char->IsPlayer()) && GetRelation(Char) == FRIEND && !Char->HasAllBodyParts()) {
        bodypart *BodyPart = Char->FindRandomOwnBodyPart(false);
        //
        if (BodyPart) { HurtOne = Char; SeveredOne = BodyPart; }
      }
    }
  }
  //
  if (HurtOne) {
    if (HurtOne->IsPlayer()) ADD_MESSAGE("%s puts your %s back to its place.", CHAR_DESCRIPTION(DEFINITE), SeveredOne->GetBodyPartName().CStr());
    else if (CanBeSeenByPlayer()) ADD_MESSAGE("%s helps %s by putting %s %s in its old place.", CHAR_DESCRIPTION(DEFINITE), HurtOne->CHAR_DESCRIPTION(DEFINITE), HurtOne->GetPossessivePronoun().CStr(), SeveredOne->GetBodyPartName().CStr());
    SeveredOne->SetHP(1);
    SeveredOne->RemoveFromSlot();
    HurtOne->AttachBodyPart(SeveredOne);
    LastHealed = game::GetTick();
    DexterityAction(10);
    return true;
  }
  return false;
}


// temporary until wings are bodyparts
int angel::GetAttribute (int Identifier, truth AllowBonus) const {
  if (Identifier == LEG_STRENGTH) return GetDefaultLegStrength();
  if (Identifier == AGILITY) return GetDefaultAgility();
  return humanoid::GetAttribute(Identifier, AllowBonus);
}


col24 angel::GetBaseEmitation () const {
  switch (GetMasterGod()->GetBasicAlignment()) {
    case GOOD: return MakeRGB24(150, 150, 150);
    case NEUTRAL: return MakeRGB24(120, 120, 150);
    case EVIL: return MakeRGB24(150, 110, 110);
  }
  return 0;
}


truth angel::CanCreateBodyPart (int I) const {
  return (I == TORSO_INDEX || I == HEAD_INDEX || I == RIGHT_ARM_INDEX || I == LEFT_ARM_INDEX);
}


#endif
