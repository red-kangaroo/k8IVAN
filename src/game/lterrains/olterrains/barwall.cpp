#ifdef HEADER_PHASE
OLTERRAIN(barwall, olterrain)
{
public:
  void Break ();
};


#else


void barwall::Break () {
  if (GetConfig() == BROKEN_BARWALL) {
    olterrain::Break();
  } else {
    barwall *Temp = barwall::Spawn(BROKEN_BARWALL, NO_MATERIALS);
    //
    Temp->InitMaterials(GetMainMaterial()->SpawnMore());
    GetLSquareUnder()->ChangeOLTerrainAndUpdateLights(Temp);
  }
}


#endif
