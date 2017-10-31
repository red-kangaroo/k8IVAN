#ifdef HEADER_PHASE
ITEM(magicalwhistle, whistle)
{
 public:
  magicalwhistle() : LastUsed(0) { }
  virtual void BlowEffect(character*);
  virtual void Load(inputfile&);
  virtual void Save(outputfile&) const;
  virtual void FinalProcessForBone();
 protected:
  feuLong LastUsed;
};


#else


struct distancepair
{
  distancepair(sLong Distance, character* Char) : Distance(Distance), Char(Char) { }
  bool operator<(const distancepair& D) const { return Distance > D.Distance; }
  sLong Distance;
  character* Char;
};


void magicalwhistle::BlowEffect(character* Whistler)
{
  if(LastUsed && game::GetTick() - LastUsed < 2000)
  {
    whistle::BlowEffect(Whistler);
    return;
  }
  else
    LastUsed = game::GetTick();

  if(Whistler->IsPlayer())
  {
    if(Whistler->CanHear())
      ADD_MESSAGE("You produce a peculiar sound.");
    else
      ADD_MESSAGE("You blow %s.", CHAR_NAME(DEFINITE));
  }
  else if(Whistler->CanBeSeenByPlayer())
  {
    if(PLAYER->CanHear())
      ADD_MESSAGE("%s blows %s and produces a peculiar sound.", Whistler->CHAR_NAME(DEFINITE), CHAR_NAME(DEFINITE));
    else
      ADD_MESSAGE("%s blows %s.", Whistler->CHAR_NAME(DEFINITE), CHAR_NAME(DEFINITE));
  }
  else if(PLAYER->CanHear())
    ADD_MESSAGE("You hear a strange tune playing.");

  const std::list<character*>& Member = Whistler->GetTeam()->GetMember();
  std::vector<distancepair> ToSort;
  v2 Pos = Whistler->GetPos();

  for(std::list<character*>::const_iterator i = Member.begin(); i != Member.end(); ++i)
    if((*i)->IsEnabled() && Whistler != *i)
      ToSort.push_back(distancepair((Pos - (*i)->GetPos()).GetLengthSquare(), *i));

  if(ToSort.size() > 5)
    std::sort(ToSort.begin(), ToSort.end());

  for(uInt c = 0; c < 5 && c < ToSort.size(); ++c)
    ToSort[c].Char->TeleportNear(Whistler);

  game::CallForAttention(GetPos(), 400);
}



void magicalwhistle::Save(outputfile& SaveFile) const
{
  whistle::Save(SaveFile);
  SaveFile << LastUsed;
}



void magicalwhistle::Load(inputfile& SaveFile)
{
  whistle::Load(SaveFile);
  SaveFile >> LastUsed;
}



void magicalwhistle::FinalProcessForBone()
{
  whistle::FinalProcessForBone();
  LastUsed = 0;
}
#endif
