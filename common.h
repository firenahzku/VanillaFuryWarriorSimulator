#ifndef __COMMON_H_INCLUDED__
#define __COMMON_H_INCLUDED__

const int SIZE_CHARRACE = 8;
enum CharRace : int {	// Race-specific bonuses are handled in CharWarrior::addRacialStats()
	RACE_HUMAN		= 0,
	RACE_GNOME		= 1,
	RACE_DWARF		= 2,
	RACE_NIGHTELF	= 3,
	RACE_ORC		= 4,
	RACE_TAUREN		= 5,
	RACE_TROLL		= 6,
	RACE_UNDEAD		= 7
};

const int SIZE_ROLLRESULT = 5;
enum RollResult : int {
	ROLL_MISS		= 0,
	ROLL_DODGE		= 1,
	ROLL_GLANCING	= 2,
	ROLL_CRITICAL	= 3,
	ROLL_NORMAL		= 4
};

const int SIZE_RESISTRESULT = 5;
enum ResistResult : int {
	RESIST_0		= 0,
	RESIST_25		= 1,
	RESIST_50		= 2,
	RESIST_75		= 3,
	RESIST_100		= 4
};

const int SIZE_STATTYPE = 27;
enum StatType : int {
	STAT_STRENGTH				= 0,
	STAT_AGILITY				= 1,
	STAT_STAMINA				= 2,
	STAT_INTELLECT				= 3,
	STAT_SPIRIT					= 4,
	STAT_STATMULTIPLIER			= 5,
	STAT_ARMOR					= 6,
	STAT_MELEEHIT				= 7,
	STAT_MELEECRIT				= 8,
	STAT_SPELLHIT				= 9,
	STAT_SPELLCRIT				= 10,
	STAT_AP						= 11,
	STAT_SP						= 12,
	STAT_PENETRATION			= 13,
	STAT_DAMAGEMULTIPLIER		= 14,
	STAT_ABILITYCRITMULTIPLIER	= 15,
	STAT_1HSWORDS				= 16,
	STAT_2HSWORDS				= 17,
	STAT_1HMACES				= 18,
	STAT_2HMACES				= 19,
	STAT_1HAXES					= 20,
	STAT_2HAXES					= 21,
	STAT_DAGGERS				= 22,
	STAT_FISTS					= 23,
	STAT_STAVES					= 24,
	STAT_POLEARMS				= 25,
	STAT_HASTECOEFFICIENT		= 26
};

const int SIZE_PROCTYPE = 35;
enum ProcType : int {
	PROC_NONE						= 0,
	PROC_TALENT_UNBRIDLEDWRATH		= 1,
	PROC_ENCHANTMENT_CRUSADER		= 2,
	PROC_ENCHANTMENT_FIERY			= 3,
	PROC_ENCHANTMENT_LIFESTEALING	= 4,
	PROC_ENCHANTMENT_INSTANTPOISON	= 5,
	PROC_WEAPON_ELEMENTALDMG		= 6,	// Not used
	PROC_WEAPON_THUNDERFURY			= 7,
	PROC_WEAPON_VISKAG				= 8,
	PROC_WEAPON_IRONFOE				= 9,
	PROC_WEAPON_EMPYREAN			= 10,
	PROC_WEAPON_THRASHBLADE			= 11,
	PROC_WEAPON_MANSLAYER			= 12,
	PROC_WEAPON_DEATHBRINGER		= 13,
	PROC_WEAPON_PERDITION			= 14,
	PROC_WEAPON_SERVOARM			= 15,
	PROC_WEAPON_UNTAMEDBLADE		= 16,
	PROC_WEAPON_ANNIHILATOR			= 17,
	PROC_WEAPON_ESKHANDAR			= 18,
	PROC_WEAPON_FLURRYAXE			= 19,
	PROC_WEAPON_SWORDOFZEAL			= 20,
	PROC_WEAPON_FELSTRIKER			= 21,
	PROC_WEAPON_DEMONSHEAR			= 22,
	PROC_WEAPON_MALOWN				= 23,
	PROC_WEAPON_DEEPWOODS			= 24,
	PROC_WEAPON_NIGHTFALL			= 25,
	PROC_WEAPON_DESTINY				= 26,
	PROC_ITEM_MAELSTROM				= 27,
	PROC_ITEM_HOJ					= 28,
	PROC_ITEM_WYRMTHALAK			= 29,
	PROC_ITEM_SWARMGUARD			= 30,
	PROC_TALENT_SWORDSPECIALIZATION	= 31,
	PROC_TALENT_FLURRY				= 32,
	PROC_ENCHANTMENT_WINDFURY		= 33,
	PROC_WEAPON_SULFURAS			= 34
};

const int SIZE_USEEFFECT = 16;
enum UseEffect : int {
	USE_NONE					= 0,
	USE_ITEM_MIGHTYRAGE			= 1,
	USE_ITEM_EARTHSTRIKE		= 2,
	USE_ITEM_DIAMONDFLASK		= 3,
	USE_ITEM_SWARMGUARD			= 4,
	USE_ITEM_JOMGABBAR			= 5,
	USE_ITEM_CLOUDKEEPER		= 6,
	USE_ITEM_BAROV				= 7,
	USE_ITEM_KISSOFTHESPIDER	= 8,
	USE_ITEM_SLAYERSCREST		= 9,
	USE_SPELL_BLOODRAGE			= 10,
	USE_SPELL_BERSERKERRAGE		= 11,
	USE_SPELL_DEATHWISH			= 12,
	USE_SPELL_BERSERKING		= 13,
	USE_SPELL_BLOODFURY			= 14,
	USE_ITEM_HARVESTREAPER		= 15
};

const int SIZE_ITEMTYPE = 23;
enum ItemType : int {
	ITEM_NONE			= 0,
	ITEM_SWORD			= 1,
	ITEM_MACE			= 2,
	ITEM_AXE			= 3,
	ITEM_2HSWORD		= 4,
	ITEM_2HMACE			= 5,
	ITEM_2HAXE			= 6,
	ITEM_DAGGER			= 7,
	ITEM_FIST			= 8,
	ITEM_RANGED			= 9,
	ITEM_HEAD			= 10,
	ITEM_NECK			= 11,
	ITEM_SHOULDERS		= 12,
	ITEM_BACK			= 13,
	ITEM_CHEST			= 14,
	ITEM_WRIST			= 15,
	ITEM_HANDS			= 16,
	ITEM_WAIST			= 17,
	ITEM_LEGS			= 18,
	ITEM_FEET			= 19,
	ITEM_RING			= 20,
	ITEM_TRINKET		= 21,
	ITEM_ENCHANTMENT	= 22
};

const int SIZE_SETBONUS = 7;
enum SetBonus : int {	// Set bonus actions are handled in CharWarrior::addSetBonuses()
	SET_NONE			= 0,
	SET_DALRENDSARMS	= 1,	// +50 AP with 2
	SET_HAKKARI_BLADES	= 2,	// +6 swords skill with 2
	SET_DEFILERS		= 3,	// +1% crit with 3
	SET_CHAMPION		= 4,	// 40 AP with 2
	SET_WARLORD			= 5,	// 40 AP with 6
	SET_DEVILSAUR		= 6	// 2% hit with 2
};

const int SIZE_ITEMSTATTYPE = 27;
enum ItemStatType : int {
	item_damageLower			= 0,
	item_damageUpper			= 1,
	item_elementalLower			= 2,
	item_elementalUpper			= 3,
	item_elementalSchool		= 4,
	item_weaponAllDamage		= 5,
	item_elementalAllDamage		= 6,
	item_strength				= 7,
	item_agility				= 8,
	item_attackPower			= 9,
	item_spellPower				= 10,
	item_meleeCrit				= 11,
	item_meleeHit				= 12,
	item_spellCrit				= 13,
	item_spellHit				= 14,
	item_weaponSkillSwords		= 15,
	item_weaponSkill2hSwords	= 16,
	item_weaponSkillMaces		= 17,
	item_weaponSkill2hMaces		= 18,
	item_weaponSkillAxes		= 19,
	item_weaponSkill2hAxes		= 20,
	item_weaponSkillDaggers		= 21,
	item_weaponSkillFists		= 22,
	item_weaponSkillStaves		= 23,
	item_weaponSkillPolearms	= 24,
	item_haste					= 25,
	item_armor					= 26
};

const int SIZE_INVENTORYSLOT = 29;
enum InventorySlot : int {
	SLOT_HEAD					= 0, 
	SLOT_HEAD_ENCHANTMENT		= 1,
	SLOT_NECK					= 2, 
	SLOT_SHOULDERS				= 3, 
	SLOT_SHOULDERS_ENCHANTMENT	= 4, 
	SLOT_BACK					= 5, 
	SLOT_BACK_ENCHANTMENT		= 6, 
	SLOT_CHEST					= 7, 
	SLOT_CHEST_ENCHANTMENT		= 8, 
	SLOT_WRIST					= 9, 
	SLOT_WRIST_ENCHANTMENT		= 10, 
	SLOT_GLOVES					= 11, 
	SLOT_GLOVES_ENCHANTMENT		= 12, 
	SLOT_WAIST					= 13, 
	SLOT_LEGS					= 14, 
	SLOT_LEGS_ENCHANTMENT		= 15, 
	SLOT_FEET					= 16, 
	SLOT_FEET_ENCHANTMENT		= 17, 
	SLOT_RING1					= 18, 
	SLOT_RING2					= 19, 
	SLOT_TRINKET1				= 20, 
	SLOT_TRINKET2				= 21, 
	SLOT_MH						= 22, 
	SLOT_MH_ENCHANTMENT			= 23, 
	SLOT_MH_ENCHANTMENT_TEMP	= 24, 
	SLOT_OH						= 25, 
	SLOT_OH_ENCHANTMENT			= 26, 
	SLOT_OH_ENCHANTMENT_TEMP	= 27, 
	SLOT_RANGED					= 28
};

const int SIZE_MAGICSCHOOL = 7;
enum MagicSchool : int {
	SCHOOL_PHYSICAL		= 0,
	SCHOOL_FROST		= 1,
	SCHOOL_FIRE			= 2,
	SCHOOL_NATURE		= 3,
	SCHOOL_ARCANE		= 4,
	SCHOOL_SHADOW		= 5,
	SCHOOL_HOLY			= 6
};

const int SIZE_TALENTWARRIOR = 52;
enum TalentWarrior : int {
	TALENT_IMPROVEDHEROICSTRIKE			= 0,
	TALENT_DEFLECTION					= 1,
	TALENT_IMPROVEDREND					= 2,
	TALENT_IMPROVEDCHARGE				= 3,
	TALENT_TACTICALMASTERY				= 4,
	TALENT_IMPROVEDTHUNDERCLAP			= 5,
	TALENT_IMPROVEDOVERPOWER			= 6,
	TALENT_ANGERMANAGEMENT				= 7,
	TALENT_DEEPWOUNDS					= 8,
	TALENT_2HWEAPONSPECIALIZATION		= 9,
	TALENT_IMPALE						= 10,
	TALENT_AXESPECIALIZATION			= 11,
	TALENT_SWEEPINGSTRIKES				= 12,
	TALENT_MACESPECIALIZATION			= 13,
	TALENT_SWORDSPECIALIZATION			= 14,
	TALENT_POLEARMSPECIALIZATION		= 15,
	TALENT_IMPROVEDHAMSTRING			= 16,
	TALENT_MORTALSTRIKE					= 17,
	TALENT_BOOMINGVOICE					= 18,
	TALENT_CRUELTY						= 19,
	TALENT_IMPROVEDDEMORALIZINGSHOUT	= 20,
	TALENT_UNBRIDLEDWRATH				= 21,
	TALENT_IMPROVEDCLEAVE				= 22,
	TALENT_PIERCINGHOWL					= 23,
	TALENT_BLOODCRAZE					= 24,
	TALENT_IMPROVEDBATTLESHOUT			= 25,
	TALENT_DUALWIELDSPECIALIZATION		= 26,
	TALENT_IMPROVEDEXECUTE				= 27,
	TALENT_ENRAGE						= 28,
	TALENT_IMPROVEDSLAM					= 29,
	TALENT_DEATHWISH					= 30,
	TALENT_IMPROVEDINTERCEPT			= 31,
	TALENT_IMPROVEDBERSERKERRAGE		= 32,
	TALENT_FLURRY						= 33,
	TALENT_BLOODTHIRST					= 34,
	TALENT_SHIELDSPECIALIZATION			= 35,
	TALENT_ANTICIPATION					= 36,
	TALENT_IMPROVEDBLOODRAGE			= 37,
	TALENT_TOUGHNESS					= 38,
	TALENT_IRONWILL						= 39,
	TALENT_LASTSTAND					= 40,
	TALENT_IMPROVEDSHIELDBLOCK			= 41,
	TALENT_IMPROVEDREVENGE				= 42,
	TALENT_DEFIANCE						= 43,
	TALENT_IMPROVEDSUNDERARMOR			= 44,
	TALENT_IMPROVEDDISARM				= 45,
	TALENT_IMPROVEDTAUNT				= 46,
	TALENT_IMPROVEDSHIELDWALL			= 47,
	TALENT_CONCUSSIONBLOW				= 48,
	TALENT_IMPROVEDSHIELDBASH			= 49,
	TALENT_1HWEAPONSPECIALIZATION		= 50,
	TALENT_SHIELDSLAM					= 51
};

const int SIZE_ABILITYWARRIOR = 17;
enum AbilityWarrior : int {
	ABILITY_HAMSTRING		= 0,
	ABILITY_HEROICSTRIKE	= 1,
	ABILITY_OVERPOWER		= 2,
	ABILITY_SWEEPINGSTRIKES	= 3,
	ABILITY_MORTALSTRIKE	= 4,
	ABILITY_CLEAVE			= 5,
	ABILITY_DEATHWISH		= 6,
	ABILITY_EXECUTE			= 7,
	ABILITY_PUMMEL			= 8,
	ABILITY_SLAM			= 9,
	ABILITY_WHIRLWIND		= 10,
	ABILITY_BLOODTHIRST		= 11,
	ABILITY_REVENGE			= 12,
	ABILITY_SHIELDBLOCK		= 13,
	ABILITY_SUNDERARMOR		= 14,
	ABILITY_SHIELDSLAM		= 15,
	ABILITY_SPELL			= 16
};

const int SIZE_ABILITYMODIFIERS = 4;
enum AbilityModifier : int {
	MODIFIER_NORMALDAMAGE			= 0,
	MODIFIER_CRITICALDAMAGE			= 1,
	MODIFIER_ADDEDCRITICALCHANCE	= 2,
	MODIFIER_ADDEDDODGECHANCE		= 3
};

const int SIZE_EVENTTYPE = 60;
const double EVENT_MAXTIME = 10000000;
enum EventType : int {	// in order of priority
	// Periodic events
	EVENT_AngerManagementTick			= 0,
	EVENT_JomGabbarTick					= 1,
	EVENT_BloodrageTick					= 2,
	EVENT_CastFinished					= 3,
	EVENT_MHSwing						= 4,
	EVENT_OHSwing						= 5,
	// Cooldowns ending					= ,
	EVENT_BloodrageCooldown				= 6,
	EVENT_MightyRagePotionCooldown		= 7,
	EVENT_OverpowerCooldown				= 8,
	EVENT_MortalStrikeCooldown			= 9,
	EVENT_WhirlwindCooldown				= 10,
	EVENT_BloodthirstCooldown			= 11,
	EVENT_BerserkerRageCooldown			= 12,
	EVENT_BarovPeasantCallerCooldown	= 13,
	EVENT_EarthstrikeCooldown			= 14,
	EVENT_DiamondFlaskCooldown			= 15,
	EVENT_BadgeOfSwarmguardCooldown		= 16,
	EVENT_JomGabbarCooldown				= 17,
	EVENT_CloudkeeperCooldown			= 18,
	EVENT_KissOfTheSpiderCooldown		= 19,
	EVENT_SlayersCrestCooldown			= 20,
	EVENT_DeathWishCooldown				= 21,
	EVENT_BerserkingCooldown			= 22,
	EVENT_BloodFuryCooldown				= 23,
	// Global Cooldowns					= ,
	EVENT_StanceCooldown				= 24,
	EVENT_TrinketCooldown				= 25,
	EVENT_GlobalCooldown				= 26,
	// Fading auras						= ,
	EVENT_OverpowerWindowClosed			= 27,
	EVENT_MHCrusaderFaded				= 28,
	EVENT_OHCrusaderFaded				= 29,
	EVENT_FlurryFaded					= 30,
	EVENT_DeathWishFaded				= 31,
	EVENT_EarthStrikeFaded				= 32,
	EVENT_DiamondFlaskFaded				= 33,
	EVENT_MightyRageFaded				= 34,
	EVENT_EmpyreanDemolisherHasteFaded	= 35,
	EVENT_BadgeOfSwarmguardFaded		= 36,
	EVENT_JomGabbarFaded				= 37,
	EVENT_UntamedFuryFaded				= 38,
	EVENT_DestinyFaded					= 39,
	EVENT_EskhandarHasteFaded			= 40,
	EVENT_BonereaverFaded				= 41,
	EVENT_CloudkeeperFaded				= 42,
	EVENT_BarovPeasantsDespawned		= 43,
	EVENT_KissOfTheSpiderFaded			= 44,
	EVENT_SlayersCrestFaded				= 45,
	EVENT_BerserkingFaded				= 46,
	EVENT_ZealFaded						= 47,
	EVENT_BloodFuryFaded				= 48,
	EVENT_FelstrikerFaded				= 49,
	EVENT_MalownsSlamFaded				= 50,
	EVENT_WindfuryAPFaded				= 51,
	EVENT_NightfallFaded				= 52,
	EVENT_BloodrageFaded				= 53,
	EVENT_HarvestReaperDespawned		= 54,
	EVENT_HarvestReaperHit				= 55,
	EVENT_BarovPeasant1Hit				= 56,
	EVENT_BarovPeasant2Hit				= 57,
	EVENT_BarovPeasant3Hit				= 58,
	// dummy							= ,
	EVENT_None							= 59
};

const int SIZE_CHARACTERBUFF = 20;
enum CharacterBuff : int {
	// Consumables
	BUFF_Mongoose			= 0,
	BUFF_Dumplings			= 1,
	BUFF_JujuPower			= 2,
	BUFF_JujuMight			= 3,
	// World buffs	
	BUFF_ROIDS				= 4,
	BUFF_Scorpok			= 5,
	BUFF_Songflower			= 6,
	BUFF_Dragonslayer		= 7,
	BUFF_Warchief			= 8,
	BUFF_Zandalar			= 9,
	BUFF_Silithyst			= 10,
	BUFF_SaygeDamage		= 11,
	// Paladin	
	BUFF_BOMight			= 12,
	BUFF_BOKings			= 13,
	// Shaman	
	BUFF_StrengthOfEarth	= 14,
	BUFF_GraceOfAir			= 15,
	// Druid
	BUFF_LeaderOfPack		= 16,
	BUFF_ImprovedMOTW		= 17,
	// Hunter
	BUFF_Trueshot			= 18,
	// Warrior
	BUFF_BattleShout		= 19
};

#endif