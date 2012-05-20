COMMAND(Sit) {
  lsquare *Square = Char->GetLSquareUnder();
  return (Square->GetOLTerrain() && Square->GetOLTerrain()->SitOn(Char)) || Square->GetGLTerrain()->SitOn(Char);
};
