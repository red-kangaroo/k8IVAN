#ifdef HEADER_PHASE
ITEM(constitution, holybook)
{
public:
  virtual void FinishReading(character*);
protected:
  virtual col16 GetMaterialColorA(int) const;
};


#else



void constitution::FinishReading (character *Reader) {
  if (Reader->IsPlayer()) {
    game::TextScreen(CONST_S(
      "You see here an article on repelling an imperialist invasion.\n"
      "It is difficult to read, being rather hastily handwritten, probably by the Tweraifian\n"
      "lawmakers just before the invasion was complete. It reads:\n"
      "\n"
      "\"Article 62.5: In Case of Imperialist Invasion:\"\n"
      "\n"
      "\"Having freed the people of Tweraif of the Imperialist government, take the seedling\n"
      "of the Holy Mango Tree, and plant the seedling upon that wretched banana drop area. \n"
      "Then, thou wilt be pronounced the Liberator of Tweraif.\""));
    GetArea()->SendNewDrawRequest();
    ADD_MESSAGE("You feel you know now what you must do. The constitution vanishes.");
    RemoveFromSlot();
    SendToHell();
  }
}



col16 constitution::GetMaterialColorA (int) const {
  return MakeRGB16(200, 200, 200);
}
#endif
