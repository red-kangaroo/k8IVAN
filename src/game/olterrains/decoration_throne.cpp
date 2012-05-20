#ifdef HEADER_PHASE
OLTERRAIN(throne, decoration)
{
 public:
  virtual truth SitOn(character*);
};


#else



truth throne::SitOn (character *Sitter) {
  Sitter->EditAP(-1000);
  if (Sitter->HasPetrussNut() && Sitter->HasGoldenEagleShirt() && game::GetGod(VALPURUS)->GetRelation() != 1000) {
    ADD_MESSAGE("You have a strange vision of yourself becoming great ruler. The daydream fades in a whisper: \"Thou shalt be a Our Champion first!\"");
    return true;
  }
  if (Sitter->HasPetrussNut() && !Sitter->HasGoldenEagleShirt() && game::GetGod(VALPURUS)->GetRelation() == 1000) {
    ADD_MESSAGE("You have a strange vision of yourself becoming great ruler. The daydream fades in a whisper: \"Thou shalt wear Our shining armor first!\"");
    return true;
  }
  if (!Sitter->HasPetrussNut() && Sitter->HasGoldenEagleShirt() && game::GetGod(VALPURUS)->GetRelation() == 1000) {
    ADD_MESSAGE("You have a strange vision of yourself becoming great ruler. The daydream fades in a whisper: \"Thou shalt surpass thy predecessor first!\"");
    return true;
  }
  if (Sitter->HasPetrussNut() && Sitter->HasGoldenEagleShirt() && game::GetGod(VALPURUS)->GetRelation() == 1000) {
    game::TextScreen(CONST_S("A heavenly choir starts to sing Grandis Rana and a booming voice fills the air:\n\n\"Mortal! Thou hast surpassed Petrus, and pleased Us greatly during thy adventures!\nWe hereby title thee as Our new high priest!\"\n\nYou are victorious!"));
    game::GetCurrentArea()->SendNewDrawRequest();
    game::DrawEverything();
    PLAYER->ShowAdventureInfo();
    festring Msg = CONST_S("became the new high priest of the Great Frog");
    PLAYER->AddScoreEntry(Msg, 5, false);
    game::End(Msg);
    return true;
  }
  ADD_MESSAGE("You feel somehow out of place.");
  return true;
}
#endif
