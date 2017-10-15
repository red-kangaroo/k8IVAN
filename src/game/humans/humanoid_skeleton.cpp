#ifdef HEADER_PHASE
CHARACTER(skeleton, humanoid)
{
 public:
  virtual void BeTalkedTo ();
  virtual item* SevereBodyPart (int, truth = false, stack* = 0);
  virtual truth BodyPartIsVital (int) const;
  virtual sLong GetBodyPartVolume (int) const;
 protected:
  virtual void CreateCorpse (lsquare *);
};


#else


truth skeleton::BodyPartIsVital (int I) const { return (I == GROIN_INDEX || I == TORSO_INDEX); }


void skeleton::CreateCorpse (lsquare *Square) {
  if (GetHead()) {
    item* Skull = SevereBodyPart(HEAD_INDEX, false, Square->GetStack());
    Square->AddItem(Skull);
  }

  int Amount = 2+(RAND()&3);

  for (int c = 0; c < Amount; ++c) Square->AddItem(bone::Spawn());

  SendToHell();
}


void skeleton::BeTalkedTo () {
  if (GetHead()) {
    humanoid::BeTalkedTo();
  } else {
    ADD_MESSAGE("The headless %s remains silent.", PLAYER->CHAR_DESCRIPTION(UNARTICLED));
  }
}


item *skeleton::SevereBodyPart (int BodyPartIndex, truth ForceDisappearance, stack* EquipmentDropStack) {
       if (BodyPartIndex == RIGHT_ARM_INDEX) EnsureCurrentSWeaponSkillIsCorrect(CurrentRightSWeaponSkill, 0);
  else if (BodyPartIndex == LEFT_ARM_INDEX) EnsureCurrentSWeaponSkillIsCorrect(CurrentLeftSWeaponSkill, 0);

  item *BodyPart = GetBodyPart(BodyPartIndex);
  item *Bone = 0;

  if (!ForceDisappearance) {
    if (BodyPartIndex == HEAD_INDEX) {
      if (GetConfig() == WAR_LORD)
        Bone = skullofxinroch::Spawn(0, NO_MATERIALS);
      else
        Bone = skull::Spawn(0, NO_MATERIALS);
    } else {
      Bone = bone::Spawn(0, NO_MATERIALS);
    }

    Bone->InitMaterials(BodyPart->GetMainMaterial());
    BodyPart->DropEquipment(EquipmentDropStack);
    BodyPart->RemoveFromSlot();
    BodyPart->SetMainMaterial(0, NO_PIC_UPDATE|NO_SIGNALS);
  } else {
    BodyPart->DropEquipment(EquipmentDropStack);
    BodyPart->RemoveFromSlot();
  }

  BodyPart->SendToHell();
  CalculateAttributeBonuses();
  CalculateBattleInfo();
  SignalPossibleTransparencyChange();
  RemoveTraps(BodyPartIndex);
  return Bone;
}


sLong skeleton::GetBodyPartVolume (int I) const {
  switch (I) {
    case HEAD_INDEX: return 600;
    case TORSO_INDEX: return (GetTotalVolume() - 600) * 13 / 30;
    case RIGHT_ARM_INDEX:
    case LEFT_ARM_INDEX: return (GetTotalVolume() - 600) / 10;
    case GROIN_INDEX: return (GetTotalVolume() - 600) / 10;
    case RIGHT_LEG_INDEX:
    case LEFT_LEG_INDEX: return ((GetTotalVolume() - 600) << 1) / 15;
  }

  ABORT("Illegal humanoid bodypart volume request!");
  return 0;
}

#endif
