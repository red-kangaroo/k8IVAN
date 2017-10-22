#ifdef HEADER_PHASE
CHARACTER(exiledpriest, priest)
{
public:
  exiledpriest ();
  virtual ~exiledpriest ();

  virtual void Save (outputfile &) const;
  virtual void Load (inputfile &);
  virtual void BeTalkedTo ();
  virtual truth MoveTowardsHomePos ();
protected:
  virtual void GetAICommand ();
  void healBodyParts ();
  void healDeseases ();
  void healAll ();
protected:
  truth mAtHome;
};


#else



exiledpriest::exiledpriest () :
  exiledpriestsysbase(),
  mAtHome(false)
{
}



exiledpriest::~exiledpriest () {
}



void exiledpriest::Save (outputfile &saveFile) const {
  priest::Save(saveFile);
  saveFile << mAtHome;
}



void exiledpriest::Load (inputfile &saveFile) {
  priest::Load(saveFile);
  saveFile >> mAtHome;
}



void exiledpriest::healBodyParts () {
  for (int c = 0; c < PLAYER->GetBodyParts(); ++c) {
    if (!PLAYER->GetBodyPart(c) && PLAYER->CanCreateBodyPart(c)) {
      truth HasOld = false;
      for (std::list<feuLong>::const_iterator i = PLAYER->GetOriginalBodyPartID(c).begin(); i != PLAYER->GetOriginalBodyPartID(c).end(); ++i) {
        bodypart *OldBodyPart = static_cast<bodypart *>(PLAYER->SearchForItem(*i));
        if (OldBodyPart) {
          HasOld = true;
          if (!OldBodyPart->CanRegenerate())
            ADD_MESSAGE("\"Sorry, I cannot put back bodyparts made of %s, not even your severed %s.\"", OldBodyPart->GetMainMaterial()->GetName(false, false).CStr(), PLAYER->GetBodyPartName(c).CStr());
          else {
            ADD_MESSAGE("\"I will put your old %s back.\"", PLAYER->GetBodyPartName(c).CStr());
            if (game::TruthQuestion(CONST_S("Do you agree?"))) {
              OldBodyPart->SetHP(1);
              OldBodyPart->RemoveFromSlot();
              PLAYER->AttachBodyPart(OldBodyPart);
              healBodyParts(); //FIXME
              return;
            }
          }
        }
      }
      if (HasOld)
        ADD_MESSAGE("\"I could summon up a new %s.\"", PLAYER->GetBodyPartName(c).CStr());
      else
        ADD_MESSAGE("\"Since you don't seem to have your original %s with you, I could summon up a new one.\"", PLAYER->GetBodyPartName(c).CStr());
      if (game::TruthQuestion(CONST_S("Agreed?"))) {
        PLAYER->CreateBodyPart(c);
        PLAYER->GetBodyPart(c)->SetHP(1);
        healBodyParts(); //FIXME
        return;
      }
    }
  }
}



void exiledpriest::healDeseases () {
  if (PLAYER->TemporaryStateIsActivated(POISONED)) {
    ADD_MESSAGE("\"You seem to be rather ill. I could give you a small dose of antidote.\"");
    if (game::TruthQuestion(CONST_S("Do you agree?"))) {
      ADD_MESSAGE("You feel better.");
      PLAYER->DeActivateTemporaryState(POISONED);
    }
  }
  if (PLAYER->TemporaryStateIsActivated(PARASITIZED)) {
    ADD_MESSAGE("\"You seem to have something inside you. I could give you a small dose of antidote.\"");
    if (game::TruthQuestion(CONST_S("Do you agree?"))) {
      ADD_MESSAGE("You feel better.");
      PLAYER->DeActivateTemporaryState(PARASITIZED);
    }
  }
  if (PLAYER->TemporaryStateIsActivated(LEPROSY)) {
    ADD_MESSAGE("\"You seem to have contracted the vile disease of leprosy. I could give you a small dose of medicince.\"");
    if (game::TruthQuestion(CONST_S("Do you agree?"))) {
      ADD_MESSAGE("You feel better.");
      PLAYER->DeActivateTemporaryState(LEPROSY);
    }
  }
  if (PLAYER->TemporaryStateIsActivated(LYCANTHROPY)) {
    ADD_MESSAGE("\"You seem to be turning into a werewolf quite frequently. Well, everyone has right to little secret habits, but I could pray %s to remove the canine blood from your veins, just so you don't scare our blessed youth.\"", GetMasterGod()->GetName());
    if (game::TruthQuestion(CONST_S("Do you agree?"))) {
      ADD_MESSAGE("You feel better.");
      PLAYER->DeActivateTemporaryState(LYCANTHROPY);
    }
  }
}



void exiledpriest::healAll () {
  healBodyParts();
  healDeseases();
}



void exiledpriest::BeTalkedTo () {
  if (GetRelation(PLAYER) != HOSTILE && game::GetLiberator()) {
    if (!mAtHome) {
      ADD_MESSAGE(
        "Priestess says: "
        "\"Let %s bless you! I can return to my temple now! "
        "Get this things and remember how kind %s is!\"",
        GetMasterGod()->GetName(), GetMasterGod()->GetName());
      //GetMasterGod()->GetObjectPronoun());
      mAtHome = true;
      //GetGiftStack()
      {
        potion *bottle = potion::Spawn(0, NO_MATERIALS);
        bottle->InitMaterials(MAKE_MATERIAL(GLASS), MAKE_MATERIAL(HEALING_LIQUID));
        PLAYER->GetStack()->AddItem(bottle);
        ADD_MESSAGE("Priestess gives %s to you.", bottle->CHAR_DESCRIPTION(DEFINITE));
      }
      {
        potion *bottle = potion::Spawn(0, NO_MATERIALS);
        bottle->InitMaterials(MAKE_MATERIAL(GLASS), MAKE_MATERIAL(ANTIDOTE_LIQUID));
        PLAYER->GetStack()->AddItem(bottle);
        ADD_MESSAGE("Priestess gives %s to you.", bottle->CHAR_DESCRIPTION(DEFINITE));
      }
      if (RAND_N(100) >= 90) {
        potion *bottle = potion::Spawn(0, NO_MATERIALS);
        bottle->InitMaterials(MAKE_MATERIAL(GLASS), MAKE_MATERIAL(VODKA));
        PLAYER->GetStack()->AddItem(bottle);
        ADD_MESSAGE("Priestess gives %s to you.", bottle->CHAR_DESCRIPTION(DEFINITE));
      }
    }
    ADD_MESSAGE("I will heal you for free.");
    healAll();
    return;
  }
  priest::BeTalkedTo();
}



truth exiledpriest::MoveTowardsHomePos () {
  if (!mAtHome) return false;
  v2 homePos(12, 10);
  if (GetPos() != homePos) {
    SetGoingTo(homePos);
    return MoveTowardsTarget(false) || (!GetPos().IsAdjacent(homePos) && MoveRandomly());
  }
  return false;
}



void exiledpriest::GetAICommand () {
  priest::GetAICommand();
}
#endif
