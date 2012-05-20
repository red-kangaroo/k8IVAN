#ifdef HEADER_PHASE
ITEM(mondedrpass, scroll)
{
public:
  virtual void FinishReading (character *);
  virtual truth IsMondedrPass () const { return true; }
};


#else



void mondedrpass::FinishReading (character *Reader) {
  ADD_MESSAGE("This sheet of paper contains many interesting facts about Mondedr. After finishing reading the pass burns up.");
  RemoveFromSlot();
  SendToHell();
  Reader->EditExperience(INTELLIGENCE, 3000, 1 << 12);
}
#endif
