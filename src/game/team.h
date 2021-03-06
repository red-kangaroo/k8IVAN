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
#ifndef __TEAM_H__
#define __TEAM_H__

#include "ivancommon.h"

#include <vector>
#include <list>
#include <map>

#include "typedef.h"


class outputfile;
class inputfile;
class character;

typedef std::vector<character *> charactervector;


class team {
public:
  team ();
  team (feuLong);
  virtual ~team () {}

  void SetRelation (team *, int);
  int GetRelation (const team *) const;
  void Hostility (team *);
  /*feuLong*/int GetID () const { return ID; }
  void SetID (/*feuLong*/int What) { ID = What; }
  void Save (outputfile &) const;
  void Load (inputfile &);
  void SetLeader (character *What) { Leader = What; }
  character *GetLeader () const { return Leader; }
  //std::list<character *>::iterator Add (character *);
  void Add (character *Who);
  void Remove (character *Who);
  //void Remove (std::list<character *>::iterator);
  const std::list<character *> &GetMember () const { return Member; }
  int GetKillEvilness () const { return KillEvilness; }
  void SetKillEvilness (int What) { KillEvilness = What; }
  truth HasEnemy () const;
  int GetMembers () const { return Member.size(); }
  int GetEnabledMembers () const;
  void MoveMembersTo (charactervector &);
  cfestring &GetName () const { return Name; }
  void SetName (cfestring &newname) { Name = newname; }

private:
  character *Leader;
  std::map<feuLong, int> Relation;
  std::list<character *> Member;
  /*feuLong*/int ID;
  int KillEvilness;
  festring Name;
};


outputfile &operator << (outputfile &, const team *);
inputfile &operator >> (inputfile &, team *&);


#endif
