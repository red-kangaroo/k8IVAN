#ifdef HEADER_PHASE
ITEM(scrollofteleportation, scroll)
{
 public:
  virtual void FinishReading(character*);
};


#else



void scrollofteleportation::FinishReading(character* Reader)
{
  if(!Reader->IsPlayer() && Reader->CanBeSeenByPlayer())
    ADD_MESSAGE("%s disappears!", Reader->CHAR_NAME(DEFINITE));

  Reader->TeleportRandomly(true);

  if(Reader->IsPlayer())
    ADD_MESSAGE("Suddenly you realize you have teleported.");

  RemoveFromSlot();
  SendToHell();
  Reader->EditExperience(INTELLIGENCE, 150, 1 << 12);
}
#endif
