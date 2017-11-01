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
#include "script.h"
#include "fesave.h"
#include "feparse.h"
#include "game.h"
#include "materia.h"
#include "char.h"
#include "proto.h"
#include "allocate.h"

script::datamap posscript::DataMap;
script::datamap materialscript::DataMap;
script::datamap basecontentscript::DataMap;
script::datamap contentscript<character>::DataMap;
script::datamap contentscript<item>::DataMap;
script::datamap contentscript<glterrain>::DataMap;
script::datamap contentscript<olterrain>::DataMap;
script::datamap squarescript::DataMap;
script::datamap roomscript::DataMap;
script::datamap levelscript::DataMap;
script::datamap dungeonscript::DataMap;
script::datamap teamscript::DataMap;
script::datamap gamescript::DataMap;

template <class type, class contenttype> script::datamap contentmap<type, contenttype>::DataMap;


template <class type> void scriptmember<type>::ReadFrom (TextInput &SaveFile) {
  //SrcFile = SaveFile.GetFileName();
  //SrcLine = SaveFile.CurrentLine();
  if (!Member) Member = new type;
  ReadData(*Member, SaveFile);
}


template <class type> void scriptmember<type>::Replace (scriptmemberbase &Base) {
  scriptmember<type> &Data = static_cast<scriptmember<type> &>(Base);
  //
  if (Data.Member) {
    delete Member;
    Member = Data.Member;
    Data.Member = 0;
  }
  //SrcFile = Base.SrcFile;
  //SrcLine = Base.SrcLine;
}


template <class type> void scriptmember<type>::Save (outputfile &SaveFile) const {
  if (Member) {
    SaveFile.Put(1);
    //SaveFile << SrcFile << SrcLine;
    SaveFile << *Member;
  } else {
    SaveFile.Put(0);
  }
}

template <class type> void scriptmember<type>::Load (inputfile &SaveFile) {
  if (SaveFile.Get()) {
    Member = new type;
    //SaveFile >> SrcFile >> SrcLine;
    SaveFile >> *Member;
  }
}


#define INST_SCRIPT_MEMBER(type)\
template void scriptmember< type >::ReadFrom(TextInput &);\
template void scriptmember< type >::Replace(scriptmemberbase &);\
template void scriptmember< type >::Save(outputfile &) const;\
template void scriptmember< type >::Load(inputfile &)

INST_SCRIPT_MEMBER(uChar);
INST_SCRIPT_MEMBER(short);
INST_SCRIPT_MEMBER(int);
//INST_SCRIPT_MEMBER(sLong); //k8:64
INST_SCRIPT_MEMBER(v2);
INST_SCRIPT_MEMBER(festring);
INST_SCRIPT_MEMBER(fearray<v2>);
INST_SCRIPT_MEMBER(rect);
INST_SCRIPT_MEMBER(interval);
INST_SCRIPT_MEMBER(region);
INST_SCRIPT_MEMBER(posscript);
INST_SCRIPT_MEMBER(materialscript);
INST_SCRIPT_MEMBER(squarescript);
INST_SCRIPT_MEMBER(roomscript);
INST_SCRIPT_MEMBER(levelscript);
INST_SCRIPT_MEMBER(contentscript<character>);
INST_SCRIPT_MEMBER(fearray<contentscript<item> >);
INST_SCRIPT_MEMBER(contentscript<glterrain>);
INST_SCRIPT_MEMBER(contentscript<olterrain>);
INST_SCRIPT_MEMBER(charactercontentmap);
INST_SCRIPT_MEMBER(itemcontentmap);
INST_SCRIPT_MEMBER(glterraincontentmap);
INST_SCRIPT_MEMBER(olterraincontentmap);
INST_SCRIPT_MEMBER(fearray<packv2>);


template <class type> void fastscriptmember<type>::ReadFrom (TextInput &SaveFile) {
  //SrcFile = SaveFile.GetFileName();
  //SrcLine = SaveFile.CurrentLine();
  ReadData(*&Member, SaveFile); // gcc 3.4.1 sucks
}


template <class type> void fastscriptmember<type>::Replace (scriptmemberbase &Base) {
  fastscriptmember<type> &Data = static_cast<fastscriptmember<type> &>(Base);
  //
  Member = Data.Member;
  //SrcFile = Base.SrcFile;
  //SrcLine = Base.SrcLine;
}


template <class type> void fastscriptmember<type>::Save(outputfile &SaveFile) const {
  //SaveFile << SrcFile << SrcLine;
  SaveFile << Member;
}


template <class type> void fastscriptmember<type>::Load (inputfile &SaveFile) {
  //SaveFile >> SrcFile >> SrcLine;
  SaveFile >> *&Member; // gcc 3.4.1 sucks
}


#define INST_FAST_SCRIPT_MEMBER(type)\
template void fastscriptmember< type >::ReadFrom(TextInput&);\
template void fastscriptmember< type >::Replace(scriptmemberbase&);\
template void fastscriptmember< type >::Save(outputfile&) const;\
template void fastscriptmember< type >::Load(inputfile&)

INST_FAST_SCRIPT_MEMBER(char);
INST_FAST_SCRIPT_MEMBER(uChar);
INST_FAST_SCRIPT_MEMBER(int);
//INST_FAST_SCRIPT_MEMBER(sLong); //k8:64
INST_FAST_SCRIPT_MEMBER(feuLong);
INST_FAST_SCRIPT_MEMBER(packv2);


void script::Save (outputfile &SaveFile) const {
  //SaveFile << SrcFile << SrcLine;
  SaveDataMap(GetDataMap(), SaveFile);
}


void script::Load (inputfile &SaveFile) {
  //SaveFile >> SrcFile >> SrcLine;
  LoadDataMap(GetDataMap(), SaveFile);
}


truth script::ReadMember (TextInput &SaveFile, cfestring &Word) {
  scriptmemberbase *Data = GetData(Word.CStr());
  //
  if (Data) {
    Data->ReadFrom(SaveFile);
    return true;
  }
  return false;
}


scriptmemberbase *script::GetDataFromMap (const datamap &DataMap, cchar *Identifier) {
  datamap::const_iterator i = DataMap.find(Identifier);
  //
  return i != DataMap.end() ? &(this->*i->second) : 0;
}


void script::SaveDataMap (const datamap &DataMap, outputfile &SaveFile) const {
  for (datamap::const_iterator i = DataMap.begin(); i != DataMap.end(); ++i) (this->*i->second).Save(SaveFile);
}


void script::LoadDataMap (const datamap &DataMap, inputfile &SaveFile) {
  for (datamap::const_iterator i = DataMap.begin(); i != DataMap.end(); ++i) (this->*i->second).Load(SaveFile);
}


template <class scriptmemberptr> void InitMember (script::datamap &DataMap, cchar *Identifier, scriptmemberptr DataMember) {
  DataMap[Identifier] = reinterpret_cast<scriptmemberbase script::*>(DataMember);
}


#define INIT_ENTRY(name) InitMember(DataMap, #name, &scripttype::name##Holder)
#define INIT(name, value) name##Holder(value)


void posscript::InitDataMap () {
  INIT_ENTRY(Vector);
  INIT_ENTRY(Flags);
  INIT_ENTRY(Borders);
}


void posscript::ReadFrom (TextInput &SaveFile) {
  static festring Word;
  //
  //SrcFile = SaveFile.GetFileName();
  //SrcLine = SaveFile.CurrentLine();
  SaveFile.ReadWord(Word);
  if (Word == "Pos") {
    Random = false;
    VectorHolder.ReadFrom(SaveFile);
  } else if (Word == "Random") {
    Random = true;
    FlagsHolder.ReadFrom(SaveFile);
  } else if (Word == "BoundedRandom") {
    Random = true;
    BordersHolder.ReadFrom(SaveFile);
    FlagsHolder.ReadFrom(SaveFile);
  }
}


void posscript::Save (outputfile &SaveFile) const {
  script::Save(SaveFile);
  SaveFile << Random;
}


void posscript::Load (inputfile &SaveFile) {
  script::Load(SaveFile);
  SaveFile >> Random;
}


void materialscript::InitDataMap () {
  INIT_ENTRY(Volume);
}


void materialscript::ReadFrom (TextInput &SaveFile) {
  static festring Word;
  //
  //SrcFile = SaveFile.GetFileName();
  //SrcLine = SaveFile.CurrentLine();
  SaveFile.ReadWord(Word);
  if (Word == "=") SaveFile.ReadWord(Word);
  if (Word == "0") {
    Config = 0;
  } else {
    valuemap::const_iterator i = game::GetGlobalValueMap().find(Word);
    //
    if (i != game::GetGlobalValueMap().end()) Config = i->second;
    else ABORT("Unconfigured material script detected at line %d!", SaveFile.TokenLine());
  }
  if (SaveFile.ReadWord() != "{") return;
  for (SaveFile.ReadWord(Word); Word != "}"; SaveFile.ReadWord(Word)) {
    if (!ReadMember(SaveFile, Word)) {
      ABORT("Odd script term %s encountered in material script line %d!", Word.CStr(), SaveFile.TokenLine());
    }
  }
}


material *materialscript::Instantiate () const {
  return MAKE_MATERIAL(Config, GetVolume() ? GetVolume()->Randomize() : 0);
}


void materialscript::Save (outputfile &SaveFile) const {
  script::Save(SaveFile);
  SaveFile << (uShort)Config;
}


void materialscript::Load (inputfile &SaveFile) {
  script::Load(SaveFile);
  Config = 0;
  uShort s2;
  SaveFile >> s2;
  Config = s2;
}


void basecontentscript::InitDataMap () {
  INIT_ENTRY(MainMaterial);
  INIT_ENTRY(SecondaryMaterial);
  INIT_ENTRY(Parameters);
}


basecontentscript::basecontentscript () : script(), ContentType(0), Random(false), Config(0), INIT(Parameters, NO_PARAMETERS) {
}


void basecontentscript::ReadFrom (TextInput &SaveFile) {
  static festring Word;
  //SrcFile = SaveFile.GetFileName();
  //SrcLine = SaveFile.CurrentLine();
  for (;;) {
    SaveFile.ReadWord(Word);
    if (Word == "on") {
      //mCode = SaveFile.ReadCode();
      mCode.collectSource(SaveFile);
      SaveFile.skipBlanks();
      int ch = SaveFile.GetChar();
      if (ch != ';') SaveFile.UngetChar(ch);
      continue;
    }
    break;
  }
  if (Word == "=" || Word == ",") SaveFile.ReadWord(Word);
  valuemap::const_iterator i = game::GetGlobalValueMap().find(Word);
  if (i != game::GetGlobalValueMap().end()) {
    if (!GetMainMaterial()) MainMaterialHolder.Member = new materialscript;
    MainMaterialHolder.Member->SetConfig(i->second);
    SaveFile.ReadWord(Word);
    i = game::GetGlobalValueMap().find(Word);
    if (i != game::GetGlobalValueMap().end()) {
      if (!GetSecondaryMaterial()) SecondaryMaterialHolder.Member = new materialscript;
      SecondaryMaterialHolder.Member->SetConfig(i->second);
      SaveFile.ReadWord(Word);
    }
  }
  if (Word == "NaturalMaterialForm") {
    Random = false;
    ContentType = NATURAL_MATERIAL_FORM;
    SaveFile.ReadWord(Word);
  } else if (Word == "Random") {
    Random = true;
    SaveFile.ReadWord(Word);
  } else {
    Random = false;
    ContentType = SearchCodeName(Word);
    if (ContentType || Word == "0") SaveFile.ReadWord(Word);
    else ABORT("Odd script term %s encountered in %s content script, file %s line %d!", Word.CStr(), GetClassID(), SaveFile.GetFileName().CStr(), SaveFile.TokenLine());
  }
  if (Word == "(") {
    Config = SaveFile.ReadNumber();
    SaveFile.ReadWord(Word);
  } else {
    Config = 0;
  }
  if (Word == "{") {
    for (SaveFile.ReadWord(Word); Word != "}"; SaveFile.ReadWord(Word)) {
      if (!ReadMember(SaveFile, Word)) ABORT("Odd script term %s encountered in %s content script, file %s line %d!", Word.CStr(), GetClassID(), SaveFile.GetFileName().CStr(), SaveFile.TokenLine());
    }
  } else {
    /*
    if (Word == "[") {
      mCode = SaveFile.ReadCode();
      SaveFile.ReadWord(Word);
    }
    */
    if (Word != ";" && Word != ",") ABORT("Odd terminator %s encountered in %s content script, file %s line %d!", Word.CStr(), GetClassID(), SaveFile.GetFileName().CStr(), SaveFile.TokenLine());
  }
}


scriptmemberbase *basecontentscript::GetData (cchar *String) {
  scriptmemberbase *Return = GetDataFromMap(GetDataMap(), String);
  return Return ? Return : GetDataFromMap(DataMap, String);
}


void basecontentscript::Save (outputfile &SaveFile) const {
  //SaveFile << SrcFile << SrcLine;
  SaveDataMap(GetDataMap(), SaveFile);
  SaveDataMap(DataMap, SaveFile);
  SaveFile << ContentType;
  SaveFile.Put(!!Random);
  SaveFile << Config;
}


void basecontentscript::Load (inputfile &SaveFile) {
  //SaveFile >> SrcFile >> SrcLine;
  LoadDataMap(GetDataMap(), SaveFile);
  LoadDataMap(DataMap, SaveFile);
  ContentType = ReadType(uShort, SaveFile);
  Random = SaveFile.Get();
  SaveFile >> Config;
}


template <class type> type *contentscripttemplate<type>::BasicInstantiate (int SpecialFlags) const {
  type *Instance = 0;
  const typename type::prototype *Proto = protocontainer<type>::GetProto(ContentType);
  if (!Proto) ABORT("BasicInstantiate error for type '%s'!", getCPPTypeName<type>().CStr());
  const typename type::database *const *ConfigData = Proto->GetConfigData();
  const materialscript *MainMaterial = GetMainMaterial();
  const materialscript *SecondaryMaterial = GetSecondaryMaterial();
  const typename type::database *DataBase = *ConfigData;
  truth UseOverriddenMaterials = false;
  //
  if (!Config && DataBase->IsAbstract) {
    while (!Instance) {
      DataBase = ConfigData[1+RAND()%(Proto->GetConfigSize()-1)];
      if (DataBase->AllowRandomInstantiation()) {
        if (!(SpecialFlags & NO_MATERIALS) && MainMaterial && (!DataBase->HasSecondaryMaterial || SecondaryMaterial)) {
          SpecialFlags |= NO_MATERIALS;
          UseOverriddenMaterials = true;
        }
        Instance = Proto->Spawn(DataBase->Config, SpecialFlags|NO_PIC_UPDATE);
      }
    }
  } else {
    if (!(SpecialFlags & NO_MATERIALS) && MainMaterial && (!DataBase->HasSecondaryMaterial || SecondaryMaterial)) {
      SpecialFlags |= NO_MATERIALS;
      UseOverriddenMaterials = true;
    }
    Instance = Proto->Spawn(Config, SpecialFlags|NO_PIC_UPDATE);
  }
  if (GetParameters() != NO_PARAMETERS) Instance->SetParameters(GetParameters());
  if (UseOverriddenMaterials) {
    Instance->InitMaterials(MainMaterial, SecondaryMaterial, false);
  } else {
    if (MainMaterial) Instance->ChangeMainMaterial(MainMaterial->Instantiate(), SpecialFlags|NO_PIC_UPDATE);
    if (SecondaryMaterial) Instance->ChangeSecondaryMaterial(SecondaryMaterial->Instantiate(), SpecialFlags|NO_PIC_UPDATE);
  }
  if (!(SpecialFlags & NO_PIC_UPDATE)) Instance->UpdatePictures();
  return Instance;
}


/* Called by an inline function in script.h... */

template glterrain *contentscripttemplate<glterrain>::BasicInstantiate (int) const;

template <class type> int contentscripttemplate<type>::SearchCodeName (cfestring &String) const {
  return protocontainer<type>::SearchCodeName(String);
}

/* GCC 2.952 SUCKS!!! IT MUST BURN!!! */

template int contentscripttemplate<character>::SearchCodeName(cfestring &) const;
template int contentscripttemplate<item>::SearchCodeName(cfestring &) const;
template int contentscripttemplate<glterrain>::SearchCodeName(cfestring &) const;
template int contentscripttemplate<olterrain>::SearchCodeName(cfestring &) const;

cchar *contentscript<character>::GetClassID () const { return "character"; }
cchar *contentscript<item>::GetClassID () const { return "item"; }
cchar *contentscript<glterrain>::GetClassID () const { return "glterrain"; }
cchar *contentscript<olterrain>::GetClassID () const { return "olterrain"; }


void contentscript<character>::InitDataMap () {
  INIT_ENTRY(Inventory);
  INIT_ENTRY(WayPoint);
  INIT_ENTRY(Team);
  INIT_ENTRY(Flags);
}


contentscript<character>::contentscript () : INIT(Team, DEFAULT_TEAM), INIT(Flags, 0) {
}


character *contentscript<character>::Instantiate (int SpecialFlags) const {
  character *Instance = contentscripttemplate<character>::BasicInstantiate(SpecialFlags);
  //fprintf(stderr, "instantiating character '%s'\n", Instance->GetNameSingular().CStr());

  if (GetTeam() != DEFAULT_TEAM) Instance->SetTeam(game::GetTeam(GetTeam()));
  const fearray<contentscript<item> > *Inventory = GetInventory();
  if (Inventory) Instance->AddToInventory(*Inventory, SpecialFlags);
  const fearray<packv2> *WayPoint = GetWayPoint();
  if (WayPoint) Instance->SetWayPoints(*WayPoint);
  Instance->RestoreHP();
  Instance->RestoreStamina();

  if (!game::RunCharAllowScript(Instance, mCode, "spawn")) {
    //fprintf(stderr, "dropping character '%s'\n", Instance->GetNameSingular().CStr());
    delete Instance;
    Instance = 0;
  }

  return Instance;
}


contentscript<item>::contentscript () :
  INIT(Category, ANY_CATEGORY),
  INIT(MinPrice, 0),
  INIT(MaxPrice, MAX_PRICE),
  INIT(Team, DEFAULT_TEAM),
  INIT(SquarePosition, CENTER),
  INIT(Chance, 100),
  INIT(ConfigFlags, 0),
  INIT(SpoilPercentage, 0),
  INIT(Enchantment, 0),
  INIT(IsActive, false)
{
}


void contentscript<item>::InitDataMap () {
  INIT_ENTRY(ItemsInside);
  INIT_ENTRY(Times);
  INIT_ENTRY(MinPrice);
  INIT_ENTRY(MaxPrice);
  INIT_ENTRY(LifeExpectancy);
  INIT_ENTRY(Team);
  INIT_ENTRY(Category);
  INIT_ENTRY(SquarePosition);
  INIT_ENTRY(Chance);
  INIT_ENTRY(ConfigFlags);
  INIT_ENTRY(SpoilPercentage);
  INIT_ENTRY(Enchantment);
  INIT_ENTRY(IsActive);
}


item *contentscript<item>::InstantiateBasedOnMaterial (int MaterialConfig, int SpecialFlags) const {
  if (ContentType == NATURAL_MATERIAL_FORM) {
    const materialscript *MainMaterial = GetMainMaterial();
    sLong Volume = MainMaterial && MainMaterial->GetVolume() ? MainMaterial->GetVolume()->Randomize() : 0;
    return material::CreateNaturalForm(MaterialConfig, Volume);
  }
  return Instantiate(SpecialFlags);
}


item *contentscript<item>::Instantiate (int SpecialFlags) const {
  int Chance = GetChance();
  item *Instance = 0;

  if (Chance != 100 && Chance <= RAND_N(100)) return 0;

  if (Random) {
    Instance = protosystem::BalancedCreateItem(0, GetMinPrice(), GetMaxPrice(), GetCategory(), SpecialFlags, GetConfigFlags());
  } else {
    Instance = contentscripttemplate<item>::BasicInstantiate(SpecialFlags);
  }

  if (!game::RunItemAllowScript(Instance, mCode, "spawn")) {
    //fprintf(stderr, "dropping character '%s'\n", Instance->GetNameSingular().CStr());
    delete Instance;
    return 0;
  }

  if (GetLifeExpectancy()) Instance->SetLifeExpectancy(GetLifeExpectancy()->Min, (GetLifeExpectancy()->Max - GetLifeExpectancy()->Min) + 1);
  if (GetTeam() != DEFAULT_TEAM) Instance->SetTeam(GetTeam());
  if (IsActive()) Instance->SetIsActive(true);
  if (GetEnchantment() != 0) Instance->SetEnchantment(GetEnchantment());
  const fearray<contentscript<item> > *ItemsInside = GetItemsInside();
  if (ItemsInside) Instance->SetItemsInside(*ItemsInside, SpecialFlags);
  if (GetSpoilPercentage() != 0) Instance->SetSpoilPercentage(GetSpoilPercentage());
  return Instance;
}


truth IsValidScript (const fearray<contentscript<item> > *Array) {
  for (uInt c = 0; c < Array->Size; ++c) if (IsValidScript(&Array->Data[c])) return true;
  return false;
}


void contentscript<glterrain>::InitDataMap () {
  INIT_ENTRY(IsInside);
}


contentscript<olterrain>::contentscript () :
  INIT(VisualEffects, 0),
  INIT(AttachedArea, DEFAULT_ATTACHED_AREA),
  INIT(AttachedEntry, DEFAULT_ATTACHED_ENTRY)
{
}


void contentscript<olterrain>::InitDataMap () {
  INIT_ENTRY(ItemsInside);
  INIT_ENTRY(Text);
  INIT_ENTRY(VisualEffects);
  INIT_ENTRY(AttachedArea);
  INIT_ENTRY(AttachedEntry);
}


olterrain *contentscript<olterrain>::Instantiate (int SpecialFlags) const {
  if (!ContentType) return 0;
  olterrain *Instance = contentscripttemplate<olterrain>::BasicInstantiate(SpecialFlags);
  if (GetVisualEffects()) {
    Instance->SetVisualEffects(GetVisualEffects());
    Instance->UpdatePictures();
  }
  if (GetAttachedArea() != DEFAULT_ATTACHED_AREA) Instance->SetAttachedArea(GetAttachedArea());
  if (GetAttachedEntry() != DEFAULT_ATTACHED_ENTRY) Instance->SetAttachedEntry(GetAttachedEntry());
  cfestring *Text = GetText();
  if (Text) Instance->SetText(*Text);
  const fearray<contentscript<item> > *ItemsInside = GetItemsInside();
  if (ItemsInside) Instance->SetItemsInside(*ItemsInside, SpecialFlags);
  return Instance;
}


squarescript::squarescript () : INIT(EntryIndex, NO_ENTRY), INIT(AttachRequired, false) {
}


void squarescript::InitDataMap () {
  INIT_ENTRY(Position);
  INIT_ENTRY(Character);
  INIT_ENTRY(Items);
  INIT_ENTRY(GTerrain);
  INIT_ENTRY(OTerrain);
  INIT_ENTRY(Times);
  INIT_ENTRY(EntryIndex);
  INIT_ENTRY(AttachRequired);
}


void squarescript::ReadFrom (TextInput &SaveFile) {
  static festring Word;
  //
  //SrcFile = SaveFile.GetFileName();
  //SrcLine = SaveFile.CurrentLine();
  SaveFile.ReadWord(Word);
  if (Word != "=") {
    PositionHolder.ReadFrom(SaveFile);
    if (SaveFile.ReadWord() != "{") ABORT("Bracket missing in square script line %d!", SaveFile.TokenLine());
    for (SaveFile.ReadWord(Word); Word != "}"; SaveFile.ReadWord(Word)) {
      if (!ReadMember(SaveFile, Word)) ABORT("Odd script term %s encountered in square script line %d!", Word.CStr(), SaveFile.TokenLine());
    }
  } else {
    GTerrainHolder.ReadFrom(SaveFile);
    OTerrainHolder.ReadFrom(SaveFile);
  }
}


template <class type, class contenttype> contentmap<type, contenttype>::contentmap() : ContentMap(0) {
}


template <class type, class contenttype> contentmap<type, contenttype>::~contentmap<type, contenttype> () {
  delete [] ContentMap;
}


template <class type, class contenttype> void contentmap<type, contenttype>::InitDataMap () {
  INIT_ENTRY(Size);
  INIT_ENTRY(Pos);
}


template <class type, class contenttype> void contentmap<type, contenttype>::ReadFrom (TextInput &SaveFile) {
  typedef std::map<int, contenttype> maptype;
  typedef typename maptype::iterator mapiterator;
  festring Word1, Word2;
  //
  //SrcFile = SaveFile.GetFileName();
  //SrcLine = SaveFile.CurrentLine();
  if (ContentMap) ABORT("Illegal %s content map redefinition on line %d!", protocontainer<type>::GetMainClassID(), SaveFile.TokenLine());
  if (SaveFile.ReadWord() != "{") ABORT("Bracket missing in %s content map script line %d!", protocontainer<type>::GetMainClassID(), SaveFile.TokenLine());
  SymbolMap.insert(std::pair<int, contenttype>('.', contenttype()));
  for (SaveFile.ReadWord(Word1); Word1 != "}"; Word1 = SaveFile.ReadWord()) {
    if (Word1 == "Types") {
      if (SaveFile.ReadWord() != "{") ABORT("Missing bracket in %s content map script line %d!", protocontainer<type>::GetMainClassID(), SaveFile.TokenLine());
      for (SaveFile.ReadWord(Word2); Word2 != "}"; Word2 = SaveFile.ReadWord()) {
        std::pair<mapiterator, bool> Return = SymbolMap.insert(std::pair<int, contenttype>(Word2[0], contenttype()));
        //
        if (Return.second) {
          ReadData(Return.first->second, SaveFile);
        } else {
          ABORT("Symbol %c defined again in %s content map script line %d!", Word2[0], protocontainer<type>::GetMainClassID(), SaveFile.TokenLine());
        }
      }
      continue;
    }
    if (!ReadMember(SaveFile, Word1)) ABORT("Odd script term %s encountered in %s content script line %d!", Word1.CStr(), protocontainer<type>::GetMainClassID(), SaveFile.TokenLine());
  }
  //
  v2 Size = *GetSize();
  //
  Alloc2D(ContentMap, Size.X, Size.Y);
  if (SaveFile.ReadWord() != "{") ABORT("Missing bracket in %s content map script line %d!", protocontainer<type>::GetMainClassID(), SaveFile.TokenLine());
  for (int y = 0; y < Size.Y; ++y) {
    for (int x = 0; x < Size.X; ++x) {
      int Char = SaveFile.ReadLetter();
      //typename std::map<int, contenttype>::iterator i = SymbolMap.find(Char);
      auto i = SymbolMap.find(Char);
      if (i != SymbolMap.end()) {
        ContentMap[x][y] = std::make_pair(Char, &i->second);
      } else {
        if (Char <= ' ') {
          ABORT("Illegal content '\\x%02x' in %s content map in file '%s' line %d!", (unsigned)Char, protocontainer<type>::GetMainClassID(), SaveFile.GetFileName().CStr(), SaveFile.TokenLine());
        } else {
          ABORT("Illegal content '%c' in %s content map in file '%s' line %d!", Char, protocontainer<type>::GetMainClassID(), SaveFile.GetFileName().CStr(), SaveFile.TokenLine());
        }
      }
    }
  }
  if (SaveFile.ReadWord() != "}") ABORT("Missing bracket in %s content map script in file '%s' line %d!", protocontainer<type>::GetMainClassID(), SaveFile.GetFileName().CStr(), SaveFile.TokenLine());
}


template <class type, class contenttype> void contentmap<type, contenttype>::Save (outputfile &SaveFile) const {
  script::Save(SaveFile);
  SaveFile << SymbolMap;
  //
  v2 Size = *GetSize();
  for (int y = 0; y < Size.Y; ++y)
    for (int x = 0; x < Size.X; ++x)
      SaveFile << char(ContentMap[x][y].first);
}


template <class type, class contenttype> void contentmap<type, contenttype>::Load (inputfile &SaveFile) {
  script::Load(SaveFile);
  SaveFile >> SymbolMap;
  //
  v2 Size = *GetSize();
  //
  Alloc2D(ContentMap, Size.X, Size.Y);
  for (int y = 0; y < Size.Y; ++y) {
    for (int x = 0; x < Size.X; ++x) {
      int Char = ReadType(char, SaveFile);
      //
      ContentMap[x][y] = std::make_pair(Char, &SymbolMap.find(Char)->second);
    }
  }
}


const std::list<squarescript> &roomscript::GetSquare () const { return Square; }


void roomscript::InitDataMap () {
  INIT_ENTRY(CharacterMap);
  INIT_ENTRY(ItemMap);
  INIT_ENTRY(GTerrainMap);
  INIT_ENTRY(OTerrainMap);
  INIT_ENTRY(WallSquare);
  INIT_ENTRY(FloorSquare);
  INIT_ENTRY(DoorSquare);
  INIT_ENTRY(Size);
  INIT_ENTRY(Pos);
  INIT_ENTRY(AltarPossible);
  INIT_ENTRY(GenerateDoor);
  INIT_ENTRY(GenerateTunnel);
  INIT_ENTRY(DivineMaster);
  INIT_ENTRY(GenerateLanterns);
  INIT_ENTRY(Type);
  INIT_ENTRY(GenerateFountains);
  INIT_ENTRY(AllowLockedDoors);
  INIT_ENTRY(AllowBoobyTrappedDoors);
  INIT_ENTRY(Shape);
  INIT_ENTRY(IsInside);
  INIT_ENTRY(GenerateWindows);
  INIT_ENTRY(UseFillSquareWalls);
  INIT_ENTRY(Flags);
  INIT_ENTRY(GenerateWards);
  INIT_ENTRY(AllowedDivineMasters);
}


void roomscript::ReadFrom (TextInput &SaveFile) {
  festring Word;
  //
  //SrcFile = SaveFile.GetFileName();
  //SrcLine = SaveFile.CurrentLine();
  if (SaveFile.ReadWord() != "{") ABORT("Bracket missing in room script line %d!", SaveFile.TokenLine());
  for (SaveFile.ReadWord(Word); Word != "}"; SaveFile.ReadWord(Word)) {
    if (Word == "Square") {
      Square.push_back(squarescript());
      Square.back().ReadFrom(SaveFile);
      continue;
    }
    if (!ReadMember(SaveFile, Word)) ABORT("Odd script term %s encountered in room script line %d!", Word.CStr(), SaveFile.TokenLine());
  }
}


void roomscript::Save (outputfile &SaveFile) const {
  script::Save(SaveFile);
  SaveFile << Square;
}


void roomscript::Load (inputfile &SaveFile) {
  script::Load(SaveFile);
  SaveFile >> Square;
}


const std::list<squarescript> &levelscript::GetSquare () const { return Square; }
const std::list<roomscript> &levelscript::GetRoom () const { return Room; }


void levelscript::InitDataMap () {
  INIT_ENTRY(RoomDefault);
  INIT_ENTRY(FillSquare);
  INIT_ENTRY(TunnelSquare);
  INIT_ENTRY(LevelMessage);
  INIT_ENTRY(Size);
  INIT_ENTRY(Items);
  INIT_ENTRY(Rooms);
  INIT_ENTRY(GenerateMonsters);
  INIT_ENTRY(IsOnGround);
  INIT_ENTRY(EarthquakesAffectTunnels);
  INIT_ENTRY(TeamDefault);
  INIT_ENTRY(Description);
  INIT_ENTRY(LOSModifier);
  INIT_ENTRY(IgnoreDefaultSpecialSquares);
  INIT_ENTRY(DifficultyBase);
  INIT_ENTRY(DifficultyDelta);
  INIT_ENTRY(MonsterAmountBase);
  INIT_ENTRY(MonsterAmountDelta);
  INIT_ENTRY(MonsterGenerationIntervalBase);
  INIT_ENTRY(MonsterGenerationIntervalDelta);
  INIT_ENTRY(AutoReveal);
  INIT_ENTRY(ShortDescription);
  INIT_ENTRY(CanGenerateBone);
  INIT_ENTRY(ItemMinPriceBase);
  INIT_ENTRY(ItemMinPriceDelta);
  INIT_ENTRY(Type);
  INIT_ENTRY(EnchantmentMinusChanceBase);
  INIT_ENTRY(EnchantmentMinusChanceDelta);
  INIT_ENTRY(EnchantmentPlusChanceBase);
  INIT_ENTRY(EnchantmentPlusChanceDelta);
  INIT_ENTRY(BackGroundType);
  INIT_ENTRY(IsCatacomb);
  INIT_ENTRY(EnterImage);
  INIT_ENTRY(EnterTextDisplacement);
  INIT_ENTRY(Tag);
}


void levelscript::ReadFrom (TextInput &SaveFile) {
  festring Word;
  //
  //SrcFile = SaveFile.GetFileName();
  //SrcLine = SaveFile.CurrentLine();
  if (SaveFile.ReadWord() != "{") ABORT("Bracket missing in level script line %d!", SaveFile.TokenLine());
  if (Base) {
    cv2 *Size = static_cast<const levelscript *>(Base)->GetSize();
    //
    if (Size) {
      game::GetGlobalValueMap()["XSize"] = Size->X;
      game::GetGlobalValueMap()["YSize"] = Size->Y;
    }
  }
  for (SaveFile.ReadWord(Word); Word != "}"; SaveFile.ReadWord(Word)) {
    if (Word == "Square") {
      Square.push_back(squarescript());
      Square.back().ReadFrom(SaveFile);
      continue;
    }
    if (Word == "Room") {
      Room.push_back(roomscript());
      const roomscript *RoomDefault = GetRoomDefault();
      if (RoomDefault) Room.back().SetBase(RoomDefault);
      Room.back().ReadFrom(SaveFile);
      continue;
    }
    if (!ReadMember(SaveFile, Word)) ABORT("Odd script term %s encountered in level script line %d!", Word.CStr(), SaveFile.TokenLine());
    if (Word == "Size") {
      game::GetGlobalValueMap()["XSize"] = GetSize()->X;
      game::GetGlobalValueMap()["YSize"] = GetSize()->Y;
    }
  }
  //
  const levelscript *LevelBase = static_cast<const levelscript *>(Base);
  //
  if (LevelBase && RoomDefaultHolder.Member) RoomDefaultHolder.Member->SetBase(LevelBase->RoomDefaultHolder.Member);
  valuemap::iterator i = game::GetGlobalValueMap().find("XSize");
  if (i != game::GetGlobalValueMap().end()) game::GetGlobalValueMap().erase(i);
  i = game::GetGlobalValueMap().find("YSize");
  if (i != game::GetGlobalValueMap().end()) game::GetGlobalValueMap().erase(i);
}


void levelscript::Combine (levelscript &Script) {
  if (!Base) Base = Script.Base;
  Square.splice(Square.end(), Script.Square);
  Room.splice(Room.end(), Script.Room);
  for (std::list<roomscript>::iterator i1 = Room.begin(); i1 != Room.end(); ++i1) i1->SetBase(GetRoomDefault());
  for (datamap::const_iterator i2 = DataMap.begin(); i2 != DataMap.end(); ++i2) (this->*i2->second).Replace(Script.*i2->second);
}


void levelscript::SetBase (const scriptwithbase *What) {
  const levelscript *LevelBase = static_cast<const levelscript *>(Base = What);
  roomscript *BaseRoomDefault = LevelBase->RoomDefaultHolder.Member;
  //
  if (BaseRoomDefault) {
    roomscript *ThisRoomDefault = RoomDefaultHolder.Member;
    if (!ThisRoomDefault) {
      for (std::list<roomscript>::iterator i = Room.begin(); i != Room.end(); ++i) i->SetBase(BaseRoomDefault);
    } else {
      ThisRoomDefault->SetBase(BaseRoomDefault);
    }
  }
}


void levelscript::Save (outputfile &SaveFile) const {
  script::Save(SaveFile);
  //if (Tag) SaveFile << Tag; else SaveFile << "";
  SaveFile << Square << Room;
}


void levelscript::Load (inputfile &SaveFile) {
  script::Load(SaveFile);
  SaveFile >> Square >> Room;
  //
  const roomscript *RoomDefault = GetRoomDefault();
  if (RoomDefault) for (std::list<roomscript>::iterator i = Room.begin(); i != Room.end(); ++i) i->SetBase(RoomDefault);
}


dungeonscript::dungeonscript () {
}


dungeonscript::~dungeonscript () {
}


const std::map<int, levelscript> &dungeonscript::GetLevel () const { return Level; }


void dungeonscript::InitDataMap () {
  INIT_ENTRY(LevelDefault);
  INIT_ENTRY(Levels);
  INIT_ENTRY(Description);
  INIT_ENTRY(ShortDescription);
}


void dungeonscript::ReadFrom (TextInput &SaveFile) {
  festring Word;
  //
  //SrcFile = SaveFile.GetFileName();
  //SrcLine = SaveFile.CurrentLine();
  if (SaveFile.ReadWord() != "{") ABORT("Bracket missing in dungeon script line %d!", SaveFile.TokenLine());
  for (SaveFile.ReadWord(Word); Word != "}"; SaveFile.ReadWord(Word)) {
    if (Word == "Level") {
      int Index = SaveFile.ReadNumber();
      std::pair<std::map<int, levelscript>::iterator, bool> Return = Level.insert(std::make_pair(Index, levelscript()));
      //
      if (Return.second) {
        levelscript &LS = Return.first->second;
        const levelscript *LevelDefault = GetLevelDefault();
        //
        if (LevelDefault) LS.SetBase(LevelDefault);
        LS.ReadFrom(SaveFile);
      } else {
        ABORT("Level #%d defined again in dungeon script line %d!", Index, SaveFile.TokenLine());
      }
      continue;
    }
    if (Word == "RandomLevel") {
      interval Interval;
      ReadData(Interval, SaveFile);
      RandomLevel.push_back(std::make_pair(Interval, levelscript()));
      const levelscript *LevelDefault = GetLevelDefault();
      //
      if (LevelDefault) RandomLevel.back().second.SetBase(LevelDefault);
      RandomLevel.back().second.ReadFrom(SaveFile);
      continue;
    }
    if (!ReadMember(SaveFile, Word)) ABORT("Odd script term %s encountered in dungeon script line %d!", Word.CStr(), SaveFile.TokenLine());
  }
}


void dungeonscript::RandomizeLevels () {
  for (std::list<std::pair<interval, levelscript> >::iterator i = RandomLevel.begin(); i != RandomLevel.end(); ++i) {
    int Index = i->first.Randomize();
    Level[Index].Combine(i->second);
  }
  RandomLevel.clear();
}


void dungeonscript::Save (outputfile &SaveFile) const {
  script::Save(SaveFile);
  SaveFile << Level << RandomLevel;
}


void dungeonscript::Load (inputfile &SaveFile) {
  script::Load(SaveFile);
  SaveFile >> Level >> RandomLevel;
  const levelscript *LevelDefault = GetLevelDefault();
  if (LevelDefault) {
    for (std::map<int, levelscript>::iterator i1 = Level.begin(); i1 != Level.end(); ++i1) i1->second.SetBase(LevelDefault);
    for (std::list<std::pair<interval, levelscript> >::iterator i2 = RandomLevel.begin(); i2 != RandomLevel.end(); ++i2) i2->second.SetBase(LevelDefault);
  }
}


const std::vector<std::pair<int, int> > &teamscript::GetRelation () const { return Relation; }


void teamscript::InitDataMap () {
  INIT_ENTRY(KillEvilness);
  INIT_ENTRY(Name);
}


void teamscript::ReadFrom (TextInput &SaveFile) {
  festring Word;
  //
  //SrcFile = SaveFile.GetFileName();
  //SrcLine = SaveFile.CurrentLine();
  if (SaveFile.ReadWord() != "{") ABORT("Bracket missing in team script line %d!", SaveFile.TokenLine());
  for (SaveFile.ReadWord(Word); Word != "}"; SaveFile.ReadWord(Word)) {
    if (Word == "Relation") {
      std::pair<int, int> Rel;
      //
      Rel.first = SaveFile.ReadNumber();
      Rel.second = SaveFile.ReadNumber();
      Relation.push_back(Rel);
      continue;
    }
    if (!ReadMember(SaveFile, Word)) ABORT("Odd script term %s encountered in team script line %d!", Word.CStr(), SaveFile.TokenLine());
  }
}


void teamscript::Save (outputfile &SaveFile) const {
  script::Save(SaveFile);
  SaveFile << Relation;
}


void teamscript::Load (inputfile &SaveFile) {
  script::Load(SaveFile);
  SaveFile >> Relation;
}


const std::list<std::pair<int, teamscript> > &gamescript::GetTeam () const { return Team; }
const std::map<int, dungeonscript> &gamescript::GetDungeon () const { return Dungeon; }


void gamescript::InitDataMap () {
  //INIT_ENTRY(Dungeons);
  //INIT_ENTRY(Teams);
}


void gamescript::ReadFrom (TextInput &SaveFile) {
  festring Word;
  //
  //SrcFile = SaveFile.GetFileName();
  //SrcLine = SaveFile.CurrentLine();
  SaveFile.setGetVarCB(game::ldrGetVar);
  for (SaveFile.ReadWord(Word, false); !SaveFile.Eof(); SaveFile.ReadWord(Word, false)) {
    if (Word == "Dungeon") {
      int Index = SaveFile.ReadNumber();
      //
      if (Index < 0 || Index > 16383) ABORT("Invalid dungeon number (%d) in game script file %s line %d!", Index, SaveFile.GetFileName().CStr(), SaveFile.TokenLine());
      if (mDungeons < Index) mDungeons = Index;
      //fprintf(stderr, "dungeon: %d; mDungeons: %d\n", Index, mDungeons);
      std::pair<dungeonlist::iterator, bool> Return = Dungeon.insert(std::make_pair(Index, dungeonscript()));
      //
      if (Return.second) Return.first->second.ReadFrom(SaveFile);
      else ABORT("Dungeon #%d defined again in game script file %s line %d!", Index, SaveFile.GetFileName().CStr(), SaveFile.TokenLine());
      continue;
    }
    if (Word == "Team") {
      int Index = SaveFile.ReadNumber();
      //
      if (Index < 0 || Index > 16383) ABORT("Invalid team number (%d) in game script file %s line %d!", Index, SaveFile.GetFileName().CStr(), SaveFile.TokenLine());
      for (teamlist::const_iterator it = Team.begin(); it != Team.end(); ++it) {
        if (it->first == Index) ABORT("Team #%d redefinition in file %s at line %d!", Index, SaveFile.GetFileName().CStr(), SaveFile.TokenLine());
      }
      if (mTeams < Index+1) mTeams = Index+1;
      /*
      {
        fprintf(stderr, "new team #%d\n", Index);
        fprintf(stderr, "defined teams:\n");
        for (teamlist::const_iterator it = Team.begin(); it != Team.end(); ++it) fprintf(stderr, " %d\n", it->first);
      }
      */
      //
      Team.push_back(teamlistitem(Index, teamscript()));
      Team.back().second.ReadFrom(SaveFile);
      continue;
    }
    if (Word == "Include") {
      Word = SaveFile.ReadWord();
      if (SaveFile.ReadWord() != ";") ABORT("Invalid terminator in file %s at line %d!", SaveFile.GetFileName().CStr(), SaveFile.TokenLine());
      //fprintf(stderr, "loading: %s\n", Word.CStr());
      TextInputFile incf(inputfile::buildIncludeName(SaveFile.GetFileName(), Word), &game::GetGlobalValueMap());
      ReadFrom(incf);
      continue;
    }
    if (Word == "Message") {
      Word = SaveFile.ReadWord();
      if (SaveFile.ReadWord() != ";") ABORT("Invalid terminator in file %s at line %d!", SaveFile.GetFileName().CStr(), SaveFile.TokenLine());
      fprintf(stderr, "MESSAGE: %s\n", Word.CStr());
      continue;
    }
    if (!ReadMember(SaveFile, Word)) ABORT("Odd script term %s encountered in game script file %s line %d!", Word.CStr(), SaveFile.GetFileName().CStr(), SaveFile.TokenLine());
  }
}


void gamescript::RandomizeLevels () {
  for (std::map<int, dungeonscript>::iterator i = Dungeon.begin(); i != Dungeon.end(); ++i) i->second.RandomizeLevels();
}


void gamescript::Save (outputfile &SaveFile) const {
  script::Save(SaveFile);
  SaveFile << Team << Dungeon;
}


void gamescript::Load (inputfile &SaveFile) {
  script::Load(SaveFile);
  SaveFile >> Team >> Dungeon;
}


outputfile &operator << (outputfile &SaveFile, const gamescript *Script) {
  Script->Save(SaveFile);
  return SaveFile;
}


inputfile &operator >> (inputfile &SaveFile, gamescript *&Script) {
  Script = new gamescript;
  Script->Load(SaveFile);
  return SaveFile;
}


void scriptsystem::Initialize () {
  posscript::InitDataMap();
  materialscript::InitDataMap();
  basecontentscript::InitDataMap();
  contentscript<character>::InitDataMap();
  contentscript<item>::InitDataMap();
  contentscript<glterrain>::InitDataMap();
  contentscript<olterrain>::InitDataMap();
  squarescript::InitDataMap();
  itemcontentmap::InitDataMap();
  charactercontentmap::InitDataMap();
  glterraincontentmap::InitDataMap();
  olterraincontentmap::InitDataMap();
  roomscript::InitDataMap();
  levelscript::InitDataMap();
  dungeonscript::InitDataMap();
  teamscript::InitDataMap();
  gamescript::InitDataMap();
}
