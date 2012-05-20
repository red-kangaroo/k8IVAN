COMMAND(Polymorph) {
  character *NewForm;
  if (!Char->GetNewFormForPolymorphWithControl(NewForm)) return true;
  Char->Polymorph(NewForm, game::NumberQuestion(CONST_S("For how long?"), WHITE));
  return true;
};
