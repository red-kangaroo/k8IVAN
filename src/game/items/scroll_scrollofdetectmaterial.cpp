#ifdef HEADER_PHASE
ITEM(scrollofdetectmaterial, scroll)
{
 public:
  virtual void FinishReading(character*);
};


#else



void scrollofdetectmaterial::FinishReading(character* Reader)
{
  material* TempMaterial;

  for(;;)
  {
    festring Temp = game::DefaultQuestion(CONST_S("What material do you want to detect?"),
            game::GetDefaultDetectMaterial());
    TempMaterial = protosystem::CreateMaterial(Temp);

    if(TempMaterial)
      break;
    else
      game::DrawEverythingNoBlit();
  }

  level* Level = GetLevel();
  int Squares = Level->DetectMaterial(TempMaterial);
  RemoveFromSlot();
  SendToHell();

  if(Squares > Reader->GetAttribute(INTELLIGENCE) * (25 + RAND() % 51))
  {
    ADD_MESSAGE("An enormous burst of geographical information overwhelms your consciousness. Your mind cannot cope with it and your memories blur.");
    Level->BlurMemory();
    Reader->BeginTemporaryState(CONFUSED, 1000 + RAND() % 1000);
    Reader->EditExperience(INTELLIGENCE, -100, 1 << 12);
  }
  else if(!Squares)
  {
    ADD_MESSAGE("You feel a sudden urge to imagine the dark void of a starless night sky.");
    Reader->EditExperience(INTELLIGENCE, 200, 1 << 12);
  }
  else
  {
    ADD_MESSAGE("You feel attracted to all things made of %s.", TempMaterial->GetName(false, false).CStr());
    game::PositionQuestion(CONST_S("Detecting material [direction keys move cursor, space exits]"), Reader->GetPos(), 0, 0, false);
    Reader->EditExperience(INTELLIGENCE, 300, 1 << 12);
  }

  delete TempMaterial;
  Level->CalculateLuminances();
  game::SendLOSUpdateRequest();
}
#endif
