#ifdef HEADER_PHASE
ROOM(library, room) {
public:
  virtual void Enter(character*);
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


void library::Enter (character *Customer) {
  if (Customer->IsPlayer()) {
    if (MasterIsActive()) {
      if (GetMaster()->GetRelation(Customer) != HOSTILE && Customer->CanBeSeenBy(GetMaster())) {
        if (GetMaster()->CanBeSeenByPlayer()) {
          ADD_MESSAGE(
            "%s looks at you suspiciously. \"Feel free to open the shelves, but be quiet in the library!\" %s whispers.",
            GetMaster()->CHAR_NAME(DEFINITE),
            GetMaster()->GetPersonalPronoun().CStr());
        } else {
          ADD_MESSAGE("You feel somebody staring at you.");
        }
      }
    } else {
      ADD_MESSAGE("The library appears to be deserted.");
    }
  }
}


truth library::PickupItem (character *Customer, item *ForSale, int Amount) {
  if (!MasterIsActive() || Customer == GetMaster() || GetMaster()->GetRelation(Customer) == HOSTILE) return true;
  if (ForSale->IsLanternOnWall()) {
    ADD_MESSAGE("\"I'd appreciate it if you left my light sources alone, thank you!\"");
    return false;
  }

  sLong Price = ForSale->GetTruePrice()*Amount*100/(100+Customer->GetAttribute(CHARISMA));

  if (!Customer->IsPlayer()) {
    if (Customer->CanBeSeenByPlayer() && Customer->GetMoney() >= Price) {
      ADD_MESSAGE("%s buys %s.", Customer->CHAR_NAME(DEFINITE),
      ForSale->GetName(INDEFINITE, Amount).CStr());
      Customer->EditMoney(-Price);
      GetMaster()->EditMoney(Price);
      Customer->EditDealExperience(Price);
      return true;
    }
    return false;
  }

  if (Customer->CanBeSeenBy(GetMaster())) {
    if (ForSale->IsHeadOfElpuri() || ForSale->IsGoldenEagleShirt() || ForSale->IsPetrussNut() ||
        ForSale->IsTheAvatar() || ForSale->IsEncryptedScroll()) {
      ADD_MESSAGE("\"I think it is yours. Take it.\"");
      return true;
    }
    if (!Price || !ForSale->CanBeSoldInLibrary(GetMaster())) {
      ADD_MESSAGE("\"Thank you for cleaning that junk out of my floor.\"");
      return true;
    }

    if (Customer->GetMoney() >= Price) {
      if (Amount == 1)
        ADD_MESSAGE("\"Ah! That %s costs %d gold pieces. No haggling, please.\"", ForSale->CHAR_NAME(UNARTICLED), Price);
      else
        ADD_MESSAGE("\"Ah! Those %d %s cost %d gold pieces. No haggling, please.\"", Amount, ForSale->CHAR_NAME(PLURAL), Price);
      if (game::TruthQuestion(CONST_S("Do you accept this deal?"))) {
        Customer->EditMoney(-Price);
        GetMaster()->EditMoney(Price);
        Customer->EditDealExperience(Price);
        return true;
      }
      return false;
    } else {
      if (Amount == 1)
        ADD_MESSAGE("\"Don't touch that %s, beggar! It is worth %d gold pieces!\"", ForSale->CHAR_NAME(UNARTICLED), Price);
      else
        ADD_MESSAGE("\"Don't touch those %s, beggar! They are worth %d gold pieces!\"", ForSale->CHAR_NAME(PLURAL), Price);
      return false;
    }
  }
  if (game::TruthQuestion(CONST_S("Are you sure you want to commit this thievery?"))) {
    Customer->Hostility(GetMaster());
    return true;
  }
  return false;
}


truth library::DropItem (character *Customer, item *ForSale, int Amount) {
  if (!MasterIsActive() || Customer == GetMaster() || GetMaster()->GetRelation(Customer) == HOSTILE) return true;

  sLong Price = ForSale->GetTruePrice()*Amount*(100+Customer->GetAttribute(CHARISMA))/400;

  if (!Customer->IsPlayer()) {
    if (Price && Customer->CanBeSeenByPlayer() && GetMaster()->GetMoney() >= Price) {
      ADD_MESSAGE("%s sells %s.", Customer->CHAR_NAME(DEFINITE),
      ForSale->GetName(INDEFINITE, Amount).CStr());
      Customer->SetMoney(Customer->GetMoney() + Price);
      GetMaster()->SetMoney(GetMaster()->GetMoney() - Price);
      Customer->EditDealExperience(Price);
      return true;
    }
    return false;
  }

  if (Customer->CanBeSeenBy(GetMaster())) {
    if (ForSale->IsHeadOfElpuri() || ForSale->IsGoldenEagleShirt() || ForSale->IsPetrussNut() ||
        ForSale->IsTheAvatar() || ForSale->IsEncryptedScroll()) {
      ADD_MESSAGE("\"Oh no! You need it far more than I!\"");
      return false;
    }

    if (!Price || !ForSale->CanBeSoldInLibrary(GetMaster())) {
      ADD_MESSAGE("\"Sorry, but I don't think %s into my collection.\"",
      Amount == 1 ? "that fits" : "those fit");
      return false;
    }

    if (GetMaster()->GetMoney()) {
      if (GetMaster()->GetMoney() < Price) Price = GetMaster()->GetMoney();
      if (Amount == 1)
        ADD_MESSAGE("\"What an interesting %s. I'll pay %d gold pieces for it.\"", ForSale->CHAR_NAME(UNARTICLED), Price);
      else
        ADD_MESSAGE("\"What an interesting collection of %d %s. I'll pay %d gold pieces for it.\"", Amount, ForSale->CHAR_NAME(PLURAL), Price);
      if (game::TruthQuestion(CONST_S("Do you want to sell ")+(Amount == 1 ? "this item" : "these items")+"?")) {
        Customer->EditMoney(Price);
        GetMaster()->EditMoney(-Price);
        Customer->EditDealExperience(Price);
        return true;
      }
      return false;
    } else {
      ADD_MESSAGE("\"I would pay you %d gold pieces for %s, but I'm temporarily short of cash. Sorry.\"",
      Price, Amount == 1 ? "it" : "them");
      return false;
    }
  } else {
    ADD_MESSAGE("The librarian doesn't see you, so you cannot trade with him.");
    return game::TruthQuestion(CONST_S("Still drop ")+(Amount == 1 ? "this item" : "these items")+"?");
  }
}


void library::KickSquare (character *Infidel, lsquare *Square) {
  if (!AllowKick(Infidel, Square)) {
    ADD_MESSAGE("\"You book vandal!\"");
    Infidel->Hostility(GetMaster());
  }
}


truth library::ConsumeItem (character *, item *, int) {
  return true;
}


void library::TeleportSquare (character *Infidel, lsquare *Square) {
  if (Square->GetStack()->GetItems() && MasterIsActive() &&
      Infidel && Infidel != GetMaster() &&
      GetMaster()->GetRelation(Infidel) != HOSTILE &&
      Square->CanBeSeenBy(GetMaster()))
  {
    ADD_MESSAGE("\"You book hater!\"");
    Infidel->Hostility(GetMaster());
  }
}


truth library::AllowKick (ccharacter *Char, const lsquare *LSquare) const {
  return
    !LSquare->GetStack()->GetItems() ||
    !MasterIsActive() || Char == GetMaster() ||
    GetMaster()->GetRelation(Char) == HOSTILE ||
    !LSquare->CanBeSeenBy(GetMaster());
}


void library::HostileAction (character *Guilty) const {
  if (MasterIsActive() && Guilty && Guilty != GetMaster() &&
      GetMaster()->GetRelation(Guilty) != HOSTILE &&
      Guilty->CanBeSeenBy(GetMaster())) {
    ADD_MESSAGE("\"You infidel!\"");
    Guilty->Hostility(GetMaster());
  }
}


#endif
