#ifdef HEADER_PHASE
ROOM(bananadroparea, room) {
public:
  virtual truth PickupItem (character *, item *, int);
  virtual truth DropItem (character *, item *, int);
  virtual void KickSquare (character *, lsquare *);
  virtual truth ConsumeItem (character *, item *, int);
  virtual truth AllowDropGifts () const;
  virtual void TeleportSquare (character *, lsquare *);
  virtual truth AllowKick (ccharacter *, const lsquare *) const;
  virtual void HostileAction (character *) const;
};


#else


truth bananadroparea::AllowDropGifts () const { return false; }


truth bananadroparea::PickupItem (character *Hungry, item *Item, int) {
  if (game::GetTeam(NEW_ATTNAM_TEAM)->GetRelation(Hungry->GetTeam()) == HOSTILE) return true;
  if (Hungry->IsPlayer()) {
    if (!Item->IsBanana() && !Item->IsLanternOnWall()) return true;
    if (game::GetLiberator()) return true;
    ADD_MESSAGE("That would be stealing.");
    if (game::TruthQuestion(CONST_S("Do you still want to do this?"))) {
      Hungry->GetTeam()->Hostility(game::GetTeam(NEW_ATTNAM_TEAM));
      return true;
    }
  }
  return false;
}


truth bananadroparea::DropItem (character *Dropper, item *Item, int) {
  if (Dropper->IsPlayer() && (Item->IsMangoSeedling()) && game::GetTeam(NEW_ATTNAM_TEAM)->GetRelation(Dropper->GetTeam()) != HOSTILE) {
    if (game::TruthQuestion(CONST_S("Do you wish to plant the mango seedling at this time?")) && game::TweraifIsFree()) {
      game::TextScreen(CONST_S(
        "You plant the seedling of the Holy Mango Tree in New Attnam.\n"
        "The people of your home village gather around you cheering! Tweraif is\n"
        "now restored to its former glory and you remain there as honourary\n"
        "spiritual leader and hero of the new republic. You ensure that free\n"
        "and fair elections quickly ensue.\n"
        "\n"
        "You are victorious!"));
      game::GetCurrentArea()->SendNewDrawRequest();
      game::DrawEverything();
      PLAYER->ShowAdventureInfo();
      festring Msg = CONST_S("restored Tweraif to independence and continued to further adventures");
      Dropper->AddScoreEntry(Msg, 1.1, false);
      game::End(Msg);
    } else {
      if (Dropper->IsPlayer() && !game::TweraifIsFree()) ADD_MESSAGE("You feel that the climate is not quite right for growing mangoes.");
      else ADD_MESSAGE("You choose not to plant the seedling.");
      return false;
    }
  }
  //
  return (game::GetTeam(NEW_ATTNAM_TEAM)->GetRelation(Dropper->GetTeam()) == HOSTILE ||
    (Dropper->IsPlayer() && ((!Item->IsBanana() && !Item->IsLanternOnWall()) ||
      game::TruthQuestion(CONST_S("Do you wish to donate this item to the town?")))));
}


void bananadroparea::KickSquare (character *Kicker, lsquare *Square) {
  if (game::GetLiberator()) return;
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
    if (game::GetLiberator()) return true;
    ADD_MESSAGE("Eating this is forbidden.");
    if (game::TruthQuestion(CONST_S("Do you still want to do this?"))) {
      HungryMan->GetTeam()->Hostility(game::GetTeam(NEW_ATTNAM_TEAM));
      return true;
    }
  }
  return HungryMan->IsSumoWrestler();
}


void bananadroparea::TeleportSquare (character *Infidel, lsquare *Square) {
  if (!Infidel || game::GetTeam(NEW_ATTNAM_TEAM)->GetRelation(Infidel->GetTeam()) == HOSTILE) return;
  if (game::GetLiberator()) return;
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
