#ifdef HEADER_PHASE
CHARACTER(necromancer, humanoid)
{
 public:
  virtual truth TryToRaiseZombie();
  virtual void RaiseSkeleton();
  virtual void BeTalkedTo();
 protected:
  virtual void GetAICommand();
  int GetSpellAPCost() const;
};


#else


void necromancer::BeTalkedTo () {
  if (GetConfig() != IMPRISONED_NECROMANCER) {
    humanoid::BeTalkedTo();
    return;
  }

  /* From here we are talking to the necromancer in the attnamese catacombs */
  if (GetRelation(PLAYER) == HOSTILE) {
    ADD_MESSAGE("I will bury you in the catacombs with all the others!");
    return;
  }

  if(PLAYER->HasShadowVeil() && (game::GetXinrochTombStoryState() == 1))
  {
    if(PLAYER->RemoveShadowVeil())
    {
      game::TextScreen(CONST_S("The Necromancer takes the Shadow Veil.\n"
                               "\"At last I can make my escape from Petrus' wretched clutches!\"\n"
                               "\n"
                               "The necromancer looks up \n"
                               "\"Oh, you are still here. Good! Pray tell me, what did you find in the Tomb?\n"
                               "A portal? Did you traverse it? Of course! You can't do so bodily,\n"
                               "unless you were... ...changed in some way?\"\n"));

      game::TextScreen(CONST_S("You feel a cold, tingling sensation in the middle of your forehead.\n"
                               "\n\n"
                               "\"Here, I give you the seal of the undead. You will be able to traverse\n"
                               "the portal without the use of the shadow veil. Go forth!\""));

      GetArea()->SendNewDrawRequest();
      ADD_MESSAGE("\"You can still retrieve the lost flaming ruby sword. If you go beyond the portal, you will find the one who carries this lost sword. But be warned, he is a terrible foe!\"");
      game::SetXinrochTombStoryState(2);
    }
    return;
  }

  if(PLAYER->HasLostRubyFlamingSword())
  {
    ADD_MESSAGE("The necromancer exclaims: \"What are you still doing down here? That sword belongs to the Champion of Infuscor!\"");
    return;
  }
  else if(game::GetXinrochTombStoryState() == 2)
    ADD_MESSAGE("The necromancer says: \"I am just preparing to leave. Have you found that flaming ruby sword yet?\"");

  if(game::GetXinrochTombStoryState() == 1)
  {
    ADD_MESSAGE("The necromancer says: \"Bring me the shadow veil and we'll talk.\"");
    return;
  }

  if(PLAYER->HasEncryptedScroll() && !game::GetXinrochTombStoryState())
  {
    ADD_MESSAGE("The necromancer looks up. \"Have you got the encrypted scroll?\"");

    if(game::TruthQuestion(CONST_S("Will you give the encrypted scroll to the necromancer? [y/n]"), REQUIRES_ANSWER))
    {
      if(PLAYER->RemoveEncryptedScroll())
      {
        game::TextScreen(CONST_S("The necromancer takes the scroll and mutters an incantation in a low voice.\n"
                                 "To your surprise, the words rearrange themselves on the page,\n"
                                 "revealing a previously inscrutable message.\n"
                                 "The necromancer scans the page from left to right several times. His face contorts:\n"
                                 "\"Bah! A canticle of Saint Petrus the Lion-Hearted!\"\n"
                                 "He continues down the page. His eyes widen:\n"
                                 "\"O ho! 10 000 bananas? It sounds bad out in the colonies. I'm sorry to hear about it.\"\n"
                                 "\n"
                                 "The necromancer allows the scroll to burn and the ashes wither away in his hands.\n"
                                 "\"Alas, no news about my trial. But thank you for sharing.\"\n"
                                 "\n"
                                 "\"What am I doing here you ask? You could say I spent some time arranging things\n"
                                 "in the catacombs below. I was the undertaker for the city of Attnam, you see.\n"
                                 "Well, curiosity got the better of me and I admit I dabbled in some necromancy.\n"
                                 "223 years later, and I'm still down here, drinking blood, eating bones, and generally \n"
                                 "trying all the old life-extension tricks. Finally I got caught out by that meddling Haedlac.\n"
                                 "He's got nothing better to do these days. Sent me here to the Cellar, never too far away\n"
                                 "from my minions. But alas, no more necromancy, that stupid floating eye hovers by here\n"
                                 "every now and again to check up on me.\"\n"));

        game::TextScreen(CONST_S("\"Wait, don't go yet! It gets lonely here, with no one to talk to but the punishers.\n"
                                 "Keep me company a little longer, please... Maybe I can tell you a story? I can relate\n"
                                 "the history of dark knighthood to you.\n\n"
                                 "Long ago, there lived a powerful warrior, Xinroch, who rose up the ranks\n"
                                 "of the fearsome order of the dark knights, to become the grand master dark knight. \n\n"
                                 "His soul dwells within his mausoleum, not far from here. He doesn't stand a chance\n"
                                 "of returning to us; not without a piece of his soul getting out. There is a cadre\n"
                                 "of devoted dark knights, called the Templars. Being eager to protect the resting place\n"
                                 "of their legendary master, they may obstruct your entry to the tomb. Little do they know\n"
                                 "that in order for their master to be reborn, his spirit must be freed from the place.\n"
                                 "Of course, disturbing such a restless soul would be dangerous. You may need to subdue it\n"
                                 "by force to gain what you need. Legend has it Xinroch's spirit is able to wield weapons,\n"
                                 "and possesses a cloak of unimaginable usefulness: The Shadow Veil.\""));

        game::TextScreen(CONST_S("The necromancer suddenly looks at you intently.\n"
                                 "\"Tell you what, I think you can help me out. But first I'll need proof of your abilities.\n"
                                 "It will take all your wits to survive the powers of the Tomb of Xinroch to the very end.\"\n\n"
                                 "Bring me this shadow veil, and I might be able to help you in a lasting way. I need the\n"
                                 "shadow veil to help make my escape from Attnam. It has certain properties conducive to\n"
                                 "getting away unnoticed.\"\n\n"
                                 "\"There is also the matter of Xinroch's lost sword. Its power lies in its symbolism.\n"
                                 "If you were to gain it somehow, then I imagine most believers would be convinced that\n"
                                 "you were Xinroch himself, returned to the flesh. Although you would need to prove this\n"
                                 "with the help of our god, Infuscor... ...it might require some offering, or exchange?\n"
                                 "I cannot say what trial would await you to retrieve the lost sword.\""));

        game::LoadWorldMap();
        v2 XinrochTombPos = game::GetWorldMap()->GetEntryPos(0, XINROCH_TOMB);
        game::GetWorldMap()->GetWSquare(XinrochTombPos)->ChangeOWTerrain(xinrochtomb::Spawn());
        game::GetWorldMap()->RevealEnvironment(XinrochTombPos, 1);
        game::SaveWorldMap();

        ADD_MESSAGE("\"By the way, if you find anything belonging to Xinroch, then don't lose it! I have a feeling it will help you greatly in your quest.\"");
        game::SetXinrochTombStoryState(1);
        return;
      }
    }
    else
    {
      ADD_MESSAGE("The necromancer looks downcast. "
                  "\"I see. I guess I shall have to wait for another adventurer then.\"");
      return;
    }
  }
  else if(!game::GetXinrochTombStoryState()) /* XinrochTombStoryState == 0 */
    ADD_MESSAGE("The necromancer says: \"Bring me the encrypted scroll and we'll talk.\"");

  return;
}


void necromancer::GetAICommand()
{
  SeekLeader(GetLeader());

  if(FollowLeader(GetLeader()))
    return;

  character* NearestEnemy = 0;
  sLong NearestEnemyDistance = 0x7FFFFFFF;
  v2 Pos = GetPos();

  for(int c = 0; c < game::GetTeams(); ++c)
    if(GetTeam()->GetRelation(game::GetTeam(c)) == HOSTILE)
    {
      for(std::list<character*>::const_iterator i = game::GetTeam(c)->GetMember().begin(); i != game::GetTeam(c)->GetMember().end(); ++i)
        if((*i)->IsEnabled())
        {
          sLong ThisDistance = Max<sLong>(abs((*i)->GetPos().X - Pos.X), abs((*i)->GetPos().Y - Pos.Y));

          if((ThisDistance < NearestEnemyDistance || (ThisDistance == NearestEnemyDistance && !(RAND() % 3))) && (*i)->CanBeSeenBy(this))
          {
            NearestEnemy = *i;
            NearestEnemyDistance = ThisDistance;
          }
        }
    }

  if(NearestEnemy && NearestEnemy->GetPos().IsAdjacent(Pos))
  {
    if(GetConfig() == MASTER_NECROMANCER && !(RAND() & 3))
    {
      if(CanBeSeenByPlayer())
        ADD_MESSAGE("%s invokes a spell and disappears.", CHAR_NAME(DEFINITE));

      TeleportRandomly(true);
      EditAP(-GetSpellAPCost());
      return;
    }
    else if(NearestEnemy->IsSmall()
            && GetAttribute(WISDOM) < NearestEnemy->GetAttackWisdomLimit()
            && !(RAND() & 3)
            && Hit(NearestEnemy, NearestEnemy->GetPos(), game::GetDirectionForVector(NearestEnemy->GetPos() - GetPos())))
          return;
  }

  if(!NearestEnemy)
  {
    if(!RAND_N(3) && TryToRaiseZombie())
      return;
  }
  else
  {
    if(!RAND_N(6) && TryToRaiseZombie())
      return;
  }

  if(NearestEnemy && !(RAND() % (GetConfig() == APPRENTICE_NECROMANCER ? 3 : 2)))
  {
    lsquare* Square = NearestEnemy->GetLSquareUnder();
    EditAP(-GetSpellAPCost());

    if(CanBeSeenByPlayer())
      ADD_MESSAGE("%s invokes a spell!", CHAR_NAME(DEFINITE));

    truth Interrupt = false;

    switch(GetConfig())
    {
     case APPRENTICE_NECROMANCER:
      RaiseSkeleton();
      break;
     case MASTER_NECROMANCER:
      if(RAND() % 5)
        RaiseSkeleton();
      else
      {
        Square->DrawLightning(v2(8, 8), WHITE, YOURSELF);

        beamdata Beam
          (
            this,
            CONST_S("killed by the spells of ") + GetName(INDEFINITE),
            YOURSELF,
            0
          );

        Square->Lightning(Beam);
        Interrupt = true;
      }

      break;
    }

    if(Interrupt) {
      if(CanBeSeenByPlayer())
        NearestEnemy->DeActivateVoluntaryAction(CONST_S("The spell of ") + GetName(DEFINITE) + CONST_S(" interrupts you."));
      else
        NearestEnemy->DeActivateVoluntaryAction(CONST_S("The spell interrupts you."));
    }
    return;
  }

  if(NearestEnemy && (NearestEnemyDistance < 10 || StateIsActivated(PANIC)) && RAND() & 3)
  {
    SetGoingTo((Pos << 1) - NearestEnemy->GetPos());

    if(MoveTowardsTarget(true))
      return;
  }

  if (GetConfig() == IMPRISONED_NECROMANCER && !(GetRelation(PLAYER) == HOSTILE)) {
    humanoid::MoveTowardsHomePos();
    return;
  }

  if(CheckForDoors())
    return;

  if(CheckSadism())
    return;

  if(MoveRandomly())
    return;

  EditAP(-1000);
}



truth necromancer::TryToRaiseZombie()
{
  for(int c = 0; c < game::GetTeams(); ++c)
    for(std::list<character*>::const_iterator i = game::GetTeam(c)->GetMember().begin();
  i != game::GetTeam(c)->GetMember().end(); ++i)
      if(!(*i)->IsEnabled() && (*i)->GetMotherEntity()
   && (*i)->GetMotherEntity()->Exists()
   && (GetConfig() == MASTER_NECROMANCER
       || (*i)->GetMotherEntity()->GetSquareUnderEntity()->CanBeSeenBy(this)))
      {
  character* Zombie = (*i)->GetMotherEntity()->TryNecromancy(this);

  if(Zombie)
  {
    if(Zombie->CanBeSeenByPlayer())
      ADD_MESSAGE("%s calls %s back to cursed undead life.", CHAR_DESCRIPTION(DEFINITE), Zombie->CHAR_NAME(INDEFINITE));
    else if(CanBeSeenByPlayer())
      ADD_MESSAGE("%s casts a spell, but you notice no effect.", CHAR_NAME(DEFINITE));

    EditAP(-GetSpellAPCost());
    return true;
  }
      }

  return false;
}



void necromancer::RaiseSkeleton()
{
  /* Gum solution */

  const database* WarLordDataBase;
  databasecreator<character>::FindDataBase(WarLordDataBase, &skeleton::ProtoType, WAR_LORD);
  skeleton* Skeleton;

  if(GetConfig() == MASTER_NECROMANCER && !(WarLordDataBase->Flags & HAS_BEEN_GENERATED) && !(game::GetCurrentDungeonIndex() == XINROCH_TOMB) && !(RAND() % 250))
  {
    Skeleton = skeleton::Spawn(WAR_LORD);
    Skeleton->SetTeam(GetTeam());
    Skeleton->PutNear(GetPos());
    Skeleton->SignalGeneration();

    if(Skeleton->CanBeSeenByPlayer())
      ADD_MESSAGE("The whole area trembles terribly as %s emerges from the ground.", Skeleton->CHAR_NAME(DEFINITE));
    else if(CanBeSeenByPlayer())
      ADD_MESSAGE("%s casts a powerful spell which makes the whole area tremble.", CHAR_NAME(DEFINITE));
    else
      ADD_MESSAGE("You feel the presence of an ancient evil being awakened from its long slumber. You shiver.");
  }
  else
  {
    Skeleton = skeleton::Spawn(GetConfig() == APPRENTICE_NECROMANCER ? 0 : WARRIOR, NO_EQUIPMENT);
    Skeleton->SetTeam(GetTeam());
    Skeleton->PutNear(GetPos());

    if(Skeleton->CanBeSeenByPlayer())
      ADD_MESSAGE("The ground shakes and %s emerges from it.", Skeleton->CHAR_NAME(INDEFINITE));
    else if(CanBeSeenByPlayer())
      ADD_MESSAGE("%s casts a spell, but you notice no effect.", CHAR_NAME(DEFINITE));
  }

  Skeleton->SetGenerationDanger(GetGenerationDanger());
  EditAP(-GetSpellAPCost());
}



int necromancer::GetSpellAPCost() const
{
  switch(GetConfig())
  {
   case APPRENTICE_NECROMANCER: return 2000;
   case MASTER_NECROMANCER: return 1000;
  }

  return 4000;
}
#endif
