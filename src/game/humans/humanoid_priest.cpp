#ifdef HEADER_PHASE
CHARACTER(priest, humanoid)
{
protected:
  virtual void GetAICommand () { StandIdleAI(); }
  virtual void BeTalkedTo ();
};


#else


void priest::BeTalkedTo () {
  if (GetRelation(PLAYER) == HOSTILE) {
    ADD_MESSAGE("\"Sinner! My hands shall pour Divine Wrath upon thee!\"");
    return;
  }
  for (int c = 0; c < PLAYER->GetBodyParts(); ++c) {
    if (!PLAYER->GetBodyPart(c) && PLAYER->CanCreateBodyPart(c)) {
      truth HasOld = false;
      for (std::list<feuLong>::const_iterator i = PLAYER->GetOriginalBodyPartID(c).begin(); i != PLAYER->GetOriginalBodyPartID(c).end(); ++i) {
        bodypart *OldBodyPart = static_cast<bodypart *>(PLAYER->SearchForItem(*i));
        if (OldBodyPart) {
          HasOld = true;
          sLong Price = GetConfig() == VALPURUS ? 50 : 10;
          if (PLAYER->GetMoney() >= Price) {
            if (!OldBodyPart->CanRegenerate())
              ADD_MESSAGE("\"Sorry, I cannot put back bodyparts made of %s, not even your severed %s.\"", OldBodyPart->GetMainMaterial()->GetName(false, false).CStr(), PLAYER->GetBodyPartName(c).CStr());
            else {
              ADD_MESSAGE("\"I could put your old %s back in exchange for %d gold.\"", PLAYER->GetBodyPartName(c).CStr(), Price);
              if (game::TruthQuestion(CONST_S("Do you agree?"))) {
                OldBodyPart->SetHP(1);
                PLAYER->SetMoney(PLAYER->GetMoney()-Price);
                SetMoney(GetMoney()+Price);
                OldBodyPart->RemoveFromSlot();
                PLAYER->AttachBodyPart(OldBodyPart);
                return;
              }
            }
          } else {
            ADD_MESSAGE("\"Your %s is severed. Help yourself and get %dgp and I'll help you too.\"", PLAYER->GetBodyPartName(c).CStr(), Price);
          }
        }
      }
      sLong Price = GetConfig() == VALPURUS ? 100 : 20;
      if (PLAYER->GetMoney() >= Price) {
        if (HasOld)
          ADD_MESSAGE("\"I could still summon up a new one for %d gold.\"", Price);
        else
          ADD_MESSAGE("\"Since you don't seem to have your original %s with you, I could summon up a new one for %d gold.\"", PLAYER->GetBodyPartName(c).CStr(), Price);
        if (game::TruthQuestion(CONST_S("Agreed?"))) {
          PLAYER->SetMoney(PLAYER->GetMoney()-Price);
          SetMoney(GetMoney()+Price);
          PLAYER->CreateBodyPart(c);
          PLAYER->GetBodyPart(c)->SetHP(1);
          return;
        }
      } else if (!HasOld) {
        ADD_MESSAGE("\"You don't have your original %s with you. I could create you a new one, but my Divine Employer is not a communist and you need %dgp first.\"", PLAYER->GetBodyPartName(c).CStr(), Price);
      }
    }
  }
  if (PLAYER->TemporaryStateIsActivated(POISONED)) {
    sLong Price = GetConfig() == VALPURUS ? 25 : 5;
    if (PLAYER->GetMoney() >= Price) {
      ADD_MESSAGE("\"You seem to be rather ill. I could give you a small dose of antidote for %d gold pieces.\"", Price);
      if (game::TruthQuestion(CONST_S("Do you agree?"))) {
        ADD_MESSAGE("You feel better.");
        PLAYER->DeActivateTemporaryState(POISONED);
        PLAYER->SetMoney(PLAYER->GetMoney()-Price);
        SetMoney(GetMoney()+Price);
        return;
      }
    } else {
      ADD_MESSAGE("\"You seem to be rather ill. Get %d gold pieces and I'll fix that.\"", Price);
    }
  }
  if (PLAYER->TemporaryStateIsActivated(LEPROSY)) {
    sLong Price = GetConfig() == VALPURUS ? 100 : 20;
    if (PLAYER->GetMoney() >= Price) {
      ADD_MESSAGE("\"You seem to have contracted the vile disease of leprosy. I could give you a small dose of medicince for %d gold pieces.\"", Price);
      if (game::TruthQuestion(CONST_S("Do you agree?"))) {
        ADD_MESSAGE("You feel better.");
        PLAYER->DeActivateTemporaryState(LEPROSY);
        PLAYER->SetMoney(PLAYER->GetMoney()-Price);
        SetMoney(GetMoney()+Price);
        return;
      }
    } else {
      ADD_MESSAGE("\"You seem to be falling apart. Get %d gold pieces and I'll fix that.\"", Price);
    }
  }
  if (PLAYER->TemporaryStateIsActivated(LYCANTHROPY)) {
    sLong Price = GetConfig() == VALPURUS ? 100 : 20;
    if (PLAYER->GetMoney() >= Price) {
      ADD_MESSAGE("\"You seem to be turning into a werewolf quite frequently. Well, everyone has right to little secret habits, but if you wish to donate %dgp to %s, maybe I could pray %s to remove the canine blood from your veins, just so you don't scare our blessed youth.\"", Price, GetMasterGod()->GetName(), GetMasterGod()->GetObjectPronoun());
      if (game::TruthQuestion(CONST_S("Do you agree?"))) {
        ADD_MESSAGE("You feel better.");
        PLAYER->DeActivateTemporaryState(LYCANTHROPY);
        PLAYER->SetMoney(PLAYER->GetMoney()-Price);
        SetMoney(GetMoney()+Price);
        return;
      }
    } else {
      ADD_MESSAGE("\"You seem to be lycanthropic. I might be able to do something for that but I need %dgp for the ritual materials first.\"", Price);
    }
  }
  if (PLAYER->TemporaryStateIsActivated(VAMPIRISM)) {
    sLong Price = GetConfig() == VALPURUS ? 100 : 20;
    if (PLAYER->GetMoney() >= Price) {
      ADD_MESSAGE("\"You seem to have an addiction to drinking blood. Well, everyone has right to little secret habits, but if you wish to donate %dgp to %s, maybe I could pray %s to remove your vampiric urges, just so you don't victimize our besotted youth.\"", Price, GetMasterGod()->GetName(), GetMasterGod()->GetObjectPronoun());
      if (game::TruthQuestion(CONST_S("Do you agree?"))) {
        ADD_MESSAGE("You feel better.");
        PLAYER->DeActivateTemporaryState(VAMPIRISM);
        PLAYER->SetMoney(PLAYER->GetMoney()-Price);
        SetMoney(GetMoney()+Price);
        return;
      }
    } else {
      ADD_MESSAGE("\"You seem to be vampiric. I might be able to do something for that but I need %dgp for the ritual materials first.\"", Price);
    }
  }
  humanoid::BeTalkedTo();
}


#endif
