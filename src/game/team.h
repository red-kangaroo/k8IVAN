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
  team (uLong);
  void SetRelation (team *, int);
  int GetRelation (const team *) const;
  void Hostility (team *);
  uLong GetID () const { return ID; }
  void SetID (uLong What) { ID = What; }
  void Save (outputfile &) const;
  void Load (inputfile &);
  void SetLeader (character *What) { Leader = What; }
  character *GetLeader () const { return Leader; }
  std::list<character *>::iterator Add (character *);
  void Remove (std::list<character *>::iterator);
  const std::list<character *> &GetMember () const { return Member; }
  int GetKillEvilness () const { return KillEvilness; }
  void SetKillEvilness (int What) { KillEvilness = What; }
  truth HasEnemy () const;
  int GetMembers () const { return Member.size(); }
  int GetEnabledMembers () const;
  void MoveMembersTo (charactervector &);

private:
  character *Leader;
  std::map<uLong, int> Relation;
  std::list<character *> Member;
  uLong ID;
  int KillEvilness;
};


outputfile &operator << (outputfile &, const team *);
inputfile &operator >> (inputfile &, team *&);


#endif
