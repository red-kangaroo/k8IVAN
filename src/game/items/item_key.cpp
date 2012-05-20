#ifdef HEADER_PHASE
ITEM(key, item)
{
 public:
  virtual truth Apply(character*);
  virtual truth IsAppliable(ccharacter*) const { return true; }
  virtual truth CanOpenDoors() const { return true; }
  virtual truth CanOpenLockType(int AnotherLockType) const { return GetConfig() == AnotherLockType; }
};


#else



truth key::Apply(character* User)
{
  if(User->IsPlayer())
  {
    if(!User->CanOpen())
    {
      ADD_MESSAGE("This monster type cannot use keys.");
      return false;
    }
    int Key;
    truth OpenableItems = User->GetStack()->SortedItems(User, &item::HasLock);

    if(OpenableItems)
      Key = game::AskForKeyPress(CONST_S("What do you wish to lock or unlock? [press a direction key, space or i]"));
    else
      Key = game::AskForKeyPress(CONST_S("What do you wish to lock or unlock? [press a direction key or space]"));

    if(Key == 'i' && OpenableItems)
    {
      item* Item = User->GetStack()->DrawContents(User, CONST_S("What do you want to lock or unlock?"), 0, &item::IsOpenable);
      return Item && Item->TryKey(this, User);
    }

    v2 DirVect = game::GetDirectionVectorForKey(Key);

    if(DirVect != ERROR_V2 && User->GetArea()->IsValidPos(User->GetPos() + DirVect))
      return GetLevel()->GetLSquare(User->GetPos() + DirVect)->TryKey(this, User);
  }
  return true;
}
#endif
