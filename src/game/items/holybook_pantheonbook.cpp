#ifdef HEADER_PHASE
ITEM(pantheonbook, holybook)
{
 public:
  virtual void FinishReading(character*);
 protected:
  virtual col16 GetMaterialColorA(int) const;
};


#else



void pantheonbook::FinishReading(character* Reader)
{
  if(Reader->IsPlayer())
  {
    PLAYER->EditExperience(INTELLIGENCE, 75, 1 << 12); //CLIVAN: 2000
    PLAYER->EditExperience(WISDOM, 1000, 1 << 12); //CLIVAN: 2000

    ADD_MESSAGE("The book reveals many divine secrets of the pantheon to you and disappears.");
    game::AdjustRelationsToAllGods(75);
    RemoveFromSlot();
    SendToHell();
  }
}



col16 pantheonbook::GetMaterialColorA(int) const
{
  return MakeRGB16(200, 200, 200);
}
#endif
