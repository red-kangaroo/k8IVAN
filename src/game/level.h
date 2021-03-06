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

#ifndef __LEVEL_H__
#define __LEVEL_H__

#include "ivancommon.h"

#include <vector>
#include <queue>
#include <set>

#include "area.h"
#include "square.h"
#include "ivandef.h"

class levelscript;
class roomscript;
class squarescript;
class glterrain;
class olterrain;
class dungeon;
class lsquare;
class room;
class item;
class liquid;
class gas;
class level;
class material;
class team;
struct node;
struct emitter;
template <class type> struct fearray;

struct nodepointerstorer
{
  nodepointerstorer(node* Node) : Node(Node) { }
  bool operator<(const nodepointerstorer&) const;
  node* Node;
};

struct spawnresult
{
  ccharacter* Pioneer;
  int Seen;
};

typedef std::priority_queue<nodepointerstorer> nodequeue;
typedef std::vector<item*> itemvector;
typedef std::vector<character*> charactervector;
typedef std::vector<emitter> emittervector;
typedef std::vector<feuLong> sunemittervector;
typedef character* (*characterspawner)(int, int);

struct node
{
  node(int x, int y, lsquare* Square) : Square(Square), Pos(x, y) { }
  void CalculateNextNodes();
  lsquare* Square;
  node* Last;
  v2 Pos;
  sLong Distance;
  sLong Remaining;
  sLong TotalDistanceEstimate;
  sLong Diagonals;
  truth InNodeQueue;
  truth Processed;
  static node*** NodeMap;
  static int RequiredWalkability;
  static ccharacter* SpecialMover;
  static v2 To;
  static uChar** WalkabilityMap;
  static int XSize, YSize;
  static nodequeue* NodeQueue;
};

struct explosion
{
  character* Terrorist;
  festring DeathMsg;
  v2 Pos;
  feuLong ID;
  int Strength;
  int RadiusSquare;
  int Size;
  truth HurtNeutrals;
};

struct beamdata
{
  beamdata(character*, cfestring&, int, feuLong);
  beamdata(character*, cfestring&, v2, col16, int, int, int, feuLong);
  character* Owner;
  festring DeathMsg;
  v2 StartPos;
  col16 BeamColor;
  int BeamEffect;
  int Direction;
  int Range;
  feuLong SpecialParameters;
};

inline beamdata::beamdata
(
  character* Owner,
  cfestring& DeathMsg,
  int Direction,
  feuLong SpecialParameters
) :
Owner(Owner),
DeathMsg(DeathMsg),
Direction(Direction),
SpecialParameters(SpecialParameters)
{ }

inline beamdata::beamdata
(
  character* Owner,
  cfestring& DeathMsg,
  v2 StartPos,
  col16 BeamColor,
  int BeamEffect,
  int Direction,
  int Range,
  feuLong SpecialParameters
) :
Owner(Owner),
DeathMsg(DeathMsg),
StartPos(StartPos),
BeamColor(BeamColor),
BeamEffect(BeamEffect),
Direction(Direction),
Range(Range),
SpecialParameters(SpecialParameters)
{ }

struct maze {
  maze (int x, int y) : MazeXSize(x+2), MazeYSize(y+2) { MazeVector.resize(MazeXSize*MazeYSize); }
  void CreateMaze ();
  void InitializeMaze ();
  std::vector<truth> MazeKernel; // returns only the mazey bit in the middle (the kernel), without the exterior walls. The kernel size is the input size of maze - 2 for each dimension.
  void CarveMaze (int, int); // Carves the maze
  void StripMazeHusk (); // Removes the husk (skin and shell), and populates MazeKernel with the kernel.
  /*const*/ uInt MazeXSize; // XSize of the MazeVector
  /*const*/ uInt MazeYSize; // YSize of the MazeVector
 private:
  std::vector<truth> MazeVector; // Consists of the outer buffer (empty, called the skin) and the shell wall (next layer in, all full). Together they comprise the husk.
};

class level : public area
{
 public:
  level();
  virtual ~level();

  truth Generate(int);
  v2 GetRandomSquare(ccharacter* = 0, int = 0, const rect* = 0) const;
  void GenerateMonsters();
  lsquare* GetLSquare(v2 Pos) const { return Map[Pos.X][Pos.Y]; }
  lsquare* GetLSquare(int x, int y) const { return Map[x][y]; }
  void GenerateTunnel(int, int, int, int, truth);
  void ExpandPossibleRoute(int, int, int, int, truth);
  void ExpandStillPossibleRoute(int, int, int, int, truth);
  void Save(outputfile&) const;
  void Load(inputfile&);
  void FiatLux();
  int GetIdealPopulation() const { return IdealPopulation; }
  double GetDifficulty() const { return Difficulty; }
  void GenerateNewMonsters(int, truth = true);
  void AttachPos(int, int);
  void AttachPos(v2 Pos) { AttachPos(Pos.X, Pos.Y); }
  void CreateItems(int);
  truth MakeRoom(const roomscript*);
  void ParticleTrail(v2, v2);
  festring GetLevelMessage() { return LevelMessage; }
  void SetLevelMessage(cfestring& What) { LevelMessage = What; }
  void SetLevelScript(const levelscript* What) { LevelScript = What; }
  truth IsOnGround() const;
  truth EarthquakesAffectTunnels() const;
  const levelscript* GetLevelScript() const { return LevelScript; }
  int GetLOSModifier() const;
  room* GetRoom(int) const;
  void SetRoom(int, room*);
  void AddRoom(room*);
  void Explosion(character*, cfestring&, v2, int, truth = true);
  truth CollectCreatures(charactervector&, character*, truth);
  void ApplyLSquareScript(const squarescript*);
  virtual void Draw(truth) const;
  v2 GetEntryPos(ccharacter*, int) const;
  void GenerateRectangularRoom(std::vector<v2>&, std::vector<v2>&, std::vector<v2>&, const roomscript*, room*, v2, v2);
  void Reveal();
  static void (level::*GetBeam(int))(beamdata&);
  void ParticleBeam(beamdata&);
  void LightningBeam(beamdata&);
  void ShieldBeam(beamdata&);
  dungeon* GetDungeon() const { return Dungeon; }
  void SetDungeon(dungeon* What) { Dungeon = What; }
  int GetIndex() const { return Index; }
  void SetIndex(int What) { Index = What; }
  truth DrawExplosion(const explosion*) const;
  int TriggerExplosions(int);
  lsquare*** GetMap() const { return Map; }
  v2 GetNearestFreeSquare(ccharacter*, v2, truth = true) const;
  v2 FreeSquareSeeker(ccharacter*, v2, v2, int, truth) const;
  v2 GetFreeAdjacentSquare(ccharacter*, v2, truth) const;
  static void (level::*GetBeamEffectVisualizer(int))(const fearray<lsquare*>&, col16) const;
  void ParticleVisualizer(const fearray<lsquare*>&, col16) const;
  void LightningVisualizer(const fearray<lsquare*>&, col16) const;
  truth PreProcessForBone();
  truth PostProcessForBone();
  void FinalProcessForBone();
  truth GenerateDungeon(int);
  truth GenerateDesert();
  truth GenerateJungle();
  truth GenerateSteppe();
  truth GenerateLeafyForest();
  truth GenerateEvergreenForest();
  truth GenerateTundra();
  truth GenerateGlacier();
  void CreateTunnelNetwork(int, int, int, int, v2);
  void SetWalkability(v2 Pos, int What) { WalkabilityMap[Pos.X][Pos.Y] = What; }
  node* FindRoute(v2, v2, const std::set<v2>&, int, ccharacter* = 0);
  void AddToAttachQueue(v2);
  void CollectEverything(itemvector&, charactervector&);
  void CreateGlobalRain(liquid*, v2);
  void CheckSunLight();
  col24 GetSunLightEmitation() const { return SunLightEmitation; }
  void InitSquarePartEmitationTicks();
  col24 GetAmbientLuminance() const { return AmbientLuminance; }
  void ForceEmitterNoxify(const emittervector&) const;
  void ForceEmitterEmitation(const emittervector&, const sunemittervector&, feuLong = 0) const;
  void UpdateLOS();
  void EnableGlobalRain();
  void DisableGlobalRain();
  void InitLastSeen();
  lsquare** GetSquareStack() const { return SquareStack; }
  col24 GetNightAmbientLuminance() const { return NightAmbientLuminance; }
  int DetectMaterial(cmaterial*);
  void BlurMemory();
  void CalculateLuminances();
  int AddRadiusToSquareStack(v2, sLong) const;
  olterrain* GetRandomFountainWithWater(olterrain*) const;
  int GetEnchantmentMinusChance() { return EnchantmentMinusChance; }
  int GetEnchantmentPlusChance() { return EnchantmentPlusChance; }
  void Amnesia(int);
  spawnresult SpawnMonsters(characterspawner, team*, v2, int = 0, int = 1, truth = false);
  void AddSpecialCursors();
  void GasExplosion(gas* GasMaterial, lsquare* Square, character* Terrorist=0);

  // check for special levels
  truth IsGCIvanLevel () const;
  /*
  truth IsUTVesanaLevel () const;
  truth IsGCEnnerLevel () const;
  truth IsGCElpuriLevel () const; // also nicknamed "dark level"
  truth IsGCOreeLevel () const;
  truth IsSolicitusLevel () const;
  truth IsAlienQueenLevel () const;
  */

 protected:
  truth GenerateLanterns(int, int, int) const;
  truth GenerateWindows(int, int) const;
  void CreateRoomSquare(glterrain*, olterrain*, int, int, int, int) const;
  void EmitSunBeams();
  void EmitSunBeam(v2, feuLong, int) const;
  void ChangeSunLight();
  void EmitSunLight(v2);
  lsquare*** Map;
  const levelscript* LevelScript;
  festring LevelMessage;
  std::vector<v2> Door;
  std::vector<room*> Room;
  int IdealPopulation;
  int MonsterGenerationInterval;
  double Difficulty;
  dungeon* Dungeon;
  int Index;
  std::vector<explosion*> ExplosionQueue;
  std::vector<truth> PlayerHurt;
  node*** NodeMap;
  uChar** WalkabilityMap;
  std::vector<v2> AttachQueue;
  liquid* GlobalRainLiquid;
  v2 GlobalRainSpeed;
  col24 SunLightEmitation;
  v2 SunLightDirection;
  col24 AmbientLuminance;
  static feuLong NextExplosionID;
  lsquare** SquareStack;
  col24 NightAmbientLuminance;
  int EnchantmentMinusChance;
  int EnchantmentPlusChance;
};

outputfile& operator<<(outputfile&, const level*);
inputfile& operator>>(inputfile&, level*&);

#endif
