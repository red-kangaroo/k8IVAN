#ifdef HEADER_PHASE
ITEM(maptotombofxinroch, locationmap)
{
public:
  virtual void FinishReading (character *);
};


#else

class worldmap;


void maptotombofxinroch::FinishReading (character *Reader) {
  if (!Reader->IsPlayer() && Reader->CanBeSeenByPlayer()) ADD_MESSAGE("%s disappears!", Reader->CHAR_NAME(DEFINITE));
  if (Reader->IsPlayer()) {
    game::RevealPOI(game::xinrochtombPOI());
    ADD_MESSAGE("The map reveals to you the location of the Tomb of Xinroch and burns.");
  }
  RemoveFromSlot();
  SendToHell();
  Reader->EditExperience(INTELLIGENCE, 150, 1 << 12);
}


#endif
