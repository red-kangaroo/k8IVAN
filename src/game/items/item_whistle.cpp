#ifdef HEADER_PHASE
ITEM(whistle, item)
{
 public:
  virtual truth Apply(character*);
  virtual truth IsAppliable(ccharacter*) const { return true; }
  virtual void BlowEffect(character*);
 protected:
  virtual col16 GetMaterialColorB(int) const;
};


#else



col16 whistle::GetMaterialColorB(int) const { return MakeRGB16(80, 32, 16); }



truth whistle::Apply(character* Whistler)
{
  if(!Whistler->HasHead())
  {
    ADD_MESSAGE("You must have a head to do this.");
    return false;
  }

  BlowEffect(Whistler);
  Whistler->EditAP(-1000);
  return true;
}



void whistle::BlowEffect(character* Whistler)
{
  if(Whistler->IsPlayer())
  {
    if(Whistler->CanHear())
      ADD_MESSAGE("You produce an interesting sound.");
    else
      ADD_MESSAGE("You blow %s", CHAR_NAME(DEFINITE));
  }
  else if(Whistler->CanBeSeenByPlayer())
  {
    if(PLAYER->CanHear())
      ADD_MESSAGE("%s blows %s and produces an interesting sound.", Whistler->CHAR_NAME(DEFINITE), CHAR_NAME(DEFINITE));
    else
      ADD_MESSAGE("%s blows %s.", Whistler->CHAR_NAME(DEFINITE), CHAR_NAME(DEFINITE));
  }
  else if(PLAYER->CanHear())
    ADD_MESSAGE("You hear a whistle playing.");

  game::CallForAttention(GetPos(), 400);
}
#endif
