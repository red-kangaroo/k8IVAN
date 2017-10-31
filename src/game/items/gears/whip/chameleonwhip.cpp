#ifdef HEADER_PHASE
ITEM(chameleonwhip, whip)
{
 public:
  virtual truth HitEffect(character*, character*, v2, int, int, truth);
 protected:
  virtual truth ScabiesHelps(ccharacter*, ccharacter*) const;
};


#else



truth chameleonwhip::HitEffect(character* Enemy, character* Hitter, v2 HitPos, int BodyPartIndex, int Direction, truth BlockedByArmour)
{
  truth BaseSuccess = meleeweapon::HitEffect(Enemy, Hitter, HitPos, BodyPartIndex, Direction, BlockedByArmour);

  if(!IsBroken() && Enemy->IsEnabled() && ScabiesHelps(Enemy, Hitter))
  {
    if(Enemy->IsPlayer() || Hitter->IsPlayer() || Enemy->CanBeSeenByPlayer() || Hitter->CanBeSeenByPlayer())
      ADD_MESSAGE("%s whip asks for the help of Scabies as it polymorphs %s.", Hitter->CHAR_PERSONAL_PRONOUN, Enemy->CHAR_DESCRIPTION(DEFINITE));

    if(Hitter->IsPlayer())
    {
      game::DoEvilDeed(20);
      game::GetGod(SCABIES)->AdjustRelation(10);
    }

    int CurrentDanger = int(Enemy->GetRelativeDanger(PLAYER) * 1000);
    Enemy->PolymorphRandomly(CurrentDanger / 4, Min(CurrentDanger, 999999), 100 + RAND() % 400);
    return true;
  }
  else
    return BaseSuccess;
}



truth chameleonwhip::ScabiesHelps(ccharacter* Enemy, ccharacter* Hitter) const
{
  if(!Enemy->IsPolymorphable())
    return false;

  if(Hitter->IsPlayer())
  {
    if(game::GetGod(SCABIES)->GetRelation() < 0)
      return false;
    else
      return !(RAND() % (20 - game::GetGod(SCABIES)->GetRelation() / 150));
  }
  else
    return !(RAND() % 20);
}
#endif
