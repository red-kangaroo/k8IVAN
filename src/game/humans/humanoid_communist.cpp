#ifdef HEADER_PHASE
CHARACTER(communist, humanoid)
{
 public:
  virtual truth MoveRandomly();
  virtual void BeTalkedTo();
  virtual truth BoundToUse(citem*, int) const;
  //virtual truth MustBeRemovedFromBone() const;
 protected:
  virtual truth ShowClassDescription() const;
};


#else


truth communist::ShowClassDescription () const { return (GetAssignedName() != "Ivan"); }


void communist::BeTalkedTo () {
  if (GetRelation(PLAYER) != HOSTILE && GetTeam() != PLAYER->GetTeam() && PLAYER->GetRelativeDanger(this, true) > 0.1) {
    ADD_MESSAGE("%s seems to be very friendly. \"%s make good communist. %s go with %s!\"", CHAR_DESCRIPTION(DEFINITE), PLAYER->GetAssignedName().CStr(), CHAR_NAME(UNARTICLED), PLAYER->GetAssignedName().CStr());
    for (std::list<character*>::const_iterator i = GetTeam()->GetMember().begin(); i != GetTeam()->GetMember().end();) {
      if (*i != this) {
        character* Char = *i++;
        Char->ChangeTeam(PLAYER->GetTeam());
        if (GetTeam()->GetMembers() == 1) break; // only Ivan left in Party
      } else {
        ++i;
      }
    }
    ChangeTeam(PLAYER->GetTeam());
  } else if (GetTeam() != PLAYER->GetTeam() && !(RAND() % 5)) {
    ADD_MESSAGE("\"You weak. Learn killing and come back.\"");
  } else {
    character::BeTalkedTo();
  }
}


truth communist::MoveRandomly () {
  switch (RAND()%1000) {
    case 0:
      if (CanBeSeenByPlayer()) ADD_MESSAGE("%s engraves something to the ground.", CHAR_NAME(UNARTICLED));
      Engrave(CONST_S("The bourgeois is a bourgeois -- for the benefit of the working class."));
      return true;
    case 1:
      if (CanBeSeenByPlayer()) ADD_MESSAGE("%s engraves something to the ground.", CHAR_NAME(UNARTICLED));
      Engrave(CONST_S("Proletarians of all countries, unite!"));
      return true;
    case 2:
      if (CanBeSeenByPlayer()) ADD_MESSAGE("%s engraves something to the ground.", CHAR_NAME(UNARTICLED));
      Engrave(CONST_S("Capital is therefore not only personal; it is a social power."));
      return true;
    default:
      return character::MoveRandomly();
  }
}


truth communist::BoundToUse (citem* Item, int I) const {
  return (Item && Item->IsGorovitsFamilyRelic() && Item->IsInCorrectSlot(I));
}


/*
truth communist::MustBeRemovedFromBone () const {
  //return (!IsEnabled() || GetTeam()->GetID() != IVAN_TEAM || GetDungeon()->GetIndex() != ELPURI_CAVE || GetLevel()->GetIndex() != IVAN_LEVEL);
  return (!IsEnabled() || GetTeam()->GetID() != IVAN_TEAM || IsHomeLevel(GetLevel()));
}
*/


#endif
