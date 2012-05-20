COMMAND(ShowCoords) {
  v2 xy = Char->GetPos();
  ADD_MESSAGE("Coordinates: X=%d; Y=%d", xy.X, xy.Y);
  return false;
};
