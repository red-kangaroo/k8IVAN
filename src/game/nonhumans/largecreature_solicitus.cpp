#ifdef HEADER_PHASE
CHARACTER(solicitus, largecreature)
{
public:
  virtual void Save(outputfile&) const;
  virtual void Load(inputfile&);
  virtual void FinalProcessForBone();
  virtual void BeTalkedTo();
protected:
  virtual void GetAICommand();
  virtual void CreateCorpse(lsquare*);
  virtual truth MustBeRemovedFromBone() const;
};


#else



void solicitus::BeTalkedTo () {
  if (GetRelation(PLAYER) == HOSTILE) {
    ADD_MESSAGE("Oh no. Now is for the figthing!!!");
    return;
  }
  //
  if (PLAYER->StateIsActivated(PANIC) && !game::PlayerIsSolicitusChampion()) {
    ADD_MESSAGE("Solicitus perks up. \"Well hullo there mortal! Would you care to be my Champion? I'll give you a free copy of my celestial monograph on Atheism!\"");
    if (game::TruthQuestion(CONST_S("Do you choose to become the Champion of Solicitus?"), REQUIRES_ANSWER)) {
      game::TextScreen(CONST_S(
        "Solicitus speaks:\n"
        "\"Becoming my champion involves my changing your sweat material into pure liquified fear.\"\n"
        "\"Now, hold still while I administer to your body what powers I have left!\"\n"));
      game::TextScreen(CONST_S("You feel Solicitus changing your sweat glands. It feels disgusting."));
      game::MakePlayerSolicitusChampion();
      PLAYER->EditCurrentSweatMaterial(LIQUID_HORROR);
      (celestialmonograph::Spawn())->MoveTo(PLAYER->GetStack());
      //pantheonbook* NewBook = pantheonbook::Spawn();
      //AddPlace->AddItem(NewBook);
      ADD_MESSAGE("\"Go forth, you are anointed! And here's your personal copy of my monograph, mortal. Enjoy!\"");
      GetArea()->SendNewDrawRequest();
    } else {
      ADD_MESSAGE("\"Not a problem, perhaps another time. It's not for everyone, you know.\"");
      return;
    }
  } else if (PLAYER->StateIsActivated(PANIC) && game::PlayerIsSolicitusChampion()) {
    ADD_MESSAGE("\"I suppose you want to hear my life story?\"");
  } else {
    ADD_MESSAGE("\"Maybe you should empathise with my situation first. Go drink some liquified fear and then we'll talk.\"");
  }
}



void solicitus::GetAICommand () {
  if (MoveRandomly()) return;
  EditAP(-2000);
  return;
}



void solicitus::CreateCorpse (lsquare *Square) {
  //for(int c = 0; c < 3; ++c) Square->AddItem(pineapple::Spawn());
  //largecreature::CreateCorpse(Square);
  ADD_MESSAGE("You hear a booming voice: \"No, mortal! This will not be done!\"");
  game::GetCurrentLevel()->Explosion(this, CONST_S("killed by an explosion of the toppled-god Solicitus"), PLAYER->GetPos(), 1300>>3, false);
  SendToHell();
}



truth solicitus::MustBeRemovedFromBone () const {
  return !IsEnabled() || GetTeam()->GetID() != MONSTER_TEAM || GetDungeon()->GetIndex() != UNDER_WATER_TUNNEL || GetLevel()->GetIndex() != VESANA_LEVEL;
}



void solicitus::Save (outputfile &SaveFile) const {
  nonhumanoid::Save(SaveFile);
}



void solicitus::Load (inputfile &SaveFile) {
  nonhumanoid::Load(SaveFile);
}



void solicitus::FinalProcessForBone () {
  largecreature::FinalProcessForBone();
}
#endif
