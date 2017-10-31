#ifdef HEADER_PHASE
CHARACTER(carnivorousplant, nonhumanoid)
{
 protected:
  virtual col16 GetTorsoSpecialColor() const;
  virtual void GetAICommand();
  virtual void CreateCorpse(lsquare*);
};


#else



col16 carnivorousplant::GetTorsoSpecialColor() const // the flower
{
  if(!GetConfig())
    return MakeRGB16(RAND() % 100, 125 + RAND() % 125, RAND() % 100);
  else if(GetConfig() == GREATER)
    return MakeRGB16(RAND() % 100, RAND() % 100, 125 + RAND() % 125);
  else
    return MakeRGB16(125 + RAND() % 125, 125 + RAND() % 125, RAND() % 100);
}



void carnivorousplant::CreateCorpse(lsquare* Square)
{
  int Amount = !GetConfig() ? (RAND() % 7 ? 0 : 1) : GetConfig() == GREATER ? (RAND() & 1 ? 0 : (RAND() % 5 ? 1 : (RAND() % 5 ? 2 : 3))) : (!(RAND() % 3) ? 0 : (RAND() % 3 ? 1 : (RAND() % 3 ? 2 : 3)));

  for(int c = 0; c < Amount; ++c)
    Square->AddItem(kiwi::Spawn());

  nonhumanoid::CreateCorpse(Square);
}



void carnivorousplant::GetAICommand()
{
  SeekLeader(GetLeader());

  if(FollowLeader(GetLeader()))
    return;

  if(AttackAdjacentEnemyAI())
    return;

  if(CheckForUsefulItemsOnGround())
    return;

  if(MoveRandomly())
    return;

  EditAP(-1000);
}
#endif
