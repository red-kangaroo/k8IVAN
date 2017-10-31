#ifdef HEADER_PHASE
ITEM(eptyron, meleeweapon)
{
 public:
  virtual truth HitEffect(character*, character*, v2, int, int, truth);
  virtual void BlockEffect(character*, character*, item*, int Type);
  virtual truth AllowAlphaEverywhere() const { return true; }
  //virtual void Break(character*, int); //should it have a break effect??
 protected:
  virtual int GetClassAnimationFrames() const;
  virtual col16 GetOutlineColor(int) const;
  virtual alpha GetOutlineAlpha(int) const;
};


#else



int eptyron::GetClassAnimationFrames() const { return !IsBroken() ? 128 : 1; }



truth eptyron::HitEffect(character* Enemy, character* Hitter, v2 HitPos, int BodyPartIndex, int Direction, truth BlockedByArmour)
{
  truth BaseSuccess = meleeweapon::HitEffect(Enemy, Hitter, HitPos, BodyPartIndex, Direction, BlockedByArmour);
  int Index = 0;


  if(!IsBroken() && Enemy->IsEnabled() && Enemy->IsHumanoid() /*&& !(RAND() % 2)*/)
  {
    //bodypart* BodyPartThatWasHit = Enemy -> GetBodyPart(BodyPartIndex);
    item* MainArmor = 0;

    switch(BodyPartIndex)
    {
      case TORSO_INDEX:
      MainArmor = Enemy->GetEquipment(BODY_ARMOR_INDEX);
      break;
      case HEAD_INDEX:
      MainArmor = Enemy->GetEquipment(HELMET_INDEX);
      break;
      case RIGHT_ARM_INDEX:
      MainArmor = Enemy->GetEquipment(RIGHT_GAUNTLET_INDEX);
      //BodyPartThatWasHit = Enemy -> GetBodyPart(BodyPartIndex);
      break;
      case LEFT_ARM_INDEX:
      MainArmor = Enemy->GetEquipment(LEFT_GAUNTLET_INDEX);
      //BodyPartThatWasHit = Enemy -> GetBodyPart(BodyPartIndex);
      break;
      case GROIN_INDEX:
      MainArmor = Enemy->GetEquipment(BELT_INDEX);
      break;
      case RIGHT_LEG_INDEX:
      MainArmor = Enemy->GetEquipment(RIGHT_BOOT_INDEX);
      //BodyPartThatWasHit = Enemy -> GetBodyPart(BodyPartIndex);
      break;
      case LEFT_LEG_INDEX:
      MainArmor = Enemy->GetEquipment(LEFT_BOOT_INDEX);
      //BodyPartThatWasHit = Enemy -> GetBodyPart(BodyPartIndex);
      break;
    }

    if(MainArmor)
    {
      material* OldMaterial = MainArmor->GetMainMaterial();
      int OldModifier = OldMaterial->GetHardenModifier(MainArmor);
      materialvector MaterialVector;
      protosystem::CreateEveryMaterial(MaterialVector);
      truth Changed = false;
      int * CandidateMaterialIndex;
      CandidateMaterialIndex = new int[MaterialVector.size()];

      festring Desc;
      MainArmor->AddName(Desc, UNARTICLED);

      for(unsigned int c = 0; c < MaterialVector.size(); ++c)
      {
        if((MaterialVector[c]->GetCommonFlags() & CAN_BE_WISHED)
          && (MaterialVector[c]->GetConsumeType() & OldMaterial->GetConsumeType()) )
        {
          material* Material = MaterialVector[c];

          if(Material->GetHardenModifier(MainArmor) < OldModifier
            /*&& Material->GetRawPrice() < OldMaterial->GetRawPrice()*/
            && ((Material->GetCommonFlags() & IS_VALUABLE) == (OldMaterial->GetCommonFlags() & IS_VALUABLE)) //Hint: ie only valuable can entropy to valuable, and non-valuable to non-valuable
            && ( 10*(1 + OldMaterial->GetIntelligenceRequirement() - Material->GetIntelligenceRequirement()) < (Hitter->GetAttribute(INTELLIGENCE))*(Hitter->GetSWeaponSkillLevel(this)) ) )
          {
            CandidateMaterialIndex[Index++] = c;
          }
        }
      }
      if(Index)
      {
        int Chosen = CandidateMaterialIndex[(RAND() % Index)];
        material* ChosenMaterial = MaterialVector[Chosen];
        MainArmor->ChangeMainMaterial(ChosenMaterial->SpawnMore());
        if(ChosenMaterial != NONE)
        {
          if(Hitter->IsPlayer())
          {
            ADD_MESSAGE("%s's %s softens into %s!", Enemy->CHAR_DESCRIPTION(DEFINITE), Desc.CStr() , ChosenMaterial->GetName(false, false).CStr());
          }
          else
            ADD_MESSAGE("Your %s softens into %s!", Desc.CStr(), ChosenMaterial->GetName(false, false).CStr());
          Changed = true;
        }
        else
        {
          Changed = false;
        }
      }
      delete[] CandidateMaterialIndex;

      for(unsigned int c = 0; c < MaterialVector.size(); ++c) delete MaterialVector[c];
      if(!Changed) {
        if(Hitter->IsPlayer())
        {
          ADD_MESSAGE("%s's %s vibrates slightly but remains unchanged.", Enemy->CHAR_DESCRIPTION(DEFINITE), MainArmor->CHAR_NAME(UNARTICLED) );
        }
        else
          ADD_MESSAGE("Your %s vibrates slightly but remains unchanged.", MainArmor->CHAR_NAME(UNARTICLED) );
      }
      return Changed;
    }

    /*this bit softens enemy limbs!!! but pictures do not update :( */
    //if(BodyPartThatWasHit && !(Enemy -> BodyPartIsVital(BodyPartIndex)) )
    //{
    //  material* OldMaterial = BodyPartThatWasHit->GetMainMaterial();
    //  int OldModifier = OldMaterial->GetHardenModifier(BodyPartThatWasHit);
    //  materialvector MaterialVector;
    //  protosystem::CreateEveryMaterial(MaterialVector);
    //  truth Changed = false;

////      for(c = 0; c < MaterialVector.size(); ++c)
    //  {
    //    if(MaterialVector[c]->GetCommonFlags() & CAN_BE_WISHED)
    //    {
    //      material* Material = MaterialVector[c];

////          if(Material->GetHardenModifier(BodyPartThatWasHit) < OldModifier
    //        /*&& !RAND_N(Max(Material->GetIntelligenceRequirement() - 15, 1))*/)
    //      {
    //        BodyPartThatWasHit->ChangeMainMaterial(Material->SpawnMore());
    //        if(Hitter->IsPlayer())
    //        {
    //          ADD_MESSAGE("%s %s softens into %s!", Enemy->CHAR_POSSESSIVE_PRONOUN, BodyPartThatWasHit->GetBodyPartName().CStr() , Material->GetName(false, false).CStr());
    //        }
    //        else
    //          ADD_MESSAGE("Your %s softens into %s!", BodyPartThatWasHit->GetBodyPartName().CStr(), Material->GetName(false, false).CStr());
    //        Changed = true;
    //        break;
    //      }
    //    }
    //  }
    //  for(c = 0; c < MaterialVector.size(); ++c)
    //    delete MaterialVector[c];
    //  if(!Changed)
    //    if(Hitter->IsPlayer())
    //    {
    //      ADD_MESSAGE("%s %s vibrates slightly but remains unchanged.", Enemy->CHAR_POSSESSIVE_PRONOUN, BodyPartThatWasHit->GetBodyPartName().CStr() );
    //    }
    //    else
    //      ADD_MESSAGE("Your %s vibrates slightly but remains unchanged.", BodyPartThatWasHit->GetBodyPartName().CStr() );
    //
    //  return Changed;
    //}


    ////if(BlockedByArmour)
    ////{
    //  if(MainArmor)
    //  {
    //    if(MainArmor->IsMaterialChangeable())
    //    {
    //      festring Desc;
    //      MainArmor->AddName(Desc, UNARTICLED);
    //      Desc << " is transformed";
    //
    //      switch(RAND() % 3)
    //      {
    //      case 0:
    //      MainArmor->ChangeMainMaterial(MAKE_MATERIAL(CLOTH));
    //      break;
    //      case 1:
    //      MainArmor->ChangeMainMaterial(MAKE_MATERIAL(PLANT_FIBER));
    //      break;
    //      case 2:
    //      MainArmor->ChangeMainMaterial(MAKE_MATERIAL(PARCHMENT));
    //      break;
    //      }
    //      if(Hitter->IsPlayer())
    //      {
    //      ADD_MESSAGE("%s's %s", Enemy->CHAR_DESCRIPTION(DEFINITE), Desc.CStr());
    //      }
    //      else
    //      ADD_MESSAGE("Your %s", Desc.CStr());
    //    }
    //    else
    //      ADD_MESSAGE("%s's %s emits a strange light but remain unchanged.", Enemy->CHAR_DESCRIPTION(DEFINITE), MainArmor->CHAR_NAME(DEFINITE));
    //  }
    //  return true;
    ////}
    //
    //if(BodyPartThatWasHit && !(Enemy -> BodyPartIsVital(BodyPartIndex)) )
    //{
    //  if(BodyPartThatWasHit->IsMaterialChangeable())
    //  {
    //    if(Hitter->IsPlayer())
    //      {
    //      ADD_MESSAGE("%s's %s softens into banana flesh.", Enemy->CHAR_DESCRIPTION(DEFINITE), BodyPartThatWasHit->GetBodyPartName().CStr() );
    //      }
    //        else
    //        ADD_MESSAGE("Your %s softens into banana flesh.", BodyPartThatWasHit->GetBodyPartName().CStr() );
    //
    //      BodyPartThatWasHit->ChangeMainMaterial(MAKE_MATERIAL(BANANA_FLESH));
    //  }
    //  else
    //    ADD_MESSAGE("%s's %s emits a strange light but remains unchanged.", Enemy->CHAR_DESCRIPTION(DEFINITE), BodyPartThatWasHit->GetBodyPartName().CStr() );
   // }
    //return true;
  }
  return BaseSuccess;
}



void eptyron::BlockEffect(character* Blocker, character* Attacker, item* Weapon, int Type)
{
  int Index = 0;

  if(!IsBroken())
  {
    if(Weapon && !(Weapon->IsWhip()))
    {
      material* OldMaterial = Weapon->GetMainMaterial();
      int OldModifier = OldMaterial->GetHardenModifier(Weapon);
      materialvector MaterialVector;
      protosystem::CreateEveryMaterial(MaterialVector);
      truth Changed = false;
      int * CandidateMaterialIndex;
      CandidateMaterialIndex = new int[MaterialVector.size()];

      festring Desc;
      Weapon->AddName(Desc, UNARTICLED);

      for(unsigned int c = 0; c < MaterialVector.size(); ++c)
      {
        if((MaterialVector[c]->GetCommonFlags() & CAN_BE_WISHED)
          && (MaterialVector[c]->GetConsumeType() & OldMaterial->GetConsumeType()) )
        {
          material* Material = MaterialVector[c];

          if(Material->GetHardenModifier(Weapon) < OldModifier
            && Material->GetRawPrice() < OldMaterial->GetRawPrice()
            && ((Material->GetCommonFlags() & IS_VALUABLE) == (OldMaterial->GetCommonFlags() & IS_VALUABLE)) //ie only valuable can entropy to valuable, and non-valuable to non-valuable
            && ( 10*(1 + OldMaterial->GetIntelligenceRequirement() - Material->GetIntelligenceRequirement()) < (Blocker->GetAttribute(INTELLIGENCE))*(Blocker->GetSWeaponSkillLevel(this)) ) )
          {
            CandidateMaterialIndex[Index++] = c;
          }
        }
      }
      if(Index)
      {
        int Chosen = CandidateMaterialIndex[(RAND() % Index)];
        material* ChosenMaterial = MaterialVector[Chosen];
        Weapon->ChangeMainMaterial(ChosenMaterial->SpawnMore());
        if(ChosenMaterial != NONE)
        {
          if(Blocker->IsPlayer())
          {
            ADD_MESSAGE("%s's %s softens into %s!", Attacker->CHAR_DESCRIPTION(DEFINITE), Desc.CStr() , ChosenMaterial->GetName(false, false).CStr());
          }
          else
            ADD_MESSAGE("Your %s softens into %s!", Desc.CStr(), ChosenMaterial->GetName(false, false).CStr());
          Changed = true;
        }
        else
        {
          Changed = false;
        }
      }
      delete[] CandidateMaterialIndex;

      for(unsigned int c = 0; c < MaterialVector.size(); ++c) delete MaterialVector[c];
      if(!Changed) {
        if(Blocker->IsPlayer())
        {
          ADD_MESSAGE("%s's %s vibrates slightly but remains unchanged.", Attacker->CHAR_DESCRIPTION(DEFINITE), Weapon->CHAR_NAME(UNARTICLED) );
        }
        else
          ADD_MESSAGE("Your %s vibrates slightly but remains unchanged.", Weapon->CHAR_NAME(UNARTICLED) );
      }
      return;
    }
  }
  return;
}



alpha eptyron::GetOutlineAlpha(int Frame) const
{
  if(!IsBroken())
  {
    Frame &= 31;
    return Frame * (31 - Frame) >> 1;
  }
  else
    return 255;
}



col16 eptyron::GetOutlineColor(int Frame) const
{
  if(!IsBroken())
    switch((Frame&127) >> 5)
    {
     case 0: return BLUE;
     case 1: return GREEN;
     case 2: return RED;
     case 3: return YELLOW;
    }

  return TRANSPARENT_COLOR;
}
#endif
