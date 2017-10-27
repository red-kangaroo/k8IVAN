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
#ifndef __IVANDEF_H__
#define __IVANDEF_H__

/*
 * Global defines for the project IVAN.
 * This file is created to decrease the need of including headers in
 * other headers just for the sake of some silly macros, because it
 * decreases compilation efficiency and may cause cross-including
 *
 * List of macros that should be gathered here:
 * 1. all numeric defines used in multiple .cpp or .h files
 *    except those #defined in balance.h and confdef.h
 * 2. all inline functions used in multiple .cpp or .h files
 *    and independent enough (do not require other headers)
 * 3. class construction macros used in multiple .h files
 */

#include "ivancommon.h"

#include "v2.h"
#include "festring.h"

#define IVAN_VERSION "0.666.2"

class item;
class material;
class character;

typedef const item citem;
typedef const material cmaterial;
typedef const character ccharacter;

struct databasebase
{
  int Config;
  feuLong CommonFlags;
  feuLong NameFlags;
  festring CfgStrName;
};

template <class type, class base, class prototype>
class sysbase : public base
{
 public:
  typedef sysbase<type, base, prototype> mybase;
  static type* Spawn(int Config = 0, int SpecialFlags = 0)
  {
    type* T = new type;
    T->Initialize(Config, SpecialFlags);
    return T;
  }

  virtual ~sysbase () {}

  static type* Clone(const type* T) { return new type(*T); }
  virtual const prototype* FindProtoType() const { return &ProtoType; }
  static const prototype ProtoType;
};

template <class type, class base, class prototype>
class simplesysbase : public base
{
 public:
  typedef simplesysbase<type, base, prototype> mybase;
  static type* Spawn() { return new type; }

  virtual ~simplesysbase () {}

  virtual const prototype* GetProtoType() const { return &ProtoType; }
  static const prototype ProtoType;
};

#define SYSTEM_SPECIALIZATIONS(name)\
name##prototype** name##_ProtoData;\
valuemap name##_CodeNameMap;\
int name##_ProtoSize;\
\
template<> name##prototype**& protocontainer<name>::GetProtoData()\
{ return name##_ProtoData; }\
template<> valuemap& protocontainer<name>::GetCodeNameMap()\
{ return name##_CodeNameMap; }\
template<> int& protocontainer<name>::GetSizeRef()\
{ return name##_ProtoSize; }

#define EXTENDED_SYSTEM_SPECIALIZATIONS(name)\
SYSTEM_SPECIALIZATIONS(name)\
databasecreator<name>::databasemembermap name##_DataBaseMemberMap;\
template<> databasecreator<name>::databasemembermap&\
databasecreator<name>::GetDataBaseMemberMap()\
{ return name##_DataBaseMemberMap; }\
const name##prototype name::ProtoType

#define DATA_BASE_VALUE(type, data) type Get##data() const { return DataBase->data; }
#define DATA_BASE_VALUE_WITH_PARAMETER(type, data, param) type Get##data(param) const { return DataBase->data; }
#define DATA_BASE_TRUTH(data) truth data() const { return DataBase->data; }
#define DATA_BASE_TRUTH_WITH_PARAMETER(data, param) truth data(param) const { return DataBase->data; }

#define HAS_HIT  (game::GetGlobalConst("HAS_HIT"))
#define HAS_BLOCKED  (game::GetGlobalConst("HAS_BLOCKED"))
#define HAS_DODGED  (game::GetGlobalConst("HAS_DODGED"))
#define HAS_DIED  (game::GetGlobalConst("HAS_DIED"))
#define DID_NO_DAMAGE  (game::GetGlobalConst("DID_NO_DAMAGE"))
#define HAS_FAILED  (game::GetGlobalConst("HAS_FAILED"))

#define OVER_FED_LEVEL  (game::GetGlobalConst("OVER_FED_LEVEL"))
#define BLOATED_LEVEL  (game::GetGlobalConst("BLOATED_LEVEL"))
#define SATIATED_LEVEL  (game::GetGlobalConst("SATIATED_LEVEL"))
#define NOT_HUNGER_LEVEL  (game::GetGlobalConst("NOT_HUNGER_LEVEL"))
#define HUNGER_LEVEL  (game::GetGlobalConst("HUNGER_LEVEL"))
#define VERY_HUNGER_LEVEL  (game::GetGlobalConst("VERY_HUNGER_LEVEL"))

#define OVER_LOADED  (game::GetGlobalConst("OVER_LOADED"))
#define STRESSED  (game::GetGlobalConst("STRESSED"))
#define BURDENED  (game::GetGlobalConst("BURDENED"))
#define UNBURDENED  (game::GetGlobalConst("UNBURDENED"))

#define STARVING  (game::GetGlobalConst("STARVING"))
#define VERY_HUNGRY  (game::GetGlobalConst("VERY_HUNGRY"))
#define HUNGRY  (game::GetGlobalConst("HUNGRY"))
#define NOT_HUNGRY  (game::GetGlobalConst("NOT_HUNGRY"))
#define SATIATED  (game::GetGlobalConst("SATIATED"))
#define BLOATED  (game::GetGlobalConst("BLOATED"))
#define OVER_FED  (game::GetGlobalConst("OVER_FED"))

//WARNING! state count and state order MUST be synced with "define.dat"
#define STATES 30

#define POLYMORPHED  (game::GetGlobalConst("POLYMORPHED"))
#define HASTE  (game::GetGlobalConst("HASTE"))
#define SLOW  (game::GetGlobalConst("SLOW"))
#define POLYMORPH_CONTROL  (game::GetGlobalConst("POLYMORPH_CONTROL"))
#define LIFE_SAVED  (game::GetGlobalConst("LIFE_SAVED"))
#define LYCANTHROPY  (game::GetGlobalConst("LYCANTHROPY"))
#define INVISIBLE  (game::GetGlobalConst("INVISIBLE"))
#define INFRA_VISION  (game::GetGlobalConst("INFRA_VISION"))
#define ESP  (game::GetGlobalConst("ESP"))
#define POISONED  (game::GetGlobalConst("POISONED"))
#define TELEPORT  (game::GetGlobalConst("TELEPORT"))
#define POLYMORPH  (game::GetGlobalConst("POLYMORPH"))
#define TELEPORT_CONTROL  (game::GetGlobalConst("TELEPORT_CONTROL"))
#define PANIC  (game::GetGlobalConst("PANIC"))
#define CONFUSED  (game::GetGlobalConst("CONFUSED"))
#define PARASITIZED  (game::GetGlobalConst("PARASITIZED"))
#define SEARCHING  (game::GetGlobalConst("SEARCHING"))
#define GAS_IMMUNITY  (game::GetGlobalConst("GAS_IMMUNITY"))
#define LEVITATION  (game::GetGlobalConst("LEVITATION"))
#define LEPROSY  (game::GetGlobalConst("LEPROSY"))
#define HICCUPS  (game::GetGlobalConst("HICCUPS"))
#define VAMPIRISM  (game::GetGlobalConst("VAMPIRISM"))
#define SWIMMING  (game::GetGlobalConst("SWIMMING"))
#define DETECTING  (game::GetGlobalConst("DETECTING"))
#define ETHEREAL_MOVING  (game::GetGlobalConst("ETHEREAL_MOVING"))
#define FEARLESS  (game::GetGlobalConst("FEARLESS"))
#define POLYMORPH_LOCK  (game::GetGlobalConst("POLYMORPH_LOCK"))
#define REGENERATION  (game::GetGlobalConst("REGENERATION"))
#define DISEASE_IMMUNITY  (game::GetGlobalConst("DISEASE_IMMUNITY"))
#define TELEPORT_LOCK  (game::GetGlobalConst("TELEPORT_LOCK"))


#define THROW_ITEM_TYPES  (game::GetGlobalConst("THROW_ITEM_TYPES"))
/*ThrowFlags */
#define THROW_BONE  (game::GetGlobalConst("THROW_BONE"))
#define THROW_POTION  (game::GetGlobalConst("THROW_POTION"))
#define THROW_AXE  (game::GetGlobalConst("THROW_AXE"))
#define THROW_GAS_GRENADE  (game::GetGlobalConst("THROW_GAS_GRENADE"))
#define THROW_WAND  (game::GetGlobalConst("THROW_WAND"))


#define TORSO 1
#define HEAD 2
#define RIGHT_ARM 4
#define LEFT_ARM 8
#define ARMS 12
#define GROIN 16
#define RIGHT_LEG 32
#define LEFT_LEG 64
#define LEGS 96
#define OTHER 128
#define ALL 255


#define PHYSICAL_DAMAGE  (game::GetGlobalConst("PHYSICAL_DAMAGE"))
#define SOUND  (game::GetGlobalConst("SOUND"))
#define ACID  (game::GetGlobalConst("ACID"))
#define FIRE  (game::GetGlobalConst("FIRE"))
#define ELECTRICITY  (game::GetGlobalConst("ELECTRICITY"))
#define ENERGY  (game::GetGlobalConst("ENERGY"))
#define POISON  (game::GetGlobalConst("POISON"))
#define DRAIN  (game::GetGlobalConst("DRAIN"))
#define MUSTARD_GAS_DAMAGE  (game::GetGlobalConst("MUSTARD_GAS_DAMAGE"))
#define PSI  (game::GetGlobalConst("PSI"))
#define THROW  (game::GetGlobalConst("THROW"))


#define UNDEFINED  (game::GetGlobalConst("UNDEFINED"))
#define MALE  (game::GetGlobalConst("MALE"))
#define FEMALE  (game::GetGlobalConst("FEMALE"))
#define TRANSSEXUAL  (game::GetGlobalConst("TRANSSEXUAL"))

#define ALL_BODYPART_FLAGS 0x7F

/* The maximum bodyparts a character can have */

#define MAX_BODYPARTS 7
#define HUMANOID_BODYPARTS 7

#define TORSO_INDEX 0
#define HEAD_INDEX 1
#define RIGHT_ARM_INDEX 2
#define LEFT_ARM_INDEX 3
#define GROIN_INDEX 4
#define RIGHT_LEG_INDEX 5
#define LEFT_LEG_INDEX 6

#define NONE_INDEX MAX_BODYPARTS

#define DIRECTION_COMMAND_KEYS 8
#define EXTENDED_DIRECTION_COMMAND_KEYS 9
#define YOURSELF 8
#define RANDOM_DIR 9

#ifndef LIGHT_BORDER
#define LIGHT_BORDER 80
#endif

#define ALPP 0
#define ALP 1
#define AL 2
#define ALM 3
#define ANP 4
#define AN 5
#define ANM 6
#define ACP 7
#define AC 8
#define ACM 9
#define ACMM 10

#define UNARTICLED 0
#define PLURAL 1
#define ARTICLE_BIT 2
#define DEFINITE 2
#define INDEFINE_BIT 4
#define INDEFINITE 6
#define STRIPPED 8

#define TRANSPARENT_COLOR 0xF81F // pink

#define RAW_TYPES 7

#define GR_GLTERRAIN 0
#define GR_OLTERRAIN 1
#define GR_ITEM 2
#define GR_CHARACTER 3
#define GR_HUMANOID 4
#define GR_EFFECT 5
#define GR_CURSOR 6

#define GRAPHIC_TYPES 4

#define GR_WTERRAIN 0
#define GR_FOW 1
#define GR_SYMBOL 2
#define GR_SMILEY 3

/* SpecialFlags for graphics system. No one knows what "ST_" means... */

#define ST_NORMAL 0
#define ST_RIGHT_ARM 8
#define ST_LEFT_ARM 16
#define ST_GROIN 24
#define ST_RIGHT_LEG 32
#define ST_LEFT_LEG 40
#define ST_OTHER_BODYPART 48
#define ST_WIELDED 56
#define ST_CLOAK 64
#define ST_LIGHTNING 128
#define ST_DISALLOW_R_COLORS 256
#define ST_FLAME_1 512
#define ST_FLAME_2 1024
#define ST_FLAME_3 2048
#define ST_FLAME_4 4096
#define ST_FLAMES (ST_FLAME_1|ST_FLAME_2|ST_FLAME_3|ST_FLAME_4)
#define ST_FLAME_SHIFT 9

#define WOBBLE 1
#define WOBBLE_HORIZONTALLY_BIT 2
#define WOBBLE_VERTICALLY WOBBLE
#define WOBBLE_HORIZONTALLY (WOBBLE|WOBBLE_HORIZONTALLY_BIT)
#define WOBBLE_SPEED_SHIFT 2
#define WOBBLE_SPEED_RANGE (3<<WOBBLE_SPEED_SHIFT)
#define WOBBLE_FREQ_SHIFT 4
#define WOBBLE_FREQ_RANGE (3<<WOBBLE_FREQ_SHIFT)

cv2 SILHOUETTE_SIZE(48, 64);

//WARNING! keep in sync with "define.dat"!
#define ITEM_CATEGORIES 18

#define ANY_CATEGORY  (game::GetGlobalConst("ANY_CATEGORY"))
#define HELMET  (game::GetGlobalConst("HELMET"))
#define AMULET  (game::GetGlobalConst("AMULET"))
#define CLOAK  (game::GetGlobalConst("CLOAK"))
#define BODY_ARMOR  (game::GetGlobalConst("BODY_ARMOR"))
#define WEAPON  (game::GetGlobalConst("WEAPON"))
#define SHIELD  (game::GetGlobalConst("SHIELD"))
#define RING  (game::GetGlobalConst("RING"))
#define GAUNTLET  (game::GetGlobalConst("GAUNTLET"))
#define BELT  (game::GetGlobalConst("BELT"))
#define BOOT  (game::GetGlobalConst("BOOT"))
#define FOOD  (game::GetGlobalConst("FOOD"))
#define POTION  (game::GetGlobalConst("POTION"))
#define SCROLL  (game::GetGlobalConst("SCROLL"))
#define BOOK  (game::GetGlobalConst("BOOK"))
#define WAND  (game::GetGlobalConst("WAND"))
#define TOOL  (game::GetGlobalConst("TOOL"))
#define VALUABLE  (game::GetGlobalConst("VALUABLE"))
#define MISC  (game::GetGlobalConst("MISC"))

#define GOOD  (game::GetGlobalConst("GOOD"))
#define NEUTRAL  (game::GetGlobalConst("NEUTRAL"))
#define EVIL  (game::GetGlobalConst("EVIL"))
// new
#define TOPPLED  (game::GetGlobalConst("TOPPLED"))

/* ConsumeTypes */

#define CT_FRUIT  (game::GetGlobalConst("CT_FRUIT"))
#define CT_MEAT  (game::GetGlobalConst("CT_MEAT"))
#define CT_METAL  (game::GetGlobalConst("CT_METAL"))
#define CT_MINERAL  (game::GetGlobalConst("CT_MINERAL"))
#define CT_LIQUID  (game::GetGlobalConst("CT_LIQUID"))
#define CT_BONE  (game::GetGlobalConst("CT_BONE"))
#define CT_PROCESSED  (game::GetGlobalConst("CT_PROCESSED"))
#define CT_MISC_ORGANIC  (game::GetGlobalConst("CT_MISC_ORGANIC"))
#define CT_PLASTIC  (game::GetGlobalConst("CT_PLASTIC"))
#define CT_GAS  (game::GetGlobalConst("CT_GAS"))

/* Possible square positions for item. The first four are used for items
   on walls */

#define LEFT  (game::GetGlobalConst("LEFT"))
#define DOWN  (game::GetGlobalConst("DOWN"))
#define UP  (game::GetGlobalConst("UP"))
#define RIGHT  (game::GetGlobalConst("RIGHT"))
#define CENTER  (game::GetGlobalConst("CENTER")) // item on ground

#define HOSTILE  (game::GetGlobalConst("HOSTILE"))
#define UNCARING  (game::GetGlobalConst("UNCARING"))
#define FRIEND  (game::GetGlobalConst("FRIEND"))

#define MARTIAL_SKILL_CATEGORIES 3
#define WEAPON_SKILL_CATEGORIES 11

#define UNARMED  (game::GetGlobalConst("UNARMED"))
#define KICK  (game::GetGlobalConst("KICK"))
#define BITE  (game::GetGlobalConst("BITE"))
#define UNCATEGORIZED  (game::GetGlobalConst("UNCATEGORIZED"))
#define SMALL_SWORDS  (game::GetGlobalConst("SMALL_SWORDS"))
#define LARGE_SWORDS  (game::GetGlobalConst("LARGE_SWORDS"))
#define BLUNT_WEAPONS  (game::GetGlobalConst("BLUNT_WEAPONS"))
#define AXES  (game::GetGlobalConst("AXES"))
#define POLE_ARMS  (game::GetGlobalConst("POLE_ARMS"))
#define WHIPS  (game::GetGlobalConst("WHIPS"))
#define SHIELDS  (game::GetGlobalConst("SHIELDS"))

#define LOCKED  (game::GetGlobalConst("LOCKED"))

#define EFFECT_NOTHING  (game::GetGlobalConst("EFFECT_NOTHING"))
#define EFFECT_POISON  (game::GetGlobalConst("EFFECT_POISON"))
#define EFFECT_DARKNESS  (game::GetGlobalConst("EFFECT_DARKNESS"))
#define EFFECT_OMMEL_URINE  (game::GetGlobalConst("EFFECT_OMMEL_URINE"))
#define EFFECT_PEPSI  (game::GetGlobalConst("EFFECT_PEPSI"))
#define EFFECT_KOBOLD_FLESH  (game::GetGlobalConst("EFFECT_KOBOLD_FLESH"))
#define EFFECT_HEAL  (game::GetGlobalConst("EFFECT_HEAL"))
#define EFFECT_LYCANTHROPY  (game::GetGlobalConst("EFFECT_LYCANTHROPY"))
#define EFFECT_SCHOOL_FOOD  (game::GetGlobalConst("EFFECT_SCHOOL_FOOD"))
#define EFFECT_ANTIDOTE  (game::GetGlobalConst("EFFECT_ANTIDOTE"))
#define EFFECT_CONFUSE  (game::GetGlobalConst("EFFECT_CONFUSE"))
#define EFFECT_POLYMORPH  (game::GetGlobalConst("EFFECT_POLYMORPH"))
#define EFFECT_ESP  (game::GetGlobalConst("EFFECT_ESP"))
#define EFFECT_SKUNK_SMELL  (game::GetGlobalConst("EFFECT_SKUNK_SMELL"))
#define EFFECT_MAGIC_MUSHROOM  (game::GetGlobalConst("EFFECT_MAGIC_MUSHROOM"))
#define EFFECT_TRAIN_PERCEPTION  (game::GetGlobalConst("EFFECT_TRAIN_PERCEPTION"))
#define EFFECT_HOLY_BANANA  (game::GetGlobalConst("EFFECT_HOLY_BANANA"))
#define EFFECT_EVIL_WONDER_STAFF_VAPOUR  (game::GetGlobalConst("EFFECT_EVIL_WONDER_STAFF_VAPOUR"))
#define EFFECT_GOOD_WONDER_STAFF_VAPOUR  (game::GetGlobalConst("EFFECT_GOOD_WONDER_STAFF_VAPOUR"))
#define EFFECT_PEA_SOUP  (game::GetGlobalConst("EFFECT_PEA_SOUP"))
#define EFFECT_BLACK_UNICORN_FLESH  (game::GetGlobalConst("EFFECT_BLACK_UNICORN_FLESH"))
#define EFFECT_GRAY_UNICORN_FLESH  (game::GetGlobalConst("EFFECT_GRAY_UNICORN_FLESH"))
#define EFFECT_WHITE_UNICORN_FLESH  (game::GetGlobalConst("EFFECT_WHITE_UNICORN_FLESH"))
#define EFFECT_TELEPORT_CONTROL  (game::GetGlobalConst("EFFECT_TELEPORT_CONTROL"))
#define EFFECT_MUSHROOM  (game::GetGlobalConst("EFFECT_MUSHROOM"))
#define EFFECT_OMMEL_CERUMEN  (game::GetGlobalConst("EFFECT_OMMEL_CERUMEN"))
#define EFFECT_OMMEL_SWEAT  (game::GetGlobalConst("EFFECT_OMMEL_SWEAT"))
#define EFFECT_OMMEL_TEARS  (game::GetGlobalConst("EFFECT_OMMEL_TEARS"))
#define EFFECT_OMMEL_SNOT  (game::GetGlobalConst("EFFECT_OMMEL_SNOT"))
#define EFFECT_OMMEL_BONE  (game::GetGlobalConst("EFFECT_OMMEL_BONE"))
#define EFFECT_MUSTARD_GAS  (game::GetGlobalConst("EFFECT_MUSTARD_GAS"))
#define EFFECT_MUSTARD_GAS_LIQUID  (game::GetGlobalConst("EFFECT_MUSTARD_GAS_LIQUID"))
// new; wtf is EFFECT_PANIC?
#define EFFECT_PANIC  (game::GetGlobalConst("EFFECT_PANIC"))
#define EFFECT_TELEPORT  (game::GetGlobalConst("EFFECT_TELEPORT"))
#define EFFECT_VAMPIRISM  (game::GetGlobalConst("EFFECT_VAMPIRISM"))
#define EFFECT_DETECTING  (game::GetGlobalConst("EFFECT_DETECTING"))
#define EFFECT_HOLY_MANGO  (game::GetGlobalConst("EFFECT_HOLY_MANGO"))

/* CEM = Consume End Message */

#define CEM_NOTHING  (game::GetGlobalConst("CEM_NOTHING"))
#define CEM_SCHOOL_FOOD  (game::GetGlobalConst("CEM_SCHOOL_FOOD"))
#define CEM_BONE  (game::GetGlobalConst("CEM_BONE"))
#define CEM_FROG_FLESH  (game::GetGlobalConst("CEM_FROG_FLESH"))
#define CEM_OMMEL  (game::GetGlobalConst("CEM_OMMEL"))
#define CEM_PEPSI  (game::GetGlobalConst("CEM_PEPSI"))
#define CEM_KOBOLD_FLESH  (game::GetGlobalConst("CEM_KOBOLD_FLESH"))
#define CEM_HEALING_LIQUID  (game::GetGlobalConst("CEM_HEALING_LIQUID"))
#define CEM_ANTIDOTE  (game::GetGlobalConst("CEM_ANTIDOTE"))
#define CEM_ESP  (game::GetGlobalConst("CEM_ESP"))
#define CEM_HOLY_BANANA  (game::GetGlobalConst("CEM_HOLY_BANANA"))
#define CEM_PEA_SOUP  (game::GetGlobalConst("CEM_PEA_SOUP"))
#define CEM_BLACK_UNICORN_FLESH  (game::GetGlobalConst("CEM_BLACK_UNICORN_FLESH"))
#define CEM_GRAY_UNICORN_FLESH  (game::GetGlobalConst("CEM_GRAY_UNICORN_FLESH"))
#define CEM_WHITE_UNICORN_FLESH  (game::GetGlobalConst("CEM_WHITE_UNICORN_FLESH"))
#define CEM_OMMEL_BONE  (game::GetGlobalConst("CEM_OMMEL_BONE"))
// new
#define CEM_LIQUID_HORROR  (game::GetGlobalConst("CEM_LIQUID_HORROR"))
#define CEM_HOLY_MANGO  (game::GetGlobalConst("CEM_HOLY_MANGO"))
// alien mod
#define CEM_ALIEN_FLESH  (game::GetGlobalConst("CEM_ALIEN_FLESH"))

/* HM = Hit Message */

#define HM_NOTHING  (game::GetGlobalConst("HM_NOTHING"))
#define HM_SCHOOL_FOOD  (game::GetGlobalConst("HM_SCHOOL_FOOD"))
#define HM_FROG_FLESH  (game::GetGlobalConst("HM_FROG_FLESH"))
#define HM_OMMEL  (game::GetGlobalConst("HM_OMMEL"))
#define HM_PEPSI  (game::GetGlobalConst("HM_PEPSI"))
#define HM_KOBOLD_FLESH  (game::GetGlobalConst("HM_KOBOLD_FLESH"))
#define HM_HEALING_LIQUID  (game::GetGlobalConst("HM_HEALING_LIQUID"))
#define HM_ANTIDOTE  (game::GetGlobalConst("HM_ANTIDOTE"))
#define HM_CONFUSE  (game::GetGlobalConst("HM_CONFUSE"))
#define HM_HOLY_BANANA  (game::GetGlobalConst("HM_HOLY_BANANA"))
#define HM_HOLY_MANGO  (game::GetGlobalConst("HM_HOLY_MANGO"))
// alien mod
#define HM_ALIEN_FLESH  (game::GetGlobalConst("HM_ALIEN_FLESH"))

#define UNARMED_ATTACK  (game::GetGlobalConst("UNARMED_ATTACK"))
#define WEAPON_ATTACK  (game::GetGlobalConst("WEAPON_ATTACK"))
#define KICK_ATTACK  (game::GetGlobalConst("KICK_ATTACK"))
#define BITE_ATTACK  (game::GetGlobalConst("BITE_ATTACK"))
#define THROW_ATTACK  (game::GetGlobalConst("THROW_ATTACK"))

#define USE_ARMS  (game::GetGlobalConst("USE_ARMS"))
#define USE_LEGS  (game::GetGlobalConst("USE_LEGS"))
#define USE_HEAD  (game::GetGlobalConst("USE_HEAD"))

#define ATTRIBUTES 11
#define BASE_ATTRIBUTES 7

#define ENDURANCE 0
#define PERCEPTION 1
#define INTELLIGENCE 2
#define WISDOM 3
#define WILL_POWER 4
#define CHARISMA 5
#define MANA 6

#define ARM_STRENGTH 7
#define LEG_STRENGTH 8
#define DEXTERITY 9
#define AGILITY 10

#define F_ENDURANCE (1<<ENDURANCE)
#define F_PERCEPTION (1<<PERCEPTION)
#define F_INTELLIGENCE (1<<INTELLIGENCE)
#define F_WISDOM (1<<WISDOM)
#define F_WILL_POWER (1<<WILL_POWER)
#define F_CHARISMA (1<<CHARISMA)
#define F_MANA (1<<MANA)

#define F_ARM_STRENGTH (1<<ARM_STRENGTH)
#define F_LEG_STRENGTH (1<<LEG_STRENGTH)
#define F_DEXTERITY (1<<DEXTERITY)
#define F_AGILITY (1<<AGILITY)

#define NO 0
#define YES 1
#define REQUIRES_ANSWER -1

#define DIR_ERROR 0xFF

#define MAX_EQUIPMENT_SLOTS 13

#define HELMET_INDEX 0
#define AMULET_INDEX 1
#define CLOAK_INDEX 2
#define BODY_ARMOR_INDEX 3
#define BELT_INDEX 4
#define RIGHT_WIELDED_INDEX 5
#define LEFT_WIELDED_INDEX 6
#define RIGHT_RING_INDEX 7
#define LEFT_RING_INDEX 8
#define RIGHT_GAUNTLET_INDEX 9
#define LEFT_GAUNTLET_INDEX 10
#define RIGHT_BOOT_INDEX 11
#define LEFT_BOOT_INDEX 12

#define WORLD_MAP 255

#define DEFAULT_TEAM 0xFF

/* Hard-coded teams */
#define PLAYER_TEAM  (game::GetGlobalConst("PLAYER_TEAM"))
#define MONSTER_TEAM  (game::GetGlobalConst("MONSTER_TEAM"))
#define ATTNAM_TEAM  (game::GetGlobalConst("ATTNAM_TEAM"))
#define SUMO_TEAM  (game::GetGlobalConst("SUMO_TEAM"))
#define VALPURUS_ANGEL_TEAM  (game::GetGlobalConst("VALPURUS_ANGEL_TEAM"))
#define GC_SHOPKEEPER_TEAM  (game::GetGlobalConst("GC_SHOPKEEPER_TEAM"))
#define IVAN_TEAM  (game::GetGlobalConst("IVAN_TEAM"))
#define NEW_ATTNAM_TEAM  (game::GetGlobalConst("NEW_ATTNAM_TEAM"))
#define COLONIST_TEAM  (game::GetGlobalConst("COLONIST_TEAM"))
#define TOURIST_GUIDE_TEAM  (game::GetGlobalConst("TOURIST_GUIDE_TEAM"))
#define TOURIST_TEAM  (game::GetGlobalConst("TOURIST_TEAM"))
#define BETRAYED_TEAM  (game::GetGlobalConst("BETRAYED_TEAM"))
#define MONDEDR_TEAM  (game::GetGlobalConst("MONDEDR_TEAM"))
#define KHARAZ_ARAD_TEAM  (game::GetGlobalConst("KHARAZ_ARAD_TEAM"))
#define FORESTMAN_TEAM  (game::GetGlobalConst("FORESTMAN_TEAM"))
#define SOLICITUS_TEAM  (game::GetGlobalConst("SOLICITUS_TEAM"))
#define MORBE_TEAM  (game::GetGlobalConst("MORBE_TEAM"))
#define XINROCH_TOMB_ENTRY_TEAM  (game::GetGlobalConst("XINROCH_TOMB_ENTRY_TEAM"))
#define XINROCH_TOMB_NECRO_TEAM  (game::GetGlobalConst("XINROCH_TOMB_NECRO_TEAM"))
#define XINROCH_TOMB_KAMIKAZE_DWARF_TEAM  (game::GetGlobalConst("XINROCH_TOMB_KAMIKAZE_DWARF_TEAM"))
#define PRISONER_TEAM  (game::GetGlobalConst("PRISONER_TEAM"))
//#define NO_TEAM 0xFFFF
#define NO_TEAM  (game::GetGlobalConst("NO_TEAM"))

#define LOAD 1
#define NO_PIC_UPDATE 2
#define NO_EQUIPMENT_PIC_UPDATE (NO_PIC_UPDATE<<1)
#define NO_MATERIALS 8
#define NO_EQUIPMENT 16
#define NO_SIGNALS 32

#define NOT_WALKABLE  (game::GetGlobalConst("NOT_WALKABLE"))
#define HAS_CHARACTER  (game::GetGlobalConst("HAS_CHARACTER"))
#define IN_ROOM  (game::GetGlobalConst("IN_ROOM"))
#define NOT_IN_ROOM  (game::GetGlobalConst("NOT_IN_ROOM"))
#define ATTACHABLE  (game::GetGlobalConst("ATTACHABLE")) /* overrides IN_ROOM */
#define HAS_NO_OTERRAIN  (game::GetGlobalConst("HAS_NO_OTERRAIN"))

#define DEFAULT_ATTACHED_AREA 0xFE
#define DEFAULT_ATTACHED_ENTRY 0xFE
#define NO_ENTRY 0

#define RANDOM  (game::GetGlobalConst("RANDOM"))
#define ELPURI_CAVE  (game::GetGlobalConst("ELPURI_CAVE"))
#define ATTNAM  (game::GetGlobalConst("ATTNAM"))
#define NEW_ATTNAM  (game::GetGlobalConst("NEW_ATTNAM"))
#define UNDER_WATER_TUNNEL  (game::GetGlobalConst("UNDER_WATER_TUNNEL"))
#define MONDEDR  (game::GetGlobalConst("MONDEDR"))
#define MUNTUO  (game::GetGlobalConst("MUNTUO"))
#define DRAGON_TOWER  (game::GetGlobalConst("DRAGON_TOWER"))
#define DARK_FOREST  (game::GetGlobalConst("DARK_FOREST"))
#define XINROCH_TOMB  (game::GetGlobalConst("XINROCH_TOMB"))
// alien mod
#define ALIEN_VESSEL  (game::GetGlobalConst("ALIEN_VESSEL"))

#define KHARAZ_ARAD_SHOP  (game::GetGlobalConst("KHARAZ_ARAD_SHOP"))
#define UNDER_WATER_TUNNEL_EXIT  (game::GetGlobalConst("UNDER_WATER_TUNNEL_EXIT"))

//#define VESANA_LEVEL  (game::GetGlobalConst("VESANA_LEVEL"))
//#define CRYSTAL_LEVEL  (game::GetGlobalConst("CRYSTAL_LEVEL"))
//#define SPIDER_LEVEL  (game::GetGlobalConst("SPIDER_LEVEL"))
//#define ENNER_BEAST_LEVEL  (game::GetGlobalConst("ENNER_BEAST_LEVEL"))
#define ZOMBIE_LEVEL  (game::GetGlobalConst("ZOMBIE_LEVEL"))
//#define IVAN_LEVEL  (game::GetGlobalConst("IVAN_LEVEL"))
#define DARK_LEVEL  (game::GetGlobalConst("DARK_LEVEL"))
#define OREE_LAIR  (game::GetGlobalConst("OREE_LAIR"))

// alien mod
//#define ALIENQUEEN_LEVEL  (game::GetGlobalConst("ALIENQUEEN_LEVEL"))

//#define DUAL_ENNER_BEAST_LEVEL  (game::GetGlobalConst("DUAL_ENNER_BEAST_LEVEL"))
//#define NECRO_CHAMBER_LEVEL  (game::GetGlobalConst("NECRO_CHAMBER_LEVEL"))

/* stack::DrawContents flags */

#define NO_SELECT 1       // only show items
#define NO_MULTI_SELECT 2     // select only one item
#define NO_SPECIAL_INFO 4     // show only name and amount
#define REMEMBER_SELECTED 8   // if DrawContents will be called multiple times, remember the selected item
#define NONE_AS_CHOICE 16     // "none" is a choice, for instance when wielding
#define SELECT_PAIR 32        // if NO_MULTI_SELECT is on, selects a pair if appropriate
#define SKIP_FIRST_IF_NO_OLD 64  // skip first list item if `hiitem` in `stack::DrawContents()` is non-empty
#define SELECT_MOST_RECENT  128  // select most recent picked item in `stack::DrawContents()` (with timeout)

#define RECTANGLE  (game::GetGlobalConst("RECTANGLE"))
#define ROUND_CORNERS  (game::GetGlobalConst("ROUND_CORNERS"))
#define MAZE_ROOM  (game::GetGlobalConst("MAZE_ROOM"))

/* Gods, 0 == none */

//#define GODS 15
#define GODS   (game::GetGlobalConst("LAST_REAL_GOD"))

#define VALPURUS  (game::GetGlobalConst("VALPURUS"))
#define LEGIFER  (game::GetGlobalConst("LEGIFER"))
#define ATAVUS  (game::GetGlobalConst("ATAVUS"))
#define DULCIS  (game::GetGlobalConst("DULCIS"))
#define SEGES  (game::GetGlobalConst("SEGES"))
#define SOPHOS  (game::GetGlobalConst("SOPHOS"))
#define TERRA  (game::GetGlobalConst("TERRA"))
#define SILVA  (game::GetGlobalConst("SILVA"))
#define LORICATUS  (game::GetGlobalConst("LORICATUS"))
#define MELLIS  (game::GetGlobalConst("MELLIS"))
#define CLEPTIA  (game::GetGlobalConst("CLEPTIA"))
#define NEFAS  (game::GetGlobalConst("NEFAS"))
#define SCABIES  (game::GetGlobalConst("SCABIES"))
#define INFUSCOR  (game::GetGlobalConst("INFUSCOR"))
#define CRUENTUS  (game::GetGlobalConst("CRUENTUS"))
#define MORTIFER  (game::GetGlobalConst("MORTIFER"))
#define ATHEIST  (game::GetGlobalConst("ATHEIST"))
#define SOLICITU  (game::GetGlobalConst("SOLICITU"))

#define MAX_PRICE  (2147483647)

#define PERMANENT 0xFFFF

#define MISSED 0
#define HIT 1
#define CATCHED 2

#define BEAM_EFFECTS 13

#define BEAM_POLYMORPH  (game::GetGlobalConst("BEAM_POLYMORPH"))
#define BEAM_STRIKE  (game::GetGlobalConst("BEAM_STRIKE"))
#define BEAM_FIRE_BALL  (game::GetGlobalConst("BEAM_FIRE_BALL"))
#define BEAM_TELEPORT  (game::GetGlobalConst("BEAM_TELEPORT"))
#define BEAM_HASTE  (game::GetGlobalConst("BEAM_HASTE"))
#define BEAM_SLOW  (game::GetGlobalConst("BEAM_SLOW"))
#define BEAM_RESURRECT  (game::GetGlobalConst("BEAM_RESURRECT"))
#define BEAM_INVISIBILITY  (game::GetGlobalConst("BEAM_INVISIBILITY"))
#define BEAM_DUPLICATE  (game::GetGlobalConst("BEAM_DUPLICATE"))
#define BEAM_LIGHTNING  (game::GetGlobalConst("BEAM_LIGHTNING"))
#define BEAM_DOOR_CREATION  (game::GetGlobalConst("BEAM_DOOR_CREATION"))
#define BEAM_ACID_RAIN  (game::GetGlobalConst("BEAM_ACID_RAIN"))
#define BEAM_NECROMANCY  (game::GetGlobalConst("BEAM_NECROMANCY"))

// WARNING! sync with "define.dat"
#define BEAM_STYLES 3

#define PARTICLE_BEAM  (game::GetGlobalConst("PARTICLE_BEAM"))
#define LIGHTNING_BEAM  (game::GetGlobalConst("LIGHTNING_BEAM"))
#define SHIELD_BEAM  (game::GetGlobalConst("SHIELD_BEAM"))


#define RANDOM_COLOR 0x10000

/* Entry indices, not actual config defines */

//#define STAIRS_UP 100
//#define STAIRS_DOWN 200
//#define WAYPOINT_DEEPER 1100
//#define WAYPOINT_SHALLOWER 1200
//#define FOUNTAIN 0xFFFF  // in confdef.h

#define NO_LIMIT 0xFFFF

#define ALL_ITEMS 0xFFFF

/* StateData flags */

#define NO_FLAGS 0
#define SECRET 1
#define DUR_TEMPORARY 2
#define DUR_PERMANENT 4
#define DUR_FLAGS (2|4)
#define SRC_FOUNTAIN 8
#define SRC_MUSHROOM 16
#define SRC_MAGIC_MUSHROOM 32
#define SRC_CONFUSE_READ 64
#define SRC_EVIL 128
#define SRC_GOOD 256
#define SRC_FLAGS (8|16|32|64|128|256)
#define RANDOMIZABLE (DUR_FLAGS|SRC_FLAGS)

#define MAP_HIDDEN 0
#define SHOW_MAP_IN_TRUE_LIGHT 1
#define SHOW_MAP_IN_UNIFORM_LIGHT 2

#define DIM_LUMINANCE 0x6E6E6E

#define SUPER  64
#define BROKEN 128
#define WINDOW 1024

/* item flags */

#define CANNIBALIZED 4
#define SQUARE_POSITION_BITS (16|32|64)
#define SQUARE_POSITION_SHIFT 4

/* bodypart flags */

#define UNIQUE 128
#define BADLY_HURT 256
#define STUCK 512
#define BODYPART_SPARKLE_SHIFT 9

#define NO_BROKEN 1
#define IGNORE_BROKEN_PRICE 2

#define MAX_SQUARES_UNDER 16
#define MAX_NEIGHBOUR_SQUARES 20

#define N_LOCK_ID  (game::GetGlobalConst("N_LOCK_ID"))
#define S_LOCK_ID  (game::GetGlobalConst("S_LOCK_ID"))
#define LOCK_DELTA  (game::GetGlobalConst("LOCK_DELTA"))

#define LOCK_BITS 0xFC00

#define BROKEN_LOCK  (game::GetGlobalConst("BROKEN_LOCK"))

/* Normal lock types, which can be randomized */

#define ROUND_LOCK  (game::GetGlobalConst("ROUND_LOCK"))
#define SQUARE_LOCK  (game::GetGlobalConst("SQUARE_LOCK"))
#define TRIANGULAR_LOCK  (game::GetGlobalConst("TRIANGULAR_LOCK"))

/* Special lock types, which must be generated in the script */

#define HEXAGONAL_LOCK  (game::GetGlobalConst("HEXAGONAL_LOCK"))
#define OCTAGONAL_LOCK  (game::GetGlobalConst("OCTAGONAL_LOCK"))
#define HEART_SHAPED_LOCK  (game::GetGlobalConst("HEART_SHAPED_LOCK"))
#define PENTAGONAL_LOCK  (game::GetGlobalConst("PENTAGONAL_LOCK"))

#define DESERT  (game::GetGlobalConst("DESERT"))
#define JUNGLE  (game::GetGlobalConst("JUNGLE"))
#define STEPPE  (game::GetGlobalConst("STEPPE"))
#define LEAFY_FOREST  (game::GetGlobalConst("LEAFY_FOREST"))
#define EVERGREEN_FOREST  (game::GetGlobalConst("EVERGREEN_FOREST"))
#define TUNDRA  (game::GetGlobalConst("TUNDRA"))
#define GLACIER  (game::GetGlobalConst("GLACIER"))

#define NO_MOVE  (game::GetGlobalConst("NO_MOVE"))
#define WALK  (game::GetGlobalConst("WALK"))
#define SWIM  (game::GetGlobalConst("SWIM"))
#define FLY  (game::GetGlobalConst("FLY"))
#define ETHEREAL  (game::GetGlobalConst("ETHEREAL"))
#define ANY_MOVE  (game::GetGlobalConst("ANY_MOVE"))

#define KEY_UP_INDEX 1
#define KEY_LEFT_INDEX 3
#define KEY_RIGHT_INDEX 4
#define KEY_DOWN_INDEX 6

#define NO_ACTION 0
#define SUCCESS 1
#define BLOCKED 2

#define STACK_SLOT 1
#define CHARACTER_SLOT 2
#define GEAR_SLOT 3

#define NOT_RUSTED  (game::GetGlobalConst("NOT_RUSTED"))
#define SLIGHTLY_RUSTED  (game::GetGlobalConst("SLIGHTLY_RUSTED"))
#define RUSTED  (game::GetGlobalConst("RUSTED"))
#define VERY_RUSTED  (game::GetGlobalConst("VERY_RUSTED"))

#define HUMAN_BODY_ARMOR_PIXELS 68

#define ARMOR_OUTLINE_PRIORITY ((7<<4)+7)
#define CLOAK_PRIORITY ((8<<4)+7)

#define BODY_ARMOR_PARTS 6

#define SUMO_ROOM_POS v2(25, 35)
#define SUMO_ARENA_POS v2(19, 12)

#define MAX_RAIN_DROPS 32

#define WON 0
#define LOST 1
#define DISQUALIFIED 2

#define EMITTER_IDENTIFIER_BITS 0xFFFF
#define EMITTER_SQUARE_PART_BITS 0xF000000
#define EMITTER_SHADOW_BITS 0xF0000000
#define EMITTER_SQUARE_PART_SHIFT 24
#define EMITTER_SHADOW_SHIFT 28

#define RE_SUN_EMITATED 0x200000
#define ID_X_COORDINATE 0x400000
#define ID_BEGIN 0x800000

#define FORCE_ADD 0x400000
#define SECONDARY_SUN_LIGHT 0x800000

/* square & lsquare flags */

#define ALLOW_EMITATION_CONTINUE 1
#define FREEZED 2 /* also a stack flag */
#define INSIDE 4
#define NEW_DRAW_REQUEST 8
#define STRONG_BIT 16
#define STRONG_NEW_DRAW_REQUEST (NEW_DRAW_REQUEST|STRONG_BIT)
#define DESCRIPTION_CHANGE 32
#define MEMORIZED_UPDATE_REQUEST 128
#define IN_SQUARE_STACK 256
#define CHECK_SUN_LIGHT_NEEDED 512
#define IS_TRANSPARENT 1024
#define PERFECTLY_QUADRI_HANDLED 2048

/* Slows down protosystem::BalancedCreateItem() but makes it produce more
   accurate results */

#define BALANCED_CREATE_ITEM_ITERATIONS 100

#define CONFIG_TABLE_SIZE 256

#define SPARKLE_POS_X_ERROR 128

#define SKIN_COLOR 1
#define CAP_COLOR 2
#define HAIR_COLOR 4
#define EYE_COLOR 8
#define TORSO_MAIN_COLOR 16
#define BELT_COLOR 32
#define BOOT_COLOR 64
#define TORSO_SPECIAL_COLOR 128
#define ARM_MAIN_COLOR 256
#define GAUNTLET_COLOR 512
#define ARM_SPECIAL_COLOR 1024
#define LEG_MAIN_COLOR 2048
#define LEG_SPECIAL_COLOR 4096
#define CLOTH_COLOR (CAP_COLOR\
        |TORSO_MAIN_COLOR\
        |ARM_MAIN_COLOR\
        |GAUNTLET_COLOR\
        |LEG_MAIN_COLOR)

/* contentscript<character> flags */

#define IS_LEADER  (game::GetGlobalConst("IS_LEADER"))
#define IS_MASTER  (game::GetGlobalConst("IS_MASTER"))

/* stack flags */

/* If set, all items are always considered visible, so CanBeSeenBy calls
   become unneeded */

#define HIDDEN 1

/* All costly updates (like emitation's) are avoided if this is set.
   Allows much faster removing and adding items, but make sure the stack is
   returned to the original state (excluding item order) before switching
   this off. Note: also an lsquare flag */

#define FREEZED 2

/* End stack Flags */

#define SUN_BEAM_DIRECTIONS 48

/* Square part flags */

#define SP_TOP_LEFT 1
#define SP_TOP_RIGHT 2
#define SP_BOTTOM_LEFT 4
#define SP_BOTTOM_RIGHT 8
#define SP_TOP (SP_TOP_LEFT|SP_TOP_RIGHT)
#define SP_LEFT (SP_TOP_LEFT|SP_BOTTOM_LEFT)
#define SP_RIGHT (SP_TOP_RIGHT|SP_BOTTOM_RIGHT)
#define SP_BOTTOM (SP_BOTTOM_LEFT|SP_BOTTOM_RIGHT)

#define CONDITION_COLORS 5

#define NATURAL_MATERIAL_FORM 0x7FFF

#define EXP_DIVISOR 2e-8
#define EXP_MULTIPLIER 5e+7
#define MIN_EXP 5e+7
#define MAX_EXP 5e+10

#define HAS_BEEN_GENERATED 1
#define HAS_BEEN_SEEN 2

#define DEPENDS_ON_ATTRIBUTES 0xFFFF

/* Tiredness states */

#define FAINTING 0
#define EXHAUSTED 1
#define UNTIRED 2

#define DEFAULT_GENERATION_DANGER 0.05
#define ANGEL_GENERATION_DANGER 0.10

/* Duplication flags */

#define MIRROR_IMAGE 1
#define IGNORE_PROHIBITIONS 2
#define CHANGE_TEAM 4
#define LE_BASE_SHIFT 3
#define LE_BASE_RANGE 0x7FFF
#define LE_RAND_SHIFT 18
#define LE_RAND_RANGE 0x3FFF

/* action flags */

#define IN_DND_MODE 1
#define TERMINATING 2

/* fluid flags */

#define HAS_BODY_ARMOR_PICTURES 1
#define FLUID_INSIDE 2

#define COMMAND_FLAGS 4
#define ALL_COMMAND_FLAGS (1|2|4|8)

#define FOLLOW_LEADER 1
#define FLEE_FROM_ENEMIES 2
#define DONT_CHANGE_EQUIPMENT 4
#define DONT_CONSUME_ANYTHING_VALUABLE 8

#define CHAT_MENU_ENTRIES 5
#define ALL_MANAGEMENT_FLAGS (1|2|4|8|16)

#define CHANGE_EQUIPMENT 1
#define TAKE_ITEMS 2
#define GIVE_ITEMS 4
#define ISSUE_COMMANDS 8
#define CHAT_IDLY 16

#define NO_PARAMETERS 0xFF

#define CURSOR_TYPES 4

#define DARK_CURSOR 0
#define RED_CURSOR 1
#define BLUE_CURSOR 2
#define YELLOW_CURSOR 3
#define CURSOR_SHADE 0x1000
#define CURSOR_FLASH 0x2000
#define CURSOR_TARGET 0x4000
#define CURSOR_BIG 0x8000
#define CURSOR_FLAGS (CURSOR_SHADE|CURSOR_BIG|CURSOR_FLASH|CURSOR_TARGET)

#define GRAY_FRACTAL 0
#define RED_FRACTAL 1
#define GREEN_FRACTAL 2
#define BLUE_FRACTAL 3
#define YELLOW_FRACTAL 4

#define DAMAGE_TYPES 3

#define BLUNT 1
#define SLASH 2
#define PIERCE 4

#define SILHOUETTE_TYPES 2

#define SILHOUETTE_NORMAL 0
#define SILHOUETTE_INTER_LACED 1

#define WARNED 1
#define HAS_CAUSED_PANIC 2

/* MaxHP calculation flags */

#define MAY_CHANGE_HPS 1
#define CHECK_USABILITY 2

#define ITEM_TRAP 0x8000
#define FLUID_TRAP 0x10000

/* Death flags */

#define FORCE_MSG 1
#define FORCE_DEATH 2
#define DISALLOW_CORPSE 4
#define DISALLOW_MSG 8
#define IGNORE_UNCONSCIOUSNESS 16
#define IGNORE_TRAPS 32
#define FORBID_REINCARNATION 64

/* character flags */

#define C_PLAYER 4
#define C_INITIALIZING 8
#define C_POLYMORPHED 16
#define C_IN_NO_MSG_MODE 32
#define C_PICTURE_UPDATES_FORBIDDEN 64

/*************************/
/* Common DataBase flags */
/*************************/

/* CommonFlags */
#define IS_ABSTRACT  (game::GetGlobalConst("IS_ABSTRACT"))
#define HAS_SECONDARY_MATERIAL  (game::GetGlobalConst("HAS_SECONDARY_MATERIAL"))
#define CREATE_DIVINE_CONFIGURATIONS  (game::GetGlobalConst("CREATE_DIVINE_CONFIGURATIONS"))
#define CAN_BE_WISHED  (game::GetGlobalConst("CAN_BE_WISHED"))
#define CAN_BE_DESTROYED  (game::GetGlobalConst("CAN_BE_DESTROYED"))
#define IS_VALUABLE  (game::GetGlobalConst("IS_VALUABLE"))
#define CAN_BE_MIRRORED  (game::GetGlobalConst("CAN_BE_MIRRORED"))

/* NameFlags */
#define USE_AN  (game::GetGlobalConst("USE_AN"))
#define USE_ADJECTIVE_AN  (game::GetGlobalConst("USE_ADJECTIVE_AN"))
#define NO_ARTICLE  (game::GetGlobalConst("NO_ARTICLE")) /* for instance "Petrus's wive number 4" */
#define FORCE_THE  (game::GetGlobalConst("FORCE_THE"))
#define SHOW_MATERIAL  (game::GetGlobalConst("SHOW_MATERIAL")) /* only works for terrains */

/* Spatial Flags (unused for now) */
#define MF_OBJECT  256
#define MF_FLOOR   512
#define MF_CEIL    1024
#define MF_WALL    2048
#define MF_ONFLOOR 4096
#define MF_LOWWALL 8192
#define MF_TABLE   16384
#define MF_WINDOW  128
#define MF_LARGE   64
#define MF_STAIR   32

/****************************/
/* Character DataBase flags */
/****************************/

/* CommonFlags */
/* NameFlags */

/* BodyFlags */
/*
#define HAS_HEAD
#define HAS_EYES
#define HAS_A_LEG
#define SPILLS_BLOOD
#define SWEATS
#define USES_NUTRITION
#define ALWAYS_USE_MATERIAL_ATTRIBUTES
#define IS_ENORMOUS
#define IS_EXTRA_FRAGILE
#define IS_PLANT
#define IS_ROOTED
*/

/* AbilityFlags */
/*
#define CAN_USE_EQUIPMENT
#define CAN_KICK
#define CAN_TALK
#define CAN_READ
#define CAN_OPEN
#define CAN_ZAP
#define CAN_THROW
#define CAN_APPLY
#define CAN_HEAR
*/

/* CopyrightFlags */
/*
#define IS_UNIQUE
#define CAN_BE_GENERATED
#define CAN_BE_NAMED
*/

/* EffectFlags; */
/*
#define BODY_PARTS_DISAPPEAR_WHEN_SEVERED
#define DESTROYS_WALLS
#define BITE_CAPTURES_BODY_PART
*/

/* ImmunityFlags */
/*
#define IMMUNITY_POLYMORPH
#define IMMUNITY_CHARM
#define IMMUNITY_CLONING
#define IMMUNITY_CONFUSE
#define IMMUNITY_LEPROSY
#define IMMUNITY_ITEM_TELEPORT
#define IMMUNITY_STICKINESS
#define IMMUNITY_CHOKING
#define IMMUNITY_UNCONSCIOUSNESS
*/

/* MiscFlags */
#define CREATE_GOLEM_MATERIAL_CONFIGURATIONS 2
#define IGNORE_DANGER 4
#define AUTOMATICALLY_SEEN 8
#define WILL_CARRY_ITEMS 16
#define IS_EXTRA_COWARD 32

/***********************/
/* Item DataBase flags */
/***********************/

/* CommonFlags */
/* NameFlags */
/* AttributeAffectFlags */

/* GenerationFlags*/
#define CREATE_LOCK_CONFIGURATIONS 2
#define CAN_BE_AUTO_INITIALIZED 4 /* used only in WMode */
#define CAN_BE_GENERATED_IN_CONTAINER 8
#define CAN_BE_SPAWNED_BY_POLYMORPH 16

/* InteractionFlags */
/*
#define MATERIAL_CAN_BE_CHANGED
#define CAN_BE_POLYMORPHED
#define CAN_BE_CLONED
#define CAN_BE_ENCHANTED
#define CAN_BE_BROKEN
#define AFFECTS_CARRYING_CAPACITY
*/

/* CategoryFlags */
/*
#define IS_QUEST_ITEM
#define CAN_BE_USED_BY_SMITH
#define IS_KAMIKAZE_WEAPON
#define IS_TWO_HANDED
#define IS_GOOD_WITH_PLANTS
*/

/* MiscFlags */
/*
#define HANDLE_IN_PAIRS
#define PRICE_IS_PROPORTIONAL_TO_ENCHANTMENT
#define FLEXIBILITY_IS_ESSENTIAL
#define HAS_NORMAL_PICTURE_DIRECTION
#define CAN_BE_PILED
#define CAN_BE_PICKED_UP
#define ALLOW_EQUIP
*/

/**************************/
/* Terrain DataBase flags */
/**************************/

/* CommonFlags */
/* NameFlags */

/* OLTerrainFlags */
#define CREATE_LOCK_CONFIGURATIONS 2
#define CREATE_WINDOW_CONFIGURATIONS 4
/*
#define IS_UP_LINK
#define IS_WALL
#define USE_BORDER_TILES
#define IS_ALWAYS_TRANSPARENT
#define SHOW_THINGS_UNDER
#define IS_SAFE_TO_CREATE_DOOR
*/

/***************************/
/* Material DataBase flags */
/***************************/

/* CommonFlags */
/* NameFlags (only USE_AN) */

/* CategoryFlags */
#define IS_METAL  (game::GetGlobalConst("IS_METAL"))
#define IS_BLOOD  (game::GetGlobalConst("IS_BLOOD"))
#define CAN_BE_TAILORED  (game::GetGlobalConst("CAN_BE_TAILORED"))
#define IS_SPARKLING  (game::GetGlobalConst("IS_SPARKLING"))
#define IS_SCARY  (game::GetGlobalConst("IS_SCARY"))
#define IS_GOLEM_MATERIAL  (game::GetGlobalConst("IS_GOLEM_MATERIAL"))
#define IS_BEVERAGE  (game::GetGlobalConst("IS_BEVERAGE"))

/* BodyFlags */
#define IS_ALIVE  (game::GetGlobalConst("IS_ALIVE"))
#define IS_WARM  (game::GetGlobalConst("IS_WARM"))
#define CAN_HAVE_PARASITE  (game::GetGlobalConst("CAN_HAVE_PARASITE"))
#define USE_MATERIAL_ATTRIBUTES  (game::GetGlobalConst("USE_MATERIAL_ATTRIBUTES"))
#define CAN_REGENERATE  (game::GetGlobalConst("CAN_REGENERATE"))
#define IS_WARM_BLOODED  (game::GetGlobalConst("IS_WARM_BLOODED"))

/* InteractionFlags */
#define CAN_BURN  (game::GetGlobalConst("CAN_BURN"))
#define CAN_EXPLODE  (game::GetGlobalConst("CAN_EXPLODE"))
#define CAN_DISSOLVE  (game::GetGlobalConst("CAN_DISSOLVE"))
#define AFFECT_INSIDE  (game::GetGlobalConst("AFFECT_INSIDE"))
#define EFFECT_IS_GOOD  (game::GetGlobalConst("EFFECT_IS_GOOD"))
#define IS_AFFECTED_BY_MUSTARD_GAS  (game::GetGlobalConst("IS_AFFECTED_BY_MUSTARD_GAS"))

/*************************/
/* End of DataBase flags */
/*************************/

#define TILE_SIZE 16
cv2 TILE_V2(TILE_SIZE, TILE_SIZE);

#define SQUARE_INDEX_MASK 0xFFFF
#define ALLOW_ANIMATE 0x10000
#define ALLOW_ALPHA 0x20000

#define TALENTS 4

#define TALENT_STRONG 0
#define TALENT_FAST_N_ACCURATE 1
#define TALENT_HEALTHY 2
#define TALENT_CLEVER 3

#define BORDER_PARTNER_ANIMATED (16<<24)

/* room flags */

#define NO_MONSTER_GENERATION 1

#define NO_TAMING -1

#define SADIST_HIT 1

#define EQUIPMENT_DATAS 13

#define SPECIAL_CONFIGURATION_GENERATION_LEVELS 2

#endif
