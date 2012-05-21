#ifdef HEADER_PHASE
ITEM(celestialmonograph, holybook)
{
public:
  virtual void FinishReading (character *);

protected:
  virtual col16 GetMaterialColorA (int) const;
};


#else


void celestialmonograph::FinishReading (character *Reader) {
  if (Reader->IsPlayer()) {
    PLAYER->EditExperience(INTELLIGENCE, 500, 1<<12);
    PLAYER->EditExperience(WISDOM, 250, 1<<12);
    game::SetRelationsToAllGods(0);
    for (int c = 1; c <= GODS; ++c) if (game::GetGod(c)->IsKnown()) game::GetGod(c)->SetIsKnown(false);
    ADD_MESSAGE("The stimulating celestial monograph removes all your memory of the pantheon and disappears.");
    RemoveFromSlot();
    SendToHell();
  }
}


col16 celestialmonograph::GetMaterialColorA (int) const {
  return MakeRGB16(40, 140, 40);
}
#endif
