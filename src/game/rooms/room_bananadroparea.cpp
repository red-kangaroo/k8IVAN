#ifdef HEADER_PHASE
ROOM(bananadroparea, room) {
public:
  virtual truth PickupItem(character*, item*, int);
  virtual truth DropItem(character*, item*, int);
  virtual void KickSquare(character*, lsquare*);
  virtual truth ConsumeItem(character*, item*, int);
  virtual truth AllowDropGifts() const { return false; }
  virtual void TeleportSquare(character*, lsquare*);
  virtual truth AllowKick(ccharacter*, const lsquare*) const;
  virtual void HostileAction(character*) const;
};


#else



truth bananadroparea::PickupItem (character *Hungry, item *Item, int) {
  if (game::GetTeam(NEW_ATTNAM_TEAM)->GetRelation(Hungry->GetTeam()) == HOSTILE) return true;
  if (Hungry->IsPlayer()) {
    if (!Item->IsBanana() && !Item->IsLanternOnWall()) return true;
    if (game::GetLiberator()) return true;
    ADD_MESSAGE("That would be stealing.");
    if (game::TruthQuestion(CONST_S("Do you still want to do this? [y/N]"))) {
      Hungry->GetTeam()->Hostility(game::GetTeam(NEW_ATTNAM_TEAM));
      return true;
    }
  }
  return false;
}



truth bananadroparea::DropItem (character *Dropper, item *Item, int) {
  return (game::GetTeam(NEW_ATTNAM_TEAM)->GetRelation(Dropper->GetTeam()) == HOSTILE ||
    (Dropper->IsPlayer() &&
     ((!Item->IsBanana() && !Item->IsLanternOnWall()) ||
      game::TruthQuestion(CONST_S("Do you wish to donate this item to the town? [y/N]")))));
}



void bananadroparea::KickSquare (character *Kicker, lsquare *Square) {
  if (AllowKick(Kicker, Square) && Kicker->IsPlayer() && game::GetTeam(NEW_ATTNAM_TEAM)->GetRelation(Kicker->GetTeam()) != HOSTILE) {
    for (stackiterator i = Square->GetStack()->GetBottom(); i.HasItem(); ++i) {
      if (i->IsBanana() || i->IsLanternOnWall()) {
        ADD_MESSAGE("You have harmed the property of the town!");
        Kicker->GetTeam()->Hostility(game::GetTeam(NEW_ATTNAM_TEAM));
        return;
      }
    }
  }
}



truth bananadroparea::ConsumeItem (character *HungryMan, item *Item, int) {
  if (game::GetTeam(NEW_ATTNAM_TEAM)->GetRelation(HungryMan->GetTeam()) == HOSTILE) return true;
  if (HungryMan->IsPlayer()) {
    if (!Item->IsBanana() && !Item->IsLanternOnWall()) return true;
    ADD_MESSAGE("Eating this is forbidden.");
    if (game::TruthQuestion(CONST_S("Do you still want to do this? [y/N]"))) {
      HungryMan->GetTeam()->Hostility(game::GetTeam(NEW_ATTNAM_TEAM));
      return true;
    }
  }
  return HungryMan->IsSumoWrestler();
}



void bananadroparea::TeleportSquare (character *Infidel, lsquare *Square) {
  if (!Infidel || game::GetTeam(NEW_ATTNAM_TEAM)->GetRelation(Infidel->GetTeam()) == HOSTILE) return;
  for (stackiterator i = Square->GetStack()->GetBottom(); i.HasItem(); ++i) {
    if (i->IsBanana() || i->IsLanternOnWall()) {
      Infidel->GetTeam()->Hostility(game::GetTeam(NEW_ATTNAM_TEAM));
      return;
    }
  }
}



truth bananadroparea::AllowKick (ccharacter *Char, const lsquare *) const {
  return (!Char->IsPlayer() || (game::GetTeam(NEW_ATTNAM_TEAM)->GetRelation(Char->GetTeam()) == HOSTILE));
}



void bananadroparea::HostileAction (character *Guilty) const {
  if (Guilty) Guilty->GetTeam()->Hostility(game::GetTeam(NEW_ATTNAM_TEAM));
}
#endif
