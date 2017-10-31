#ifdef HEADER_PHASE
ITEM(mangoseedling, item)
{
public:
  //truth mangoseedling::Apply (character *);
  virtual truth AllowAlphaEverywhere() const { return true; }
  virtual truth IsMangoSeedling () const { return true; }

protected:
  virtual int GetClassAnimationFrames () const { return 32; }
  virtual col16 GetOutlineColor (int) const;
  virtual alpha GetOutlineAlpha (int) const;
};


#else


col16 mangoseedling::GetOutlineColor (int) const { return MakeRGB16(118, 158, 226); }


/*
truth mangoseedling::Apply (character *Applier) {
  if ((game::GetCurrentDungeonIndex() == NEW_ATTNAM)) {
    if (Applier->IsPlayer() && game::TweraifIsFree()) {
      Applier->EditAP(-1000);
      game::TextScreen(CONST_S(
        "You plant the seedling of the first new mango tree in New Attnam.\n"
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
      Applier->AddScoreEntry(Msg, 1, false);
      game::End(Msg);
    }
    RemoveFromSlot();
    SendToHell();
  } else {
    if (Applier->IsPlayer()) ADD_MESSAGE("You feel that the climate is not quite right for growing mangoes.");
  }
  Applier->EditAP(-1000);
  return true;
}
*/


alpha mangoseedling::GetOutlineAlpha (int Frame) const {
  Frame &= 31;
  return 50+(Frame*(31-Frame)>>1);
}


#endif
