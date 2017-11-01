/*
 *
 *  Iter Vehemens ad Necem (IVAN)
 *  Copyright (C) Timo Kiviluoto
 *  Released under the GNU General
 *  Public License
 *
 *  See LICENSING which should be included
 *  along with this file for more details
 *
 */
/* Compiled through wmapset.cpp */
#include "database.h"


// ////////////////////////////////////////////////////////////////////////// //
truth DrawOrderer (const std::pair<v2, int> &Pair1, const std::pair<v2, int> &Pair2) {
  return Pair1.second < Pair2.second;
}


// ////////////////////////////////////////////////////////////////////////// //
void wterrain::AddName (festring &String, int Case) const {
  if (!(Case & PLURAL)) {
    if (!(Case & ARTICLE_BIT)) String << GetNameStem();
    else if (!(Case & INDEFINE_BIT)) String << "the " << GetNameStem();
    else String << (UsesLongArticle() ? "an " : "a ") << GetNameStem();
  } else {
    if (!(Case & ARTICLE_BIT)) String << GetNameStem() << " terrains";
    else if (!(Case & INDEFINE_BIT)) String << "the " << GetNameStem() << " terrains";
    else String << GetNameStem() << " terrains";
  }
}


festring wterrain::GetName (int Case) const {
  static festring Name;
  Name.Empty();
  AddName(Name, Case);
  return Name;
}


void wterrain::Save (outputfile &SaveFile) const {
}


void wterrain::Load (inputfile &) {
  WSquareUnder = (wsquare*)game::GetSquareInLoad();
}


// ////////////////////////////////////////////////////////////////////////// //
void gwterraindatabase::InitDefaults (const gwterrainprototype *NewProtoType, int NewConfig, cfestring &acfgstrname)
{
  IsAbstract = false;
  ProtoType = NewProtoType;
  Config = NewConfig;
  CfgStrName = acfgstrname;
}


gwterrainprototype::gwterrainprototype (const gwterrainprototype *Base, gwterrainspawner Spawner, cchar *ClassID) :
  Base(Base),
  Spawner(Spawner),
  ClassID(ClassID)
{
  Index = protocontainer<gwterrain>::Add(this);
}


gwterrain *gwterrainprototype::Spawn (int Config, int SpecialFlags) const {
  gwterrain *res = Spawner(Config, SpecialFlags);
  return res;
}


gwterrain *gwterrainprototype::SpawnAndLoad (inputfile &SaveFile) const {
  gwterrain *terra = Spawner(0, LOAD);
  terra->Load(SaveFile);
  terra->SetAnimationFrames(terra->GetAnimationFrames());
  return terra;
}


void gwterrain::InstallDataBase (int NewConfig) { databasecreator<gwterrain>::InstallDataBase(this, NewConfig); }


void gwterrain::Initialize (int NewConfig, int SpecialFlags) {
  if (!(SpecialFlags&LOAD)) {
    databasecreator<gwterrain>::InstallDataBase(this, NewConfig);
    SetAnimationFrames(GetAnimationFrames());
  }
}


//int gwterrain::GetWalkability () const { return ANY_MOVE&~SWIM; }


void gwterrain::Draw (blitdata &BlitData) const {
  cint AF = AnimationFrames;
  cint F = !(BlitData.CustomData & ALLOW_ANIMATE) || AF == 1 ? 0 : GET_TICK() & (AF - 1);
  BlitData.Src = GetBitmapPos(F);
  igraph::GetWTerrainGraphic()->LuminanceBlit(BlitData);
  for (int c = 0; c < 8 && Neighbour[c].second; ++c) {
    BlitData.Src = Neighbour[c].first;
    igraph::GetWTerrainGraphic()->LuminanceMaskedBlit(BlitData);
  }
  BlitData.Src.X = BlitData.Src.Y = 0;
}


void gwterrain::Save (outputfile &SaveFile) const {
  wterrain::Save(SaveFile);
  SaveFile << (uShort)(GetConfig());
}


void gwterrain::Load (inputfile &SaveFile) {
  //fprintf(stderr, "gwterrain::Load: pos0=0x%08x\n", (unsigned)SaveFile.TellPos());
  wterrain::Load(SaveFile);
  databasecreator<gwterrain>::InstallDataBase(this, ReadType(uShort, SaveFile));
}


void gwterrain::CalculateNeighbourBitmapPoses () {
  int Index = 0;
  v2 Pos = GetPos();
  worldmap *WorldMap = GetWorldMap();
  int Priority = GetPriority();
  for (int d = 0; d < 8; ++d) {
    wsquare *NeighbourSquare = WorldMap->GetNeighbourWSquare(Pos, d);
    if (NeighbourSquare) {
      gwterrain *DoNeighbour = NeighbourSquare->GetGWTerrain();
      int NeighbourPriority = DoNeighbour->GetPriority();
      if (NeighbourPriority > Priority) {
        Neighbour[Index].first = DoNeighbour->GetBitmapPos(0)-(game::GetMoveVector(d) << 4);
        Neighbour[Index].second = NeighbourPriority;
        ++Index;
      }
    }
  }
  std::sort(Neighbour, Neighbour+Index, DrawOrderer);
  if (Index < 8) Neighbour[Index].second = 0;
}


gwterrain *GWSpawn (int type) {
  if (type < 1) ABORT("Invalid gwterrain requested");
  auto xtype = protocontainer<gwterrain>::SearchCodeName("gwterrain");
  if (!xtype) ABORT("Your worldmap is dull and empty.");
  const gwterrain::prototype* proto = protocontainer<gwterrain>::GetProto(xtype);
  if (!proto) ABORT("wtf?!");
  const gwterrain::database *const *configs = proto->GetConfigData();
  if (!configs) ABORT("wtf?!");
  return proto->Spawn(type);
}


// ////////////////////////////////////////////////////////////////////////// //
void owterraindatabase::InitDefaults (const owterrainprototype *NewProtoType, int NewConfig, cfestring &acfgstrname)
{
  IsAbstract = false;
  ProtoType = NewProtoType;
  Config = NewConfig;
  CfgStrName = acfgstrname;
}


owterrainprototype::owterrainprototype (const owterrainprototype *Base, owterrainspawner Spawner, cchar *ClassID) :
  Base(Base),
  Spawner(Spawner),
  ClassID(ClassID)
{
  Index = protocontainer<owterrain>::Add(this);
  //fprintf(stderr, "CID: <%s> (%s)\n", ClassID, (Spawner ? "tan" : "ona"));
}


owterrain *owterrainprototype::Spawn (int Config, int SpecialFlags) const {
  owterrain *res = Spawner(Config, SpecialFlags);
  res->mPos = v2(-1, -1);
  res->mRevealed = false;
  res->mPlaced = false;
  res->mGenerated = false;
  return res;
}


owterrain* owterrainprototype::SpawnAndLoad (inputfile &SaveFile) const {
  owterrain *terra = Spawner(0, LOAD);
  terra->Load(SaveFile);
  return terra;
}


void owterrain::InstallDataBase (int NewConfig) { databasecreator<owterrain>::InstallDataBase(this, NewConfig); }

int owterrain::GetWalkability () const { return ANY_MOVE; }
int owterrain::GetAttachedEntry () const { return STAIRS_UP; }


owterrain *owterrain::Clone () const {
  owterrain *res = ProtoType.Spawn(GetConfig());
  res->mPos = mPos;
  res->mRevealed = mRevealed;
  res->mPlaced = mPlaced;
  res->mGenerated = mGenerated;
  return res;
}


void owterrain::Initialize (int NewConfig, int SpecialFlags) {
  mRevealed = false;
  mPlaced = false;
  if (!(SpecialFlags&LOAD)) {
    databasecreator<owterrain>::InstallDataBase(this, NewConfig);
  }
}


void owterrain::Draw (blitdata &BlitData) const {
  cint AF = AnimationFrames;
  cint F = !(BlitData.CustomData & ALLOW_ANIMATE) || AF == 1 ? 0 : GET_TICK() & (AF - 1);
  BlitData.Src = GetBitmapPos(F);
  igraph::GetWTerrainGraphic()->LuminanceMaskedBlit(BlitData);
  BlitData.Src.X = BlitData.Src.Y = 0;
}


void owterrain::Save (outputfile &SaveFile) const {
  wterrain::Save(SaveFile);
  SaveFile << (uShort)(GetConfig());
  SaveFile << mRevealed;
  SaveFile << mPlaced;
  SaveFile << mPos;
  SaveFile << mGenerated;
}


void owterrain::Load (inputfile &SaveFile) {
  //fprintf(stderr, "owterrain::Load: pos0=0x%08x\n", (unsigned)SaveFile.TellPos());
  wterrain::Load(SaveFile);
  databasecreator<owterrain>::InstallDataBase(this, ReadType(uShort, SaveFile));
  SaveFile >> mRevealed;
  SaveFile >> mPlaced;
  SaveFile >> mPos;
  SaveFile >> mGenerated;
}


truth owterrain::Enter (truth DirectionUp) const {
  if (DirectionUp) {
    if (!PLAYER->IsFlying()) ADD_MESSAGE("You jump into the air. For some reason you don't get too far above.");
    else ADD_MESSAGE("You fly around for some time.");
    return false;
  }
  return game::TryTravel(GetAttachedDungeon(), GetAttachedArea(), GetAttachedEntry());
}


/*
v2 owterrain::GetBitmapPos (int) const { return DataBase->BitmapPos; }
festring owterrain::GetNameStem () const { return DataBase->NameStem; }
*/


// base ivan thing
truth owterrain::IsSuitableContinent (continent *Continent) {
  if (!Continent) return false;
  if (Continent->GetSize() < 25 || Continent->GetSize() > 700) return false; // 1000 is the previous value. It was lowered to make continents smaller.
  return
     Continent->GetGTerrainAmount(EGForestType()) &&
     Continent->GetGTerrainAmount(SnowType()) &&
     Continent->GetGTerrainAmount(SteppeType());
}
