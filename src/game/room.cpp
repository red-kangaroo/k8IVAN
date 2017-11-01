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

/* Compiled through roomset.cpp */

roomprototype::roomprototype (roomspawner Spawner, cchar *ClassID) : Spawner(Spawner), ClassID(ClassID) {
  Index = protocontainer<room>::Add(this);
}


void room::Save (outputfile &SaveFile) const {
  SaveFile << Pos << Size << Index << DivineMaster << MasterID;
}


void room::Load (inputfile &SaveFile) {
  SaveFile >> Pos >> Size >> Index >> DivineMaster >> MasterID;
}


void room::FinalProcessForBone () {
  if (MasterID) {
    boneidmap::iterator BI = game::GetBoneCharacterIDMap().find(MasterID);
    //
    if (BI != game::GetBoneCharacterIDMap().end()) MasterID = BI->second; else MasterID = 0;
  }
}


room *roomprototype::SpawnAndLoad (inputfile &SaveFile) const {
  room *Room = Spawner();
  //
  Room->Load(SaveFile);
  return Room;
}


void room::DestroyTerrain (character *Who) {
  if (Who && MasterIsActive()) Who->Hostility(GetMaster());
  if (Who && Who->IsPlayer() && DivineMaster) game::GetGod(DivineMaster)->AdjustRelation(GetGodRelationAdjustment());
}


/* returns true if player agrees to continue */
truth room::CheckDestroyTerrain (character *Infidel) {
  if (!MasterIsActive() || Infidel == GetMaster() || GetMaster()->GetRelation(Infidel) == HOSTILE) return true;
  ADD_MESSAGE("%s might not like this.", GetMaster()->CHAR_NAME(DEFINITE));
  if (game::TruthQuestion(CONST_S("Are you sure you want to do this?"))) {
    DestroyTerrain(Infidel);
    return true;
  }
  return false;
}


truth room::MasterIsActive () const {
  character *Master = GetMaster();
  //
  return Master && Master->IsEnabled() && Master->IsConscious();
}


truth room::CheckKickSquare (ccharacter *Kicker, const lsquare *LSquare) const {
  if (!AllowKick(Kicker, LSquare)) {
    ADD_MESSAGE("That would be vandalism.");
    if (!game::TruthQuestion(CONST_S("Do you still want to do this?"))) return false;
  }
  return true;
}


character *room::GetMaster () const {
  feuLong Tick = game::GetTick();
  //
  if (LastMasterSearchTick == Tick) return Master;
  LastMasterSearchTick = Tick;
  return Master = game::SearchCharacter(MasterID);
}


truth room::WardIsActive () const {
  olterrain *PossibleWard = GetWard();
  //
  if (!PossibleWard) return false;
  return PossibleWard->IsWard(); //if it is broken, then it will return zero hopefully
}


olterrain *room::GetWard () const {
  feuLong Tick = game::GetTick();
  //
  if (LastWardSearchTick == Tick) {
    return Ward;
  } else {
    LastWardSearchTick = Tick;
    //
    std::vector<olterrain*> Found;
    olterrain *OLTerrain;
    v2 RoomPos = /*Room->*/GetPos();
    v2 RoomSize = /*Room->*/GetSize();
    //
    for (int x = RoomPos.X; x < (RoomPos.X + RoomSize.X); ++x) {
      for (int y = RoomPos.Y; y < ( RoomPos.Y + RoomSize.Y); ++y) {
        OLTerrain = game::GetCurrentLevel()->GetLSquare(x,y)->GetOLTerrain();
        if (OLTerrain && OLTerrain->IsWard()) return OLTerrain;
      }
    }
    return 0;
  }
}


truth room::IsOKToTeleportInto () const {
  return !WardIsActive();
}


truth room::IsOKToDestroyWalls (ccharacter *Infidel) const {
  return !MasterIsActive() || Infidel == GetMaster() || GetMaster()->GetRelation(Infidel) == HOSTILE;
}
