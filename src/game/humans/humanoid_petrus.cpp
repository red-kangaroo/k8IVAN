#ifdef HEADER_PHASE
CHARACTER(petrus, humanoid)
{
 public:
  petrus();
  virtual ~petrus();

  virtual void Save(outputfile&) const;
  virtual void Load(inputfile&);
  virtual void BeTalkedTo();
  truth HealFully(character*);
  virtual void FinalProcessForBone();
  virtual truth MoveTowardsHomePos();
 protected:
  virtual void CreateCorpse(lsquare*);
  virtual void GetAICommand();
  feuLong LastHealed;
};


#else



petrus::petrus () : LastHealed(0) {
  game::SetPetrus(this);
}



petrus::~petrus () {
  game::SetPetrus(0);
}



void petrus::FinalProcessForBone () {
  humanoid::FinalProcessForBone();
  LastHealed = 0;
}



truth petrus::HealFully (character *ToBeHealed) {
  truth DidSomething = false;
  for (int c = 0; c < ToBeHealed->GetBodyParts(); ++c) {
    if (!ToBeHealed->GetBodyPart(c)) {
      bodypart* BodyPart = 0;
      for (std::list<feuLong>::const_iterator i = ToBeHealed->GetOriginalBodyPartID(c).begin(); i != ToBeHealed->GetOriginalBodyPartID(c).end(); ++i) {
        BodyPart = static_cast<bodypart *>(ToBeHealed->SearchForItem(*i));
        if (BodyPart) break;
      }
      if (!BodyPart || !BodyPart->CanRegenerate()) continue;
      BodyPart->RemoveFromSlot();
      ToBeHealed->AttachBodyPart(BodyPart);
      BodyPart->RestoreHP();
      DidSomething = true;
      if (ToBeHealed->IsPlayer())
        ADD_MESSAGE("%s attaches your old %s back and heals it.", CHAR_NAME(DEFINITE), BodyPart->GetBodyPartName().CStr());
      else if (CanBeSeenByPlayer())
        ADD_MESSAGE("%s attaches the old %s of %s back and heals it.", CHAR_NAME(DEFINITE), BodyPart->GetBodyPartName().CStr(), ToBeHealed->CHAR_DESCRIPTION(DEFINITE));
    }
  }
  if (ToBeHealed->IsInBadCondition()) {
    ToBeHealed->RestoreLivingHP();
    DidSomething = true;
    if (ToBeHealed->IsPlayer())
      ADD_MESSAGE("%s heals you fully.", CHAR_DESCRIPTION(DEFINITE));
    else if (CanBeSeenByPlayer())
      ADD_MESSAGE("%s heals %s fully.", CHAR_DESCRIPTION(DEFINITE), ToBeHealed->CHAR_DESCRIPTION(DEFINITE));
  }
  if (ToBeHealed->TemporaryStateIsActivated(POISONED)) {
    ToBeHealed->DeActivateTemporaryState(POISONED);
    DidSomething = true;
    if (ToBeHealed->IsPlayer())
      ADD_MESSAGE("%s removes the foul poison in your body.", CHAR_DESCRIPTION(DEFINITE));
    else if (CanBeSeenByPlayer())
      ADD_MESSAGE("%s removes the foul poison in %s's body.", CHAR_DESCRIPTION(DEFINITE), ToBeHealed->CHAR_DESCRIPTION(DEFINITE));
  }
  if (DidSomething) {
    LastHealed = game::GetTick();
    DexterityAction(10);
    return true;
  }
  return false;
}



void petrus::Save (outputfile &SaveFile) const {
  humanoid::Save(SaveFile);
  SaveFile << LastHealed;
}



void petrus::Load (inputfile &SaveFile) {
  humanoid::Load(SaveFile);
  SaveFile >> LastHealed;
  game::SetPetrus(this);
}



void petrus::GetAICommand () {
  int Enemies = 0;
  for (int c = 0; c < game::GetTeams(); ++c) {
    if (GetTeam()->GetRelation(game::GetTeam(c)) == HOSTILE) Enemies += game::GetTeam(c)->GetEnabledMembers();
  }

  if (Enemies && !RAND_N(250 / Min(Enemies, 50))) {
    if (CanBeSeenByPlayer()) ADD_MESSAGE("%s shouts a magnificent prayer to Valpurus.", CHAR_NAME(DEFINITE));
    angel *Angel = angel::Spawn(VALPURUS);
    Angel->SetLifeExpectancy(10000, 0);
    v2 Where = GetLevel()->GetNearestFreeSquare(Angel, GetPos());
    if (Where == ERROR_V2) Where = GetLevel()->GetRandomSquare(Angel);
    Angel->SetTeam(GetTeam());
    Angel->PutTo(Where);
    if (Angel->CanBeSeenByPlayer()) ADD_MESSAGE("%s materializes.", Angel->CHAR_NAME(INDEFINITE));
    EditAP(-1000);
    return;
  }

  if (HP << 1 < MaxHP && (GetPos() - v2(28, 20)).GetLengthSquare() > 400 && !RAND_N(10)) {
    if (CanBeSeenByPlayer()) ADD_MESSAGE("%s disappears.", CHAR_NAME(DEFINITE));
    GetLevel()->GetLSquare(28, 20)->KickAnyoneStandingHereAway();
    Move(v2(28, 20), true);
    EditAP(-1000);
    return;
  }

  if (!LastHealed || game::GetTick()-LastHealed > 16384) {
    for (int d = 0; d < GetNeighbourSquares(); ++d) {
      square *Square = GetNeighbourSquare(d);
      if (Square) {
        character *Char = Square->GetCharacter();
        if (Char && GetRelation(Char) == FRIEND && HealFully(Char)) return;
      }
    }
  }

  StandIdleAI();
}



void petrus::CreateCorpse (lsquare *Square) {
  if (game::GetStoryState() == 2) game::GetTeam(ATTNAM_TEAM)->SetRelation(game::GetTeam(PLAYER_TEAM), FRIEND);
  Square->AddItem(leftnutofpetrus::Spawn());
  SendToHell();
}



void petrus::BeTalkedTo () {
  if (GetRelation(PLAYER) == HOSTILE) {
    ADD_MESSAGE("Heretic! Dev/null is a place not worthy to receive thee!");
    return;
  }

  if (PLAYER->HasGoldenEagleShirt()) {
    ADD_MESSAGE("Petrus smiles. \"Thou hast defeated Oree! Mayst thou be blessed by Valpurus for the rest of thy life! And thou possess the Shirt of the Golden Eagle, the symbol of Our status! Return it now, please.\"");
    if (game::TruthQuestion(CONST_S("Will you give the Shirt of the Golden Eagle to Petrus? [y/n]"), REQUIRES_ANSWER)) {
      game::TextScreen(CONST_S(
        "The Holy Shirt is returned to its old owner and you kneel down to receive your reward.\n"
        "Petrus taps your shoulder with the Justifier and raises you to nobility."));
      if (true || game::TruthQuestion(CONST_S("Do you want to become a duke? [y/n]"), REQUIRES_ANSWER)) {
        game::TextScreen(CONST_S(
          "Later you receive a small dukedom in the middle of tundra\n"
          "where you rule with justice till the end of your content life.\n\n"
          "You are victorious!"));
        game::GetCurrentArea()->SendNewDrawRequest();
        game::DrawEverything();
        PLAYER->ShowAdventureInfo();
        festring Msg = CONST_S("retrieved the Shirt of the Golden Eagle and was raised to nobility");
        AddScoreEntry(Msg, 4, false);
        game::End(Msg);
      } else {
        GetArea()->SendNewDrawRequest();
      }
      return;
    }
    ADD_MESSAGE("Petrus's face turns red. \"I see. Thy greed hath overcome thy wisdom. Then, we shall fight for the shiny shirt. May Valpurus bless him who is better.\"");
    /* And now we actually make his face change color ;-) */
    GetHead()->GetMainMaterial()->SetSkinColor(MakeRGB16(255, 75, 50));
    GetHead()->UpdatePictures();
    SendNewDrawRequest();
    game::AskForEscPress(CONST_S("You are attacked!"));
    PLAYER->GetTeam()->Hostility(GetTeam());
    game::SetStoryState(2);
    return;
  }

  if (PLAYER->HasHeadOfElpuri()) {
    game::TextScreen(CONST_S(
      "You have slain Elpuri, and Petrus grants you the freedom you desire.\n"
      "You spend the next months in Attnam as an honored hero."));
    if (true || game::TruthQuestion(CONST_S("Do you want to sail beyond the sunset? [y/n]"), REQUIRES_ANSWER)) {
      game::TextScreen(CONST_S(
        "When the sea finally melts, you board the first ship,\n"
        "leaving your past forever behind.\n\n"
        "You are victorious!"));
      game::GetCurrentArea()->SendNewDrawRequest();
      game::DrawEverything();
      PLAYER->ShowAdventureInfo();
      festring Msg = CONST_S("defeated Elpuri and continued to further adventures");
      AddScoreEntry(Msg, 2, false);
      game::End(Msg);
    } else {
      GetArea()->SendNewDrawRequest();
    }
    return;
  }

  if (!game::GetStoryState()) {
    if (PLAYER->RemoveEncryptedScroll()) {
      game::TextScreen(CONST_S(
        "You kneel down and bow before the high priest and hand him the encrypted scroll.\n"
        "Petrus raises his arm, the scroll glows yellow, and lo! The letters are clear and\n"
        "readable. Petrus asks you to voice them aloud. The first two thousand words praise\n"
        "Valpurus the Creator and all His manifestations and are followed by a canticle of\n"
        "Saint Petrus the Lion-Hearted lasting roughly three thousand words. Finally there\n"
        "are some sentences actually concerning your mission:\n\n"
        "\"Alas, I fear dirty tongues have spread lies to my Lord's ears. I assure all tales\n"
        "of treasures here in New Attnam are but mythic legends. There is nothing of value here.\n"
        "The taxes are already an unbearable burden and I can't possibly pay more. However I do\n"
        "not question the wisdom of the government's decisions. I will contribute what I can:\n"
        "the ostriches will deliver an extra 10000 bananas to the capital and additionally the\n"
        "slave that brought the message will henceforth be at Your disposal. I am certain this\n"
        "satisfies the crown's needs.\"\n\n"
        "\"Yours sincerely,\n"
        "Richel Decos, the viceroy of New Attnam\""));
      game::TextScreen(CONST_S(
        "You almost expected the last bit. Petrus seems to be deep in his thoughts and you\n"
        "wonder what shape your destiny is taking in his mind. Suddenly he seems to return\n"
        "to this reality and talks to you.\n\n"
        "\"Oh, thou art still here. We were just discussing telepathically with Sir Galladon.\n"
        "We started doubting Decos's alleged poverty a while back when he bought a couple of\n"
        "medium-sized castles nearby. Thy brethren from New Attnam have also told Us about\n"
        "vast riches seized from them. Our law says all such stolen valuables belong to\n"
        "the Cathedral's treasury, so this is a severe claim. However, proof is needed,\n"
        "and even if such was provided, We couldn't send soldiers over the snow fields\n"
        "ere spring.\""));
      if (game::GetLiberator()) {
        //TODO: do something?
        game::TextScreen(CONST_S(
          "\"Thy brethren from New Attnam have also told Us that Decos dies while sleeping.\n"
          "While We are sad to hear this, We hope that We shall not be forced to send our\n"
          "soldiers to New Attnam. We are sure that Our people there will work even better now.\""));
      }
      game::TextScreen(CONST_S(
        "\"However, since thou now servest Us, We ought to find thee something to do. Sir\n"
        "Galladon hath told Us his agents witnessed thou leaving the dreaded underwater tunnel.\n"
        "This means thou most likely hast defeated genetrix vesana and art a talented warrior.\n"
        "We happen to have a task perfect for such a person. An evil dark frog named Elpuri who\n"
        "hates Valpurus and Attnam more than anything hath taken control over an abandoned mine\n"
        "nearby. It is pestering our fine city in many ways and reconnaissance has reported an\n"
        "army of monsters gathering in the cave. Our guards are not trained to fight underground\n"
        "and We dare not send them. To make things worse, someone hath recently stolen Us the\n"
        "greatest armor in existence - the Shirt of the Golden Eagle. Elpuri cannot wear\n"
        "it but he who can is now nearly immortal.\"\n\n"
        "\"We have marked the location of the gloomy cave on thy world map. We want you to dive\n"
        "into it and slay the vile frog. Bring Us its head and We reward thee with freedom.\n"
        "Shouldst thou also find the Shirt, We'll knight thee. Good luck, and return when\n"
        "thou hast succeeded.\""));
      game::LoadWorldMap();
      v2 ElpuriCavePos = game::GetWorldMap()->GetEntryPos(0, ELPURI_CAVE);
      game::GetWorldMap()->GetWSquare(ElpuriCavePos)->ChangeOWTerrain(elpuricave::Spawn());
      game::GetWorldMap()->RevealEnvironment(ElpuriCavePos, 1);
      game::SaveWorldMap();
      GetArea()->SendNewDrawRequest();
      ADD_MESSAGE("\"And by the way, visit the librarian. He might have advice for thee.\"");
      game::SetStoryState(1);
    } else {
      ADD_MESSAGE("\"Yes, citizen? We are quite busy now, thou shalt not disturb Us without proper cause.\"");
    }
    return;
  }
  /* StoryState == 1 */
  ADD_MESSAGE("Petrus says: \"Bring me the head of Elpuri and we'll talk.\"");
}



truth petrus::MoveTowardsHomePos () {
  if (GetPos() != v2(28, 20)) {
    if (CanBeSeenByPlayer()) ADD_MESSAGE("%s disappears.", CHAR_NAME(DEFINITE));
    GetLevel()->GetLSquare(28, 20)->KickAnyoneStandingHereAway();
    Move(v2(28, 20), true);
    if (CanBeSeenByPlayer()) ADD_MESSAGE("%s appears.", CHAR_NAME(DEFINITE));
    EditAP(-1000);
    return true;
  }
  return false;
}
#endif
