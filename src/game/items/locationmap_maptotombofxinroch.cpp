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
    //game::RevealXinrochTomb();
    game::LoadWorldMap();
    v2 XinrochTombPos = game::GetWorldMap()->GetEntryPos(0, XINROCH_TOMB);
    game::GetWorldMap()->GetWSquare(XinrochTombPos)->ChangeOWTerrain(xinrochtomb::Spawn());
    game::GetWorldMap()->RevealEnvironment(XinrochTombPos, 1);
    game::SaveWorldMap();
    ADD_MESSAGE("The map reveals to you the location of the Tomb of Xinroch and burns.");
  }
  RemoveFromSlot();
  SendToHell();
  Reader->EditExperience(INTELLIGENCE, 150, 1 << 12);
}


#endif
