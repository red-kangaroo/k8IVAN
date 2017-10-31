#ifdef HEADER_PHASE
ITEM(stethoscope, item)
{
 public:
  virtual truth Apply(character*);
  virtual truth IsAppliable(ccharacter*) const { return true; };
};


#else



truth stethoscope::Apply(character* Doctor)
{
  if(!Doctor->CanUseStethoscope(true))
    return false;

  if(!Doctor->IsPlayer())
    ABORT("Doctor is not here, man, but these pills taste just as good anyway.");

  int Dir = game::DirectionQuestion(CONST_S("What do you want to inspect? [press a direction key]"), false,true);

  if(Dir == DIR_ERROR)
    return false;

  Doctor->DexterityAction(2);
  character* Char = GetNearSquare(GetPos() + game::GetMoveVector(Dir))->GetCharacter();

  if(!Char)
  {
    ADD_MESSAGE("There's no-one here.");
    return false;
  }

  Char->DisplayStethoscopeInfo(Doctor);
  return true;
}
#endif
