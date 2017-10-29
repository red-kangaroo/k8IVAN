#ifdef HEADER_PHASE
OLTERRAIN(fountain, olterrain)
{
public:
  fountain () : SecondaryMaterial(0) {}
  virtual ~fountain ();

  virtual truth SitOn (character *);
  virtual truth Drink (character *);
  virtual truth HasDrinkEffect () const;
  virtual void DryOut ();
  virtual truth DipInto (item *, character *);
  virtual truth IsDipDestination () const;
  virtual material *GetSecondaryMaterial () const;
  virtual void SetSecondaryMaterial (material *, int = 0);
  virtual void ChangeSecondaryMaterial (material *, int = 0);
  void InitMaterials (material *, material *, truth = true);
  virtual void Save (outputfile &) const;
  virtual void Load (inputfile &);
  virtual int GetMaterials () const;
  virtual material *GetMaterial (int) const;
  virtual void InitMaterials (const materialscript *, const materialscript *, truth);
  virtual truth IsFountainWithWater () const;
  virtual int GetSparkleFlags () const;

protected:
  virtual void GenerateMaterials ();
  virtual col16 GetMaterialColorB (int) const;
  virtual void AddPostFix (festring &String, int) const;
  virtual truth AddAdjective (festring &, truth) const;
  virtual v2 GetBitmapPos (int) const;

protected:
  material *SecondaryMaterial;
};


#else


truth fountain::HasDrinkEffect () const { return true; }
material *fountain::GetSecondaryMaterial () const { return SecondaryMaterial; }
int fountain::GetMaterials () const { return 2; }
void fountain::AddPostFix (festring &String, int) const { AddContainerPostFix(String); }
void fountain::SetSecondaryMaterial (material *What, int SpecialFlags) { SetMaterial(SecondaryMaterial, What, 0, SpecialFlags); }
void fountain::ChangeSecondaryMaterial (material *What, int SpecialFlags) { ChangeMaterial(SecondaryMaterial, What, 0, SpecialFlags); }
void fountain::InitMaterials (material *M1, material *M2, truth CUP) { ObjectInitMaterials(MainMaterial, M1, 0, SecondaryMaterial, M2, 0, CUP); }
v2 fountain::GetBitmapPos (int) const { return v2(GetSecondaryMaterial() ? 16 : 32, 288); }
void fountain::InitMaterials (const materialscript *M, const materialscript *C, truth CUP) { InitMaterials(M->Instantiate(), C->Instantiate(), CUP); }


truth fountain::SitOn (character *Sitter) {
  if (GetSecondaryMaterial()) {
    ADD_MESSAGE("You sit on the fountain. Water falls on your head and you get quite wet. You feel like a moron.");
    Sitter->EditAP(-1000);
    return true;
  }
  return olterrain::SitOn(Sitter);
}


truth fountain::Drink (character *Drinker) {
  if (GetSecondaryMaterial()) {
    if (GetSecondaryMaterial()->GetConfig() == WATER) {
      room *Room = GetRoom();
      if (Room && Room->HasDrinkHandler() && !Room->Drink(Drinker)) return false;
      if (!game::TruthQuestion(CONST_S("Do you want to drink from the fountain?"))) return false;
      Drinker->EditAP(-1000);
      switch (RAND()%12) {
        case 0:
          if (RAND_N(3)) {
            ADD_MESSAGE("The water is contaminated!");
            Drinker->EditNP(100);
            if (!RAND_4) Drinker->PolymorphRandomly(0, 1000000, 2500 + RAND() % 2500);
            else {
              Drinker->ChangeRandomAttribute(-1);
              Drinker->CheckDeath(CONST_S("died of contaminated water"));
            }
            break;
          }
        case 1:
          ADD_MESSAGE("The water tasted very good.");
          Drinker->EditNP(2500);
          Drinker->ChangeRandomAttribute(1);
          break;
        case 2:
          if (!(RAND()%15)) {
            ADD_MESSAGE("You have freed a spirit that grants you a wish. You may wish for an item.");
            game::Wish(Drinker,
              "%s appears from nothing and the spirit flies happily away!",
              "Two %s appear from nothing and the spirit flies happily away!");
          } else {
            DryOut();
          }
          break;
        case 4:
          if (RAND() & 7) {
            ADD_MESSAGE("The water tastes normal, but there is an odd after taste.");
            Drinker->ActivateRandomState(SRC_FOUNTAIN, 10000 + RAND() % 20000);
          } else {
            ADD_MESSAGE("This water tastes very odd.");
            if (!Drinker->GainRandomIntrinsic(SRC_FOUNTAIN)) ADD_MESSAGE("You feel like a penguin."); /* This is rather rare, so no harm done */
          }
          break;
        case 5: {
          characterspawner Spawner = 0;
          int Config = 0, AddChance = 0;
          truth ForceAdjacency = false;
          switch (RAND_N(5)) {
            case 0:
              Spawner = (characterspawner)(&snake::Spawn);
              AddChance = 66;
              break;
            case 1:
              Spawner = (characterspawner)(&mommo::Spawn);
              Config = RAND_2 ? CONICAL : FLAT;
              AddChance = 50;
              break;
            case 2:
              Spawner = (characterspawner)(&spider::Spawn);
              if (RAND_4) {
                Config = LARGE;
                AddChance = 90;
              } else {
                Config = GIANT;
                AddChance = 75;
              }
              break;
            case 3:
              if (!RAND_N(50)) {
                Spawner = (characterspawner)(&dolphin::Spawn);
                AddChance = 75;
                ForceAdjacency = true;
              } else if (!RAND_N(50)) {
                Spawner = (characterspawner)(&mysticfrog::Spawn);
                Config = DARK;
                AddChance = 1;
              } else {
                Spawner = (characterspawner)(&frog::Spawn);
                if (RAND_N(5)) {
                  Config = DARK;
                  AddChance = 10;
                } else if (RAND_N(5)) {
                  Config = GREATER_DARK;
                  AddChance = 5;
                } else {
                  Config = GIANT_DARK;
                  AddChance = 2;
                }
              }
              break;
            case 4:
              Spawner = (characterspawner)(&largerat::Spawn);
              AddChance = 90;
              break;
          }
          int TeamIndex = RAND_N(3) ? MONSTER_TEAM : PLAYER_TEAM;
          team *Team = game::GetTeam(TeamIndex);
          int Amount = 1+femath::LoopRoll(AddChance, 7);
          spawnresult SR = GetLevel()->SpawnMonsters(Spawner, Team, GetPos(), Config, Amount, ForceAdjacency);
          msgsystem::EnterBigMessageMode();
          if (SR.Seen == 1) {
            ADD_MESSAGE("%s appears from the fountain!", SR.Pioneer->CHAR_NAME(INDEFINITE));
            if (TeamIndex == PLAYER_TEAM) ADD_MESSAGE("%s seems to be friendly.", SR.Pioneer->CHAR_PERSONAL_PRONOUN);
            if (Amount > SR.Seen) ADD_MESSAGE("Somehow you also sense %s isn't alone.", SR.Pioneer->CHAR_PERSONAL_PRONOUN);
          } else if (SR.Seen) {
            ADD_MESSAGE("%s appear from the fountain!", SR.Pioneer->GetName(PLURAL, SR.Seen).CStr());
            if (TeamIndex == PLAYER_TEAM) ADD_MESSAGE("They seem to be friendly.");
            if (Amount > SR.Seen) ADD_MESSAGE("Somehow you also sense you haven't yet seen all of them.");
          } else {
            ADD_MESSAGE("You feel something moving near you.");
          }
          msgsystem::LeaveBigMessageMode();
        } break;
        case 6:
          if (!RAND_N(5)) {
            item *ToBeCreated = protosystem::BalancedCreateItem(0, 0, MAX_PRICE, RING);
            GetLSquareUnder()->AddItem(ToBeCreated);
            if (ToBeCreated->CanBeSeenByPlayer()) ADD_MESSAGE("There's something sparkling in the water.");
            break;
          }
        case 7: {
          if (!RAND_N(2)) {
            olterrain *Found = GetLevel()->GetRandomFountainWithWater(this);
            Drinker->RemoveTraps();
            if (Found && RAND_N(3)) {
              ADD_MESSAGE("The fountain sucks you in. You are thrown through a network of tunnels and end up coming out from an other fountain.");
              Found->GetLSquareUnder()->KickAnyoneStandingHereAway();
              Drinker->Move(Found->GetPos(), true);
            } else {
              int To = GetLSquareUnder()->GetDungeon()->GetLevelTeleportDestination(GetLevel()->GetIndex());
              int From = GetLevel()->GetIndex();
              if (To == From) game::TryTravel(game::GetCurrentDungeonIndex(), To, RANDOM, true, false);
              else game::TryTravel(game::GetCurrentDungeonIndex(), To, FOUNTAIN, true, false);
              olterrain *OLTerrain = Drinker->GetLSquareUnder()->GetOLTerrain();
              if (OLTerrain && OLTerrain->IsFountainWithWater() && To != From)
                ADD_MESSAGE("The fountain sucks you in. You are thrown through a network of tunnels and end up coming out from an other fountain.");
              else
                ADD_MESSAGE("The fountain sucks you in. You are thrown through a network of tunnels. Suddenly the wall of the tunnel bursts open and you fly out with the water.");
            }
            Drinker->GetLSquareUnder()->SpillFluid(Drinker, liquid::Spawn(WATER, 1000 + RAND() % 501), false, false);
            break;
          }
        }
        default:
          ADD_MESSAGE("The water tastes good.");
          Drinker->EditNP(500);
          break;
      }
      // fountain might have dried out: don't do anything here.
      return true;
    }
    ADD_MESSAGE("You don't dare to drink from this fountain.");
    return false;
  }
  ADD_MESSAGE("The fountain has dried out.");
  return false;
}


void fountain::DryOut () {
  ADD_MESSAGE("%s dries out.", CHAR_NAME(DEFINITE));
  ChangeSecondaryMaterial(0);
  if (GetLSquareUnder()) {
    GetLSquareUnder()->SendNewDrawRequest();
    GetLSquareUnder()->SendMemorizedUpdateRequest();
  }
}


truth fountain::DipInto (item *ToBeDipped, character *Who) {
  ToBeDipped->DipInto(static_cast<liquid *>(GetSecondaryMaterial()->SpawnMore(100)), Who);
  return true;
}


void fountain::Save (outputfile &SaveFile) const {
  olterrain::Save(SaveFile);
  SaveFile << SecondaryMaterial;
}


void fountain::Load (inputfile &SaveFile) {
  olterrain::Load(SaveFile);
  LoadMaterial(SaveFile, SecondaryMaterial);
}


material *fountain::GetMaterial (int I) const {
  return !I ? MainMaterial : SecondaryMaterial;
}


col16 fountain::GetMaterialColorB (int) const {
  if (GetSecondaryMaterial()) return GetSecondaryMaterial()->GetColor();
  return 0;
}


void fountain::GenerateMaterials () {
  int Chosen = RandomizeMaterialConfiguration();
  const fearray<sLong> &MMC = GetMainMaterialConfig();
  InitMaterial(MainMaterial, MAKE_MATERIAL(MMC.Data[MMC.Size == 1 ? 0 : Chosen]), 0);
  const fearray<sLong> &SMC = GetSecondaryMaterialConfig();
  InitMaterial(SecondaryMaterial, MAKE_MATERIAL(SMC.Data[SMC.Size == 1 ? 0 : Chosen]), 0);
}


truth fountain::AddAdjective (festring &String, truth Articled) const {
  if (!GetSecondaryMaterial()) {
    String << (Articled ? "a dried out " : "dried out ");
    return true;
  }
  return false;
}


fountain::~fountain () {
  delete SecondaryMaterial;
}


int fountain::GetSparkleFlags () const {
  return (MainMaterial->IsSparkling() ? SPARKLING_A : 0) |
         (SecondaryMaterial && SecondaryMaterial->IsSparkling() ? SPARKLING_B : 0);
}


truth fountain::IsDipDestination () const {
  return SecondaryMaterial && SecondaryMaterial->IsLiquid();
}


truth fountain::IsFountainWithWater () const {
  return !!GetSecondaryMaterial();
}


#endif
