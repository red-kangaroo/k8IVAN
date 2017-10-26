#ifdef HEADER_PHASE
CHARACTER(elpuri, largecreature)
{
 public:
  elpuri() : Active(false) { }
  virtual void Save(outputfile&) const;
  virtual void Load(inputfile&);
  virtual truth Hit(character*, v2, int, int = 0);
  virtual int ReceiveBodyPartDamage(character*, int, int, int, int = 8, truth = false, truth = false, truth = true, truth = false);
  virtual truth SpecialEnemySightedReaction(character*);
  //virtual truth MustBeRemovedFromBone() const;
  virtual truth TryToRiseFromTheDead();
 protected:
  virtual void GetAICommand();
  virtual void CreateCorpse(lsquare*);
  truth Active;
};


#else



truth elpuri::SpecialEnemySightedReaction(character*) { return !(Active = true); }



truth elpuri::Hit(character* Enemy, v2, int, int Flags)
{
  if(CheckIfTooScaredToHit(Enemy))
    return false;

  character* EnemyHit[MAX_NEIGHBOUR_SQUARES];
  int EnemiesHit = 0;

  for(int d = 0; d < GetExtendedNeighbourSquares(); ++d)
    if(IsEnabled())
    {
      lsquare* Square = GetNeighbourLSquare(d);

      if(Square)
      {
  character* ByStander = Square->GetCharacter();

  if(ByStander && (ByStander == Enemy || GetRelation(ByStander) == HOSTILE))
  {
    truth Abort = false;

    for(int c = 0; c < EnemiesHit; ++c)
      if(EnemyHit[c] == ByStander)
        Abort = true;

    if(!Abort)
    {
      nonhumanoid::Hit(ByStander, Square->GetPos(), YOURSELF, Flags);
      ByStander->DamageAllItems(this, RAND() % 36 + RAND() % 36, PHYSICAL_DAMAGE);
      EnemyHit[EnemiesHit++] = ByStander;
    }
  }

  Square->GetStack()->ReceiveDamage(this, RAND() % 36 + RAND() % 36, PHYSICAL_DAMAGE, game::GetLargeMoveDirection(d));
      }
    }

  EditAP(-500);
  return true;
}



void elpuri::Save(outputfile& SaveFile) const
{
  largecreature::Save(SaveFile);
  SaveFile << Active;
}



void elpuri::Load(inputfile& SaveFile)
{
  largecreature::Load(SaveFile);
  SaveFile >> Active;
}



void elpuri::GetAICommand()
{
  if(Active)
    character::GetAICommand();
  else
  {
    if(CheckForEnemies(false, false, false))
      return;

    EditAP(-1000);
  }
}



int elpuri::ReceiveBodyPartDamage(character* Damager, int Damage, int Type, int BodyPartIndex, int Direction, truth PenetrateResistance, truth Critical, truth ShowNoDamageMsg, truth CaptureBodyPart)
{
  Active = true;
  return character::ReceiveBodyPartDamage(Damager, Damage, Type, BodyPartIndex, Direction, PenetrateResistance, Critical, ShowNoDamageMsg, CaptureBodyPart);
}



void elpuri::CreateCorpse(lsquare* Square)
{
  largecreature::CreateCorpse(Square);
  Square->AddItem(headofelpuri::Spawn());
}



truth elpuri::TryToRiseFromTheDead()
{
  character::TryToRiseFromTheDead();

  for(int c = 0; c < GetSquaresUnder(); ++c)
    for(stackiterator i = GetLSquareUnder(c)->GetStack()->GetBottom(); i.HasItem(); ++i)
      if(i->IsHeadOfElpuri())
      {
  i->SendToHell();
  i->RemoveFromSlot();
  return true;
      }

  if(CanBeSeenByPlayer())
  {
    ADD_MESSAGE("The headless body of %s vibrates violently.", CHAR_NAME(DEFINITE));
    ADD_MESSAGE("%s dies.", CHAR_NAME(DEFINITE));
  }

  return false;
}


/*
truth elpuri::MustBeRemovedFromBone() const
{
  //return !IsEnabled() || GetTeam()->GetID() != MONSTER_TEAM || GetDungeon()->GetIndex() != ELPURI_CAVE || GetLevel()->GetIndex() != DARK_LEVEL;
  return (!IsEnabled() || GetTeam()->GetID() != MONSTER_TEAM || GetLevel()->IsGCElpuriLevel());
}
*/

#endif
