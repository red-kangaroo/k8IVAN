#ifdef HEADER_PHASE
GOD(dulcis, god)
{
public:
  virtual cchar *GetName () const;
  virtual cchar *GetDescription () const;
  virtual int GetAlignment () const;
  virtual int GetBasicAlignment () const;
  virtual col16 GetColor () const;
  virtual col16 GetEliteColor () const;
  virtual int GetSex () const { return FEMALE; }

protected:
  virtual void PrayGoodEffect ();
  virtual void PrayBadEffect ();
};


#else


cchar *dulcis::GetName () const { return "Dulcis"; }
cchar *dulcis::GetDescription () const { return "goddess of love and art"; }
int dulcis::GetAlignment () const { return AL; }
int dulcis::GetBasicAlignment () const { return GOOD; }
col16 dulcis::GetColor () const { return LAWFUL_BASIC_COLOR; }
col16 dulcis::GetEliteColor () const { return LAWFUL_ELITE_COLOR; }


void dulcis::PrayGoodEffect () {
  ADD_MESSAGE("A beautiful melody echoes around you.");
  for (int d = 0; d < PLAYER->GetNeighbourSquares(); ++d) {
    square *Square = PLAYER->GetNeighbourSquare(d);
    //
    if (Square) {
      character *Char = Square->GetCharacter();
      //
      if (Char) {
        if (Char->CanHear()) {
          if (Char->CanTameWithDulcis(PLAYER)) {
            if (Char->GetTeam() == PLAYER->GetTeam()) {
              ADD_MESSAGE("%s seems to be very happy.", Char->CHAR_DESCRIPTION(DEFINITE));
            } else if (Char->GetRelation(PLAYER) == HOSTILE) {
              ADD_MESSAGE("%s stops fighting.", Char->CHAR_DESCRIPTION(DEFINITE));
            } else {
              ADD_MESSAGE("%s seems to be very friendly towards you.", Char->CHAR_DESCRIPTION(DEFINITE));
            }
            Char->ChangeTeam(PLAYER->GetTeam());
          } else {
            ADD_MESSAGE("%s resists its charming call.", Char->CHAR_DESCRIPTION(DEFINITE));
          }
        } else {
          ADD_MESSAGE("%s seems not affected.", Char->CHAR_DESCRIPTION(DEFINITE));
        }
      }
    }
  }
}


void dulcis::PrayBadEffect () {
  ADD_MESSAGE("%s plays a horrible tune that rots your brain.", GetName());
  PLAYER->ReceiveDamage(0, 1+RAND()%9, SOUND, HEAD);
  PLAYER->CheckDeath(CONST_S("became insane by listening ") + GetName() + " too much", 0);
}


#endif
