#ifdef HEADER_PHASE
ITEM(scrollofwishing, scroll)
{
 public:
  virtual void FinishReading(character*);
};


#else



void scrollofwishing::FinishReading (character *Reader) {
  if (game::Wish(Reader,
       "%s appears from nothing and the scroll burns!",
       "Two %s appear from nothing and the scroll burns!", true)) {
    RemoveFromSlot();
    SendToHell();
    Reader->EditExperience(INTELLIGENCE, 600, 1 << 12);
  } else {
    RemoveFromSlot();
    SendToHell();
    ADD_MESSAGE("You changed your mind, didn't you, %s? Nevertheless, the scroll burns.", game::Insult());
  }
}
#endif
