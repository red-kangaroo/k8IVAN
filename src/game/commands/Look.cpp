COMMAND(Look) {
  festring Msg;
  if (!game::IsInWilderness()) Char->GetLevel()->AddSpecialCursors();
  if (!game::IsInWilderness())
    Msg = CONST_S("Direction keys move cursor, ESC exits, 'i' examines items, 'c' examines a character.");
  else
    Msg = CONST_S("Direction keys move cursor, ESC exits, 'c' examines a character.");
  game::PositionQuestion(Msg, Char->GetPos(), &game::LookHandler, &game::LookKeyHandler, ivanconfig::GetLookZoom());
  game::RemoveSpecialCursors();
  return false;
};
