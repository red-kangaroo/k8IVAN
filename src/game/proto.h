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
#ifndef __PROTO_H__
#define __PROTO_H__

#include "ivancommon.h"

#include <map>
#include <vector>

#include "ivandef.h"
#include "fesave.h"
#include "feparse.h"
#include "festring.h"
#include "game.h"


// ////////////////////////////////////////////////////////////////////////// //
class character;
class item;
class material;
class god;
class level;
class owterrain;
template <class type> class databasecreator;
struct itemdatabase;


// ////////////////////////////////////////////////////////////////////////// //
typedef std::map<festring, sLong> valuemap;
typedef std::vector<item*> itemvector;
typedef std::vector<itemvector> itemvectorvector;
typedef std::vector<character*> charactervector;
typedef std::vector<material*> materialvector;


// ////////////////////////////////////////////////////////////////////////// //
template <class type> class protocontainer {
public:
  friend class protosystem;
  friend class databasecreator<type>;

public:
  typedef typename type::prototype prototype;

public:
  static int Add (prototype *);
  static const prototype *GetProto (int I) { return GetProtoData()[I]; }
  static int SearchCodeName (cfestring &);
  static cchar *GetMainClassID () { return GetProtoData()[1]->GetClassID(); }
  static int GetSize () { return GetSizeRef(); }

  static void Cleanup ();

private:
  static int &GetSizeRef ();
  static prototype**& GetProtoData ();
  static valuemap& GetCodeNameMap ();
};


template <class type> inline void protocontainer<type>::Cleanup () {
  // nullify undefined prototypes
  for (int idx = 1; idx < GetSize(); ++idx) {
    auto proto = GetProtoData()[idx];
    if (!proto) continue;
    if (!proto->GetConfigData()) {
      // this entity has no script prototype...
      //ABORT("Seems that database is missing <%s>!", proto->GetClassID());
      fprintf(stderr, "Removing undefined entity <%s>..\n", proto->GetClassID());
      GetProtoData()[idx] = nullptr;
      auto it = GetCodeNameMap().find(proto->GetClassID());
      if (it != GetCodeNameMap().end()) GetCodeNameMap().erase(it);
    }
  }
}


template <class type> int protocontainer<type>::Add (prototype *proto) {
  if (!GetSize()) (GetProtoData() = new prototype*[1024])[GetSizeRef()++] = 0;
  int Index = GetSizeRef()++;
  GetProtoData()[Index] = proto;
  std::pair<festring, sLong> Pair(proto->GetClassID(), Index);
  GetCodeNameMap().insert(Pair);
  return Index;
}


template <class type> inline int protocontainer<type>::SearchCodeName (cfestring &Name) {
  valuemap::iterator I = GetCodeNameMap().find(Name);
  return I != GetCodeNameMap().end() ? I->second : 0;
}


// ////////////////////////////////////////////////////////////////////////// //
class protosystem {
public:
  static character* BalancedCreateMonster(level * = 0);
  static item* BalancedCreateItem(level * = 0, sLong = 0, sLong = MAX_PRICE, sLong = ANY_CATEGORY, int = 0, int = 0, int = 0, truth = false);
  static character* CreateMonster(int = 1, int = 999999, int = 0);
  static character* CreateMonster(cfestring&, int = 0, truth = true);
  static item* CreateItem(cfestring&, truth = true);
  static material* CreateMaterial(cfestring&, sLong = 0, truth = true);
  static void CreateEveryNormalEnemy(charactervector&);
//#ifdef WIZARD
  static void CreateEveryCharacter(charactervector&);
  static void CreateEveryItem(itemvectorvector&);
  static void CreateEveryMaterial(std::vector<material*>&);
//#endif
  static void Initialize();
  static void InitCharacterDataBaseFlags();
  static void SaveCharacterDataBaseFlags(outputfile&);
  static void LoadCharacterDataBaseFlags(inputfile&);
  static void CreateEverySeenCharacter(charactervector&);
  static void CreateEveryMaterial(std::vector<material*>&, const god*, ccharacter*);
 private:
  static itemdatabase** ItemConfigData;
  static int ItemConfigDataSize;
  static itemdatabase** ItemCategoryData[ITEM_CATEGORIES];
  static int ItemCategorySize[ITEM_CATEGORIES];
  static sLong ItemCategoryPossibility[ITEM_CATEGORIES];
  static sLong TotalItemPossibility;
};


template <class type> inline outputfile &operator << (outputfile &SaveFile, const type *Class) {
  //auto cppname = getCPPTypeName<type>();
  //SaveFile << cppname;
  festring tname;
  if (Class) {
    tname = festring(Class->GetTypeID());
    if (tname.IsEmpty()) ABORT("Cannot save class without type name!");
    SaveFile << tname;
    Class->Save(SaveFile);
  } else {
    if (!tname.IsEmpty()) ABORT("The thing that should not be!");
    SaveFile << tname;
  }
  return SaveFile;
}


template <class type> inline inputfile &operator >> (inputfile &SaveFile, type *&Class) {
  //festring cppname;
  //SaveFile >> cppname;
  //if (cppname != getCPPTypeName<type>()) ABORT("Loader expected '%s' c++ class, but got '%s' instead!", getCPPTypeName<type>().CStr(), cppname.CStr());
  festring tname;
  SaveFile >> tname;
  if (tname.IsEmpty()) {
    Class = nullptr;
  } else {
    auto typeidx = protocontainer<type>::SearchCodeName(tname);
    if (typeidx < 1) ABORT("Cannod load class with type name '%s'!", tname.CStr());
    //fprintf(stderr, "LOADING: '%s'\n", tname.CStr());
    Class = protocontainer<type>::GetProto(typeidx)->SpawnAndLoad(SaveFile);
  }
  return SaveFile;
}


#endif
