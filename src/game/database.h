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
#ifndef __DATABASE_H__
#define __DATABASE_H__

#include "ivancommon.h"

#include <map>

#include "typedef.h"


class inputfile;
class TextInput;
class festring;


template <class database> struct databasememberbase {
  virtual ~databasememberbase () {}
  virtual void ReadData(database &, TextInput &) = 0;
};


template <class type> class databasecreator {
public:
  typedef typename type::database database;
  typedef typename type::prototype prototype;
  typedef std::map<festring, databasememberbase<database> *> databasemembermap;

  virtual ~databasecreator () {}

  static void ReadFrom (const festring &baseFileName);
  static void FindDataBase (const database *&, const prototype *, int);
  static void InstallDataBase (type *, int);
  static void CreateDataBaseMemberMap ();
  static int CreateDivineConfigurations (const prototype *, database **, int);

private:
  static truth AnalyzeData (TextInput &, cfestring &, database &);
  static void CheckDefaults (cfestring &, database &);
  static void CreateLTerrainDataBaseMemberMap ();
  static void SetBaseValue (cfestring &, databasememberbase<database> *, database &);
  static databasemembermap &GetDataBaseMemberMap();
};


class databasesystem {
public:
  virtual ~databasesystem () {}

  static void Initialize ();
};


#endif
