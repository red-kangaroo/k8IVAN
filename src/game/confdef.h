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

#ifndef __CONFDEF_H__
#define __CONFDEF_H__

/*
 * Configuration defines of IVAN
 */

//#define SOLID_ID (1 << 12)
#define SOLID_ID  (game::GetGlobalConst("SOLID_ID"))
#define VALPURIUM  (game::GetGlobalConst("VALPURIUM"))
#define GRAVEL  (game::GetGlobalConst("GRAVEL"))
#define MORAINE  (game::GetGlobalConst("MORAINE"))
#define OCTIRON  (game::GetGlobalConst("OCTIRON"))
#define GLASS  (game::GetGlobalConst("GLASS"))
#define PARCHMENT  (game::GetGlobalConst("PARCHMENT"))
#define CLOTH  (game::GetGlobalConst("CLOTH"))
#define MITHRIL  (game::GetGlobalConst("MITHRIL"))
#define MARBLE  (game::GetGlobalConst("MARBLE"))
#define GOLD  (game::GetGlobalConst("GOLD"))
#define GRASS  (game::GetGlobalConst("GRASS"))
#define LEATHER  (game::GetGlobalConst("LEATHER"))
#define LEAF  (game::GetGlobalConst("LEAF"))
#define FABRIC  (game::GetGlobalConst("FABRIC"))
#define PALM_LEAF  (game::GetGlobalConst("PALM_LEAF"))
#define SULFUR  (game::GetGlobalConst("SULFUR"))
#define UNICORN_HORN  (game::GetGlobalConst("UNICORN_HORN"))
#define DIAMOND  (game::GetGlobalConst("DIAMOND"))
#define SILVER  (game::GetGlobalConst("SILVER"))
#define SAPPHIRE  (game::GetGlobalConst("SAPPHIRE"))
#define RUBY  (game::GetGlobalConst("RUBY"))
#define BRONZE  (game::GetGlobalConst("BRONZE"))
#define COPPER  (game::GetGlobalConst("COPPER"))
#define TIN  (game::GetGlobalConst("TIN"))
#define SPIDER_SILK  (game::GetGlobalConst("SPIDER_SILK"))
#define KEVLAR  (game::GetGlobalConst("KEVLAR"))
#define OMMEL_HAIR  (game::GetGlobalConst("OMMEL_HAIR"))
#define HARDENED_LEATHER  (game::GetGlobalConst("HARDENED_LEATHER"))
#define TROLL_HIDE  (game::GetGlobalConst("TROLL_HIDE"))
#define NYMPH_HAIR  (game::GetGlobalConst("NYMPH_HAIR"))
#define ANGEL_HAIR  (game::GetGlobalConst("ANGEL_HAIR"))
#define PHOENIX_FEATHER  (game::GetGlobalConst("PHOENIX_FEATHER"))
#define GOLDEN_EAGLE_FEATHER  (game::GetGlobalConst("GOLDEN_EAGLE_FEATHER"))
#define ICE  (game::GetGlobalConst("ICE"))
#define DRAGON_HIDE  (game::GetGlobalConst("DRAGON_HIDE"))
#define ARCANITE  (game::GetGlobalConst("ARCANITE"))
#define ILLITHIUM  (game::GetGlobalConst("ILLITHIUM"))
#define BALSA_WOOD  (game::GetGlobalConst("BALSA_WOOD"))
#define PINE_WOOD  (game::GetGlobalConst("PINE_WOOD"))
#define FIR_WOOD  (game::GetGlobalConst("FIR_WOOD"))
#define BIRCH_WOOD  (game::GetGlobalConst("BIRCH_WOOD"))
#define OAK_WOOD  (game::GetGlobalConst("OAK_WOOD"))
#define TEAK_WOOD  (game::GetGlobalConst("TEAK_WOOD"))
#define EBONY_WOOD  (game::GetGlobalConst("EBONY_WOOD"))
#define BLUE_CRYSTAL  (game::GetGlobalConst("BLUE_CRYSTAL"))
#define PURPLE_CRYSTAL  (game::GetGlobalConst("PURPLE_CRYSTAL"))
#define GREEN_CRYSTAL  (game::GetGlobalConst("GREEN_CRYSTAL"))
#define SAND_STONE  (game::GetGlobalConst("SAND_STONE"))
#define LIME_STONE  (game::GetGlobalConst("LIME_STONE"))
#define CALCITE  (game::GetGlobalConst("CALCITE"))
#define OBSIDIAN  (game::GetGlobalConst("OBSIDIAN"))
#define GNEISS  (game::GetGlobalConst("GNEISS"))
#define SLATE  (game::GetGlobalConst("SLATE"))
#define GRANITE  (game::GetGlobalConst("GRANITE"))
#define BASALT  (game::GetGlobalConst("BASALT"))
#define MILKY_QUARTZ  (game::GetGlobalConst("MILKY_QUARTZ"))
#define FLINT  (game::GetGlobalConst("FLINT"))
#define QUARTZITE  (game::GetGlobalConst("QUARTZITE"))
#define AMETHYST  (game::GetGlobalConst("AMETHYST"))
#define CITRINE  (game::GetGlobalConst("CITRINE"))
#define ROSE_QUARTZ  (game::GetGlobalConst("ROSE_QUARTZ"))
#define JASPER  (game::GetGlobalConst("JASPER"))
#define ROCK_CRYSTAL  (game::GetGlobalConst("ROCK_CRYSTAL"))
#define DARK_GRASS  (game::GetGlobalConst("DARK_GRASS"))
#define LEAD  (game::GetGlobalConst("LEAD"))
#define BLACK_GRANITE  (game::GetGlobalConst("BLACK_GRANITE"))
#define BLACK_LEATHER  (game::GetGlobalConst("BLACK_LEATHER"))

#define FLAWLESS_DIAMOND  (game::GetGlobalConst("FLAWLESS_DIAMOND"))
#define EMERALD  (game::GetGlobalConst("EMERALD"))
#define SUN_CRYSTAL  (game::GetGlobalConst("SUN_CRYSTAL"))
#define BLACK_DIAMOND  (game::GetGlobalConst("BLACK_DIAMOND"))
#define PSYPHER  (game::GetGlobalConst("PSYPHER"))
#define EXTRA_HARD_BASALT  (game::GetGlobalConst("EXTRA_HARD_BASALT"))
#define WEAK_GLASS  (game::GetGlobalConst("WEAK_GLASS"))

#define DEAD_GRASS  (game::GetGlobalConst("DEAD_GRASS"))
#define KAURI_WOOD  (game::GetGlobalConst("KAURI_WOOD"))
#define RATA_WOOD  (game::GetGlobalConst("RATA_WOOD"))
#define NEPHRITE  (game::GetGlobalConst("NEPHRITE"))
#define HESSIAN_CLOTH  (game::GetGlobalConst("HESSIAN_CLOTH"))


//#define ORGANIC_ID (2 << 12)
#define ORGANIC_ID  (game::GetGlobalConst("ORGANIC_ID"))
#define BANANA_FLESH  (game::GetGlobalConst("BANANA_FLESH"))
#define SCHOOL_FOOD  (game::GetGlobalConst("SCHOOL_FOOD"))
#define BANANA_PEEL  (game::GetGlobalConst("BANANA_PEEL"))
#define KIWI_FLESH  (game::GetGlobalConst("KIWI_FLESH"))
#define PINEAPPLE_FLESH  (game::GetGlobalConst("PINEAPPLE_FLESH"))
#define PLANT_FIBER  (game::GetGlobalConst("PLANT_FIBER"))
#define MUTANT_PLANT_FIBER  (game::GetGlobalConst("MUTANT_PLANT_FIBER"))
#define BONE  (game::GetGlobalConst("BONE"))
#define BREAD  (game::GetGlobalConst("BREAD"))
#define HOLY_BANANA_FLESH  (game::GetGlobalConst("HOLY_BANANA_FLESH"))
#define CARROT_FLESH  (game::GetGlobalConst("CARROT_FLESH"))
#define OMMEL_CERUMEN  (game::GetGlobalConst("OMMEL_CERUMEN"))
#define OMMEL_BONE  (game::GetGlobalConst("OMMEL_BONE"))
#define OMMEL_TOOTH  (game::GetGlobalConst("OMMEL_TOOTH"))
#define RYE_BREAD  (game::GetGlobalConst("RYE_BREAD"))
#define MANGO_FLESH  (game::GetGlobalConst("MANGO_FLESH"))
#define HOLY_MANGO_FLESH  (game::GetGlobalConst("HOLY_MANGO_FLESH"))


#define GAS_ID  (game::GetGlobalConst("GAS_ID"))
#define AIR  (game::GetGlobalConst("AIR"))
#define MAGICAL_AIR  (game::GetGlobalConst("MAGICAL_AIR"))
#define SMOKE  (game::GetGlobalConst("SMOKE"))
#define SKUNK_SMELL  (game::GetGlobalConst("SKUNK_SMELL"))
#define GHOST  (game::GetGlobalConst("GHOST"))
#define MAGIC_VAPOUR  (game::GetGlobalConst("MAGIC_VAPOUR"))
#define EVIL_WONDER_STAFF_VAPOUR  (game::GetGlobalConst("EVIL_WONDER_STAFF_VAPOUR"))
#define GOOD_WONDER_STAFF_VAPOUR  (game::GetGlobalConst("GOOD_WONDER_STAFF_VAPOUR"))
#define FART  (game::GetGlobalConst("FART"))
#define MUSTARD_GAS  (game::GetGlobalConst("MUSTARD_GAS"))

#define VACUUM_BLADE_AIR  (game::GetGlobalConst("VACUUM_BLADE_AIR"))


//#define LIQUID_ID (4 << 12)
#define LIQUID_ID  (game::GetGlobalConst("LIQUID_ID"))
#define OMMEL_URINE  (game::GetGlobalConst("OMMEL_URINE"))
#define PEPSI  (game::GetGlobalConst("PEPSI"))
#define WATER  (game::GetGlobalConst("WATER"))
#define HEALING_LIQUID  (game::GetGlobalConst("HEALING_LIQUID"))
#define BLOOD  (game::GetGlobalConst("BLOOD"))
#define BROWN_SLIME  (game::GetGlobalConst("BROWN_SLIME"))
#define POISON_LIQUID  (game::GetGlobalConst("POISON_LIQUID"))
#define VALDEMAR  (game::GetGlobalConst("VALDEMAR"))
#define ANTIDOTE_LIQUID  (game::GetGlobalConst("ANTIDOTE_LIQUID"))
#define VODKA  (game::GetGlobalConst("VODKA"))
#define TROLL_BLOOD  (game::GetGlobalConst("TROLL_BLOOD"))
#define DARK_FROG_BLOOD  (game::GetGlobalConst("DARK_FROG_BLOOD"))
#define SPIDER_BLOOD  (game::GetGlobalConst("SPIDER_BLOOD"))
#define VOMIT  (game::GetGlobalConst("VOMIT"))
#define ACIDOUS_BLOOD  (game::GetGlobalConst("ACIDOUS_BLOOD"))
#define SULPHURIC_ACID  (game::GetGlobalConst("SULPHURIC_ACID"))
#define DOG_DROOL  (game::GetGlobalConst("DOG_DROOL"))
#define PEA_SOUP  (game::GetGlobalConst("PEA_SOUP"))
#define OMMEL_SWEAT  (game::GetGlobalConst("OMMEL_SWEAT"))
#define OMMEL_TEARS  (game::GetGlobalConst("OMMEL_TEARS"))
#define OMMEL_SNOT  (game::GetGlobalConst("OMMEL_SNOT"))
#define SWEAT  (game::GetGlobalConst("SWEAT"))
#define GLOWING_BLOOD  (game::GetGlobalConst("GLOWING_BLOOD"))
#define YELLOW_SLIME  (game::GetGlobalConst("YELLOW_SLIME"))
#define SICK_BLOOD  (game::GetGlobalConst("SICK_BLOOD"))
#define MUSTARD_GAS_LIQUID  (game::GetGlobalConst("MUSTARD_GAS_LIQUID"))
#define OMMEL_VOMIT  (game::GetGlobalConst("OMMEL_VOMIT"))
#define LIQUID_HORROR  (game::GetGlobalConst("LIQUID_HORROR"))
#define VINEGAR  (game::GetGlobalConst("VINEGAR"))
#define OMMEL_BLOOD  (game::GetGlobalConst("OMMEL_BLOOD"))
#define CURDLED_OMMEL_BLOOD  (game::GetGlobalConst("CURDLED_OMMEL_BLOOD"))
  // alien mod
#define ALIEN_BLOOD  (game::GetGlobalConst("ALIEN_BLOOD"))
#define ALIEN_DROOL  (game::GetGlobalConst("ALIEN_DROOL"))


//#define FLESH_ID (5 << 12)
#define FLESH_ID  (game::GetGlobalConst("FLESH_ID"))
#define GOBLINOID_FLESH  (game::GetGlobalConst("GOBLINOID_FLESH"))
#define PORK  (game::GetGlobalConst("PORK"))
#define BEEF  (game::GetGlobalConst("BEEF"))
#define FROG_FLESH  (game::GetGlobalConst("FROG_FLESH"))
#define ELPURI_FLESH  (game::GetGlobalConst("ELPURI_FLESH"))
#define HUMAN_FLESH  (game::GetGlobalConst("HUMAN_FLESH"))
#define DOLPHIN_FLESH  (game::GetGlobalConst("DOLPHIN_FLESH"))
#define BEAR_FLESH  (game::GetGlobalConst("BEAR_FLESH"))
#define WOLF_FLESH  (game::GetGlobalConst("WOLF_FLESH"))
#define DOG_FLESH  (game::GetGlobalConst("DOG_FLESH"))
#define ENNER_BEAST_FLESH  (game::GetGlobalConst("ENNER_BEAST_FLESH"))
#define SPIDER_FLESH  (game::GetGlobalConst("SPIDER_FLESH"))
#define JACKAL_FLESH  (game::GetGlobalConst("JACKAL_FLESH"))
#define MUTANT_ASS_FLESH  (game::GetGlobalConst("MUTANT_ASS_FLESH"))
#define BAT_FLESH  (game::GetGlobalConst("BAT_FLESH"))
#define WEREWOLF_FLESH  (game::GetGlobalConst("WEREWOLF_FLESH"))
#define KOBOLD_FLESH  (game::GetGlobalConst("KOBOLD_FLESH"))
#define GIBBERLING_FLESH  (game::GetGlobalConst("GIBBERLING_FLESH"))
#define CAT_FLESH  (game::GetGlobalConst("CAT_FLESH"))
#define RAT_FLESH  (game::GetGlobalConst("RAT_FLESH"))
#define ANGEL_FLESH  (game::GetGlobalConst("ANGEL_FLESH"))
#define DWARF_FLESH  (game::GetGlobalConst("DWARF_FLESH"))
#define DAEMON_FLESH  (game::GetGlobalConst("DAEMON_FLESH"))
#define MAMMOTH_FLESH  (game::GetGlobalConst("MAMMOTH_FLESH"))
#define BLACK_UNICORN_FLESH  (game::GetGlobalConst("BLACK_UNICORN_FLESH"))
#define GRAY_UNICORN_FLESH  (game::GetGlobalConst("GRAY_UNICORN_FLESH"))
#define WHITE_UNICORN_FLESH  (game::GetGlobalConst("WHITE_UNICORN_FLESH"))
#define LION_FLESH  (game::GetGlobalConst("LION_FLESH"))
#define BUFFALO_FLESH  (game::GetGlobalConst("BUFFALO_FLESH"))
#define SNAKE_FLESH  (game::GetGlobalConst("SNAKE_FLESH"))
#define ORC_FLESH  (game::GetGlobalConst("ORC_FLESH"))
#define OSTRICH_FLESH  (game::GetGlobalConst("OSTRICH_FLESH"))
#define CHAMELEON_FLESH  (game::GetGlobalConst("CHAMELEON_FLESH"))
#define FLOATING_EYE_FLESH  (game::GetGlobalConst("FLOATING_EYE_FLESH"))
#define MUSHROOM_FLESH  (game::GetGlobalConst("MUSHROOM_FLESH"))
#define MOOSE_FLESH  (game::GetGlobalConst("MOOSE_FLESH"))
#define MAGPIE_FLESH  (game::GetGlobalConst("MAGPIE_FLESH"))
#define SKUNK_FLESH  (game::GetGlobalConst("SKUNK_FLESH"))
#define HEDGEHOG_FLESH  (game::GetGlobalConst("HEDGEHOG_FLESH"))
#define MUTANT_BUNNY_FLESH  (game::GetGlobalConst("MUTANT_BUNNY_FLESH"))
#define HATTIFATTENER_FLESH  (game::GetGlobalConst("HATTIFATTENER_FLESH"))
#define BLINK_DOG_FLESH  (game::GetGlobalConst("BLINK_DOG_FLESH"))
#define MAGIC_MUSHROOM_FLESH  (game::GetGlobalConst("MAGIC_MUSHROOM_FLESH"))
#define SICK_SPIDER_FLESH  (game::GetGlobalConst("SICK_SPIDER_FLESH"))
#define MIND_WORM_FLESH  (game::GetGlobalConst("MIND_WORM_FLESH"))
#define MUTANT_HEDGEHOG_FLESH  (game::GetGlobalConst("MUTANT_HEDGEHOG_FLESH"))
#define EAGLE_FLESH  (game::GetGlobalConst("EAGLE_FLESH"))
#define KABOUTER_FLESH  (game::GetGlobalConst("KABOUTER_FLESH"))
#define ULDRA_FLESH  (game::GetGlobalConst("ULDRA_FLESH"))
#define OKAPI_FLESH  (game::GetGlobalConst("OKAPI_FLESH"))
#define VAMPIRE_FLESH  (game::GetGlobalConst("VAMPIRE_FLESH"))
#define MOUSE_FLESH  (game::GetGlobalConst("MOUSE_FLESH"))
#define FOX_FLESH  (game::GetGlobalConst("FOX_FLESH"))
#define THUNDER_BIRD_FLESH  (game::GetGlobalConst("THUNDER_BIRD_FLESH"))
#define ELF_FLESH  (game::GetGlobalConst("ELF_FLESH"))
  // alien mod
#define ALIEN_FLESH  (game::GetGlobalConst("ALIEN_FLESH"))


#define POWDER_ID  (game::GetGlobalConst("POWDER_ID"))
#define GUN_POWDER  (game::GetGlobalConst("GUN_POWDER"))
#define SNOW  (game::GetGlobalConst("SNOW"))
#define SAND  (game::GetGlobalConst("SAND"))


//#define IRON_ALLOY_ID (7 << 12)
#define IRON_ALLOY_ID  (game::GetGlobalConst("IRON_ALLOY_ID"))
#define IRON  (game::GetGlobalConst("IRON"))
#define STEEL  (game::GetGlobalConst("STEEL"))
#define METEORIC_STEEL  (game::GetGlobalConst("METEORIC_STEEL"))
#define ADAMANT  (game::GetGlobalConst("ADAMANT"))
#define DAMASCUS_STEEL  (game::GetGlobalConst("DAMASCUS_STEEL"))


#define LONG_SWORD  (game::GetGlobalConst("LONG_SWORD"))
#define TWO_HANDED_SWORD  (game::GetGlobalConst("TWO_HANDED_SWORD"))
#define TWO_HANDED_SCIMITAR  (game::GetGlobalConst("TWO_HANDED_SCIMITAR"))
#define SPEAR  (game::GetGlobalConst("SPEAR"))
#define AXE  (game::GetGlobalConst("AXE"))
#define HALBERD  (game::GetGlobalConst("HALBERD"))
#define MACE  (game::GetGlobalConst("MACE"))
#define WAR_HAMMER  (game::GetGlobalConst("WAR_HAMMER"))
#define SICKLE  (game::GetGlobalConst("SICKLE"))
#define DAGGER  (game::GetGlobalConst("DAGGER"))
#define SHORT_SWORD  (game::GetGlobalConst("SHORT_SWORD"))
#define BASTARD_SWORD  (game::GetGlobalConst("BASTARD_SWORD"))
#define BATTLE_AXE  (game::GetGlobalConst("BATTLE_AXE"))
#define SCYTHE  (game::GetGlobalConst("SCYTHE"))
#define QUARTER_STAFF  (game::GetGlobalConst("QUARTER_STAFF"))
#define HAMMER  (game::GetGlobalConst("HAMMER"))
#define KNIGHT_SWORD  (game::GetGlobalConst("KNIGHT_SWORD"))
#define KATANA  (game::GetGlobalConst("KATANA"))
#define SPETUM  (game::GetGlobalConst("SPETUM"))
#define TIP_SWORD  (game::GetGlobalConst("TIP_SWORD"))
#define KNUCKLE  (game::GetGlobalConst("KNUCKLE"))
#define RAPIER  (game::GetGlobalConst("RAPIER"))
#define GREAT_AXE  (game::GetGlobalConst("GREAT_AXE"))
#define GRAND_STOLLEN_KNIFE  (game::GetGlobalConst("GRAND_STOLLEN_KNIFE"))
#define CLAW  (game::GetGlobalConst("CLAW"))
#define LOST_RUBY_FLAMING_SWORD  (game::GetGlobalConst("LOST_RUBY_FLAMING_SWORD"))


#define GOROVITS_HAMMER  (game::GetGlobalConst("GOROVITS_HAMMER"))
#define GOROVITS_SICKLE  (game::GetGlobalConst("GOROVITS_SICKLE"))
#define GOROVITS_SCIMITAR  (game::GetGlobalConst("GOROVITS_SCIMITAR"))

#define CHAIN_MAIL  (game::GetGlobalConst("CHAIN_MAIL"))
#define PLATE_MAIL  (game::GetGlobalConst("PLATE_MAIL"))
#define ARMOR_OF_GREAT_HEALTH  (game::GetGlobalConst("ARMOR_OF_GREAT_HEALTH"))
#define DRAGON_CUIRASS  (game::GetGlobalConst("DRAGON_CUIRASS"))
#define ARMOR_OF_THE_OMMEL  (game::GetGlobalConst("ARMOR_OF_THE_OMMEL"))
#define FILTHY_TUNIC  (game::GetGlobalConst("FILTHY_TUNIC"))


#define CHEAP 1
#define EXPENSIVE 2

#define WAND_OF_POLYMORPH  (game::GetGlobalConst("WAND_OF_POLYMORPH"))
#define WAND_OF_STRIKING  (game::GetGlobalConst("WAND_OF_STRIKING"))
#define WAND_OF_FIRE_BALLS  (game::GetGlobalConst("WAND_OF_FIRE_BALLS"))
#define WAND_OF_TELEPORTATION  (game::GetGlobalConst("WAND_OF_TELEPORTATION"))
#define WAND_OF_HASTE  (game::GetGlobalConst("WAND_OF_HASTE"))
#define WAND_OF_SLOW  (game::GetGlobalConst("WAND_OF_SLOW"))
#define WAND_OF_RESURRECTION  (game::GetGlobalConst("WAND_OF_RESURRECTION"))
#define WAND_OF_DOOR_CREATION  (game::GetGlobalConst("WAND_OF_DOOR_CREATION"))
#define WAND_OF_INVISIBILITY  (game::GetGlobalConst("WAND_OF_INVISIBILITY"))
#define WAND_OF_CLONING  (game::GetGlobalConst("WAND_OF_CLONING"))
#define WAND_OF_LIGHTNING  (game::GetGlobalConst("WAND_OF_LIGHTNING"))
#define WAND_OF_ACID_RAIN  (game::GetGlobalConst("WAND_OF_ACID_RAIN"))
#define WAND_OF_MIRRORING  (game::GetGlobalConst("WAND_OF_MIRRORING"))
#define WAND_OF_NECROMANCY  (game::GetGlobalConst("WAND_OF_NECROMANCY"))


#define RUNED_WHIP  (game::GetGlobalConst("RUNED_WHIP"))

#define BIG_MINE  (game::GetGlobalConst("BIG_MINE"))

#define CLOAK_OF_INVISIBILITY  (game::GetGlobalConst("CLOAK_OF_INVISIBILITY"))
#define CLOAK_OF_FIRE_RESISTANCE  (game::GetGlobalConst("CLOAK_OF_FIRE_RESISTANCE"))
#define CLOAK_OF_ELECTRICITY_RESISTANCE  (game::GetGlobalConst("CLOAK_OF_ELECTRICITY_RESISTANCE"))
#define CLOAK_OF_ACID_RESISTANCE  (game::GetGlobalConst("CLOAK_OF_ACID_RESISTANCE"))
#define CLOAK_OF_WERE_WOLF_FUR  (game::GetGlobalConst("CLOAK_OF_WERE_WOLF_FUR"))
#define CLOAK_OF_SHADOWS  (game::GetGlobalConst("CLOAK_OF_SHADOWS"))


#define BOOT_OF_STRENGTH  (game::GetGlobalConst("BOOT_OF_STRENGTH"))
#define BOOT_OF_AGILITY  (game::GetGlobalConst("BOOT_OF_AGILITY"))
#define BOOT_OF_KICKING  (game::GetGlobalConst("BOOT_OF_KICKING"))


#define GAUNTLET_OF_STRENGTH  (game::GetGlobalConst("GAUNTLET_OF_STRENGTH"))
#define GAUNTLET_OF_DEXTERITY  (game::GetGlobalConst("GAUNTLET_OF_DEXTERITY"))


#define RING_OF_FIRE_RESISTANCE  (game::GetGlobalConst("RING_OF_FIRE_RESISTANCE"))
#define RING_OF_POLYMORPH_CONTROL  (game::GetGlobalConst("RING_OF_POLYMORPH_CONTROL"))
#define RING_OF_INFRA_VISION  (game::GetGlobalConst("RING_OF_INFRA_VISION"))
#define RING_OF_TELEPORTATION  (game::GetGlobalConst("RING_OF_TELEPORTATION"))
#define RING_OF_TELEPORT_CONTROL  (game::GetGlobalConst("RING_OF_TELEPORT_CONTROL"))
#define RING_OF_POLYMORPH  (game::GetGlobalConst("RING_OF_POLYMORPH"))
#define RING_OF_POISON_RESISTANCE  (game::GetGlobalConst("RING_OF_POISON_RESISTANCE"))
#define RING_OF_INVISIBILITY  (game::GetGlobalConst("RING_OF_INVISIBILITY"))
#define RING_OF_ELECTRICITY_RESISTANCE  (game::GetGlobalConst("RING_OF_ELECTRICITY_RESISTANCE"))
#define RING_OF_SEARCHING  (game::GetGlobalConst("RING_OF_SEARCHING"))
#define RING_OF_ACID_RESISTANCE  (game::GetGlobalConst("RING_OF_ACID_RESISTANCE"))


#define AMULET_OF_LIFE_SAVING  (game::GetGlobalConst("AMULET_OF_LIFE_SAVING"))
#define AMULET_OF_ESP  (game::GetGlobalConst("AMULET_OF_ESP"))
#define AMULET_OF_WARDING  (game::GetGlobalConst("AMULET_OF_WARDING"))
#define AMULET_OF_VANITY  (game::GetGlobalConst("AMULET_OF_VANITY"))


#define FULL_HELMET  (game::GetGlobalConst("FULL_HELMET"))
#define HELM_OF_PERCEPTION  (game::GetGlobalConst("HELM_OF_PERCEPTION"))
#define HELM_OF_UNDERSTANDING  (game::GetGlobalConst("HELM_OF_UNDERSTANDING"))
#define HELM_OF_BRILLIANCE  (game::GetGlobalConst("HELM_OF_BRILLIANCE"))
#define HELM_OF_ATTRACTIVITY  (game::GetGlobalConst("HELM_OF_ATTRACTIVITY"))
#define GOROVITS_FAMILY_GAS_MASK  (game::GetGlobalConst("GOROVITS_FAMILY_GAS_MASK"))


#define BELT_OF_CARRYING  (game::GetGlobalConst("BELT_OF_CARRYING"))
#define BELT_OF_LEVITATION  (game::GetGlobalConst("BELT_OF_LEVITATION"))


#define SMALL_CHEST  (game::GetGlobalConst("SMALL_CHEST"))
#define CHEST  (game::GetGlobalConst("CHEST"))
#define LARGE_CHEST  (game::GetGlobalConst("LARGE_CHEST"))
#define STRONG_BOX  (game::GetGlobalConst("STRONG_BOX"))
#define MAGIC_CHEST  (game::GetGlobalConst("MAGIC_CHEST"))


#define BRAVERY  (game::GetGlobalConst("BRAVERY"))
#define FEAR  (game::GetGlobalConst("FEAR"))
#define CONFUSION  (game::GetGlobalConst("CONFUSION"))


#define ROOKIE  (game::GetGlobalConst("ROOKIE"))
#define VETERAN  (game::GetGlobalConst("VETERAN"))
#define EUNUCH  (game::GetGlobalConst("EUNUCH"))
#define PATROL  (game::GetGlobalConst("PATROL"))
#define SHOP  (game::GetGlobalConst("SHOP"))
#define ELITE  (game::GetGlobalConst("ELITE"))
#define MASTER  (game::GetGlobalConst("MASTER"))
#define GRAND_MASTER  (game::GetGlobalConst("GRAND_MASTER"))
#define MONDEDR_GUARD  (game::GetGlobalConst("MONDEDR_GUARD"))
#define DWARVEN_GUARD  (game::GetGlobalConst("DWARVEN_GUARD"))
#define SENTINEL  (game::GetGlobalConst("SENTINEL"))
#define FOREST_SHOP  (game::GetGlobalConst("FOREST_SHOP"))
#define TOMB_ENTRY  (game::GetGlobalConst("TOMB_ENTRY"))
#define TOMB_ENTRY_MASTER  (game::GetGlobalConst("TOMB_ENTRY_MASTER"))
#define HONOR  (game::GetGlobalConst("HONOR"))
#define EMISSARY  (game::GetGlobalConst("EMISSARY"))
#define TRAINEE  (game::GetGlobalConst("TRAINEE"))

#define ENQUIOX  (game::GetGlobalConst("ENQUIOX"))

#define DARK  (game::GetGlobalConst("DARK"))
#define GREATER_DARK  (game::GetGlobalConst("GREATER_DARK"))
#define GIANT_DARK  (game::GetGlobalConst("GIANT_DARK"))
#define LIGHT  (game::GetGlobalConst("LIGHT"))
#define GREATER_LIGHT  (game::GetGlobalConst("GREATER_LIGHT"))
#define GIANT_LIGHT  (game::GetGlobalConst("GIANT_LIGHT"))


#define WARRIOR  (game::GetGlobalConst("WARRIOR"))
#define WAR_LORD  (game::GetGlobalConst("WAR_LORD"))


#define BERSERKER  (game::GetGlobalConst("BERSERKER"))
#define BUTCHER  (game::GetGlobalConst("BUTCHER"))
#define PRINCE  (game::GetGlobalConst("PRINCE"))
#define KING  (game::GetGlobalConst("KING"))
#define JAILER  (game::GetGlobalConst("JAILER"))
#define PRISON_WARDEN  (game::GetGlobalConst("PRISON_WARDEN"))


#define CONICAL  (game::GetGlobalConst("CONICAL"))
#define FLAT  (game::GetGlobalConst("FLAT"))


#define LARGE  (game::GetGlobalConst("LARGE"))
#define GIANT  (game::GetGlobalConst("GIANT"))
#define ARANEA  (game::GetGlobalConst("ARANEA"))


#define BLACK_BEAR  (game::GetGlobalConst("BLACK_BEAR"))
#define GRIZZLY_BEAR  (game::GetGlobalConst("GRIZZLY_BEAR"))
#define CAVE_BEAR  (game::GetGlobalConst("CAVE_BEAR"))
#define POLAR_BEAR  (game::GetGlobalConst("POLAR_BEAR"))
#define PANDA_BEAR  (game::GetGlobalConst("PANDA_BEAR"))


#define ZOMBIE_OF_KHAZ_ZADM  (game::GetGlobalConst("ZOMBIE_OF_KHAZ_ZADM"))


#define TORTURING_CHIEF  (game::GetGlobalConst("TORTURING_CHIEF"))
#define WHIP_CHAMPION  (game::GetGlobalConst("WHIP_CHAMPION"))
#define WAR_LADY  (game::GetGlobalConst("WAR_LADY"))
#define QUEEN  (game::GetGlobalConst("QUEEN"))


#define CHIEFTAIN  (game::GetGlobalConst("CHIEFTAIN"))
#define LORD  (game::GetGlobalConst("LORD"))
#define PATRIARCH  (game::GetGlobalConst("PATRIARCH"))


#define AMBULATORY  (game::GetGlobalConst("AMBULATORY"))


#define GREATER  (game::GetGlobalConst("GREATER"))
#define GIANTIC  (game::GetGlobalConst("GIANTIC"))
#define SHAMBLING  (game::GetGlobalConst("SHAMBLING"))
#define LILY  (game::GetGlobalConst("LILY"))


#define SLAUGHTERER  (game::GetGlobalConst("SLAUGHTERER"))
#define SQUAD_LEADER  (game::GetGlobalConst("SQUAD_LEADER"))
#define OFFICER  (game::GetGlobalConst("OFFICER"))
#define GENERAL  (game::GetGlobalConst("GENERAL"))
#define MARSHAL  (game::GetGlobalConst("MARSHAL"))


#define APPRENTICE  (game::GetGlobalConst("APPRENTICE"))
#define BATTLE_MAGE  (game::GetGlobalConst("BATTLE_MAGE"))
#define ELDER  (game::GetGlobalConst("ELDER"))
#define ARCH_MAGE  (game::GetGlobalConst("ARCH_MAGE"))


#define ROVER  (game::GetGlobalConst("ROVER"))
#define BAND_LEADER  (game::GetGlobalConst("BAND_LEADER"))


#define FIELD_MOUSE  (game::GetGlobalConst("FIELD_MOUSE"))
#define LABORATORY_MOUSE  (game::GetGlobalConst("LABORATORY_MOUSE"))


#define THIN_PIG  (game::GetGlobalConst("THIN_PIG"))


#define STARVED_OX  (game::GetGlobalConst("STARVED_OX"))


#define FLOATIE  (game::GetGlobalConst("FLOATIE"))


#define SONIC  (game::GetGlobalConst("SONIC"))


/* Least significant bit defines sex */
#define BABY_MALE  (game::GetGlobalConst("BABY_MALE"))
#define BABY_FEMALE  (game::GetGlobalConst("BABY_FEMALE"))
#define ADULT_MALE  (game::GetGlobalConst("ADULT_MALE"))
#define ADULT_FEMALE  (game::GetGlobalConst("ADULT_FEMALE"))


#define APPRENTICE_NECROMANCER  (game::GetGlobalConst("APPRENTICE_NECROMANCER"))
#define MASTER_NECROMANCER  (game::GetGlobalConst("MASTER_NECROMANCER"))
#define IMPRISONED_NECROMANCER  (game::GetGlobalConst("IMPRISONED_NECROMANCER"))


#define HUSBAND  (game::GetGlobalConst("HUSBAND"))
#define WIFE  (game::GetGlobalConst("WIFE"))
#define CHILD  (game::GetGlobalConst("CHILD"))


#define BOY  (game::GetGlobalConst("BOY"))
#define GIRL  (game::GetGlobalConst("GIRL"))


#define LIGHT_ASIAN_SIREN  (game::GetGlobalConst("LIGHT_ASIAN_SIREN"))
#define DARK_ASIAN_SIREN  (game::GetGlobalConst("DARK_ASIAN_SIREN"))
#define CAUCASIAN_SIREN  (game::GetGlobalConst("CAUCASIAN_SIREN"))
#define DARK_SIREN  (game::GetGlobalConst("DARK_SIREN"))
#define GREEN_SIREN  (game::GetGlobalConst("GREEN_SIREN"))


#define PARQUET  (game::GetGlobalConst("PARQUET"))
#define FLOOR  (game::GetGlobalConst("FLOOR"))
#define GROUND  (game::GetGlobalConst("GROUND"))
#define GRASS_TERRAIN  (game::GetGlobalConst("GRASS_TERRAIN"))
#define LANDING_SITE  (game::GetGlobalConst("LANDING_SITE"))
#define SNOW_TERRAIN  (game::GetGlobalConst("SNOW_TERRAIN"))
#define DARK_GRASS_TERRAIN  (game::GetGlobalConst("DARK_GRASS_TERRAIN"))
#define SAND_TERRAIN  (game::GetGlobalConst("SAND_TERRAIN"))
#define DEAD_GRASS_TERRAIN  (game::GetGlobalConst("DEAD_GRASS_TERRAIN"))


#define POOL  (game::GetGlobalConst("POOL"))
#define UNDERGROUND_LAKE  (game::GetGlobalConst("UNDERGROUND_LAKE"))


#define BRICK_FINE  (game::GetGlobalConst("BRICK_FINE"))
#define BRICK_PROPAGANDA  (game::GetGlobalConst("BRICK_PROPAGANDA"))
#define BRICK_OLD  (game::GetGlobalConst("BRICK_OLD"))
#define BRICK_PRIMITIVE  (game::GetGlobalConst("BRICK_PRIMITIVE"))
#define BRICK_PRIMITIVE_PROPAGANDA  (game::GetGlobalConst("BRICK_PRIMITIVE_PROPAGANDA"))
#define STONE_WALL  (game::GetGlobalConst("STONE_WALL"))
#define ICE_WALL  (game::GetGlobalConst("ICE_WALL"))
#define BROKEN_WALL  (game::GetGlobalConst("BROKEN_WALL"))


#define PINE  (game::GetGlobalConst("PINE"))
#define FIR  (game::GetGlobalConst("FIR"))
#define HOLY_TREE  (game::GetGlobalConst("HOLY_TREE"))
#define CARPET  (game::GetGlobalConst("CARPET"))
#define COUCH  (game::GetGlobalConst("COUCH"))
#define DOUBLE_BED  (game::GetGlobalConst("DOUBLE_BED"))
#define POOL_BORDER  (game::GetGlobalConst("POOL_BORDER"))
#define POOL_CORNER  (game::GetGlobalConst("POOL_CORNER"))
#define PALM  (game::GetGlobalConst("PALM"))
#define SNOW_PINE  (game::GetGlobalConst("SNOW_PINE"))
#define SNOW_FIR  (game::GetGlobalConst("SNOW_FIR"))
#define ANVIL  (game::GetGlobalConst("ANVIL"))
#define SHARD  (game::GetGlobalConst("SHARD"))
#define CACTUS  (game::GetGlobalConst("CACTUS"))
#define OAK  (game::GetGlobalConst("OAK"))
#define BIRCH  (game::GetGlobalConst("BIRCH"))
#define TEAK  (game::GetGlobalConst("TEAK"))
#define DWARF_BIRCH  (game::GetGlobalConst("DWARF_BIRCH"))

#define ARM_CHAIR  (game::GetGlobalConst("ARM_CHAIR"))
#define BANANA_TREE  (game::GetGlobalConst("BANANA_TREE"))
#define BENCH  (game::GetGlobalConst("BENCH"))
#define CHAIR  (game::GetGlobalConst("CHAIR"))
#define CHEAP_BED  (game::GetGlobalConst("CHEAP_BED"))
#define DEAD_TREE  (game::GetGlobalConst("DEAD_TREE"))
#define DESK  (game::GetGlobalConst("DESK"))
#define EXPENSIVE_BED  (game::GetGlobalConst("EXPENSIVE_BED"))
#define FORGE  (game::GetGlobalConst("FORGE"))
#define FURNACE  (game::GetGlobalConst("FURNACE"))
#define OVEN  (game::GetGlobalConst("OVEN"))
#define PEDESTAL  (game::GetGlobalConst("PEDESTAL"))
#define PLAIN_BED  (game::GetGlobalConst("PLAIN_BED"))
#define SHACKLES  (game::GetGlobalConst("SHACKLES"))
#define STRANGE_TREE  (game::GetGlobalConst("STRANGE_TREE"))
#define TABLE  (game::GetGlobalConst("TABLE"))
#define TORTURE_RACK  (game::GetGlobalConst("TORTURE_RACK"))
#define WELL  (game::GetGlobalConst("WELL"))
#define WOODEN_HORSE  (game::GetGlobalConst("WOODEN_HORSE"))
#define WORK_BENCH  (game::GetGlobalConst("WORK_BENCH"))

#define SNOW_BOULDER  (game::GetGlobalConst("SNOW_BOULDER"))

#define STAIRS_UP  (game::GetGlobalConst("STAIRS_UP"))
#define STAIRS_DOWN  (game::GetGlobalConst("STAIRS_DOWN"))
#define OREE_LAIR_ENTRY  (game::GetGlobalConst("OREE_LAIR_ENTRY"))
#define OREE_LAIR_EXIT  (game::GetGlobalConst("OREE_LAIR_EXIT"))
#define SUMO_ARENA_ENTRY  (game::GetGlobalConst("SUMO_ARENA_ENTRY"))
#define SUMO_ARENA_EXIT  (game::GetGlobalConst("SUMO_ARENA_EXIT"))
#define KHARAZ_ARAD_ENTRY  (game::GetGlobalConst("KHARAZ_ARAD_ENTRY"))
#define KHARAZ_ARAD_EXIT  (game::GetGlobalConst("KHARAZ_ARAD_EXIT"))
#define WAYPOINT_DEEPER  (game::GetGlobalConst("WAYPOINT_DEEPER"))
#define WAYPOINT_SHALLOWER  (game::GetGlobalConst("WAYPOINT_SHALLOWER"))
#define XINROCH_TOMB_ENTRANCE  (game::GetGlobalConst("XINROCH_TOMB_ENTRANCE"))
#define XINROCH_TOMB_EXIT  (game::GetGlobalConst("XINROCH_TOMB_EXIT"))
#define FOUNTAIN  (game::GetGlobalConst("FOUNTAIN"))


#define BOOK_CASE  (game::GetGlobalConst("BOOK_CASE"))
#define CHEST_OF_DRAWERS  (game::GetGlobalConst("CHEST_OF_DRAWERS"))
#define SHELF  (game::GetGlobalConst("SHELF"))


#define BROKEN_BARWALL  (game::GetGlobalConst("BROKEN_BARWALL"))


#define BARDOOR  (game::GetGlobalConst("BARDOOR"))
#define SECRET_DOOR  (game::GetGlobalConst("SECRET_DOOR"))


#define ROOM_NORMAL  (game::GetGlobalConst("ROOM_NORMAL"))
#define ROOM_SHOP  (game::GetGlobalConst("ROOM_SHOP"))
#define ROOM_CATHEDRAL  (game::GetGlobalConst("ROOM_CATHEDRAL"))
#define ROOM_LIBRARY  (game::GetGlobalConst("ROOM_LIBRARY"))
#define ROOM_BANANA_DROP_AREA  (game::GetGlobalConst("ROOM_BANANA_DROP_AREA"))
#define ROOM_SUMO_ARENA  (game::GetGlobalConst("ROOM_SUMO_ARENA"))
#define ROOM_VAULT  (game::GetGlobalConst("ROOM_VAULT"))


#define ALL_DUNGEONS  32767


#endif
