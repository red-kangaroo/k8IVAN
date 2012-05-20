COMMAND(WieldInLeftArm) {
  if (!Char->CanUseEquipment()) ADD_MESSAGE("You cannot wield anything.");
  else if (Char->TryToChangeEquipment(Char->GetStack(), 0, LEFT_WIELDED_INDEX)) {
    Char->DexterityAction(5);
    return true;
  }
  return false;
};
