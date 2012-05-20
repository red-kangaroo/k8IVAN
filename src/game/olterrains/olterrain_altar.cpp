#ifdef HEADER_PHASE
OLTERRAIN(altar, olterrain)
{
 public:
  virtual truth AcceptsOffers() const { return true; }
  virtual void StepOn(character*);
  virtual void BeKicked(character*, int, int);
  virtual truth ReceiveVomit(character*, liquid*);
  virtual truth Polymorph(character*);
  virtual truth SitOn(character*);
  virtual void Draw(blitdata&) const;
  virtual truth VomitingIsDangerous(ccharacter*) const;
};


#else



void altar::Draw (blitdata &BlitData) const {
  olterrain::Draw(BlitData);
  BlitData.Src.X = GetConfig()<<4;
  igraph::GetSymbolGraphic()->LuminanceMaskedBlit(BlitData);
  BlitData.Src.X = BlitData.Src.Y = 0;
}



void altar::StepOn (character *Stepper) {
  if (Stepper->IsPlayer() && !GetMasterGod()->IsKnown()) {
    ADD_MESSAGE("The ancient altar is covered with strange markings. You manage to decipher them. The altar is dedicated to %s, the %s. You now know the sacred rituals that allow you to contact this deity via prayers.", GetMasterGod()->GetName(), GetMasterGod()->GetDescription());
    game::LearnAbout(GetMasterGod());
  }
}



void altar::BeKicked (character *Kicker, int, int) {
  if (Kicker->IsPlayer()) ADD_MESSAGE("You feel like a sinner.");
  else if (Kicker->CanBeSeenByPlayer()) ADD_MESSAGE("%s looks like a sinner.", Kicker->CHAR_NAME(DEFINITE));
  if (GetRoom()) GetRoom()->DestroyTerrain(Kicker);
  if (Kicker->IsPlayer()) {
    GetMasterGod()->PlayerKickedAltar();
    if (GetConfig() > 1) game::GetGod(GetConfig()-1)->PlayerKickedFriendsAltar();
    if (GetConfig() < GODS) game::GetGod(GetConfig()+1)->PlayerKickedFriendsAltar();
  }
}



truth altar::ReceiveVomit (character *Who, liquid *Liquid) {
  if (Who->IsPlayer()) {
    if (GetRoom()) GetRoom()->HostileAction(Who);
    return GetMasterGod()->PlayerVomitedOnAltar(Liquid);
  }
  return false;
}



truth altar::VomitingIsDangerous (ccharacter *) const {
  return !GetMasterGod()->LikesVomit();
}



truth altar::Polymorph (character *) {
  room *Room = GetRoom();
  if (Room && !Room->AllowAltarPolymorph()) return false;
  if (CanBeSeenByPlayer()) ADD_MESSAGE("%s glows briefly.", CHAR_NAME(DEFINITE));
  int OldGod = GetConfig(), NewGod = GetConfig();
  while (NewGod == OldGod) NewGod = 1+RAND()%GODS;
  if (GetRoom()) GetRoom()->SetDivineMaster(NewGod);
  SetConfig(NewGod);
  GetLSquareUnder()->SendNewDrawRequest();
  GetLSquareUnder()->SendMemorizedUpdateRequest();
  return true;
}



truth altar::SitOn (character *Sitter) {
  ADD_MESSAGE("You kneel down and worship %s for a moment.", GetMasterGod()->GetName());
  if (GetMasterGod()->GetRelation() < 500) {
    if (!(RAND()%20)) {
      GetMasterGod()->AdjustRelation(2);
      game::ApplyDivineAlignmentBonuses(GetMasterGod(), 1, true);
      PLAYER->EditExperience(WISDOM, 75, 1<<6);
    }
  } else if (!(RAND()%2500)) {
    character *Angel = GetMasterGod()->CreateAngel(PLAYER->GetTeam());
    if (Angel) ADD_MESSAGE("%s seems to be very friendly towards you.", Angel->CHAR_NAME(DEFINITE));
    GetMasterGod()->AdjustRelation(50);
    game::ApplyDivineAlignmentBonuses(GetMasterGod(), 10, true);
    PLAYER->EditExperience(WISDOM, 400, 1<<11);
  }
  Sitter->EditAP(-1000);
  return true;
}
#endif
