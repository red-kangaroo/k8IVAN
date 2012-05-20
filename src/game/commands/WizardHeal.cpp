COMMAND(WizardHeal) {
  /*
  truth newbp = game::TruthQuestion(CONST_S("Do you want completely new body? [y/N]"));
  if (newbp) {
    dbp(Char, LEFT_ARM_INDEX);
    //dbp(Char, RIGHT_ARM_INDEX);
    //dbp(Char, LEFT_LEG_INDEX);
    //dbp(Char, RIGHT_LEG_INDEX);
    //dbp(Char, HEAD_INDEX);
    //dbp(Char, TORSO_INDEX);
  }
  */
  for (int c = 0; c < Char->GetBodyParts(); ++c) {
    if (Char->GetBodyPart(c)) {
      Char->GetBodyPart(c)->RemoveAllFluids();
    } else if (Char->CanCreateBodyPart(c)) {
      const std::list<feuLong> &obp = Char->GetOriginalBodyPartID(c);
      //
      for (std::list<feuLong>::const_iterator i = obp.begin(); i != obp.end(); ++i) {
        bodypart *OldBodyPart = static_cast<bodypart *>(PLAYER->SearchForItem(*i));
        //
        if (OldBodyPart && OldBodyPart->CanRegenerate()) {
          OldBodyPart->RemoveAllFluids();
          OldBodyPart->SetHP(1);
          OldBodyPart->RemoveFromSlot();
          Char->AttachBodyPart(OldBodyPart);
          break;
        } else {
          Char->CreateBodyPart(c);
          Char->GetBodyPart(c)->SetHP(1);
          break;
        }
      }
    }
  }
  //
  if (Char->TemporaryStateIsActivated(POISONED)) Char->DeActivateTemporaryState(POISONED);
  if (Char->TemporaryStateIsActivated(PARASITIZED)) Char->DeActivateTemporaryState(PARASITIZED);
  if (Char->TemporaryStateIsActivated(LEPROSY)) Char->DeActivateTemporaryState(LEPROSY);
  if (Char->TemporaryStateIsActivated(LYCANTHROPY)) Char->DeActivateTemporaryState(LYCANTHROPY);
  if (Char->TemporaryStateIsActivated(VAMPIRISM)) Char->DeActivateTemporaryState(VAMPIRISM);
  //
  ADD_MESSAGE("Wizard healing...");
  return false;
};
