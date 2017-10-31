#ifdef HEADER_PHASE
ITEM(bone, item)
{
public:
  virtual truth DogWillCatchAndConsume (ccharacter *Doggie) const;
  virtual truth IsABone () const { return true; }
  virtual truth IsBodyPart () const { return true; }
  virtual truth Necromancy (character *Necromancer);
};


#else


truth bone::DogWillCatchAndConsume (ccharacter *Doggie) const {
  return (GetConsumeMaterial(Doggie)->GetConfig() == BONE && !GetConsumeMaterial(Doggie)->GetSpoilLevel());
}


truth bone::Necromancy (character *Necromancer) {
  int NumberOfBones = 0;
  truth HasSkull = false;
  lsquare *LSquareUnder = GetLSquareUnder();

  itemvector ItemVector;
  LSquareUnder->GetStack()->FillItemVector(ItemVector);

  // First count the number of bones, and find a skull
  for (uint c = 0; c < ItemVector.size(); ++c) {
         if (ItemVector[c]->IsABone()) ++NumberOfBones;
    else if (ItemVector[c]->IsASkull() && !HasSkull) HasSkull = true;
  }
  // if we don't have the requisite number of bones, nor a skull then get out of here
  if (NumberOfBones < 5 || HasSkull == false) {
    if (CanBeSeenByPlayer()) ADD_MESSAGE("%s vibrates for some time.", CHAR_NAME(DEFINITE));
    return false;
  }
  NumberOfBones = 5;

  humanoid *Skeleton = skeleton::Spawn(Necromancer->GetAttribute(INTELLIGENCE) < 30 ? 0 : WARRIOR, NO_EQUIPMENT);
  //character *Skeleton = skeleton::CreateSkeleton(Necromancer);

  if (Skeleton) {
    Skeleton->ChangeTeam(Necromancer ? Necromancer->GetTeam() : game::GetTeam(MONSTER_TEAM));
    Skeleton->PutToOrNear(GetPos());

    // then remove the bones, and the skull from the floor
    for (uint c = 0; c < ItemVector.size(); ++c) {
      if (ItemVector[c]->IsABone() && NumberOfBones > 0) {
        ItemVector[c]->RemoveFromSlot();
        ItemVector[c]->SendToHell();
        --NumberOfBones;
      } else if (ItemVector[c]->IsASkull() && HasSkull) {
        ItemVector[c]->RemoveFromSlot();
        ItemVector[c]->SendToHell();
        HasSkull = false;
      }
    }

    if (Skeleton->CanBeSeenByPlayer()) ADD_MESSAGE("%s rises from the ground.", Skeleton->CHAR_DESCRIPTION(INDEFINITE));

    if (Necromancer && Necromancer->IsPlayer()) game::DoEvilDeed(50);

    Skeleton->GetLSquareUnder()->DrawParticles(RED);

    Skeleton->SignalStepFrom(0);
    return true;
  }

  if (CanBeSeenByPlayer()) ADD_MESSAGE("%s vibrates for some time.", CHAR_NAME(DEFINITE));
  return false;
}

#endif
