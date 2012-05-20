#ifdef HEADER_PHASE
CHARACTER(doctor, humanoid)
{
protected:
  virtual void BeTalkedTo ();
};


#else



void doctor::BeTalkedTo () {
  if (GetRelation(PLAYER) == HOSTILE) {
    ADD_MESSAGE("\"Bacillus! I surgically detach your every limb!\"");
    return;
  }
  //
  for (int c = 0; c < PLAYER->GetBodyParts(); ++c) {
    if (!PLAYER->GetBodyPart(c) && PLAYER->CanCreateBodyPart(c)) {
      truth HasOld = false;
      for (std::list<feuLong>::const_iterator i = PLAYER->GetOriginalBodyPartID(c).begin(); i != PLAYER->GetOriginalBodyPartID(c).end(); ++i) {
        bodypart *OldBodyPart = static_cast<bodypart *>(PLAYER->SearchForItem(*i));
        if (OldBodyPart) {
          HasOld = true;
          sLong Price = (2*GetAttribute(CHARISMA)) < PLAYER->GetAttribute(CHARISMA) ? 5 : (2*GetAttribute(CHARISMA)-PLAYER->GetAttribute(CHARISMA));
          if (PLAYER->GetMoney() >= Price) {
            if (GetAttribute(INTELLIGENCE) < OldBodyPart->GetMainMaterial()->GetIntelligenceRequirement() && !OldBodyPart->CanRegenerate()) {
              ADD_MESSAGE("\"I no smart enough to put back bodyparts made of %s, especially not your severed %s.\"", OldBodyPart->GetMainMaterial()->GetName(false, false).CStr(), PLAYER->GetBodyPartName(c).CStr());
            } else {
              ADD_MESSAGE("\"I could put your old %s back in exchange for %d gold pieces, yes yes.\"", PLAYER->GetBodyPartName(c).CStr(), Price);
              if (game::TruthQuestion(CONST_S("Do you agree? [y/N]"))) {
                OldBodyPart->SetHP(1);
                PLAYER->SetMoney(PLAYER->GetMoney()-Price);
                SetMoney(GetMoney()+Price);
                OldBodyPart->RemoveFromSlot();
                PLAYER->AttachBodyPart(OldBodyPart);
                return;
              }
            }
          } else {
            ADD_MESSAGE("\"Oh my! Your %s severed! Help yourself and get %dgp and Party help you too.\"", PLAYER->GetBodyPartName(c).CStr(), Price);
          }
        }
      }
      // okay, doctors cannot summon limbs
      if (!HasOld) {
        ADD_MESSAGE("\"You don't have your original %s with you. A priest or priestess might summon you a new one with the right rituals.\"", PLAYER->GetBodyPartName(c).CStr());
      }
    }
  }
  // remove limb looks like this:
  // first, select limb to detach (ask player this)
  // then detach limb
  // then make player scream, according to endurance level. above 25 endurance, no scream, nor panic
  // if scream, then panic for a duration according to (25 - Endurance)*4 or zero
  // if state confused is activated, then no scream, no panic (makes vodka handy)
  // hand over 5 gold
  if (PLAYER->GetMoney() >= 5) {
    ADD_MESSAGE("\"I can surgically remove one of your limbs in exchange for 5 gold. Flat rate, genuine bargain!\"");
    if (game::TruthQuestion(CONST_S("Do you agree? [y/N]"))) {
      PLAYER->SetMoney(PLAYER->GetMoney()-5);
      SetMoney(GetMoney()+5);
      PLAYER->SurgicallyDetachBodyPart();
      if ((PLAYER->GetAttribute(ENDURANCE) <= 24) && !PLAYER->StateIsActivated(CONFUSED)) {
        PLAYER->BeginTemporaryState(PANIC, 500+RAND()%4*(25-PLAYER->GetAttribute(ENDURANCE)));
        ADD_MESSAGE("You let out a gut-wrenching scream of agony!");
      }
      return;
    }
  }
  // cure poison
  if (PLAYER->TemporaryStateIsActivated(POISONED)) {
    sLong Price = GetAttribute(CHARISMA) < PLAYER->GetAttribute(CHARISMA) ? 5 : (GetAttribute(CHARISMA)-PLAYER->GetAttribute(CHARISMA));
    if (PLAYER->GetMoney() >= Price) {
      ADD_MESSAGE("\"You seem to be rather ill, yes yes. I give you small dose of antidote for %d gold pieces.\"", Price);
      if (game::TruthQuestion(CONST_S("Do you agree? [y/N]"))) {
        ADD_MESSAGE("You feel better.");
        PLAYER->DeActivateTemporaryState(POISONED);
        PLAYER->SetMoney(PLAYER->GetMoney()-Price);
        SetMoney(GetMoney()+Price);
        return;
      }
    } else {
      ADD_MESSAGE("\"You seem to be rather ill. Get %d gold pieces and I fix that.\"", Price);
    }
  }
  // cure leprosy
  if (PLAYER->TemporaryStateIsActivated(LEPROSY)) {
    sLong Price = GetAttribute(CHARISMA) < PLAYER->GetAttribute(CHARISMA) ? 5 : (GetAttribute(CHARISMA)-PLAYER->GetAttribute(CHARISMA));
    if (PLAYER->GetMoney() >= Price) {
      ADD_MESSAGE("\"You seem to have contracted vile disease of leprosy, yes yes. I can give you small dose of medicince for %d gold pieces.\"", Price);
      if (game::TruthQuestion(CONST_S("Do you agree? [y/N]"))) {
        ADD_MESSAGE("You feel better.");
        PLAYER->DeActivateTemporaryState(LEPROSY);
        PLAYER->SetMoney(PLAYER->GetMoney()-Price);
        SetMoney(GetMoney()+Price);
        return;
      }
    } else {
      ADD_MESSAGE("\"You seem to be falling apart. Get %d gold pieces and I fix that.\"", Price);
    }
  }
  // cure lycanthropy
  if (PLAYER->TemporaryStateIsActivated(LYCANTHROPY)) {
    sLong Price = GetAttribute(CHARISMA) < PLAYER->GetAttribute(CHARISMA) ? 5 : (GetAttribute(CHARISMA)-PLAYER->GetAttribute(CHARISMA));
    if (PLAYER->GetMoney() >= Price) {
      ADD_MESSAGE("\"You seem to be turning into a werewolf quite frequently. Party must crush epidemic, special priority! If you wish to pay %dgp, I can remove the canine blood from your veins with acupucnture, yes yes.\"", Price);
      if (game::TruthQuestion(CONST_S("Do you agree? [y/N]"))) {
        ADD_MESSAGE("You feel better.");
        PLAYER->DeActivateTemporaryState(LYCANTHROPY);
        PLAYER->SetMoney(PLAYER->GetMoney()-Price);
        SetMoney(GetMoney()+Price);
        return;
      }
    } else {
      ADD_MESSAGE("\"You seem to be lycanthropic. I might be able to do something for that but I need %dgp for the Party first.\"", Price);
    }
  }
  // cure vampirism
  if (PLAYER->TemporaryStateIsActivated(VAMPIRISM)) {
    sLong Price = GetAttribute(CHARISMA) < PLAYER->GetAttribute(CHARISMA) ? 5 : (GetAttribute(CHARISMA)-PLAYER->GetAttribute(CHARISMA));
    if (PLAYER->GetMoney() >= Price) {
      ADD_MESSAGE("\"You seem to have addiction to drinking blood. Party must crush epidemic, special priority! If you wish to pay %dgp, I can remove vampiric urges with acupucnture, yes yes.\"", Price);
      if (game::TruthQuestion(CONST_S("Do you agree? [y/N]"))) {
        ADD_MESSAGE("You feel better.");
        PLAYER->DeActivateTemporaryState(VAMPIRISM);
        PLAYER->SetMoney(PLAYER->GetMoney()-Price);
        SetMoney(GetMoney()+Price);
        return;
      }
    } else {
      ADD_MESSAGE("\"You seem to be vampiric. I might be able to do something for that but I need %dgp for the Party first.\"", Price);
    }
  }
  humanoid::BeTalkedTo();
}
#endif
