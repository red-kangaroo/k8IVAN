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

uChar **continent::TypeBuffer;
short **continent::AltitudeBuffer;
uChar **continent::ContinentBuffer;


continent::continent () : Index(0), Name("") { memset(ttypeCount, 0, sizeof(ttypeCount)); }
continent::continent (int Index) : Index(Index), Name("") { memset(ttypeCount, 0, sizeof(ttypeCount)); }
continent::~continent () {}

sLong continent::GetSize () const { return Member.size(); }

v2 continent::GetMember (int I) const {
  if (I < 0 || (unsigned)I >= Member.size()) ABORT("continent::GetMember: invalid member requiested (%d)!", I);
  return Member[I];
}

int continent::GetGTerrainAmount (int Type) const {
  if (Type < 0 || Type >= TerrainTypeLimit) ABORT("continent::GetGTerrainAmount: invalid terrain type requiested (%d)!", Type);
  return ttypeCount[Type];
}


void continent::Save (outputfile &SaveFile) const {
  SaveFile << Name << Member << Index;
}


void continent::Load (inputfile &SaveFile) {
  SaveFile >> Name >> Member >> Index;
}


void continent::AttachTo (continent *Continent) {
  for (feuLong c = 0; c < Member.size(); ++c) ContinentBuffer[Member[c].X][Member[c].Y] = Continent->Index;
  if (!Continent->Member.size()) {
    Continent->Member.swap(Member);
  } else {
    Continent->Member.insert(Continent->Member.end(), Member.begin(), Member.end());
    Member.clear();
  }
}


void continent::GenerateInfo () {
  if (TerrainTypeLimit < protocontainer<gwterrain>::GetSize()+1) ABORT("Too many world terrain types!");
  memset(ttypeCount, 0, sizeof(ttypeCount));

  for (feuLong c = 0; c < Member.size(); ++c) {
    auto tt = TypeBuffer[Member[c].X][Member[c].Y];
    if (tt < 0 || tt >= TerrainTypeLimit) ABORT("continent::GenerateInfo: invalid terrain type requiested (%d)!", tt);
    ++ttypeCount[tt];
  }

  Name = CONST_S("number ");
  Name << Index;
}


//FIXME: REWRITE THIS SHIT!
v2 continent::GetRandomMember (int Type, truth *success) {
  if (Type < 0 || Type >= TerrainTypeLimit) ABORT("continent::GetRandomMember: invalid terrain type requiested (%d)!", Type);

  if (success) *success = false;

  if (!ttypeCount[Type]) {
    //ABORT("Shortage of terrain!");
    return v2(0, 0);
  }

  v2 *TypeContainer = new v2[Member.size()];
  sLong cnt = 0;
  for (feuLong c = 0; c < Member.size(); ++c) {
    if (TypeBuffer[Member[c].X][Member[c].Y] == Type) {
      TypeContainer[cnt++] = Member[c];
      if (cnt == ttypeCount[Type]) break;
    }
  }
  if (cnt == 0) ABORT("Something is very wrong with our planet!");

  v2 Return = TypeContainer[RAND()%cnt];
  delete [] TypeContainer;

  if (success) *success = true;
  return Return;
}


outputfile &operator << (outputfile &SaveFile, const continent *Continent) {
  if (Continent) {
    SaveFile.Put(1);
    Continent->Save(SaveFile);
  } else {
    SaveFile.Put(0);
  }
  return SaveFile;
}


inputfile &operator >> (inputfile &SaveFile, continent *&Continent) {
  if (SaveFile.Get()) {
    Continent = new continent;
    Continent->Load(SaveFile);
  }
  return SaveFile;
}


void continent::Add (v2 Pos) {
  Member.push_back(Pos);
  ContinentBuffer[Pos.X][Pos.Y] = Index;
}
