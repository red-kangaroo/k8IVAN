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

#ifndef __GAME_H__
#define __GAME_H__

#include "ivancommon.h"

#include <ctime>
#include <map>
#include <stack>
#include <vector>

#include "femath.h"
#include "festring.h"
#include "feparse.h"
#include "ivandef.h"


#ifndef LIGHT_BORDER
# define LIGHT_BORDER 80
#endif

#define PLAYER  game::GetPlayer()


class area;
class level;
class dungeon;
class felist;
class team;
class character;
class gamescript;
class item;
class outputfile;
class inputfile;
class worldmap;
class god;
class square;
class wsquare;
class lsquare;
class bitmap;
class festring;
class rain;
class liquid;
class entity;
class olterrain;
class owterrain;
struct explosion;

typedef std::map<festring, sLong> valuemap;
typedef truth (*stringkeyhandler)(int, festring&);
typedef v2 (*positionkeyhandler)(v2, int);
typedef void (*positionhandler)(v2);
typedef void (*bitmapeditor)(bitmap*, truth);


struct EventContext {
  enum ExecState {
    ESAllow,
    ESAllowStop,
    ESDisallow,
  };
  character *actor = nullptr; // "acting" actor, or item holder
  character *secondActor = nullptr; // actor that takes hit, or attacks, or...
  item *thing = nullptr; // used in `RunItemEvent()`
  int result = 0;
  ExecState state = ESAllow;

  inline void clear () {
    actor = nullptr;
    secondActor = nullptr; // actor that takes hit, or attacks, or...
    thing = nullptr; // used in `RunItemEvent()`
    result = 0;
    state = ESAllow;
  }

  inline truth allowed () const { return (state != ESDisallow); }
  inline truth wantnext () const { return (state == ESAllow); }

  inline void allow () { state = ESAllow; }
  inline void allowStop () { state = ESAllowStop; }
  inline void disallow () { state = ESDisallow; }
};


struct homedata {
  v2 Pos;
  int Dungeon;
  int Level;
  int Room;
};


outputfile& operator<<(outputfile&, const homedata*);
inputfile& operator>>(inputfile&, homedata*&);


struct configid {
  configid () {}
  configid (int Type, int Config) : Type(Type), Config(Config) {}
  bool operator < (const configid& CI) const { return memcmp(this, &CI, sizeof(configid)) < 0; }
  //
  int Type NO_ALIGNMENT;
  int Config NO_ALIGNMENT;
};

outputfile& operator<<(outputfile&, const configid&);
inputfile& operator>>(inputfile&, configid&);


struct dangerid {
  dangerid () : NakedDanger(0), EquippedDanger(0) {}
  dangerid (double NakedDanger, double EquippedDanger) : NakedDanger(NakedDanger), EquippedDanger(EquippedDanger) {}
  double NakedDanger;
  double EquippedDanger;
};

outputfile& operator<<(outputfile&, const dangerid&);
inputfile& operator>>(inputfile&, dangerid&);


struct ivantime {
  int Day;
  int Hour;
  int Min;
};


struct massacreid {
  massacreid () {}
  massacreid (int Type, int Config, cfestring &Name) : Type(Type), Config(Config), Name(Name) { }
  bool operator < (const massacreid &) const;
  //
  int Type;
  int Config;
  festring Name;
};

inline bool massacreid::operator<(const massacreid& MI) const
{
  if(Type != MI.Type)
    return Type < MI.Type;

  if(Config != MI.Config)
    return Config < MI.Config;

  return Name < MI.Name;
}

outputfile& operator<<(outputfile&, const massacreid&);
inputfile& operator>>(inputfile&, massacreid&);


struct killreason {
  killreason () {}
  killreason (cfestring &String, int Amount) : String(String), Amount(Amount) {}
  //
  festring String;
  int Amount;
};

outputfile& operator<<(outputfile&, const killreason&);
inputfile& operator>>(inputfile&, killreason&);


struct killdata {
  killdata (int Amount=0, double DangerSum=0) : Amount(Amount), DangerSum(DangerSum) {}
  //
  int Amount;
  double DangerSum;
  std::vector<killreason> Reason;
};

outputfile& operator<<(outputfile&, const killdata&);
inputfile& operator>>(inputfile&, killdata&);


typedef std::map<configid, dangerid> dangermap;
typedef std::map<feuLong, character*> characteridmap;
typedef std::map<feuLong, item*> itemidmap;
typedef std::map<feuLong, entity*> trapidmap;
typedef std::map<massacreid, killdata> massacremap;
typedef std::map<feuLong, feuLong> boneidmap;
typedef std::vector<item*> itemvector;
typedef std::vector<itemvector> itemvectorvector;
typedef std::vector<character*> charactervector;


class quitrequest {};
class areachangerequest {};


enum ArgTypes {
  FARG_UNDEFINED,
  FARG_STRING,
  FARG_NUMBER
};

struct FuncArg {
  FuncArg () : type(FARG_UNDEFINED), sval(""), ival(0) {}
  FuncArg (cfestring &aVal) : type(FARG_STRING), sval(aVal), ival(0) {}
  FuncArg (sLong aVal) : type(FARG_NUMBER), sval(""), ival(aVal) {}
  //
  ArgTypes type;
  festring sval;
  sLong ival;
};


class game {
public:
  enum { PickTimeout = 32 };
public:
  static void InitPlaces ();
  static void DeInitPlaces ();
  static truth Init(cfestring& = CONST_S(""));
  static void DeInit();
  static void Run();
  static int GetMoveCommandKey(int);
  static int MoveVectorToDirection (cv2 &mv); // -1: none
  static cv2 GetMoveVector(int I) { return MoveVector[I]; }
  static cv2 GetRelativeMoveVector(int I) { return RelativeMoveVector[I]; }
  static cv2 GetBasicMoveVector(int I) { return BasicMoveVector[I]; }
  static cv2 GetLargeMoveVector(int I) { return LargeMoveVector[I]; }
  static area* GetCurrentArea() { return CurrentArea; }
  static level* GetCurrentLevel() { return CurrentLevel; }
  static uChar*** GetLuxTable() { return LuxTable; }
  static character* GetPlayer() { return Player; }
  static void SetPlayer(character*);
  static v2 GetCamera() { return Camera; }
  static void UpdateCameraX();
  static void UpdateCameraY();
  static truth IsLoading() { return Loading; }
  static void SetIsLoading(truth What) { Loading = What; }
  static truth ForceJumpToPlayerBe() { return JumpToPlayerBe; }
  static void SetForceJumpToPlayerBe(truth What) { JumpToPlayerBe = What; }
  static level* GetLevel(int);
  static void InitLuxTable();
  static void DeInitLuxTable();
  static cchar* Insult();
  static truth TruthQuestion(cfestring&, int = 0, int = 0);
  static void DrawEverything();
  static truth Save(cfestring& = SaveName(""));
  static int Load(cfestring& = SaveName(""));
  static truth IsRunning() { return Running; }
  static void SetIsRunning(truth What) { Running = What; }
  static void UpdateCameraX(int);
  static void UpdateCameraY(int);
  static int GetCurrentLevelIndex() { return CurrentLevelIndex; }
  static int GetMoveCommandKeyBetweenPoints(v2, v2);
  static void DrawEverythingNoBlit(truth = false);
  static god* GetGod(int I) { return God[I]; }
  static cchar* GetAlignment(int I) { return Alignment[I]; }
  static void ApplyDivineTick();
  static void ApplyDivineAlignmentBonuses(god*, int, truth);
  static v2 GetDirectionVectorForKey(int);
  static festring SaveName(cfestring& = CONST_S(""));
  static void ShowLevelMessage();
  static double GetMinDifficulty();
  static void TriggerQuestForGoldenEagleShirt();
  static void CalculateGodNumber();
  static void IncreaseTick() { ++Tick; }
  static feuLong GetTick() { return Tick; }
  static festring GetAutoSaveFileName() { return AutoSaveFileName; }
  static int DirectionQuestion(cfestring&, truth = true, truth = false);
  static void RemoveSaves(truth = true);
  static truth IsInWilderness() { return InWilderness; }
  static void SetIsInWilderness(truth What) { InWilderness = What; }
  static worldmap* GetWorldMap() { return WorldMap; }
  static void SetAreaInLoad(area* What) { AreaInLoad = What; }
  static void SetSquareInLoad(square* What) { SquareInLoad = What; }
  static area* GetAreaInLoad() { return AreaInLoad; }
  static square* GetSquareInLoad() { return SquareInLoad; }
  static int GetLevels();
  static dungeon* GetCurrentDungeon() { return Dungeon[CurrentDungeonIndex]; }
  static dungeon* GetDungeon(int I) { return Dungeon[I]; }
  static int GetCurrentDungeonIndex() { return CurrentDungeonIndex; }
  static void InitDungeons();
  static truth OnScreen(v2);
  static void DoEvilDeed(int);
  static void SaveWorldMap(cfestring& = SaveName(""), truth = true);
  static worldmap* LoadWorldMap(cfestring& = SaveName(""));
  static void UpdateCamera();
  static feuLong CreateNewCharacterID(character*);
  static feuLong CreateNewItemID(item*);
  static feuLong CreateNewTrapID(entity*);
  static team* GetTeam(int I) { return Team[I]; }
  static int GetTeams() { return Teams; }
  static void Hostility(team*, team*);
  static void CreateTeams();
  static festring StringQuestion(cfestring&, col16, festring::sizetype, festring::sizetype, truth, stringkeyhandler = 0);
  static sLong NumberQuestion(cfestring&, int, truth = false);
  static feuLong IncreaseLOSTick();
  static feuLong GetLOSTick() { return LOSTick; }
  static void SendLOSUpdateRequest() { LOSUpdateRequested = true; }
  static void RemoveLOSUpdateRequest() { LOSUpdateRequested = false; }
  static character* GetPetrus() { return Petrus; }
  static void SetPetrus(character* What) { Petrus = What; }
  static truth HandleQuitMessage();
  static int GetDirectionForVector(v2);
  static cchar* GetVerbalPlayerAlignment();
  static void CreateGods();
  static int GetScreenXSize() { return 42; }
  static int GetScreenYSize() { return 26; }
  static v2 CalculateScreenCoordinates(v2);
  static void BusyAnimation();
  static void BusyAnimation(bitmap*, truth);
  static v2 PositionQuestion(cfestring&, v2, positionhandler = 0, positionkeyhandler = 0, truth = true);
  static void LookHandler(v2);
  static int AskForKeyPress(cfestring&);
  static void AskForEscPress(cfestring &Topic);
  static truth AnimationController();
  static gamescript* GetGameScript() { return GameScript; }
  static void InitScript();

  static valuemap& GetGlobalValueMap() { return GlobalValueMap; }
  static truth HasGlobalValue (cfestring &name);
  static sLong FindGlobalValue (cfestring &name, sLong defval=-1, truth* found=0);
  static sLong FindGlobalValue (cfestring &name, truth* found=0);

  static void ScheduleImmediateSave ();

  // this will fail if there is no such constant
  //TODO: cache values
  static sLong GetGlobalConst (cfestring &name);

  static void LoadModuleList ();
  static const std::vector<festring> &GetModuleList ();

  static void SaveModuleList (outputfile &ofile);
  static truth LoadAndCheckModuleList (inputfile &ifile); // false: incomaptible, ifile left in undefined state

  static void InitGlobalValueMap ();
  static void LoadGlobalValueMap (TextInput &SaveFile);

  static void TextScreen(cfestring&, v2 = ZERO_V2, col16 = 0xFFFF, truth = true, truth = true, bitmapeditor = 0);
  static void SetCursorPos(v2 What) { CursorPos = What; }
  static truth DoZoom() { return Zoom; }
  static void SetDoZoom(truth What) { Zoom = What; }
  static int KeyQuestion(cfestring&, int, int, ...);
  static v2 LookKeyHandler(v2, int);
  static v2 NameKeyHandler(v2, int);
  static void End(festring, truth = true, truth = true);
  static int CalculateRoughDirection(v2);
  static sLong ScrollBarQuestion(cfestring&, sLong, sLong, sLong, sLong, sLong, col16, col16, col16, void (*)(sLong) = 0);
  static truth IsGenerating() { return Generating; }
  static void SetIsGenerating(truth What) { Generating = What; }
  static void CalculateNextDanger();
  static int Menu(bitmap*, v2, cfestring&, cfestring&, col16, cfestring& = "", cfestring& = "");
  static void InitDangerMap();
  static const dangermap& GetDangerMap();
  static truth TryTravel(int, int, int, truth = false, truth = true);
  static truth LeaveArea(charactervector&, truth, truth);
  static void EnterArea(charactervector&, int, int);
  static int CompareLights(col24, col24);
  static int CompareLightToInt(col24, col24);
  static void CombineLights(col24&, col24);
  static col24 CombineConstLights(col24, col24);
  static truth IsDark(col24);
  static void SetStandardListAttributes(felist&);
  static double GetAveragePlayerArmStrengthExperience() { return AveragePlayerArmStrengthExperience; }
  static double GetAveragePlayerLegStrengthExperience() { return AveragePlayerLegStrengthExperience; }
  static double GetAveragePlayerDexterityExperience() { return AveragePlayerDexterityExperience; }
  static double GetAveragePlayerAgilityExperience() { return AveragePlayerAgilityExperience; }
  static void InitPlayerAttributeAverage();
  static void UpdatePlayerAttributeAverage();
  static void CallForAttention(v2, int);
  static character* SearchCharacter(feuLong);
  static item* SearchItem(feuLong);
  static entity* SearchTrap(feuLong);
  static void AddCharacterID(character*, feuLong);
  static void RemoveCharacterID(feuLong);
  static void AddItemID(item*, feuLong);
  static void RemoveItemID(feuLong);
  static void UpdateItemID(item*, feuLong);
  static void AddTrapID(entity*, feuLong);
  static void RemoveTrapID(feuLong);
  static void UpdateTrapID(entity*, feuLong);
  static int GetStoryState() { return StoryState; }
  static void SetStoryState(int What) { StoryState = What; }
  static int GetXinrochTombStoryState() { return XinrochTombStoryState; }
  static void SetXinrochTombStoryState(int What) { XinrochTombStoryState = What; }
  static int GetMondedrPass () { return MondedrPass; }
  static void SetMondedrPass (int What) { MondedrPass = What; }
  static int GetRingOfThieves () { return RingOfThieves; }
  static void SetRingOfThieves (int What) { RingOfThieves = What; }
  static int GetMasamune () { return Masamune; }
  static void SetMasamune (int What) { Masamune = What; }
  static int GetMuramasa () { return Muramasa; }
  static void SetMuramasa (int What) { Muramasa = What; }
  static int GetLoricatusHammer () { return LoricatusHammer; }
  static void SetLoricatusHammer (int What) { LoricatusHammer = What; }
  static int GetLiberator () { return Liberator; }
  static void SetLiberator (int What) { Liberator = What; }
  static int GetOmmelBloodMission() { return OmmelBloodMission; }
  static void SetOmmelBloodMission(int What) { OmmelBloodMission = What; }
  static int GetRegiiTalkState() { return RegiiTalkState; }
  static void SetRegiiTalkState(int What) { RegiiTalkState = What; }
  static void SetIsInGetCommand(truth What) { InGetCommand = What; }
  static truth IsInGetCommand() { return InGetCommand; }
  static festring GetHomeDir();
  static festring GetSavePath();
  static festring GetGameDir();
  static festring GetBonePath();
  static truth PlayerWasHurtByExplosion() { return PlayerHurtByExplosion; }
  static void SetPlayerWasHurtByExplosion(truth What) { PlayerHurtByExplosion = What; }
  static void SetCurrentArea(area* What) { CurrentArea = What; }
  static void SetCurrentLevel(level* What) { CurrentLevel = What; }
  static void SetCurrentWSquareMap(wsquare*** What) { CurrentWSquareMap = What; }
  static void SetCurrentLSquareMap(lsquare*** What) { CurrentLSquareMap = What; }
  static festring& GetDefaultPolymorphTo() { return DefaultPolymorphTo; }
  static festring& GetDefaultSummonMonster() { return DefaultSummonMonster; }
  static festring& GetDefaultChangeMaterial() { return DefaultChangeMaterial; }
  static festring& GetDefaultDetectMaterial() { return DefaultDetectMaterial; }
  static festring& GetDefaultTeam() { return DefaultTeam; }
  static void SignalDeath(ccharacter*, ccharacter*, festring);
  static void DisplayMassacreLists();
  static void DisplayMassacreList(const massacremap&, cchar*, sLong);
  static truth MassacreListsEmpty();
#ifdef WIZARD
  static void ActivateWizardMode() { WizardMode = true; }
  static void DeactivateWizardMode() { WizardMode = false; }
  static truth WizardModeIsActive() { return WizardMode; }
  static void SeeWholeMap();
  static int GetSeeWholeMapCheatMode() { return SeeWholeMapCheatMode; }
  static truth GoThroughWallsCheatIsActive() { return GoThroughWallsCheat; }
  static void GoThroughWalls() { GoThroughWallsCheat = !GoThroughWallsCheat; }
#else
  static truth WizardModeIsActive() { return false; }
  static int GetSeeWholeMapCheatMode() { return 0; }
  static truth GoThroughWallsCheatIsActive() { return false; }
#endif
  static truth WizardModeIsReallyActive() { return WizardMode; }
  static void CreateBone();
  static int GetQuestMonstersFound() { return QuestMonstersFound; }
  static void SignalQuestMonsterFound() { ++QuestMonstersFound; }
  static void SetQuestMonstersFound(int What) { QuestMonstersFound = What; }
  static truth PrepareRandomBone(int);
  static boneidmap& GetBoneItemIDMap() { return BoneItemIDMap; }
  static boneidmap& GetBoneCharacterIDMap() { return BoneCharacterIDMap; }
  static double CalculateAverageDanger(const charactervector&, character*);
  static double CalculateAverageDangerOfAllNormalEnemies();
  static character* CreateGhost();
  static truth TooGreatDangerFound() { return TooGreatDangerFoundTruth; }
  static void SetTooGreatDangerFound(truth What) { TooGreatDangerFoundTruth = What; }
  static void CreateBusyAnimationCache();
  static sLong GetScore();
  static truth TweraifIsFree();
  static truth IsXMas();
  static int AddToItemDrawVector(const itemvector&);
  static void ClearItemDrawVector();
  static void ItemEntryDrawer(bitmap*, v2, uInt);
  static int AddToCharacterDrawVector(character*);
  static void ClearCharacterDrawVector();
  static void CharacterEntryDrawer(bitmap*, v2, uInt);
  static void GodEntryDrawer(bitmap*, v2, uInt);
  static itemvectorvector& GetItemDrawVector() { return ItemDrawVector; }
  static charactervector& GetCharacterDrawVector() { return CharacterDrawVector; }
  static truth IsSumoWrestling() { return SumoWrestling; }
  static void SetIsSumoWrestling(truth What) { SumoWrestling = What; }
  static truth AllowHostilities() { return !SumoWrestling; }
  static truth AllBodyPartsVanish() { return SumoWrestling; }
  static truth TryToEnterSumoArena();
  static truth TryToExitSumoArena();
  static truth EndSumoWrestling(int);
  static character* GetSumo();
  static cfestring& GetPlayerName() { return PlayerName; }
  static rain* ConstructGlobalRain();
  static void SetGlobalRainLiquid(liquid* What) { GlobalRainLiquid = What; }
  static void SetGlobalRainSpeed(v2 What) { GlobalRainSpeed = What; }
  static truth PlayerIsSumoChampion() { return PlayerSumoChampion; }
  static truth PlayerIsSolicitusChampion() { return PlayerSolicitusChampion; }
  static void MakePlayerSolicitusChampion() { PlayerSolicitusChampion = true; }
  static truth ChildTouristHasSpider () { return TouristHasSpider; }
  static void SetTouristHasSpider () { TouristHasSpider = true; }
  static v2 GetSunLightDirectionVector();
  static int CalculateMinimumEmitationRadius(col24);
  static feuLong IncreaseSquarePartEmitationTicks();
  static cint GetLargeMoveDirection(int I) { return LargeMoveDirection[I]; }
  static bool Wish(character*, cchar*, cchar*, bool canAbort=false);
  static festring DefaultQuestion(festring, festring&, stringkeyhandler = 0);
  static void GetTime(ivantime&);
  static sLong GetTurn() { return Turn; }
  static void IncreaseTurn() { ++Turn; }
  static int GetTotalMinutes() { return Tick * 60 / 2000; }
  static truth PolymorphControlKeyHandler(int, festring&);
  static feuLong* GetEquipmentMemory() { return EquipmentMemory; }
  static truth PlayerIsRunning();
  static void SetPlayerIsRunning(truth What) { PlayerRunning = What; }
  static truth FillPetVector(cchar*);
  static truth CommandQuestion();
  static void NameQuestion();
  static v2 CommandKeyHandler(v2, int);
  static void CommandScreen(cfestring&, feuLong, feuLong, feuLong&, feuLong&);
  static truth CommandAll();
  static double GetDangerFound() { return DangerFound; }
  static void SetDangerFound(double What) { DangerFound = What; }
  static col16 GetAttributeColor(int);
  static void UpdateAttributeMemory();
  static void InitAttributeMemory();
  static void TeleportHandler(v2);
  static void PetHandler(v2);
  static truth SelectPet(int);
  static double GetGameSituationDanger();
  static olterrain* GetMonsterPortal() { return MonsterPortal; }
  static void SetMonsterPortal(olterrain* What) { MonsterPortal = What; }
  static truth GetCausePanicFlag() { return CausePanicFlag; }
  static void SetCausePanicFlag(truth What) { CausePanicFlag = What; }
  static sLong GetTimeSpent();
  static void AddSpecialCursor(v2, int);
  static void RemoveSpecialCursors();
  static void LearnAbout(god*);
  static truth PlayerKnowsAllGods();
  static void AdjustRelationsToAllGods(int);
  static void SetRelationsToAllGods(int);
  static void ShowDeathSmiley(bitmap*, truth);
  static void SetEnterImage(cbitmap* What) { EnterImage = What; }
  static void SetEnterTextDisplacement (v2 What) { EnterTextDisplacement = What; }

  static int ListSelector (int defsel, const cfestring title, ...); // defsel<0: first
  static int ListSelectorArray (int defsel, cfestring &title, const char *items[]); // defsel<0: first

  //static char GetNormalMoveKey (int idx);
  //static void SetNormalMoveKey (int idx, char ch);

  static truth CheckDropLeftover (item *i);

  static team *FindTeam (cfestring &name);
  //static team *FindTeam (const char *name);

  // return 'true' if `Disallow` was called
  static truth RunGlobalEvent (cfestring &ename); // global game event

  static truth RunEventWithCtx (EventContext &ctx, const EventHandlerMap &evmap, cfestring &ename);

  static truth RunCharEvent (cfestring &ename, character *who, character *second=0, item *it=0);
  static truth RunItemEvent (cfestring &ename, item *what, character *holder, character *second=0);

  static truth RunCharAllowScript (character *tospawn, const EventHandlerMap &evmap, cfestring &ename);
  static truth RunItemAllowScript (item *tospawn, const EventHandlerMap &evmap, cfestring &ename);

  static festring ldrGetVar (TextInput *fl, cfestring &name);

private:
  static truth GetWord (festring &w);
  static void UpdateCameraCoordinate (int &, int, int, int);

  static void DoOnEvent (const EventHandlerMap &emap, cfestring &ename);
  static int ParseFuncArgs (TextInput *ifl, cfestring &types, std::vector<FuncArg> &args, truth noterm=false);

  static void LoadGlobalEvents ();

  static EventContext curevctx;

private:
  static cchar* const Alignment[];
  static god** God;
  static int CurrentLevelIndex;
  static int CurrentDungeonIndex;
  static cint MoveNormalCommandKey[];
  static cv2 MoveVector[];
  static cv2 RelativeMoveVector[];
  static cv2 BasicMoveVector[];
  static cv2 LargeMoveVector[];
  static uChar*** LuxTable;
  static truth Running;
  static character* Player;
  static v2 Camera;
  static feuLong Tick;
  static festring AutoSaveFileName;
  static truth InWilderness;
  static worldmap* WorldMap;
  static area* AreaInLoad;
  static square* SquareInLoad;
  static dungeon** Dungeon;
  static feuLong NextCharacterID;
  static feuLong NextItemID;
  static feuLong NextTrapID;
  static team** Team;
  static feuLong LOSTick;
  static truth LOSUpdateRequested;
  static character* Petrus;
  static truth Loading;
  static truth JumpToPlayerBe;
  static gamescript* GameScript;
  static valuemap GlobalValueMap;
  static v2 CursorPos;
  static truth Zoom;
  static truth Generating;
  static dangermap DangerMap;
  static int NextDangerIDType;
  static int NextDangerIDConfigIndex;
  static double AveragePlayerArmStrengthExperience;
  static double AveragePlayerLegStrengthExperience;
  static double AveragePlayerDexterityExperience;
  static double AveragePlayerAgilityExperience;
  static characteridmap CharacterIDMap;
  static itemidmap ItemIDMap;
  static trapidmap TrapIDMap;
  static int Teams;
  static int Dungeons;
  static int StoryState;
  static int XinrochTombStoryState;
  static int MondedrPass;
  static int RingOfThieves;
  static int Masamune;
  static int Muramasa;
  static int LoricatusHammer;
  static int Liberator;
  static int OmmelBloodMission;
  static int RegiiTalkState;
  static truth InGetCommand;
  static truth PlayerHurtByExplosion;
  static area* CurrentArea;
  static level* CurrentLevel;
  static wsquare*** CurrentWSquareMap;
  static lsquare*** CurrentLSquareMap;
  static festring DefaultPolymorphTo;
  static festring DefaultSummonMonster;
  static festring DefaultWish;
  static festring DefaultChangeMaterial;
  static festring DefaultDetectMaterial;
  static festring DefaultTeam;
  static massacremap PlayerMassacreMap;
  static massacremap PetMassacreMap;
  static massacremap MiscMassacreMap;
  static sLong PlayerMassacreAmount;
  static sLong PetMassacreAmount;
  static sLong MiscMassacreAmount;
  static truth WizardMode;
  static int SeeWholeMapCheatMode;
  static truth GoThroughWallsCheat;
  static int QuestMonstersFound;
  static boneidmap BoneItemIDMap;
  static boneidmap BoneCharacterIDMap;
  static truth TooGreatDangerFoundTruth;
  static bitmap* BusyAnimationCache[32];
  static itemvectorvector ItemDrawVector;
  static charactervector CharacterDrawVector;
  static truth SumoWrestling;
  static festring PlayerName;
  static liquid* GlobalRainLiquid;
  static v2 GlobalRainSpeed;
  static sLong GlobalRainTimeModifier;
  static truth PlayerSumoChampion;
  static truth PlayerSolicitusChampion;
  static truth TouristHasSpider;
  static feuLong SquarePartEmitationTick;
  static cint LargeMoveDirection[];
  static sLong Turn;
  static feuLong EquipmentMemory[MAX_EQUIPMENT_SLOTS];
  static truth PlayerRunning;
  static character* LastPetUnderCursor;
  static charactervector PetVector;
  static double DangerFound;
  static int OldAttribute[ATTRIBUTES];
  static int NewAttribute[ATTRIBUTES];
  static int LastAttributeChangeTick[ATTRIBUTES];
  static int NecroCounter;
  static int CursorData;
  static olterrain* MonsterPortal;
  static truth CausePanicFlag;
  static time_t TimePlayedBeforeLastLoad;
  static time_t LastLoad;
  static time_t GameBegan;
  static std::vector<v2> SpecialCursorPos;
  static std::vector<int> SpecialCursorData;
  static truth PlayerHasReceivedAllGodsKnownBonus;
  static cbitmap* EnterImage;
  static v2 EnterTextDisplacement;

  static std::stack<TextInput *> mFEStack;
  static std::vector<festring> mModuleList;
  static truth mImmediateSave;
  static EventHandlerMap mGlobalEvents;

private:
  static void LoadModuleListFile (TextInput &fl);

  // worldmap "places of interest"
private:
  static owterrain **pois;
  static int poisSize;

public:
  static int poiCount ();
  static owterrain *poiByIndex (int pcfg, truth abortOnNotFound=true); // can return nullptr

  // not by POI name, but by POI constant name from "define.dat"!
  static owterrain *poi (cfestring &name, truth abortOnNotFound=true); // can return nullptr

  static void RevealPOI (owterrain *terra);

public:
  // convenient accessors
  //static owterrain *alienvesselPOI ();
  static owterrain *attnamPOI ();
  static owterrain *darkforestPOI ();
  //static owterrain *dragontowerPOI ();
  static owterrain *elpuricavePOI ();
  static owterrain *mondedrPOI ();
  static owterrain *muntuoPOI ();
  static owterrain *newattnamPOI ();
  static owterrain *underwatertunnelPOI ();
  static owterrain *underwatertunnelexitPOI ();
  static owterrain *xinrochtombPOI ();
};



//K8 WARNING! ABSOLUTELY NON-PORTABLE AND CAUSES UB!
//#define BCLAMP(c)  (((c)&0xff)|(255-((-(int)((c) < 256))>>24)))
#define BCLAMP(c)  ((c) > 255 ? 255 : (c) < 0 ? 0 : (c))
inline void game::CombineLights (col24 &L1, col24 L2) {
  if (L2) {
    if (L1) {
#if! defined(IVAN_NEW_LIGHTS)
# if !defined(IVAN_NEW_INTENSITY)
      sLong Red1 = (L1&0xFF0000), Red2 = (L2&0xFF0000);
      sLong New = (Red1 >= Red2 ? Red1 : Red2);
      sLong Green1 = (L1&0xFF00), Green2 = (L2&0xFF00);
      New |= (Green1 >= Green2 ? Green1 : Green2);
      sLong Blue1 = (L1&0xFF), Blue2 = (L2&0xFF);
      L1 = New|(Blue1 >= Blue2 ? Blue1 : Blue2);
# else
      int l1i = (int)(0.2126*((L1>>16)&0xFF)+0.7152*((L1>>8)&0xFF)+0.0722*(L1&0xFF)+0.5);
      int l2i = (int)(0.2126*((L2>>16)&0xFF)+0.7152*((L2>>8)&0xFF)+0.0722*(L2&0xFF)+0.5);
      //if (l2i > l1i) L1 = L2;
      l1i = BCLAMP(l1i);
      l2i = BCLAMP(l2i);
      int r = (int)(((L1>>16)&0xFF)*(l1i/255.0f)+((L2>>16)&0xFF)*(l2i/255.0f));
      int g = (int)(((L1>>8)&0xFF)*(l1i/255.0f)+((L2>>8)&0xFF)*(l2i/255.0f));
      int b = (int)((L1&0xFF)*(l1i/255.0f)+(L2&0xFF)*(l2i/255.0f));
      r = BCLAMP(r);
      g = BCLAMP(g);
      b = BCLAMP(b);
      L1 = (r<<16)|(g<<8)|b;
# endif
#else
# define L_COEFF (0.05f)
      int r = (int)(((L1>>16)&0xFF)*L_COEFF+((L2>>16)&0xFF)*L_COEFF);
      int g = (int)(((L1>>8)&0xFF)*L_COEFF+((L2>>8)&0xFF)*L_COEFF);
      int b = (int)((L1&0xFF)*L_COEFF+(L2&0xFF)*L_COEFF);
      r = BCLAMP(r);
      g = BCLAMP(g);
      b = BCLAMP(b);
      L1 = (r<<16)|(g<<8)|b;
# undef L_COEFF
#endif
    } else {
      L1 = L2;
    }
  }
}
#undef BCLAMP


inline col24 game::CombineConstLights (col24 L1, col24 L2) {
  CombineLights(L1, L2);
  return L1;
}


inline truth game::IsDark (col24 Light) {
  return
    !Light ||
    ((Light & 0xFF0000) < (LIGHT_BORDER << 16) &&
     (Light & 0x00FF00) < (LIGHT_BORDER << 8) &&
     (Light & 0x0000FF) < LIGHT_BORDER);
}

inline int game::CompareLights (col24 L1, col24 L2) {
  if (L1) {
    if ((L1 & 0xFF0000) > (L2 & 0xFF0000) ||
        (L1 & 0x00FF00) > (L2 & 0x00FF00) ||
        (L1 & 0x0000FF) > (L2 & 0x0000FF))
      return 1;
    if ((L1 & 0xFF0000) == (L2 & 0xFF0000) ||
        (L1 & 0x00FF00) == (L2 & 0x00FF00) ||
        (L1 & 0x0000FF) == (L2 & 0x0000FF))
      return 0;
    return -1;
  }
  return -int(!!L2);
}


inline v2 game::CalculateScreenCoordinates (v2 Pos) {
  return v2((Pos.X-Camera.X+1)<<4, (Pos.Y-Camera.Y+2)<<4);
}


#endif
