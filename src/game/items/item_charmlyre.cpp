#ifdef HEADER_PHASE
ITEM(charmlyre, item)
{
 public:
  charmlyre();
  virtual truth Apply(character*);
  virtual truth IsAppliable(ccharacter*) const { return true; }
  virtual void Load(inputfile&);
  virtual void Save(outputfile&) const;
  virtual void FinalProcessForBone();
 protected:
  virtual col16 GetMaterialColorB(int) const;
  feuLong LastUsed;
};


#else



col16 charmlyre::GetMaterialColorB(int) const { return MakeRGB16(150, 130, 110); }



truth charmlyre::Apply (character *Charmer) {
  if (LastUsed && game::GetTick()-LastUsed < 10000) {
    if (Charmer->IsPlayer()) {
      if (Charmer->CanHear()) ADD_MESSAGE("You produce a highly alluring sound.");
      else ADD_MESSAGE("You try to play something with the %s, but it is hard when you can't hear.", CHAR_NAME(DEFINITE));
    } else if (Charmer->CanBeSeenByPlayer()) {
      if (PLAYER->CanHear()) ADD_MESSAGE("%s plays %s and produces a highly alluring sound.", Charmer->CHAR_NAME(DEFINITE), CHAR_NAME(DEFINITE));
      else ADD_MESSAGE("%s plays %s.", Charmer->CHAR_NAME(DEFINITE), CHAR_NAME(DEFINITE));
    } else if (PLAYER->CanHear()) ADD_MESSAGE("You hear a lyre playing.");
  } else {
    LastUsed = game::GetTick();
    if (Charmer->IsPlayer()) {
      if (Charmer->CanHear()) ADD_MESSAGE("You produce a mesmerizing sound.");
      else ADD_MESSAGE("You try to play something with the %s, but it is hard when you can't hear.", CHAR_NAME(DEFINITE));
    } else if (Charmer->CanBeSeenByPlayer()) {
      if (PLAYER->CanHear()) ADD_MESSAGE("%s plays %s and produces a mesmerizing sound.", Charmer->CHAR_NAME(DEFINITE), CHAR_NAME(DEFINITE));
      else ADD_MESSAGE("%s plays %s.", Charmer->CHAR_NAME(DEFINITE), CHAR_NAME(DEFINITE));
    }
    for (int d = 0; d < Charmer->GetNeighbourSquares(); ++d) {
      square *Square = Charmer->GetNeighbourSquare(d);
      if (Square) {
        character *Char = Square->GetCharacter();
        if (Char) {
          if (Char->CanHear()) {
            if (Char->CanTameWithLyre(Charmer)) {
              if (Char->GetTeam() == Charmer->GetTeam()) ADD_MESSAGE("%s seems to be very happy.", Char->CHAR_NAME(DEFINITE));
              else if (Char->GetRelation(Charmer) == HOSTILE) ADD_MESSAGE("%s stops fighting.", Char->CHAR_NAME(DEFINITE));
              else ADD_MESSAGE("%s seems to be very friendly towards you.", Char->CHAR_NAME(DEFINITE));
              Char->ChangeTeam(Charmer->GetTeam());
            } else {
              ADD_MESSAGE("%s resists its charming call.", Char->CHAR_NAME(DEFINITE));
            }
          } else {
            ADD_MESSAGE("%s seems not affected.", Char->CHAR_NAME(DEFINITE));
          }
        }
      }
    }
  }
  Charmer->EditAP(-1000);
  game::CallForAttention(GetPos(), 100);
  return true;
}



void charmlyre::Save(outputfile& SaveFile) const
{
  item::Save(SaveFile);
  SaveFile << LastUsed;
}



void charmlyre::Load(inputfile& SaveFile)
{
  item::Load(SaveFile);
  SaveFile >> LastUsed;
}



charmlyre::charmlyre()
{
  LastUsed = 0;
}



void charmlyre::FinalProcessForBone()
{
  item::FinalProcessForBone();
  LastUsed = 0;
}
#endif
