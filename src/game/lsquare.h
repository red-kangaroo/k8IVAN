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
#ifndef __LSQUARE_H__
#define __LSQUARE_H__

#include "ivancommon.h"

#include "level.h"
#include "dungeon.h"
#include "lterra.h"


#ifndef LIGHT_BORDER
# define LIGHT_BORDER  (80)
#endif


class stack;
class glterrain;
class olterrain;
class fluid;
class material;
class item;
class smoke;
class gas;
class bodypart;
class liquid;
class rain;
class trap;
struct sortdata;

typedef std::vector<item*> itemvector;
typedef truth (item::*sorter)(ccharacter*) const;

struct emitter
{
  emitter(feuLong ID, col24 Emitation) : ID(ID), Emitation(Emitation) { }
  explicit emitter() { }
  feuLong ID;
  col24 Emitation;
};

inline feuLong MakeEmitterID(v2 Pos)
{
  return Pos.X + (Pos.Y << 8);
}

inline v2 ExtractPosFromEmitterID(feuLong ID)
{
  return v2(ID & 0xFF, (ID >> 8) & 0xFF);
}

typedef std::vector<emitter> emittervector;

outputfile& operator<<(outputfile&, const emitter&);
inputfile& operator>>(inputfile&, emitter&);

struct eyecontroller
{
  static truth Handler(int, int);
  static lsquare*** Map;
};

struct pathcontroller
{
  static truth Handler(int, int);
  static lsquare*** Map;
  static ccharacter* Character;
};

struct stackcontroller
{
  static lsquare*** Map;
  static lsquare** Stack;
  static sLong StackIndex;
  static int LevelXSize, LevelYSize;
  static v2 Center;
};

struct tickcontroller
{
  static void PrepareShiftedTick();
  static feuLong Tick;
  static feuLong ShiftedTick[4];
  static feuLong ShiftedQuadriTick[4];
};

class lsquare : public square
{
 public:
  friend class level;
  friend struct loscontroller;
  friend struct sunbeamcontroller;
  friend struct areacontroller;
  friend struct emitationcontroller;
  friend struct noxifycontroller;

 public:
  lsquare(level*, v2);
  virtual ~lsquare();

  virtual void AddCharacter(character*);
  virtual void RemoveCharacter();
  stack* GetStack() const { return Stack; }
  void AlterLuminance(feuLong, col24);
  void Emitate() { Emitate(Emitation); }
  void Emitate(col24, feuLong = 0);
  void Clean();
  truth Open(character*);
  truth Close(character*);
  virtual void Save(outputfile&) const;
  virtual void Load(inputfile&);
  void SignalEmitationIncrease(col24);
  void SignalEmitationDecrease(col24);
  col24 GetEmitation() const { return Emitation; }
  void Noxify() { Noxify(Emitation); }
  void Noxify(col24, feuLong = 0);
  truth NoxifyEmitter(feuLong);
  cchar* GetEngraved() const { return Engraved; }
  truth Engrave(cfestring&);
  void UpdateMemorizedDescription(truth = false);
  truth BeKicked(character*, item*, bodypart*, double, double, int, int, truth, truth);
  int GetDivineMaster() const;
  void Draw(blitdata&) const;
  void UpdateMemorized();
  truth CanBeDug() const;
  virtual gterrain* GetGTerrain() const { return GLTerrain; }
  virtual oterrain* GetOTerrain() const { return OLTerrain; }
  glterrain* GetGLTerrain() const { return GLTerrain; }
  olterrain* GetOLTerrain() const { return OLTerrain; }
  void ChangeLTerrain(glterrain*, olterrain*);
  level* GetLevel() const { return static_cast<level*>(AreaUnder); }
  void SetLevelUnder(level* What) { AreaUnder = What; }
  void ChangeGLTerrain(glterrain*);
  void ChangeOLTerrain(olterrain*);
  void SetLTerrain(glterrain*, olterrain*);
  void ApplyScript(const squarescript*, room*);
  virtual truth CanBeSeenByPlayer(truth = false) const;
  virtual truth CanBeSeenFrom(v2, sLong, truth = false) const ;
  void StepOn(character*, lsquare**);
  uInt GetRoomIndex() const { return RoomIndex; }
  void SetRoomIndex(uInt What) { RoomIndex = What; }
  void ReceiveVomit(character*, liquid*);
  room* GetRoom() const { return RoomIndex ? GetLevel()->GetRoom(RoomIndex) : 0; }
  void SetTemporaryEmitation(col24);
  col24 GetTemporaryEmitation() const { return TemporaryEmitation; }
  void ChangeOLTerrainAndUpdateLights(olterrain*);
  void DrawParticles(sLong, truth = true);
  v2 DrawLightning(v2, sLong, int, truth = true);
  truth DipInto(item*, character*);
  truth TryKey(item*, character*);
  void SignalSeen(feuLong);
  void CalculateLuminance();
  void DrawStaticContents(blitdata&) const;
  void DrawMemorized(blitdata&) const;
  void DrawMemorizedCharacter(blitdata&) const;
  void SendMemorizedUpdateRequest();
  lsquare* GetNeighbourLSquare(int) const;
  lsquare* GetNearLSquare(v2 Pos) const { return static_cast<lsquare*>(AreaUnder->GetSquare(Pos)); }
  truth IsDangerous(ccharacter*) const;
  truth IsScary(ccharacter*) const;
  stack* GetStackOfAdjacentSquare(int) const;
  void KickAnyoneStandingHereAway();
  truth IsDark() const;
  void AddItem(item*);
  static truth (lsquare::*GetBeamEffect(int))(const beamdata&);
  truth Polymorph(const beamdata&);
  truth Strike(const beamdata&);
  truth FireBall(const beamdata&);
  truth Teleport(const beamdata&);
  truth Haste(const beamdata&);
  truth Slow(const beamdata&);
  truth Confuse(const beamdata&);
  truth Parasitize(const beamdata&);
  truth InstillPanic(const beamdata&);
  truth Resurrect(const beamdata&);
  truth Invisibility(const beamdata&);
  truth Duplicate(const beamdata&);
  truth Lightning(const beamdata&);
  truth DoorCreation(const beamdata&);
  truth AcidRain(const beamdata&);
  truth Necromancy(const beamdata&);
  int GetLevelIndex() const { return static_cast<level*>(AreaUnder)->GetIndex(); }
  int GetDungeonIndex() const { return static_cast<level*>(AreaUnder)->GetDungeon()->GetIndex(); }
  dungeon* GetDungeon() const { return static_cast<level*>(AreaUnder)->GetDungeon(); }
  truth CheckKick(ccharacter*) const;
  void GetHitByExplosion(const explosion*);
  int GetSpoiledItems() const;
  void SortAllItems(const sortdata&);
  truth LowerEnchantment(const beamdata&);
  truth SoftenMaterial(const beamdata&);
  void RemoveSmoke(smoke*);
  void AddSmoke(gas*);
  truth IsFlyable() const { return !OLTerrain || (OLTerrain->GetWalkability() & FLY); }
  truth IsTransparent() const { return Flags & IS_TRANSPARENT; }
  void SignalSmokeAlphaChange(int);
  void ShowSmokeMessage() const;
  void DisplaySmokeInfo(festring&) const;
  truth IsDipDestination() const;
  void ReceiveEarthQuakeDamage();
  truth CanBeFeltByPlayer() const;
  void PreProcessForBone();
  void PostProcessForBone(double&, int&);
  void DisplayEngravedInfo(festring&) const;
  truth EngravingsCanBeReadByPlayer();
  truth HasEngravings() const { return !!Engraved; }
  void FinalProcessForBone();
  truth IsFreezed() const { return Flags & FREEZED; }
  truth IsDangerousToBreathe(ccharacter*) const;
  truth IsScaryToBreathe(ccharacter*) const;
  int GetWalkability() const;
  int GetTheoreticalWalkability() const { return OLTerrain ? OLTerrain->GetTheoreticalWalkability() & GLTerrain->GetTheoreticalWalkability() : GLTerrain->GetTheoreticalWalkability(); }
  virtual int GetSquareWalkability() const { return GetWalkability(); }
  void CalculateGroundBorderPartners();
  void RequestForGroundBorderPartnerUpdates();
  void CalculateOverBorderPartners();
  void RequestForOverBorderPartnerUpdates();
  void SpillFluid(character*, liquid*, truth = false, truth = true);
  void DisplayFluidInfo(festring&) const;
  void RemoveFluid(fluid*);
  fluid* AddFluid(liquid*);
  void DrawStacks(blitdata&) const;
  truth FluidIsVisible() const { return SmokeAlphaSum < 175; }
  void RemoveRain(rain*);
  void AddRain(liquid*, v2, int, truth);
  truth IsInside() const { return Flags & INSIDE; }
  void RemoveSunLight();
  void CheckIfIsSecondarySunLightEmitter();
  void CalculateNeighbourLSquares();
  const emittervector& GetEmitter() const { return Emitter; }
  void EnableGlobalRain();
  void DisableGlobalRain();
  void Freeze() { Flags |= FREEZED; }
  void UnFreeze() { Flags &= ~FREEZED; }
  void InitLastSeen();
  void GetSideItemDescription(festring&, truth = false) const;
  void AddSunLightEmitter(feuLong);
  void SendSunLightSignals();
  const sunemittervector& GetSunEmitter() const { return SunEmitter; }
  void ZeroReSunEmitatedFlags();
  virtual truth HasBeenSeen() const { return (Memorized != 0); }
  truth CalculateIsTransparent();
  void CalculateSunLightLuminance(feuLong);
  void CreateMemorized();
  truth DetectMaterial(cmaterial*) const;
  void Reveal(feuLong, truth = false);
  void DestroyMemorized();
  void SwapMemorized(lsquare*);
  truth HasNoBorderPartners() const;
  lsquare* GetRandomAdjacentSquare() const;
  void SignalPossibleTransparencyChange();
  truth AddTrap(trap*);
  void RemoveTrap(trap*);
  void DisplayTrapInfo(festring&) const;
  void FillTrapVector(std::vector<trap*>&) const;
  void ReceiveTrapDamage(character*, int, int, int = YOURSELF);
  truth HasDangerousTraps(ccharacter*) const;
  truth HasDangerousFluids(ccharacter*) const;
  void AddLocationDescription(festring&) const;
  truth VomitingIsDangerous(ccharacter*) const;
  bool TeleportAllSmokeAway();
  bool TeleportAllFluidsAway();
  bool TeleportAllTrapsAway();
  void AddSpecialCursors();
  inline truth IsGoSeen () const { return mGoSeen; }
  inline void SetGoSeen (truth v) { mGoSeen = v; }
 protected:
  void ChangeLuminance(col24&, col24);
  void RemoveLuminance(col24&);
  void CalculateEmitation();
  void UpdateStaticContentCache(col24) const;
  mutable struct staticcontentcache
  {
    staticcontentcache() : Bitmap(0), Luminance(0) { }
    bitmap* Bitmap;
    col24 Luminance;
  } StaticContentCache;
  fluid* Fluid;
  smoke* Smoke;
  rain* Rain;
  trap* Trap;
  emittervector Emitter;
  sunemittervector SunEmitter;
  glterrain* GLTerrain;
  olterrain* OLTerrain;
  stack* Stack;
  bitmap* Memorized;
  bitmap* FowMemorized;
  char* Engraved;
  glterrain** GroundBorderPartnerTerrain;
  feuLong GroundBorderPartnerInfo;
  olterrain** OverBorderPartnerTerrain;
  feuLong OverBorderPartnerInfo;
  feuLong SquarePartEmitationTick;
  feuLong SquarePartLastSeen;
  col24 Emitation;
  int SmokeAlphaSum;
  lsquare* NeighbourLSquare[8];
  col24 AmbientLuminance;
  col24 SunLightLuminance;
  col24 TemporaryEmitation;
  col24 SecondarySunLightEmitation;
  uShort LastExplosionID;
  uChar RoomIndex;
  truth mGoSeen;
};

inline truth lsquare::IsDark() const
{
  col24 Light = Luminance;

  return (!Light
    || ((Light & 0xFF0000) < (LIGHT_BORDER << 16)
        && (Light & 0x00FF00) < (LIGHT_BORDER << 8)
        && (Light & 0x0000FF) < LIGHT_BORDER));
}

inline truth eyecontroller::Handler(int x, int y)
{
  return Map[x][y]->IsTransparent();
}

inline lsquare* lsquare::GetNeighbourLSquare(int I) const
{
  return I < 8 ? NeighbourLSquare[I] : const_cast<lsquare*>(this);
}

#endif
