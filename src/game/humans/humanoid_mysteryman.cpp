#ifdef HEADER_PHASE
CHARACTER(mysteryman, humanoid)
{
public:
  virtual void BeTalkedTo();
};


#else



void mysteryman::BeTalkedTo () {
  if (GetRelation(PLAYER) == HOSTILE) {
    ADD_MESSAGE("This is why the government doesn't mess with me, fool!");
    return;
  }
  if (!game::GetMondedrPass()) {
    if (PLAYER->RemoveMondedrPass()) {
      game::TextScreen(CONST_S(
        "You hand over the mondedr pass,\n"
        "the man quickly snatches it and starts reading it right away.\n"
        "The man scans the text very quickly, his eyes run around a hundred laps per second.\n"
        "His eyes come to a halt.\n"
        "\"I have revealed the location of Mondedr.\" He hands you a map.\n"
        "\"It is a good thing I was born in this world, no? You owe me nothing, the \n"
        "the amount of information in the sheet of paper is what repays it.\"\n\n"""));
      game::RevealPOI(game::mondedrPOI());
      GetArea()->SendNewDrawRequest();
      ADD_MESSAGE("\"And by the way, the government always watches you.\"");
      game::SetMondedrPass(1);
    } else {
      ADD_MESSAGE("\"Shoo, come back if you have a sheet of paper they call 'Mondedr Pass'\"");
    }
  } else {
    /* StoryState == 100 */
    ADD_MESSAGE("The man says: \"I don't have anymore business with you, shoo.\"");
  }
}
#endif
