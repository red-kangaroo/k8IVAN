#ifdef HEADER_PHASE
ROOM(vault, room) {
public:
  vault ();

  virtual void Enter (character *);
  virtual truth PickupItem (character *, item *, int);
  virtual truth DropItem (character *, item *, int);
  virtual void KickSquare (character *, lsquare *);
  virtual truth ConsumeItem (character *, item *, int);
  virtual void SetEntered (truth What) { Entered = What; }
  virtual void Save (outputfile &) const;
  virtual void Load (inputfile &);
  virtual truth AllowDropGifts () const { return false; }
  virtual truth Drink (character *) const;
  virtual truth HasDrinkHandler () const { return true; }
  virtual truth Dip (character *) const;
  virtual truth HasDipHandler () const { return true; }
  virtual void TeleportSquare (character *, lsquare *);
  virtual truth AllowSpoil (citem *) const { return false; }
  virtual int GetGodRelationAdjustment () const { return -150; }
  virtual truth AllowKick (ccharacter *, const lsquare *) const;
  virtual void HostileAction (character *) const;
  virtual truth AllowAltarPolymorph () const { return false; }
  virtual truth AllowFoodSearch () const { return false; }
  virtual void AddItemEffect (item *);
  character *FindRandomExplosiveReceiver () const;

protected:
  truth Entered;
};


#else


vault::vault () {
  SetEntered(false);
}


void vault::Save (outputfile &SaveFile) const {
  room::Save(SaveFile);
  SaveFile << Entered;
}


void vault::Load (inputfile &SaveFile) {
  room::Load(SaveFile);
  SaveFile >> Entered;
}


void vault::Enter (character *Visitor) {
  if (Visitor->IsPlayer() && !Entered) {
    ADD_MESSAGE("You enter a well-guarded vault. Everyone eyes you like an animal looking at a fresh piece piece of meat.");
    Entered = true;
  }
}


truth vault::PickupItem (character *Visitor, item *Item, int) {
  if (game::GetStoryState() == 2 || game::GetTeam(KHARAZ_ARAD_TEAM)->GetRelation(Visitor->GetTeam()) == HOSTILE) return true;
  if (Visitor->IsPlayer()) {
    if (Item->IsHeadOfElpuri() || Item->IsGoldenEagleShirt() || Item->IsPetrussNut() || !Item->GetTruePrice() ||
        Item->IsEncryptedScroll() ||
        Item->IsMangoSeedling()) return true;
    ADD_MESSAGE("Picking up property of the Vault is prohibited.");
    if (game::TruthQuestion(CONST_S("Do you still want to do this?"))) {
      Visitor->GetTeam()->Hostility(game::GetTeam(KHARAZ_ARAD_TEAM));
      return true;
    }
  }
  return false;
}


truth vault::DropItem (character *Visitor, item *Item, int) {
  if (game::GetStoryState() == 2 || game::GetTeam(KHARAZ_ARAD_TEAM)->GetRelation(Visitor->GetTeam()) == HOSTILE) return true;
  if (Visitor->IsPlayer()) {
    if (Item->IsHeadOfElpuri() || Item->IsGoldenEagleShirt() || Item->IsPetrussNut() || Item->IsTheAvatar() ||
        Item->IsEncryptedScroll() || Item->IsMangoSeedling()) {
      ADD_MESSAGE("Donating this to the Vault wouldn't be wise. You may still need it.");
      return false;
    }
    if (game::TruthQuestion(CONST_S("Do you wish to donate this item to the Vault?"))) return true;
  }
  return false;
}


void vault::KickSquare (character *Kicker, lsquare *Square) {
  if (!AllowKick(Kicker, Square) && Kicker->IsPlayer() && game::GetStoryState() != 2 && game::GetTeam(KHARAZ_ARAD_TEAM)->GetRelation(Kicker->GetTeam()) != HOSTILE) {
    ADD_MESSAGE("You have harmed the property of the Vault!");
    Kicker->GetTeam()->Hostility(game::GetTeam(KHARAZ_ARAD_TEAM));
  }
}


truth vault::ConsumeItem (character *HungryMan, item *, int) {
  if (game::GetStoryState() == 2 || game::GetTeam(KHARAZ_ARAD_TEAM)->GetRelation(HungryMan->GetTeam()) == HOSTILE) return true;
  if (HungryMan->IsPlayer()) {
    ADD_MESSAGE("Eating the property of the Vault is forbidden.");
    if (game::TruthQuestion(CONST_S("Do you still want to do this?"))) {
      HungryMan->GetTeam()->Hostility(game::GetTeam(KHARAZ_ARAD_TEAM));
      return true;
    }
  }
  return false;
}


truth vault::Drink (character *Thirsty) const {
  if (game::GetStoryState() == 2 || game::GetTeam(KHARAZ_ARAD_TEAM)->GetRelation(Thirsty->GetTeam()) == HOSTILE) {
    return game::TruthQuestion(CONST_S("Do you want to drink?"));
  }
  if (Thirsty->IsPlayer()) {
    ADD_MESSAGE("Drinking property of the Vault is prohibited.");
    if (game::TruthQuestion(CONST_S("Do you still want to do this?"))) {
      Thirsty->GetTeam()->Hostility(game::GetTeam(KHARAZ_ARAD_TEAM));
      return true;
    }
  }
  return false;
}


void vault::TeleportSquare (character *Teleporter, lsquare *Square) {
  if (game::GetStoryState() == 2 || !Teleporter || game::GetTeam(KHARAZ_ARAD_TEAM)->GetRelation(Teleporter->GetTeam()) == HOSTILE) return;
  if (Teleporter->IsPlayer() && Square->GetStack()->GetItems()) {
    ADD_MESSAGE("You have done unnatural things to the property of the Vault!");
    Teleporter->GetTeam()->Hostility(game::GetTeam(KHARAZ_ARAD_TEAM));
  }
}


truth vault::Dip (character *Thirsty) const {
  if (game::GetStoryState() == 2 || game::GetTeam(KHARAZ_ARAD_TEAM)->GetRelation(Thirsty->GetTeam()) == HOSTILE) return true;
  if (Thirsty->IsPlayer()) {
    /*FIXME: What if it's not water? */
    ADD_MESSAGE("Stealing the precious water of the Vault is prohibited.");
    if (game::TruthQuestion(CONST_S("Are you sure you want to dip?"))) {
      Thirsty->GetTeam()->Hostility(game::GetTeam(KHARAZ_ARAD_TEAM));
      return true;
    }
  }
  return false;
}


truth vault::AllowKick (ccharacter *Char, const lsquare *LSquare) const {
  return game::GetTeam(KHARAZ_ARAD_TEAM)->GetRelation(Char->GetTeam()) == HOSTILE || !LSquare->GetStack()->GetItems();
}


void vault::HostileAction (character *Guilty) const {
  if (game::GetStoryState() != 2 && Guilty) Guilty->GetTeam()->Hostility(game::GetTeam(KHARAZ_ARAD_TEAM));
}


void vault::AddItemEffect (item *Dropped) {
  if (!Dropped->IsExplosive()) return;
  truth SeenBeforeTeleport = Dropped->CanBeSeenByPlayer();
  character *KamikazeDwarf = FindRandomExplosiveReceiver();
  if (KamikazeDwarf) {
    Dropped->MoveTo(KamikazeDwarf->GetStack());
    if (KamikazeDwarf->CanBeSeenByPlayer()) {
      if (SeenBeforeTeleport) {
        ADD_MESSAGE("%s disappears and reappears in %s's inventory.", Dropped->GetName(DEFINITE).CStr(), KamikazeDwarf->GetName(DEFINITE).CStr());
      } else {
        ADD_MESSAGE("%s appears in %s's inventory.", Dropped->GetName(DEFINITE).CStr(), KamikazeDwarf->GetName(DEFINITE).CStr());
      }
    } else if (SeenBeforeTeleport) ADD_MESSAGE("%s disappears.", Dropped->GetName(DEFINITE).CStr());
  } else {
    /* position is in kamikaze dwarf room */
    Dropped->RemoveFromSlot();
    game::GetCurrentLevel()->GetLSquare(18,21)->GetStack()->AddItem(Dropped, false);
    if (Dropped->CanBeSeenByPlayer()) {
      if (SeenBeforeTeleport) {
        ADD_MESSAGE("%s disappears and reappears in the kamikaze dwarf room.", Dropped->GetName(DEFINITE).CStr());
       } else {
         ADD_MESSAGE("%s appears in the kamikaze dwarf room.", Dropped->GetName(DEFINITE).CStr());
       }
    } else if (SeenBeforeTeleport) {
      ADD_MESSAGE("%s disappears.", Dropped->GetNameSingular().CStr());
    }
  }
}


character *vault::FindRandomExplosiveReceiver () const {
  std::vector<character *> ListOfDwarfs;
  for (std::list<character *>::const_iterator i = game::GetTeam(KHARAZ_ARAD_TEAM)->GetMember().begin();
       i != game::GetTeam(KHARAZ_ARAD_TEAM)->GetMember().end(); ++i)
    if ((*i)->IsEnabled() && (*i)->IsKamikazeDwarf()) ListOfDwarfs.push_back(*i);
  if (ListOfDwarfs.empty()) return 0;
  return ListOfDwarfs[RAND_N(ListOfDwarfs.size())];
}


#endif
