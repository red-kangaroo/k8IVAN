COMMAND(NOP) {
  Char->EditExperience(DEXTERITY, -25, 1<<3);
  Char->EditExperience(AGILITY, -25, 1<<3);
  Char->EditAP(-Char->GetStateAPGain(1000));
  return true;
};
