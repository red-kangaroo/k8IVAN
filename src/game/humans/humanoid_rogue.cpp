#ifdef HEADER_PHASE
CHARACTER(rogue, humanoid)
{
public:
  virtual void GetAICommand();
  virtual truth IsRetreating() const;
};


#else



truth rogue::IsRetreating () const {
  if (humanoid::IsRetreating()) return true;
  for (stackiterator i = GetStack()->GetBottom(); i.HasItem(); ++i) if ((*i)->GetSparkleFlags()) return true;
  return false;
}



void rogue::GetAICommand () {
  if (!IsRetreating()) {
    character *Char = GetRandomNeighbour();
    if (Char) {
      itemvector Sparkling;
      for (stackiterator i = Char->GetStack()->GetBottom(); i.HasItem(); ++i) {
        if ((*i)->GetSparkleFlags() && !MakesBurdened((*i)->GetWeight())) Sparkling.push_back(*i);
      }
      if (!Sparkling.empty()) {
        item *ToSteal = Sparkling[RAND() % Sparkling.size()];
        ToSteal->RemoveFromSlot();
        GetStack()->AddItem(ToSteal);
        if (Char->IsPlayer()) ADD_MESSAGE("%s steals your %s.", CHAR_NAME(DEFINITE), ToSteal->CHAR_NAME(UNARTICLED));
        EditAP(-500);
        return;
      }
    }
  }
  humanoid::GetAICommand();
}
#endif
