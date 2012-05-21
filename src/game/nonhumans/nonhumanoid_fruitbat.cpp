#ifdef HEADER_PHASE
CHARACTER(fruitbat, nonhumanoid)
{
public:
  virtual void GetAICommand ();
  virtual truth IsRetreating () const;

protected:
  virtual bodypart *MakeBodyPart (int) const;
};


#else


bodypart *fruitbat::MakeBodyPart (int) const { return battorso::Spawn(0, NO_MATERIALS); }


truth fruitbat::IsRetreating () const {
  if (nonhumanoid::IsRetreating()) return true;
  for (stackiterator i = GetStack()->GetBottom(); i.HasItem(); ++i) {
    if ((*i)/*->GetMainMaterial()->IsFruit()*/->IsFood()) return true;
  }
  return false;
}


void fruitbat::GetAICommand () {
  if (!IsRetreating()) {
    character *Char = GetRandomNeighbour();
    //
    if (Char) {
      itemvector Fruits;
      //
      for (stackiterator i = Char->GetStack()->GetBottom(); i.HasItem(); ++i) {
        if (((*i)->IsFood()/*->GetMainMaterial()->IsFruit())*/) && !MakesBurdened((*i)->GetWeight())) Fruits.push_back(*i);
      }
      if (!Fruits.empty()) {
        item *ToSteal = Fruits[RAND()%Fruits.size()];
        ToSteal->RemoveFromSlot();
        GetStack()->AddItem(ToSteal);
        if (Char->IsPlayer()) ADD_MESSAGE("%s steals your %s.", CHAR_NAME(DEFINITE), ToSteal->CHAR_NAME(UNARTICLED));
        EditAP(-500);
        return;
      }
    }
  }
  nonhumanoid::GetAICommand();
}


#endif
