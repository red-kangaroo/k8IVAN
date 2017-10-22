#ifdef HEADER_PHASE
ROOM(shop, room) {
public:
  virtual void Enter (character *);
  virtual truth PickupItem (character *, item *, int);
  virtual truth DropItem (character *, item *, int);
  virtual void KickSquare (character *, lsquare *);
  virtual truth ConsumeItem (character *, item *, int);
  virtual truth AllowDropGifts () const { return false; }
  virtual void TeleportSquare (character *, lsquare *);
  virtual truth AllowSpoil (citem *) const;
  virtual truth AllowKick (ccharacter *, const lsquare *) const;
  virtual void HostileAction (character *) const;
  virtual truth AllowFoodSearch () const { return false; }
  virtual void ReceiveVomit (character *);
};


#else


void shop::Enter (character *Customer) {
  if (Customer->IsPlayer()) {
    if (MasterIsActive()) {
      if (GetMaster()->GetRelation(Customer) != HOSTILE && Customer->CanBeSeenBy(GetMaster())) {
        if (GetMaster()->CanBeSeenByPlayer()) ADD_MESSAGE("%s welcomes you warmly to the shop.", GetMaster()->CHAR_NAME(DEFINITE));
        else ADD_MESSAGE("Something welcomes you warmly to the shop.");
      }
    } else {
      ADD_MESSAGE("The shop appears to be deserted.");
    }
  }
}



truth shop::PickupItem (character *Customer, item *ForSale, int Amount) {
  if (!MasterIsActive() || Customer == GetMaster() || GetMaster()->GetRelation(Customer) == HOSTILE) return true;
  //
  if (ForSale->IsLanternOnWall()) {
    ADD_MESSAGE("\"I'd appreciate it if you left my light sources alone, thank you!\"");
    return false;
  }
  //
  sLong Price = ForSale->GetTruePrice();
  //
  if (Price) {
    Price = Amount*(Price*100/(100+Customer->GetAttribute(CHARISMA))+1);
    if (GetMaster()->GetConfig() == NEW_ATTNAM) {
      if (ForSale->IsBanana()) Price = (Price>>2)+1;
      else if (ForSale->IsEatable(GetMaster())) Price <<= 2;
      else Price = 0;
    }
  }
  //
  if (!Customer->IsPlayer()) {
    if (Customer->CanBeSeenByPlayer() && Customer->GetMoney() >= Price) {
      if (Price) {
        ADD_MESSAGE("%s buys %s.", Customer->CHAR_NAME(DEFINITE), ForSale->GetName(INDEFINITE, Amount).CStr());
        Customer->EditMoney(-Price);
        GetMaster()->EditMoney(Price);
        Customer->EditDealExperience(Price);
      }
      return true;
    }
    return false;
  }
  //
  if (Customer->CanBeSeenBy(GetMaster())) {
    if (ForSale->IsHeadOfElpuri() || ForSale->IsGoldenEagleShirt() ||
        ForSale->IsPetrussNut() || ForSale->IsTheAvatar() || ForSale->IsEncryptedScroll() ||
        ForSale->IsMangoSeedling()) {
      ADD_MESSAGE("\"I think it is yours. Take it.\"");
      return true;
    }
    //
    if (!Price) {
      ADD_MESSAGE("\"Thank you for cleaning that junk out of my floor.\"");
      return true;
    }
    //
    if (Customer->GetMoney() >= Price) {
      if (Amount == 1) {
        ADD_MESSAGE("\"Ah! That %s costs %d gold pieces. No haggling, please.\"", ForSale->CHAR_NAME(UNARTICLED), Price);
      } else {
        ADD_MESSAGE("\"Ah! Those %d %s cost %d gold pieces. No haggling, please.\"", Amount, ForSale->CHAR_NAME(PLURAL), Price);
      }
      if (game::TruthQuestion(CONST_S("Do you accept this deal?"))) {
        Customer->EditMoney(-Price);
        GetMaster()->EditMoney(+Price);
        Customer->EditDealExperience(Price);
        return true;
      }
      return false;
    } else {
      if (Amount == 1) {
        ADD_MESSAGE("\"Don't touch that %s, beggar! It is worth %d gold pieces!\"", ForSale->CHAR_NAME(UNARTICLED), Price);
      } else {
        ADD_MESSAGE("\"Don't touch those %s, beggar! They are worth %d gold pieces!\"", ForSale->CHAR_NAME(PLURAL), Price);
      }
      return false;
    }
  }
  if (game::TruthQuestion(CONST_S("Are you sure you want to commit this thievery?"))) {
    Customer->Hostility(GetMaster());
    return true;
  }
  return false;
}



truth shop::DropItem (character *Customer, item *ForSale, int Amount) {
  if (!MasterIsActive() || Customer == GetMaster() || GetMaster()->GetRelation(Customer) == HOSTILE) return true;
  //
  if (GetMaster()->GetConfig() == NEW_ATTNAM) {
    ADD_MESSAGE("\"Sorry, I'm only allowed to buy from Decos Bananas Co. if I wish to stay here.\"");
    return false;
  }
  //
  sLong Price = ForSale->GetTruePrice()*Amount*(100+Customer->GetAttribute(CHARISMA))/400;
  //
  if (ForSale->IsMoneyBag()) {
    // BUGBUGBUG? don't allow to give more that one bag
    if (Amount > 1) {
      ADD_MESSAGE("You can't give more that one of it!");
      return false;
    }
    Price = ForSale->GetTruePrice()*Amount;
  }
  //
  if (!Customer->IsPlayer()) {
    if (ForSale->IsMoneyBag()) {
      if (Customer->CanBeSeenByPlayer()) ADD_MESSAGE("%s gives %s to the shopkeeper.", Customer->CHAR_NAME(DEFINITE), ForSale->GetName(INDEFINITE, Amount).CStr());
      GetMaster()->EditMoney(Price);
      return true;
    }
    //
    if (Price && GetMaster()->GetMoney() >= Price) {
      if (Customer->CanBeSeenByPlayer()) ADD_MESSAGE("%s sells %s.", Customer->CHAR_NAME(DEFINITE), ForSale->GetName(INDEFINITE, Amount).CStr());
      Customer->EditMoney(Price);
      GetMaster()->EditMoney(-Price);
      Customer->EditDealExperience(Price);
      return true;
    }
    return false;
  }
  //
  if (Customer->CanBeSeenBy(GetMaster())) {
    if (ForSale->IsHeadOfElpuri() || ForSale->IsGoldenEagleShirt() ||
        ForSale->IsPetrussNut() || ForSale->IsTheAvatar() || ForSale->IsEncryptedScroll() ||
        ForSale->IsMangoSeedling()) {
      ADD_MESSAGE("\"Oh no! You need it far more than I!\"");
      return false;
    }
    //
    if (ForSale->WillExplodeSoon()) {
      ADD_MESSAGE("\"Hey that %s is primed! Take it out! OUT, I SAY!\"",
      ForSale->CHAR_NAME(UNARTICLED));
      return false;
    }
    //
    if (!Price) {
      ADD_MESSAGE("\"Hah! I wouldn't take %s even if you paid me for it!\"", Amount == 1 ? "that" : "those");
      return false;
    }
    //
    if (ForSale->IsMoneyBag()) {
      if (!game::TruthQuestion(CONST_S("Do you want to give it to the shopkeeper?"))) return false;
      ADD_MESSAGE("You gives %s to the shopkeeper.", ForSale->GetName(INDEFINITE, Amount).CStr());
      GetMaster()->EditMoney(Price);
      return true;
    }
    //
    if (GetMaster()->GetMoney()) {
      if (GetMaster()->GetMoney() < Price) Price = GetMaster()->GetMoney();
      if (Amount == 1) ADD_MESSAGE("\"What a fine %s. I'll pay %d gold pieces for it.\"", ForSale->CHAR_NAME(UNARTICLED), Price);
      else ADD_MESSAGE("\"What a fine pile of %d %s. I'll pay %d gold pieces for them.\"", Amount, ForSale->CHAR_NAME(PLURAL), Price);
      if (game::TruthQuestion(CONST_S("Do you accept this deal?"))) {
        Customer->SetMoney(Customer->GetMoney() + Price);
        GetMaster()->SetMoney(GetMaster()->GetMoney() - Price);
        Customer->EditDealExperience(Price);
        return true;
      }
      return false;
    }
    ADD_MESSAGE("\"I would pay you %d gold pieces for %s, but I'm temporarily short of cash. Sorry.\"", Price, Amount == 1 ? "it" : "them");
    return false;
  }
  ADD_MESSAGE("The shopkeeper doesn't see you, so you cannot trade with him.");
  return game::TruthQuestion(CONST_S("Still drop ")+(Amount == 1 ? "this item" : "these items")+"?");
}


void shop::KickSquare (character *Infidel, lsquare *Square) {
  if (!AllowKick(Infidel, Square)) {
    ADD_MESSAGE("\"You infidel!\"");
    Infidel->Hostility(GetMaster());
  }
}


truth shop::ConsumeItem (character *Customer, item *, int) {
  if (!MasterIsActive() || GetMaster()->GetRelation(Customer) == HOSTILE) return true;
  if (!Customer->IsPlayer()) return false;
  if (Customer->CanBeSeenBy(GetMaster())) {
    ADD_MESSAGE("\"Buy that first, please.\"");
    return false;
  }
  if (game::TruthQuestion(CONST_S("It's illegal to eat property of others. Are you sure you sure?"))) {
    Customer->Hostility(GetMaster());
    return true;
  }
  return false;
}


void shop::TeleportSquare (character *Infidel, lsquare *Square) {
  if (Square->GetStack()->GetItems() && MasterIsActive() &&
     Infidel && Infidel != GetMaster() &&
     GetMaster()->GetRelation(Infidel) != HOSTILE &&
     Square->CanBeSeenBy(GetMaster()))
  {
    ADD_MESSAGE("\"You infidel!\"");
    Infidel->Hostility(GetMaster());
  }
}


truth shop::AllowSpoil (citem *Item) const {
  character *Master = GetMaster();
  //
  return !Master || !Master->IsEnabled() || !Item->HasPrice();
}


truth shop::AllowKick (ccharacter *Char, const lsquare *LSquare) const {
  return (
    !LSquare->GetStack()->GetItems() || !MasterIsActive() ||
    Char == GetMaster() || GetMaster()->GetRelation(Char) == HOSTILE ||
    !LSquare->CanBeSeenBy(GetMaster()));
}


void shop::HostileAction (character *Guilty) const {
  if (MasterIsActive() && Guilty && Guilty != GetMaster() &&
      GetMaster()->GetRelation(Guilty) != HOSTILE &&
      Guilty->CanBeSeenBy(GetMaster())) {
    ADD_MESSAGE("\"You infidel!\"");
    Guilty->Hostility(GetMaster());
  }
}


void shop::ReceiveVomit (character *Who) {
  if (MasterIsActive() &&
      Who->IsPlayer() &&
      Who->GetRelation(GetMaster()) != HOSTILE &&
      Who->CanBeSeenBy(GetMaster())) {
    ADD_MESSAGE("\"Unfortunately I accept no returns.\"");
  }
}


#endif
