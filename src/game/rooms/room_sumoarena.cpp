#ifdef HEADER_PHASE
ROOM(sumoarena, room) {
public:
  virtual void DestroyTerrain (character *);
  virtual void HostileAction (character *) const;
  virtual truth CheckDestroyTerrain (character *);
};


#else


void sumoarena::DestroyTerrain (character *Who) {
  if (Who) Who->GetTeam()->Hostility(game::GetTeam(NEW_ATTNAM_TEAM));
}


void sumoarena::HostileAction (character *Guilty) const {
  if (Guilty) Guilty->GetTeam()->Hostility(game::GetTeam(NEW_ATTNAM_TEAM));
}


truth sumoarena::CheckDestroyTerrain (character *Infidel) {
  if (Infidel->GetTeam()->GetRelation(game::GetTeam(NEW_ATTNAM_TEAM)) == HOSTILE) return true;
  ADD_MESSAGE("The residents of New Attnam might not like this.");
  if (game::TruthQuestion(CONST_S("Are you sure you want to do this? [y/N]"))) {
    DestroyTerrain(Infidel);
    return true;
  }
  return false;
}


#endif
