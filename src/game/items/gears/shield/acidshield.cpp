#ifdef HEADER_PHASE
ITEM(acidshield, shield)
{
 public:
  virtual void BlockEffect(character*, character*, item*, int Type);
};


#else



void acidshield::BlockEffect(character* Blocker, character* Attacker, item* Weapon, int Type)
{
   /*k8 int CheckAttackType = 0;*/
   if(!IsBroken())
   {
      if(!RAND_N(400))
      {
        if(Weapon)
        {
          Weapon->SpillFluid(Blocker, liquid::Spawn(SULPHURIC_ACID, 200 + RAND() % 51));
          ADD_MESSAGE("%s is completely doused in sulpheric acid!", Attacker->CHAR_DESCRIPTION(DEFINITE));
          return;
        }
      }

      if(RAND_2 && Weapon)
      {
        Weapon->SpillFluid(Blocker, liquid::Spawn(SULPHURIC_ACID, 20 + RAND() % 41));
        ADD_MESSAGE("%s weapon is splashed with acid from the shield!", Attacker->CHAR_POSSESSIVE_PRONOUN);
      }

      if(!RAND_N(5))
      {
        Attacker->SpillFluid(Blocker, liquid::Spawn(SULPHURIC_ACID, 5 + RAND() % 11));
        ADD_MESSAGE("%s is splashed with acid!", Attacker->CHAR_DESCRIPTION(DEFINITE));
        return;
      }

      if(RAND_2)
      {
        Attacker->SpillFluid(Blocker, liquid::Spawn(SULPHURIC_ACID, 25 + RAND() % 26));
        ADD_MESSAGE("%s is splashed with acid from the shield!", Attacker->CHAR_DESCRIPTION(DEFINITE));
      }
   }
}
#endif
