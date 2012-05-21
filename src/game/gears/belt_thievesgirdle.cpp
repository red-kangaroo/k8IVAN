#ifdef HEADER_PHASE
ITEM(thievesgirdle, belt)
{
public:
  virtual truth HitEffect (character *, character *, v2, int, int, truth);
protected:
  virtual truth CleptiaHelps (ccharacter *, ccharacter *) const;
};


#else


truth thievesgirdle::HitEffect (character *Enemy, character *Hitter, v2 HitPos, int BodyPartIndex, int Direction, truth BlockedByArmour) {
  truth BaseSuccess = item::HitEffect(Enemy, Hitter, HitPos, BodyPartIndex, Direction, BlockedByArmour);
  //
  if (Enemy->IsEnabled() && Hitter && CleptiaHelps(Enemy, Hitter)) {
    if (Hitter->IsPlayer()) {
      game::DoEvilDeed(10);
      game::GetGod(CLEPTIA)->AdjustRelation(10);
    }
    if (Enemy->IsPlayer() || Hitter->IsPlayer() || Enemy->CanBeSeenByPlayer() || Hitter->CanBeSeenByPlayer()) {
      ADD_MESSAGE("%s whip asks for the help of Cleptia as it steals %s %s.", Hitter->CHAR_POSSESSIVE_PRONOUN, Enemy->CHAR_POSSESSIVE_PRONOUN, Enemy->GetMainWielded()->CHAR_NAME(UNARTICLED));
    }
    Enemy->GetMainWielded()->MoveTo(GetLSquareUnder()->GetStack());
    return true;
  }
  return BaseSuccess;
}


truth thievesgirdle::CleptiaHelps (ccharacter *Enemy, ccharacter *Hitter) const {
  if (Enemy->IsImmuneToWhipOfThievery() || !Enemy->GetMainWielded() || GetMainMaterial()->GetFlexibility() <= 5) return false;
  //
  if (Hitter->IsPlayer()) {
    if (game::GetGod(CLEPTIA)->GetRelation() < 0) return false;
    return !RAND_N(10-game::GetGod(CLEPTIA)->GetRelation()/200);
  }
  return !RAND_N(10);
}


#endif
