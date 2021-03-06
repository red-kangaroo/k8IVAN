#ifdef HEADER_PHASE
CHARACTER(alienqueen, largecreature)
{
protected:
  virtual void GetAICommand () override;
  //virtual void CreateCorpse (lsquare *) override;
  //virtual truth MustBeRemovedFromBone () const override;
};


#else


// she moves randomly, and spawn aliens
void alienqueen::GetAICommand () {
  SeekLeader(GetLeader());
  if (FollowLeader(GetLeader())) return;

  if (!(RAND()%60)) {
    character *AlienSpawn = adultalien::Spawn();
    AlienSpawn->SetTeam(GetTeam());
    AlienSpawn->SetGenerationDanger(GetGenerationDanger());
    AlienSpawn->PutNear(GetPos());

    if (CanBeSeenByPlayer()) {
      ADD_MESSAGE("%s is shaked by small spasms.", CHAR_NAME(DEFINITE));
      if (AlienSpawn->CanBeSeenByPlayer()) ADD_MESSAGE("%s give birth!", CHAR_NAME(DEFINITE));
        //ADD_MESSAGE("%s give birth to %s!", CHAR_NAME(DEFINITE), AlienSpawn->CHAR_NAME(INDEFINITE));
    } else if (AlienSpawn->CanBeSeenByPlayer()) {
      ADD_MESSAGE("%s appears!", AlienSpawn->CHAR_NAME(INDEFINITE));
    }

    EditAP(-2000);
  }

  if (AttackAdjacentEnemyAI()) return;

  if (MoveRandomly()) return;

  EditAP(-1000);
}


#endif
