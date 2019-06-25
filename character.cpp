#include "character.h"
#include <cstdio>

const char magicSchoolNames[SIZE_MAGICSCHOOL][10] = {
	"",
	"frost ",
	"fire ",
	"nature ",
	"arcane ",
	"shadow "
};

const char abilityNames[SIZE_ABILITYWARRIOR][20] = {
	"Hamstring",
	"Heroic Strike",
	"Overpower",
	"Sweeping Strikes",
	"Mortal Strike",
	"Cleave",
	"Death Wish",
	"Execute",
	"Pummel",
	"Slam",
	"Whirlwind",
	"Bloodthirst",
	"Revenge",
	"Shield Block",
	"Sunder Armor",
	"Shield Slam",
	"ABILITY_SPELL"
};

CharWarrior::CharWarrior(CharRace race /*= RACE_HUMAN*/, const Item ** itemArray /*= NULL*/, const char * talentBuildString /*= "2030501130200000000505000552501005100000000000000000"*/, const char * buffString /*= "11111100000000100111"*/) {
	writeLog = false;
	RNG = new std::mt19937(rd());
	maxStanceSwapConservedRage = 0;
	flurryCharges = 0;
	rageGainBerserkerRage = 0;
	castTime_Slam = 1500;
	memset(stats, 0, sizeof(stats[0]) * SIZE_STATTYPE);
	stats[STAT_HASTECOEFFICIENT] = 1.0;
	memset(addedWeaponCrit, 0, sizeof(addedWeaponCrit[0]) * SIZE_ITEMTYPE);
	for (int i = 0; i < SIZE_MAGICSCHOOL; i++)
		damageMultipliers[i] = 1.0;
	for (int i = 0; i < SIZE_ABILITYWARRIOR; i++) {
		abilityModifiers[i][MODIFIER_NORMALDAMAGE] = abilityModifiers[i][MODIFIER_CRITICALDAMAGE] = 1.0;
		abilityModifiers[i][MODIFIER_ADDEDCRITICALCHANCE] = abilityModifiers[i][MODIFIER_ADDEDDODGECHANCE] = 0;
	}
	setDefaultRageCost();
	this->race = race;
	addRacialStatsAndSpells();
	std::unordered_map< SetBonus, int, std::hash<int> > bonuses;
	numOnUseTrinkets = 0;
	for (int equipmentSlot = 0; equipmentSlot < SIZE_INVENTORYSLOT; equipmentSlot++) {
		const Item * pItem = itemArray[equipmentSlot];
		// if (pItem == &items["None"]) {
		// 	fprintf(stderr, "Item in slot %d not found.\n", equipmentSlot);
		// 	continue;
		// }
		// Sum up stats
		MH.weaponDamage[0] += pItem->itemStats[item_weaponAllDamage];
		MH.weaponDamage[1] += pItem->itemStats[item_weaponAllDamage];
		MH.elementalDamage[0] += pItem->itemStats[item_elementalAllDamage];
		MH.elementalDamage[1] += pItem->itemStats[item_elementalAllDamage];
		OH.weaponDamage[0] += pItem->itemStats[item_weaponAllDamage];
		OH.weaponDamage[1] += pItem->itemStats[item_weaponAllDamage];
		OH.elementalDamage[0] += pItem->itemStats[item_elementalAllDamage];
		OH.elementalDamage[1] += pItem->itemStats[item_elementalAllDamage];
		stats[STAT_STRENGTH] += pItem->itemStats[item_strength];
		stats[STAT_AGILITY] += pItem->itemStats[item_agility];
		stats[STAT_MELEEHIT] += 100 * pItem->itemStats[item_meleeHit];
		stats[STAT_MELEECRIT] += 100 * pItem->itemStats[item_meleeCrit];
		stats[STAT_SPELLHIT] += 100 * pItem->itemStats[item_spellHit];
		stats[STAT_SPELLCRIT] += 100 * pItem->itemStats[item_spellCrit];
		stats[STAT_AP] += pItem->itemStats[item_attackPower];
		stats[STAT_SP] += pItem->itemStats[item_spellPower];
		stats[STAT_1HSWORDS] += pItem->itemStats[item_weaponSkillSwords];
		stats[STAT_2HSWORDS] += pItem->itemStats[item_weaponSkill2hSwords];
		stats[STAT_1HMACES] += pItem->itemStats[item_weaponSkillMaces];
		stats[STAT_2HMACES] += pItem->itemStats[item_weaponSkill2hMaces];
		stats[STAT_1HAXES] += pItem->itemStats[item_weaponSkillAxes];
		stats[STAT_2HAXES] += pItem->itemStats[item_weaponSkill2hAxes];
		stats[STAT_DAGGERS] += pItem->itemStats[item_weaponSkillDaggers];
		stats[STAT_FISTS] += pItem->itemStats[item_weaponSkillFists];
		stats[STAT_STAVES] += pItem->itemStats[item_weaponSkillStaves];
		stats[STAT_POLEARMS] += pItem->itemStats[item_weaponSkillPolearms];
		stats[STAT_HASTECOEFFICIENT] *= 1 + pItem->itemStats[item_haste] / 100.0;
		// Note down the bonus count
		bonuses[ pItem->setBonus ]++;
		// Add procs
		if (equipmentSlot == SLOT_MH && pItem->proc != PROC_WEAPON_SERVOARM) // Unless it's servoarm, all weapon procs apply only to the current weapon
			MHProcs.insert(pItem->proc);
		else if (equipmentSlot == SLOT_OH && pItem->proc != PROC_WEAPON_SERVOARM) // Unless it's servoarm, all weapon procs apply only to the current weapon
			OHProcs.insert(pItem->proc);
		else if ((equipmentSlot == SLOT_MH_ENCHANTMENT || equipmentSlot == SLOT_MH_ENCHANTMENT_TEMP) && pItem->proc != PROC_NONE)
			MHProcs.insert(pItem->proc);
		else if ((equipmentSlot == SLOT_OH_ENCHANTMENT || equipmentSlot == SLOT_OH_ENCHANTMENT_TEMP) && pItem->proc != PROC_NONE)
			OHProcs.insert(pItem->proc);
		else {	// All other procs count for both weapons
			MHProcs.insert(pItem->proc);
			OHProcs.insert(pItem->proc);
		}
		// if (pItem->proc == PROC_ITEM_MAELSTROM) {
		// 	fprintf(stderr, "%d has maelstrom proc on it\n", equipmentSlot);
		// }
		// Add use effects
		useEffects.insert(pItem->use);
		if ((equipmentSlot == SLOT_TRINKET1 || equipmentSlot == SLOT_TRINKET2) && pItem->use != USE_NONE)
			numOnUseTrinkets++;
		// Register weapons
		if (equipmentSlot == SLOT_MH || equipmentSlot == SLOT_OH) {
			CharWeapon * pWeapon = ((equipmentSlot == SLOT_MH) ? &MH : &OH);
			pWeapon->type = pItem->type;
			pWeapon->baseWeaponSpeed = pItem->weaponSpeed;
			pWeapon->weaponDamage[0] += pItem->itemStats[item_damageLower];
			pWeapon->weaponDamage[1] += pItem->itemStats[item_damageUpper];
			pWeapon->elementalDamage[0] += pItem->itemStats[item_elementalLower];
			pWeapon->elementalDamage[1] += pItem->itemStats[item_elementalUpper];
			pWeapon->elementalSchool = (MagicSchool)pItem->itemStats[item_elementalSchool];
		}
		// Weapon damage adding (temporary) enchantments
		if (equipmentSlot == SLOT_MH_ENCHANTMENT || equipmentSlot == SLOT_MH_ENCHANTMENT_TEMP || equipmentSlot == SLOT_OH_ENCHANTMENT || equipmentSlot == SLOT_OH_ENCHANTMENT_TEMP) {
			CharWeapon * pWeapon = ((equipmentSlot == SLOT_MH_ENCHANTMENT || equipmentSlot == SLOT_MH_ENCHANTMENT_TEMP) ? &MH : &OH);
			pWeapon->weaponDamage[0] += pItem->itemStats[item_damageLower];
			pWeapon->weaponDamage[1] += pItem->itemStats[item_damageUpper];
		}
	}
	// We use 50ms as the default latency value
	defaultLatency = 50;
	// Global cooldown for warriors is 1.5 seconds
	globalCooldownDuration = 1500;
	// Calculate set bonuses
	addSetBonuses(bonuses);
	// Cleaning up dummy proc chances and use effects
	MHProcs.erase(PROC_NONE);
	OHProcs.erase(PROC_NONE);
	useEffects.erase(USE_NONE);
	// Build a priority queue on events
	for (int i = 0; i < SIZE_EVENTTYPE; i++) {
		events[i] = CharacterEvent((EventType)i, EVENT_MAXTIME);
	}
	eventQueue = new Heap<CharacterEvent>(events, SIZE_EVENTTYPE);
	// Initialize the state of the character
	inBattleStance = false;
	resetCharacter();
	// Off-hand has 50% damage modifier by default, if we have an off-hand weapon equipped
	if (OH.type != ITEM_NONE)
		OH.damageMultiplier = 0.5;
	// Modify the character based on talents
	buildTalentTree(talentBuildString);
	// Every warrior has Bloodrage, and Might Rage Potion
	useEffects.insert(USE_SPELL_BLOODRAGE);
	useEffects.insert(USE_ITEM_MIGHTYRAGE);
	// All stats normally have 1.0 multiplier (modified by kings and zandalar)
	stats[STAT_STATMULTIPLIER] = 1.0;
	// Damage multiplier is normally 1.0 (modified by sayge, silithyst etc)
	stats[STAT_DAMAGEMULTIPLIER] = 1.0;
	// Apply character buffs
	addBuffs(buffString);
	// Add 160 base AP for level 60 (3*level - 20)
	stats[STAT_AP] += 160;
	// Calculate weapon skill contribution to combat tables
	switch (MH.type) {
		case ITEM_SWORD: MH.skill = stats[STAT_1HSWORDS]; break;
		case ITEM_MACE: MH.skill = stats[STAT_1HMACES]; break;
		case ITEM_AXE: MH.skill = stats[STAT_1HAXES]; break;
		case ITEM_2HSWORD: MH.skill = stats[STAT_2HSWORDS]; break;
		case ITEM_2HMACE: MH.skill = stats[STAT_2HMACES]; break;
		case ITEM_2HAXE: MH.skill = stats[STAT_2HAXES]; break;
		case ITEM_DAGGER: MH.skill = stats[STAT_DAGGERS]; break;
		case ITEM_FIST: MH.skill = stats[STAT_FISTS]; break;
	}
	switch (OH.type) {
		case ITEM_SWORD: OH.skill = stats[STAT_1HSWORDS]; break;
		case ITEM_MACE: OH.skill = stats[STAT_1HMACES]; break;
		case ITEM_AXE: OH.skill = stats[STAT_1HAXES]; break;
		case ITEM_2HSWORD: OH.skill = stats[STAT_2HSWORDS]; break;
		case ITEM_2HMACE: OH.skill = stats[STAT_2HMACES]; break;
		case ITEM_2HAXE: OH.skill = stats[STAT_2HAXES]; break;
		case ITEM_DAGGER: OH.skill = stats[STAT_DAGGERS]; break;
		case ITEM_FIST: OH.skill = stats[STAT_FISTS]; break;
	}
	MH.skill += 300;
	OH.skill += 300;
	int MHSkillDifference = 315 - MH.skill;
	int OHSkillDifference = 315 - OH.skill;
	// Combat table
	// Dodge
	MH.dodge = 500 + 10 * MHSkillDifference;
	OH.dodge = 500 + 10 * OHSkillDifference;
	// Hit and crit are calculated in CharWarrior:recalculateDamage()
	// Weapon speed
	recalculateSpeed();
	// Overpower cannot be dodged
	abilityModifiers[ABILITY_OVERPOWER][MODIFIER_ADDEDDODGECHANCE] -= 10000;
	// Assume that we are in berserker stance for 3% extra crit
	stats[STAT_MELEECRIT] += 300;
	// Calculate damage
	recalculateDamage();
	// for (ProcType proc : MHProcs) {
	// 	fprintf(stderr, "%d ", (int)proc);
	// }
	// fprintf(stderr, "\n");
	// for (ProcType proc : OHProcs) {
	// 	fprintf(stderr, "%d ", (int)proc);
	// }
	// fprintf(stderr, "\n");
}

void CharWarrior::addRage(const int val) {
	if (rage + val > 100)
		rageOverflow += rage + val - 100;
	rage = std::min(100, rage + val);
}

RollResult CharWarrior::rollSwingMainHand() {
	// If OH is equipped, miss chance for white swing is increased by 19% for main hand
	int miss = std::max(0, std::min(10000, MH.miss + ((OH.type != ITEM_NONE) ? 1900 : 0)));
	int dodge = std::max(0, std::min(10000, MH.dodge));
	int remaining = 10000 - miss - dodge - 4000;
	int critical = std::max(0, std::min(remaining, MH.critical));
	std::uniform_int_distribution<int> uni(1, 10000);
	int roll = uni(*RNG);
	if (roll <= miss) return ROLL_MISS;
	else roll -= miss;
	if (roll <= dodge) return ROLL_DODGE;
	else roll -= dodge;
	if (roll <= 4000) return ROLL_GLANCING;
	else roll -= 4000;
	if (roll <= critical) return ROLL_CRITICAL;
	else return ROLL_NORMAL;
}

RollResult CharWarrior::rollSwingOffHand() {
	int miss = std::max(0, std::min(10000, OH.miss));
	int dodge = std::max(0, std::min(10000, OH.dodge));
	int remaining = 10000 - miss - dodge - 4000;
	int critical = std::max(0, std::min(remaining, OH.critical));
	std::uniform_int_distribution<int> uni(1, 10000);
	int roll = uni(*RNG);
	if (roll <= miss) return ROLL_MISS;
	else roll -= miss;
	if (roll <= dodge) return ROLL_DODGE;
	else roll -= dodge;
	if (roll <= 4000) return ROLL_GLANCING;
	else roll -= 4000;
	if (roll <= critical) return ROLL_CRITICAL;
	else return ROLL_NORMAL;
}

RollResult CharWarrior::rollYellow(AbilityWarrior abilityType) {
	if (abilityType == ABILITY_SPELL) {
		// Spells have between 1% and 17% miss, cannot be dodged and have 0 base critical hit chance.
		int miss = std::min(0, std::max(10000, 1700 - (int)stats[STAT_SPELLHIT]));
		int critical = std::max(0, std::min(10000 - miss, (int)stats[STAT_SPELLCRIT]));
		std::uniform_int_distribution<int> uni(1, 10000);
		int roll = uni(*RNG);
		if (roll <= miss) return ROLL_MISS;
		else roll -= miss;
		if (roll <= critical) return ROLL_CRITICAL;
		else return ROLL_NORMAL;
	}
	else {
		int miss = std::max(0, std::min(10000, MH.miss));
		int dodge = std::max(0, std::min(10000, MH.dodge + (int)abilityModifiers[abilityType][MODIFIER_ADDEDDODGECHANCE]));
		// No glancing blows
		int remaining = 10000 - miss - dodge;
		int critical = std::max(0, std::min(remaining, MH.critical + (int)abilityModifiers[abilityType][MODIFIER_ADDEDCRITICALCHANCE]));
		std::uniform_int_distribution<int> uni(1, 10000);
		int roll = uni(*RNG);
		if (roll <= miss) return ROLL_MISS;
		else roll -= miss;
		if (roll <= dodge) return ROLL_DODGE;
		else roll -= dodge;
		if (roll <= critical) return ROLL_CRITICAL;
		else return ROLL_NORMAL;
	}
}

void CharWarrior::recalculateDamage() {
	// Main hand
	int currentAP = stats[STAT_AP] + 2 * stats[STAT_STRENGTH] * stats[STAT_STATMULTIPLIER];
	MH.whiteDamage[0] = (MH.weaponDamage[0] + currentAP / 14.0 * MH.baseWeaponSpeed / 1000.0) * MH.damageMultiplier;
	MH.whiteDamage[1] = (MH.weaponDamage[1] + currentAP / 14.0 * MH.baseWeaponSpeed / 1000.0) * MH.damageMultiplier;
	int normalizedSpeed = 2400;
	if (MH.type == ITEM_2HSWORD || MH.type == ITEM_2HMACE || MH.type == ITEM_2HAXE)
		normalizedSpeed = 3300;
	else if (MH.type == ITEM_DAGGER)
		normalizedSpeed = 1700;
	MH.normalizedDamage[0] = MH.weaponDamage[0] + currentAP / 14.0 * normalizedSpeed / 1000.0;
	MH.normalizedDamage[1] = MH.weaponDamage[1] + currentAP / 14.0 * normalizedSpeed / 1000.0;
	int MHSkillDifference = 315 - MH.skill;
	MH.miss = 500 - (int)stats[STAT_MELEEHIT] + ((MHSkillDifference > 10) ? 20 * MHSkillDifference : 10 * MHSkillDifference);
	MH.critical = 500 - 4 * MHSkillDifference + stats[STAT_MELEECRIT] + /* 100/20.0 */ 5 * (stats[STAT_AGILITY] * stats[STAT_STATMULTIPLIER]) + addedWeaponCrit[MH.type];
	// Off-hand
	if (OH.type != ITEM_NONE) {
		int OHSkillDifference = 315 - OH.skill;
		OH.whiteDamage[0] = (OH.weaponDamage[0] + currentAP / 14.0 * OH.baseWeaponSpeed / 1000.0) * OH.damageMultiplier;
		OH.whiteDamage[1] = (OH.weaponDamage[1] + currentAP / 14.0 * OH.baseWeaponSpeed / 1000.0) * OH.damageMultiplier;
		OH.miss = 500 - stats[STAT_MELEEHIT] + ((OHSkillDifference > 10) ? 20 * OHSkillDifference : 10 * OHSkillDifference);
		OH.critical = 500 - 4 * OHSkillDifference + stats[STAT_MELEECRIT] + /* 100/20.0 */ 5 *(stats[STAT_AGILITY] * stats[STAT_STATMULTIPLIER]) + addedWeaponCrit[OH.type];
	}
}

void CharWarrior::recalculateSpeed() {
	double oldMHSpeed = MH.speed;
	MH.speed = MH.baseWeaponSpeed / stats[STAT_HASTECOEFFICIENT];
	OH.speed = OH.baseWeaponSpeed / stats[STAT_HASTECOEFFICIENT];
	double boost = oldMHSpeed / MH.speed;
	double MHSwingTimer = getEventCooldown(EVENT_MHSwing);
	double OHSwingTimer = getEventCooldown(EVENT_OHSwing);
	setEventCooldownFromNow(EVENT_MHSwing, MHSwingTimer / boost);
	setEventCooldownFromNow(EVENT_OHSwing, OHSwingTimer / boost);
}

void CharWarrior::addBuffs(const char * buffString /*= NULL*/) {
	if (buffString == NULL)
		return;
	stats[STAT_AGILITY] += 25 * (buffString[BUFF_Mongoose] - '0');
	stats[STAT_MELEECRIT] += 200 * (buffString[BUFF_Mongoose] - '0');
	stats[STAT_STRENGTH] += 20 * (buffString[BUFF_Dumplings] - '0');
	stats[STAT_STRENGTH] += 30 * (buffString[BUFF_JujuPower] - '0');
	stats[STAT_AP] += 40 * (buffString[BUFF_JujuMight] - '0');
	stats[STAT_STRENGTH] += 25 * (buffString[BUFF_ROIDS] - '0');
	stats[STAT_AGILITY] += 25 * (buffString[BUFF_Scorpok] - '0');
	stats[STAT_STRENGTH] += 15 * (buffString[BUFF_Songflower] - '0');
	stats[STAT_AGILITY] += 15 * (buffString[BUFF_Songflower] - '0');
	stats[STAT_MELEECRIT] += 500 * (buffString[BUFF_Songflower] - '0');
	stats[STAT_SPELLCRIT] += 500 * (buffString[BUFF_Songflower] - '0');
	stats[STAT_AP] += 140 * (buffString[BUFF_Dragonslayer] - '0');
	stats[STAT_MELEECRIT] += 500 * (buffString[BUFF_Dragonslayer] - '0');
	stats[STAT_SPELLCRIT] += 1000 * (buffString[BUFF_Dragonslayer] - '0');
	stats[STAT_HASTECOEFFICIENT] *= 1 + 0.15 * (buffString[BUFF_Warchief] - '0');
	stats[STAT_STATMULTIPLIER] *= 1 + 0.15 * (buffString[BUFF_Zandalar] - '0');
	stats[STAT_DAMAGEMULTIPLIER] *= 1 + 0.05 * (buffString[BUFF_Silithyst] - '0');
	stats[STAT_DAMAGEMULTIPLIER] *= 1 + 0.1 * (buffString[BUFF_SaygeDamage] - '0');
	// AQ ranks
	// stats[STAT_AP] += 222 * (buffString[BUFF_BOMight] - '0');	// AQ rank with 5/5 improved blessing of might
	// stats[STAT_STRENGTH] += 88.55 * (buffString[BUFF_StrengthOfEarth] - '0');	// AQ rank with 2/2 improved talent
	// stats[STAT_AGILITY] += 88.55 * (buffString[BUFF_GraceOfAir] - '0');	// AQ rank with 2/2 improved talent
	// stats[STAT_AP] += 290 * (buffString[BUFF_BattleShout] - '0');	// AQ rank with 5/5 improved battleshout
	
	// // Non-AQ ranks
	stats[STAT_AP] += 186 * (buffString[BUFF_BOMight] - '0');	// Non-AQ rank with 5/5 improved blessing of might
	stats[STAT_STRENGTH] += 70.15 * (buffString[BUFF_StrengthOfEarth] - '0');	// Non-AQ rank with 2/2 improved talent
	stats[STAT_AGILITY] += 77.05 * (buffString[BUFF_GraceOfAir] - '0');	// Non-AQ rank with 2/2 improved talent
	stats[STAT_AP] += 231.25 * (buffString[BUFF_BattleShout] - '0');	// Non-AQ rank with 5/5 improved battleshout

	stats[STAT_STATMULTIPLIER] *= 1 + 0.1 * (buffString[BUFF_BOKings] - '0');
	stats[STAT_MELEECRIT] += 300 * (buffString[BUFF_LeaderOfPack] - '0');
	stats[STAT_STRENGTH] += 16.2 * (buffString[BUFF_ImprovedMOTW] - '0');
	stats[STAT_AGILITY] += 16.2 * (buffString[BUFF_ImprovedMOTW] - '0');
	stats[STAT_AP] += 100 * (buffString[BUFF_Trueshot] - '0');
}

void CharWarrior::buildTalentTree(const char * talentBuildString /*= NULL*/) {
	if (talentBuildString == NULL)
		return;
	abilityCost[ABILITY_HEROICSTRIKE] -= talentBuildString[TALENT_IMPROVEDHEROICSTRIKE] - '0';
	maxStanceSwapConservedRage += 5 * (talentBuildString[TALENT_TACTICALMASTERY] - '0');
	abilityModifiers[ABILITY_OVERPOWER][MODIFIER_ADDEDCRITICALCHANCE] += 2500 * (talentBuildString[TALENT_IMPROVEDOVERPOWER] - '0');
	if (talentBuildString[TALENT_ANGERMANAGEMENT] == '1') {	// The first tick may happen randomly up to 3 seconds from the start of the fight
		std::uniform_int_distribution<int> uni(0, 3000);
		int cd = uni(*RNG);
		setEventCooldownFromNow(EVENT_AngerManagementTick, cd);
	}
	if (MH.type == ITEM_2HSWORD || MH.type == ITEM_2HMACE || MH.type == ITEM_2HAXE)
		damageMultipliers[SCHOOL_PHYSICAL] *= 1 + 0.01 * (talentBuildString[TALENT_2HWEAPONSPECIALIZATION] - '0');
	if (talentBuildString[TALENT_IMPALE] > '0') {
		for (int i = 0; i < SIZE_ABILITYWARRIOR; i++)
			abilityModifiers[i][MODIFIER_CRITICALDAMAGE] *= 1 + 0.1 * (talentBuildString[TALENT_IMPALE] - '0');
	}
	addedWeaponCrit[ITEM_AXE] += 100 * (talentBuildString[TALENT_AXESPECIALIZATION] - '0');
	addedWeaponCrit[ITEM_2HAXE] += 100 * (talentBuildString[TALENT_AXESPECIALIZATION] - '0');
	if (talentBuildString[TALENT_SWORDSPECIALIZATION] > '0') {	// Adding full 5 points for the sake of simplicity
		if (MH.type == ITEM_SWORD || MH.type == ITEM_2HSWORD)
			MHProcs.insert(PROC_TALENT_SWORDSPECIALIZATION);
		if (OH.type == ITEM_SWORD)
			OHProcs.insert(PROC_TALENT_SWORDSPECIALIZATION);
	}
	if (talentBuildString[TALENT_MORTALSTRIKE] > '0')
		abilityCost[ABILITY_MORTALSTRIKE] = 30;
	else
		abilityCost[ABILITY_MORTALSTRIKE] = 999;
	stats[STAT_MELEECRIT] += 100 * (talentBuildString[TALENT_CRUELTY] - '0');
	if (talentBuildString[TALENT_UNBRIDLEDWRATH] > '0') {	// Adding full 5 points for the sake of simplicity
		MHProcs.insert(PROC_TALENT_UNBRIDLEDWRATH);
		OHProcs.insert(PROC_TALENT_UNBRIDLEDWRATH);
	}
	OH.damageMultiplier *= 1 + 0.05 * (talentBuildString[TALENT_DUALWIELDSPECIALIZATION] - '0');
	abilityCost[ABILITY_EXECUTE] -= int(2.5 * (talentBuildString[TALENT_IMPROVEDEXECUTE] - '0'));
	castTime_Slam -= 100 * (talentBuildString[TALENT_IMPROVEDSLAM] - '0');
	if (talentBuildString[TALENT_DEATHWISH] > '0') {
		useEffects.insert(USE_SPELL_DEATHWISH);
	}
	if (talentBuildString[TALENT_IMPROVEDBERSERKERRAGE] > '0') {
		useEffects.insert(USE_SPELL_BERSERKERRAGE);
		rageGainBerserkerRage += 5 * (talentBuildString[TALENT_TACTICALMASTERY] - '0');
		setEventReady(EVENT_BerserkerRageCooldown);
	}
	if (talentBuildString[TALENT_FLURRY] > '0') {	// Adding full 5 points for the sake of simplicity
		MHProcs.insert(PROC_TALENT_FLURRY);
		OHProcs.insert(PROC_TALENT_FLURRY);
	}
	if (talentBuildString[TALENT_BLOODTHIRST] > '0')
		abilityCost[ABILITY_BLOODTHIRST] = 30;
	else
		abilityCost[ABILITY_BLOODTHIRST] = 999;
	// Not going to bother with protection tree talents for now
}

EventType CharWarrior::getNextEventAndUpdateTime() {
	int next = eventQueue->getMinElementArrayIndex();
	currentTime = events[next].timestamp;
	return events[next].type;
}

bool CharWarrior::isEventScheduled(EventType event) {
	// if (event == EVENT_GlobalCooldown)
	// 	fprintf(stderr, "GCD time: %.3lf\n", events[event].timestamp / 1000.0);
	return events[event].timestamp < EVENT_MAXTIME;
}

int CharWarrior::getEventCooldown(EventType event) {
	return events[event].timestamp - currentTime;
}

void CharWarrior::setEventReady(EventType event) {
	eventQueue->change_val(event, CharacterEvent(event, currentTime));
}

void CharWarrior::cancelEvent(EventType event) {
	eventQueue->change_val(event, CharacterEvent(event, EVENT_MAXTIME));
}

void CharWarrior::setEventCooldownFromNow(EventType event, int cooldownInMiliseconds) {
	// if (writeLog && event == EVENT_CastFinished)
	// 	fprintf(stderr, "Slam cooldown that was given is: %d\n", cooldownInMiliseconds);
	eventQueue->change_val(event, CharacterEvent(event, currentTime + cooldownInMiliseconds));
	// if (event == EVENT_GlobalCooldown)
	// 	fprintf(stderr, "GCD time after change: %.3lf\n", events[event].timestamp / 1000.0);
}

void CharWarrior::setDefaultRageCost() {
	abilityCost[ABILITY_HAMSTRING] = 10;
	abilityCost[ABILITY_HEROICSTRIKE] = 15;
	abilityCost[ABILITY_OVERPOWER] = 5;
	abilityCost[ABILITY_SWEEPINGSTRIKES] = 30;
	abilityCost[ABILITY_MORTALSTRIKE] = 30;
	abilityCost[ABILITY_CLEAVE] = 20;
	abilityCost[ABILITY_DEATHWISH] = 10;
	abilityCost[ABILITY_EXECUTE] = 15;
	abilityCost[ABILITY_PUMMEL] = 10;
	abilityCost[ABILITY_SLAM] = 15;
	abilityCost[ABILITY_WHIRLWIND] = 25;
	abilityCost[ABILITY_BLOODTHIRST] = 30;
	abilityCost[ABILITY_REVENGE] = 5;
	abilityCost[ABILITY_SUNDERARMOR] = 15;
	abilityCost[ABILITY_SHIELDBLOCK] = 10;
	abilityCost[ABILITY_SHIELDSLAM] = 20;
}

void CharWarrior::addSetBonuses(std::unordered_map< SetBonus, int, std::hash<int> > & bonuses) {
	for (auto it : bonuses) {
		SetBonus bonus = it.first;
		int itemCount = it.second;
		switch (bonus) {
			case SET_DALRENDSARMS:	// +50 AP with 2
				if (writeLog)
					fprintf(stderr, "2-set Dal'Rend\n");
				if (itemCount >= 2)
					stats[STAT_AP] += 50;
				break;
			case SET_HAKKARI_BLADES:	// +6 swords skill with 2
				if (writeLog)
					fprintf(stderr, "2-set Hakkari Warblades\n");
				if (itemCount >= 2)
					stats[STAT_1HSWORDS] += 6;
				break;
			case SET_DEFILERS:	// +1% crit with 3
				if (writeLog)
					fprintf(stderr, "3-set Defilers\n");
				if (itemCount >= 3)
					stats[STAT_MELEECRIT] += 100;
				break;
			case SET_CHAMPION:	// 40 AP with 2
				if (writeLog)
					fprintf(stderr, "2-set r10 gear\n");
				if (itemCount >= 2)
					stats[STAT_AP] += 40;
				break;
			case SET_WARLORD:	// 40 AP with 6
				if (writeLog)
					fprintf(stderr, "6-set r13 gear\n");
				if (itemCount >= 6)
					stats[STAT_AP] += 40;
				break;
			case SET_DEVILSAUR:	// 2% hit with 2
				if (writeLog)
					fprintf(stderr, "2-set Devilsaur\n");
				if (itemCount >= 2)
					stats[STAT_MELEEHIT] += 200;
				break;
		}
	}
}

void CharWarrior::addRacialStatsAndSpells() {
	if (race == RACE_HUMAN) {
		stats[STAT_STRENGTH] += 120;
		stats[STAT_AGILITY] += 80;
		stats[STAT_1HSWORDS] += 5;
		stats[STAT_1HMACES] += 5;
		stats[STAT_2HSWORDS] += 5;
		stats[STAT_2HMACES] += 5;
	}
	else if (race == RACE_DWARF) {
		stats[STAT_STRENGTH] += 122;
		stats[STAT_AGILITY] += 76;
	}
	else if (race == RACE_NIGHTELF) {
		stats[STAT_STRENGTH] += 117;
		stats[STAT_AGILITY] += 85;
	}
	else if (race == RACE_GNOME) {
		stats[STAT_STRENGTH] += 115;
		stats[STAT_AGILITY] += 83;
	}
	else if (race == RACE_ORC) {
		stats[STAT_STRENGTH] += 123;
		stats[STAT_AGILITY] += 77;
		stats[STAT_1HAXES] += 5;
		stats[STAT_2HAXES] += 5;
		useEffects.insert(USE_SPELL_BLOODFURY);
	}
	else if (race == RACE_UNDEAD) {
		stats[STAT_STRENGTH] += 119;
		stats[STAT_AGILITY] += 78;
	}
	else if (race == RACE_TAUREN) {
		stats[STAT_STRENGTH] += 125;
		stats[STAT_AGILITY] += 75;
	}
	else if (race == RACE_TROLL) {
		stats[STAT_STRENGTH] += 121;
		stats[STAT_AGILITY] += 82;
		useEffects.insert(USE_SPELL_BERSERKING);
	}
}

void CharWarrior::reduceFlurryCharges() {
	if (flurryCharges > 1)
		flurryCharges--;
	else if (flurryCharges == 1) {
		handleEvent(EVENT_FlurryFaded);
	}
}

int CharWarrior::sampleAbilityDamage(AbilityWarrior abilityType, RollResult roll) {
	int minVal = 0;
	int maxVal = 0;
	switch (abilityType) {
		case ABILITY_HAMSTRING:
			minVal = 45;
			maxVal = 45;
			break;
		case ABILITY_HEROICSTRIKE:
			minVal = MH.whiteDamage[0] + 138;	// Non-AQ rank
			maxVal = MH.whiteDamage[1] + 138;
			// minVal = MH.whiteDamage[0] + 157;	// AQ rank
			// maxVal = MH.whiteDamage[1] + 157;
			break;
		case ABILITY_OVERPOWER:
			minVal = MH.normalizedDamage[0] + 35;
			maxVal = MH.normalizedDamage[1] + 35;
			break;
		case ABILITY_MORTALSTRIKE:
			minVal = MH.normalizedDamage[0] + 160;
			maxVal = MH.normalizedDamage[1] + 160;
			break;
		case ABILITY_CLEAVE:
			minVal = MH.whiteDamage[0] + 50;
			maxVal = MH.whiteDamage[1] + 50;
			break;
		case ABILITY_EXECUTE:
			minVal = 600 + 15 * rage;
			maxVal = 600 + 15 * rage;
			rage = 0;
			break;
		case ABILITY_PUMMEL:
			minVal = 50;
			maxVal = 50;
			break;
		case ABILITY_SLAM:
			minVal = MH.whiteDamage[0] + 87;
			maxVal = MH.whiteDamage[1] + 87;
			break;
		case ABILITY_WHIRLWIND:
			minVal = MH.normalizedDamage[0];
			maxVal = MH.normalizedDamage[1];
			break;
		case ABILITY_BLOODTHIRST:
			minVal = 0.45 * (stats[STAT_AP] + 2 * stats[STAT_STRENGTH] * stats[STAT_STATMULTIPLIER]);
			maxVal = 0.45 * (stats[STAT_AP] + 2 * stats[STAT_STRENGTH] * stats[STAT_STATMULTIPLIER]);
			break;
	}
	std::uniform_int_distribution<int> uni(minVal, maxVal);
	int damage = uni(*RNG) * abilityModifiers[abilityType][MODIFIER_NORMALDAMAGE] * damageMultipliers[SCHOOL_PHYSICAL];
	if (roll == ROLL_CRITICAL)
		damage += damage * abilityModifiers[abilityType][MODIFIER_CRITICALDAMAGE];
	return damage;
}

void CharWarrior::delayUntilEvent(EventType event, EventType goal) {
	setEventCooldownFromNow(event, getEventCooldown(goal));
}

void CharWarrior::handleEvent(EventType event) {
	cancelEvent(event);	// Turn every event off by default. Repeatable events will get registered again below.
	if (event == EVENT_AngerManagementTick) {
		addRage(1);
		setEventCooldownFromNow(event, 3000);
		if (writeLog)
			fprintf(stderr, "%.3lf\tYou gain 1 rage from Anger Management.\n", currentTime / 1000.0);
		// This event may result in a user action; so if GCD is ready, we poke the rotation manager.
		if (!isEventScheduled(EVENT_GlobalCooldown))	
			setEventCooldownFromNow(EVENT_GlobalCooldown, defaultLatency);
	}
	else if (event == EVENT_JomGabbarTick) {
		stats[STAT_AP] += 65;
		jomGabbarAddedAP += 65;
		recalculateDamage();
		setEventCooldownFromNow(event, 2000);
	}
	else if (event == EVENT_CastFinished) {
		doYellowAttack(spellBeingCast);
		// This event may result in a user action; so if GCD is ready, we poke the rotation manager.
		if (!isEventScheduled(EVENT_GlobalCooldown))	
			setEventCooldownFromNow(EVENT_GlobalCooldown, defaultLatency);
	}
	else if (event == EVENT_MHSwing) {
		if (isEventScheduled(EVENT_CastFinished))
			delayUntilEvent(event, EVENT_CastFinished);
		else {
			doMainHandAttack(false);
			setEventCooldownFromNow(event, MH.speed);
			if (getEventCooldown(EVENT_OHSwing) < 200)
				setEventCooldownFromNow(EVENT_OHSwing, 200);
			// This event may result in a user action; so if GCD is ready, we poke the rotation manager.
			if (!isEventScheduled(EVENT_GlobalCooldown))
				setEventCooldownFromNow(EVENT_GlobalCooldown, defaultLatency);
		}
	}
	else if (event == EVENT_OHSwing) {
		if (isEventScheduled(EVENT_CastFinished))
			delayUntilEvent(event, EVENT_CastFinished);
		else {
			doOffHandAttack();
			setEventCooldownFromNow(event, OH.speed);
			if (getEventCooldown(EVENT_MHSwing) < 200)
				setEventCooldownFromNow(EVENT_MHSwing, 200);
			// This event may result in a user action; so if GCD is ready, we poke the rotation manager.
			if (!isEventScheduled(EVENT_GlobalCooldown))	
				setEventCooldownFromNow(EVENT_GlobalCooldown, defaultLatency);
		}
	}
	else if (event == EVENT_MHCrusaderFaded || event == EVENT_OHCrusaderFaded) {
		stats[STAT_STRENGTH] -= 100;
		recalculateDamage();
		if (writeLog)
			fprintf(stderr, "%.3lf\tHoly Strength from %s Hand fades from you.\n", currentTime / 1000.0, ((event == EVENT_MHCrusaderFaded) ? "Main" : "Off"));
		auraUptime[((event == EVENT_MHCrusaderFaded) ? "Crusader MH" : "Crusader OH")] += currentTime - auraStartLast[((event == EVENT_MHCrusaderFaded) ? "Crusader MH" : "Crusader OH")];
		auraStartLast.erase(((event == EVENT_MHCrusaderFaded) ? "Crusader MH" : "Crusader OH"));
	}
	else if (event == EVENT_FlurryFaded || event == EVENT_EskhandarHasteFaded) {
		flurryCharges = 0;
		stats[STAT_HASTECOEFFICIENT] /= 1.3;
		recalculateSpeed();
		if (writeLog)
			fprintf(stderr, "%.3lf\t%s fades from you.\n", currentTime / 1000.0, ((event == EVENT_FlurryFaded) ? "Flurry" : "Eskhandar's Rage"));
		auraUptime[((event == EVENT_FlurryFaded) ? "Flurry" : "Eskhandar's Rage")] += currentTime - auraStartLast[((event == EVENT_FlurryFaded) ? "Flurry" : "Eskhandar's Rage")];
		auraStartLast.erase(((event == EVENT_FlurryFaded) ? "Flurry" : "Eskhandar's Rage"));
	}
	else if (event == EVENT_DeathWishFaded) {
		damageMultipliers[SCHOOL_PHYSICAL] /= 1.2;
		recalculateDamage();
		if (writeLog)
			fprintf(stderr, "%.3lf\tDeath Wish fades from you.\n", currentTime / 1000.0);
	}
	else if (event == EVENT_EarthStrikeFaded) {
		stats[STAT_AP] -= 280;
		recalculateDamage();
		if (writeLog)
			fprintf(stderr, "%.3lf\tEarthstrike fades from you.\n", currentTime / 1000.0);
	}
	else if (event == EVENT_DiamondFlaskFaded) {
		stats[STAT_STRENGTH] -= 75;
		recalculateDamage();
		if (writeLog)
			fprintf(stderr, "%.3lf\tDiamond Flask fades from you.\n", currentTime / 1000.0);
	}
	else if (event == EVENT_MightyRageFaded) {
		stats[STAT_STRENGTH] -= 60;
		recalculateDamage();
		if (writeLog)
			fprintf(stderr, "%.3lf\tMighty Rage fades from you.\n", currentTime / 1000.0);
	}
	else if (event == EVENT_EmpyreanDemolisherHasteFaded || event == EVENT_KissOfTheSpiderFaded) {
		stats[STAT_HASTECOEFFICIENT] /= 1.2;
		recalculateSpeed();
		if (writeLog)
			fprintf(stderr, "%.3lf\t%s fades from you.\n", currentTime / 1000.0, ((event == EVENT_EmpyreanDemolisherHasteFaded) ? "Haste" : "Kiss of the Spider"));
		if (event == EVENT_EmpyreanDemolisherHasteFaded) {
			auraUptime["Haste"] += currentTime - auraStartLast["Haste"];
			auraStartLast.erase("Haste");
		}
	}
	else if (event == EVENT_BadgeOfSwarmguardFaded) {
		MHProcs.erase(PROC_ITEM_SWARMGUARD);
		OHProcs.erase(PROC_ITEM_SWARMGUARD);
		armorPenetration -= 200 * badgeOfSwarmguardCharges;
		badgeOfSwarmguardCharges = 0;
		if (writeLog) {
			fprintf(stderr, "%.3lf\tBadge of the Swarmguard fades from you.\n", currentTime / 1000.0);
			fprintf(stderr, "%.3lf\tInsight of the Qiraji fades from you.\n", currentTime / 1000.0);
		}
	}
	else if (event == EVENT_JomGabbarFaded) {
		stats[STAT_AP] -= jomGabbarAddedAP;
		recalculateDamage();
		if (writeLog)
			fprintf(stderr, "%.3lf\tJom Gabbar fades from you.\n", currentTime / 1000.0);
		cancelEvent(EVENT_JomGabbarTick);
	}
	else if (event == EVENT_UntamedFuryFaded) {
		stats[STAT_STRENGTH] -= 300;
		recalculateDamage();
		if (writeLog)
			fprintf(stderr, "%.3lf\tUntamed Fury fades from you.\n", currentTime / 1000.0);
		auraUptime["Untamed Fury"] += currentTime - auraStartLast["Untamed Fury"];
		auraStartLast.erase("Untamed Fury");
	}
	else if (event == EVENT_DestinyFaded) {
		stats[STAT_STRENGTH] -= 200;
		recalculateDamage();
		if (writeLog)
			fprintf(stderr, "%.3lf\tDestiny fades from you.\n", currentTime / 1000.0);
		auraUptime["Destiny"] += currentTime - auraStartLast["Destiny"];
		auraStartLast.erase("Destiny");
	}
	else if (event == EVENT_BonereaverFaded) {
		if (writeLog)
			fprintf(stderr, "%.3lf\tBonereaver fades from you.\n", currentTime / 1000.0);
		auraUptime["Bonereaver"] += currentTime - auraStartLast["Bonereaver"];
		auraStartLast.erase("Bonereaver");
	}
	else if (event == EVENT_CloudkeeperFaded) {
		stats[STAT_AP] -= 100;
		recalculateDamage();
		if (writeLog)
			fprintf(stderr, "%.3lf\tHeaven's Blessing fades from you.\n", currentTime / 1000.0);
	}
	else if (event == EVENT_SlayersCrestFaded) {
		stats[STAT_AP] -= 260;
		recalculateDamage();
		if (writeLog)
			fprintf(stderr, "%.3lf\tSlayer's Crest fades from you.\n", currentTime / 1000.0);
	}
	else if (event == EVENT_BerserkingFaded) {
		stats[STAT_HASTECOEFFICIENT] /= 1 + berserkingValue;
		recalculateSpeed();
		if (writeLog)
			fprintf(stderr, "%.3lf\tBerserking fades from you.\n", currentTime / 1000.0);
	}
	else if (event == EVENT_ZealFaded) {
		MH.weaponDamage[0] -= 10;
		MH.weaponDamage[1] -= 10;
		recalculateDamage();
		if (writeLog)
			fprintf(stderr, "%.3lf\tZeal fades from you.\n", currentTime / 1000.0);
		auraUptime["Zeal"] += currentTime - auraStartLast["Zeal"];
		auraStartLast.erase("Zeal");
	}
	else if (event == EVENT_BloodFuryFaded) {
		stats[STAT_AP] -= bloodFuryValue;
		recalculateDamage();
		if (writeLog)
			fprintf(stderr, "%.3lf\tBlood Fury fades from you.\n", currentTime / 1000.0);
	}
	else if (event == EVENT_FelstrikerFaded) {
		stats[STAT_MELEEHIT] -= 10000;
		stats[STAT_MELEECRIT] -= 10000;
		recalculateDamage();
		if (writeLog)
			fprintf(stderr, "%.3lf\tFelstriker fades from you.\n", currentTime / 1000.0);
		auraUptime["Felstriker"] += currentTime - auraStartLast["Felstriker"];
		auraStartLast.erase("Felstriker");
	}
	else if (event == EVENT_MalownsSlamFaded) {
		stats[STAT_STRENGTH] -= 50;
		recalculateDamage();
		if (writeLog)
			fprintf(stderr, "%.3lf\tMalown's Slam fades from you.\n", currentTime / 1000.0);
		auraUptime["Malown's Slam"] += currentTime - auraStartLast["Malown's Slam"];
		auraStartLast.erase("Malown's Slam");
	}
	else if (event == EVENT_WindfuryAPFaded) {
		stats[STAT_AP] -= 409.5;
		recalculateDamage();
		auraUptime["Windfury AP"] += currentTime - auraStartLast["Windfury AP"];
		auraStartLast.erase("Windfury AP");
	}
	else if (event == EVENT_NightfallFaded) {
		for (int i = 0; i < SIZE_MAGICSCHOOL; i++) {
			if (i != SCHOOL_PHYSICAL)
				damageMultipliers[i] /= 1.15;
		}
		if (writeLog)
			fprintf(stderr, "%.3lf\tSpell Vulnerability fades from the target.\n", currentTime / 1000.0);
		auraUptime["Spell Vulnerability"] += currentTime - auraStartLast["Spell Vulnerability"];
		auraStartLast.erase("Spell Vulnerability");
	}
	else if (event == EVENT_BarovPeasant1Hit || event == EVENT_BarovPeasant2Hit || event == EVENT_BarovPeasant3Hit || event == EVENT_HarvestReaperHit) {
		RollResult roll = rollSwingPet();
		char petName[25] = "Servant of Alexi Barov";
		if (event == EVENT_HarvestReaperHit)
			strcpy(petName, "Compact Harvest Reaper");
		if (roll == ROLL_MISS || roll == ROLL_DODGE) {	// Attack avoided
			if (writeLog)
				fprintf(stderr, "%.3lf\t%s %s.\n", currentTime / 1000.0, petName, ((roll == ROLL_MISS) ? "misses" : "'s attack was dodged"));
		}
		else {	// Attack landed
			// Perform the attack
			std::uniform_int_distribution<int> uni(100, 135);
			int damage = uni(*RNG);
			if (roll == ROLL_CRITICAL)
				damage *= 2;
			else if (roll == ROLL_GLANCING)
				damage = getGlancingDamage(damage, 300);
			if (writeLog)
				fprintf(stderr, "%.3lf\t%s %shits for %d.%s\n", currentTime / 1000.0, petName, ((roll == ROLL_CRITICAL) ? "critically " : ""), damage, ((roll == ROLL_GLANCING) ? " (Glancing)" : ""));
			totalDamageDealt += damage;
			damagePerComponent["Pet"] += damage;
		}
		setEventCooldownFromNow(event, 2000);
	}
	else if (event == EVENT_BarovPeasantsDespawned) {
		cancelEvent(EVENT_BarovPeasant1Hit);
		cancelEvent(EVENT_BarovPeasant2Hit);
		cancelEvent(EVENT_BarovPeasant3Hit);
	}
	else if (event == EVENT_HarvestReaperDespawned) {
		cancelEvent(EVENT_HarvestReaperHit);
	}
	else if (event == EVENT_BloodrageTick) {
		addRage(1);
		setEventCooldownFromNow(EVENT_BloodrageTick, 1000);
		// This event may result in a user action; so if GCD is ready, we poke the rotation manager.
		if (!isEventScheduled(EVENT_GlobalCooldown))	
			setEventCooldownFromNow(EVENT_GlobalCooldown, defaultLatency);
	}
	else if (event == EVENT_BloodrageFaded) {
		cancelEvent(EVENT_BloodrageTick);
		if (writeLog)
			fprintf(stderr, "%.3lf\tBloodrage fades from you.\n", currentTime / 1000.0);
	}
	else if (event == EVENT_StanceCooldown || event == EVENT_WhirlwindCooldown || event == EVENT_MortalStrikeCooldown || event == EVENT_BloodthirstCooldown || event == EVENT_BerserkerRageCooldown || event == EVENT_BloodFuryCooldown || event == EVENT_OverpowerCooldown) {
		// This event may result in a user action; so if GCD is ready, we poke the rotation manager.
		if (!isEventScheduled(EVENT_GlobalCooldown))	
			setEventCooldownFromNow(EVENT_GlobalCooldown, defaultLatency);
	}
}

void CharWarrior::doYellowAttack(AbilityWarrior abilityType) {
	RollResult roll = rollYellow(abilityType);
	int rageCost = abilityCost[abilityType];
	if (rage < rageCost)
		return;
	rage -= rageCost;
	int damage = sampleAbilityDamage(abilityType, roll);
	if (roll == ROLL_MISS || roll == ROLL_DODGE) {	// Attack avoided
		if (writeLog)
			fprintf(stderr, "%.3lf\tYour %s %s.\n", currentTime / 1000.0, abilityNames[abilityType], ((roll == ROLL_MISS) ? "missed" : "was dodged"));
		if (roll == ROLL_DODGE) {
			setEventCooldownFromNow(EVENT_OverpowerWindowClosed, 5000);
			// Refund 80% of the rage cost
			addRage(0.8 * rageCost);
		}
	}
	else {	// Attack landed
		// Perform the attack
		if (writeLog)
			fprintf(stderr, "%.3lf\tYour %s %shits for %d.\n", currentTime / 1000.0, abilityNames[abilityType], ((roll == ROLL_CRITICAL) ? "critically " : ""), (int)damage);
		totalDamageDealt += damage;
		damagePerComponent[std::string(abilityNames[abilityType])] += damage;
		// Perform procs after the attack
		doProcs(roll, true, false);
	}
	switch (abilityType) {
		case ABILITY_HAMSTRING: setEventCooldownFromNow(EVENT_GlobalCooldown, globalCooldownDuration); break;
		case ABILITY_HEROICSTRIKE: setEventCooldownFromNow(EVENT_MHSwing, MH.speed); break;
		case ABILITY_OVERPOWER: setEventCooldownFromNow(EVENT_GlobalCooldown, globalCooldownDuration); setEventCooldownFromNow(EVENT_OverpowerCooldown, 5000); cancelEvent(EVENT_OverpowerWindowClosed); break;
		case ABILITY_MORTALSTRIKE: setEventCooldownFromNow(EVENT_GlobalCooldown, globalCooldownDuration); setEventCooldownFromNow(EVENT_MortalStrikeCooldown, 6000); break;
		case ABILITY_CLEAVE: setEventCooldownFromNow(EVENT_MHSwing, MH.speed); break;
		case ABILITY_EXECUTE: setEventCooldownFromNow(EVENT_GlobalCooldown, globalCooldownDuration); break;
		case ABILITY_SLAM: setEventCooldownFromNow(EVENT_MHSwing, MH.speed); break;	// Slam activates the global cooldown at the time that the cast starts.
		case ABILITY_WHIRLWIND: setEventCooldownFromNow(EVENT_GlobalCooldown, globalCooldownDuration); setEventCooldownFromNow(EVENT_WhirlwindCooldown, 10000); break;
		case ABILITY_BLOODTHIRST: setEventCooldownFromNow(EVENT_GlobalCooldown, globalCooldownDuration); setEventCooldownFromNow(EVENT_BloodthirstCooldown, 6000); break;
	}
}

int CharWarrior::getGlancingDamage(const int damage, const int weaponSkill) {
	double modifier = std::max(0.0, std::min(1.0, 1 - 5 * (pow(2.0, 63 - weaponSkill / 5.0) - 1) / 100.0));
	return damage * modifier;
}

RollResult CharWarrior::rollSwingPet() {
	int miss = 560;
	int dodge = 560;
	int glancing = 4000;
	int critical = 440;
	std::uniform_int_distribution<int> uni(1, 10000);
	int roll = uni(*RNG);
	if (roll <= miss)
		return ROLL_MISS;
	else if (roll <= miss + dodge)
		return ROLL_DODGE;
	else if (roll <= miss + dodge + glancing)
		return ROLL_GLANCING;
	else if (roll <= miss + dodge + glancing + critical)
		return ROLL_CRITICAL;
	else
		return ROLL_NORMAL;
}

void CharWarrior::doMainHandAttack(bool isExtraAttack /*= false*/) {
	if (!isExtraAttack && rage >= ((OH.type != ITEM_NONE ? 35 : 80))) {
		doYellowAttack(ABILITY_HEROICSTRIKE);
		return;
	}
	RollResult roll = rollSwingMainHand();
	if (roll == ROLL_MISS || roll == ROLL_DODGE) {	// Attack avoided
		if (writeLog)
			fprintf(stderr, "%.3lf\tYour Main Hand %s.\n", currentTime / 1000.0, ((roll == ROLL_MISS) ? "missed" : "was dodged"));
		if (roll == ROLL_DODGE)
			setEventCooldownFromNow(EVENT_OverpowerWindowClosed, 5000);
	}
	else {	// Attack landed
		// All procs are rolled and applied before the hit
		// Remove a flurry charge (if there are any present) so that if an extra attack procs it again it will be full afterwards
		reduceFlurryCharges();
		doProcs(roll, true, isExtraAttack);
		// Perform the attack
		std::uniform_int_distribution<int> uni(MH.whiteDamage[0], MH.whiteDamage[1]);
		int damage = uni(*RNG);
		if (roll == ROLL_CRITICAL)
			damage *= 2;
		else if (roll == ROLL_GLANCING)
			damage = getGlancingDamage(damage, MH.skill);
		damage *= damageMultipliers[SCHOOL_PHYSICAL];
		if (MH.elementalDamage[1]) {
			std::uniform_int_distribution<int> uni2(MH.elementalDamage[0], MH.elementalDamage[1]);
			int elementalDamage = uni2(*RNG);
			if (roll == ROLL_CRITICAL)
				elementalDamage *= 1.5;
			elementalDamage *= damageMultipliers[MH.elementalSchool];
			ResistResult resist = calculatePartialResist(MH.elementalSchool, 5);
			int amountResisted = 0.25 * int(resist) * elementalDamage;
			elementalDamage -= amountResisted;
			damage += elementalDamage;
			if (writeLog)
				fprintf(stderr, "%.3lf\tYour Main Hand %shits for %d. (%d resisted)%s\n", currentTime / 1000.0, ((roll == ROLL_CRITICAL) ? "critically " : ""), damage, amountResisted, ((roll == ROLL_GLANCING) ? " (Glancing)" : ""));
		}
		else {
			if (writeLog)
				fprintf(stderr, "%.3lf\tYour Main Hand %shits for %d.%s\n", currentTime / 1000.0, ((roll == ROLL_CRITICAL) ? "critically " : ""), damage, ((roll == ROLL_GLANCING) ? " (Glancing)" : ""));
		}
		totalDamageDealt += damage;
		if (!isExtraAttack)
			damagePerComponent["Main Hand"] += damage;
		else
			damagePerComponent["Extra attack"] += damage;
		// Award rage
		addRage(damage / 230.60000004 * 7.5);
	}
	setEventCooldownFromNow(EVENT_MHSwing, MH.speed);
}

void CharWarrior::doOffHandAttack() {
	RollResult roll = rollSwingOffHand();
	if (roll == ROLL_MISS || roll == ROLL_DODGE) {	// Attack avoided
		if (writeLog)
			fprintf(stderr, "%.3lf\tYour Off Hand %s.\n", currentTime / 1000.0, ((roll == ROLL_MISS) ? "missed" : "was dodged"));
		if (roll == ROLL_DODGE)
			setEventCooldownFromNow(EVENT_OverpowerWindowClosed, 5000);
	}
	else {	// Attack landed
		// All procs are rolled and applied before the hit
		// Remove a flurry charge (if there are any preset) so that if an extra attack procs it again it will be full afterwards
		reduceFlurryCharges();
		doProcs(roll, false, false);
		// Perform the attack
		std::uniform_int_distribution<int> uni(OH.whiteDamage[0], OH.whiteDamage[1]);
		int damage = uni(*RNG);
		if (roll == ROLL_CRITICAL)
			damage *= 2;
		else if (roll == ROLL_GLANCING)
			damage = getGlancingDamage(damage, OH.skill);
		damage *= damageMultipliers[SCHOOL_PHYSICAL];
		if (OH.elementalDamage[1]) {
			std::uniform_int_distribution<int> uni2(OH.elementalDamage[0], OH.elementalDamage[1]);
			int elementalDamage = uni2(*RNG);
			if (roll == ROLL_CRITICAL)
				elementalDamage *= 1.5;
			elementalDamage *= damageMultipliers[OH.elementalSchool];
			ResistResult resist = calculatePartialResist(OH.elementalSchool, 5);
			int amountResisted = 0.25 * int(resist) * elementalDamage;
			elementalDamage -= amountResisted;
			damage += elementalDamage;
			if (writeLog)
				fprintf(stderr, "%.3lf\tYour Off Hand %shits for %d. (%d resisted)%s\n", currentTime / 1000.0, ((roll == ROLL_CRITICAL) ? "critically " : ""), damage, amountResisted, ((roll == ROLL_GLANCING) ? " (Glancing)" : ""));
		}
		else {
			if (writeLog)
				fprintf(stderr, "%.3lf\tYour Off Hand %shits for %d.%s\n", currentTime / 1000.0, ((roll == ROLL_CRITICAL) ? "critically " : ""), damage, ((roll == ROLL_GLANCING) ? " (Glancing)" : ""));
		}
		totalDamageDealt += damage;
		damagePerComponent["Off Hand"] += damage;
		// Award rage
		addRage(damage / 230.60000004 * 7.5);
	}
	setEventCooldownFromNow(EVENT_OHSwing, OH.speed);
}

bool CharWarrior::procActivated(int procChance) {
	std::uniform_int_distribution<int> uni(1, 10000);
	int roll = uni(*RNG);
	return roll <= procChance;
}

struct ResistanceValues {
    int resist100;
    int resist75;
    int resist50;
    int resist25;
    int resist0;
    unsigned int chanceResist;
};

ResistanceValues resistValues[] = {
    {0, 0, 0, 0, 100, 0}, // 0
    {0, 0, 2, 6, 92, 3}, // 10
    {0, 1, 4, 12, 84, 5}, // 20
    {0, 1, 5, 18, 76, 8}, // 30
    {0, 1, 7, 23, 69, 10}, // 40
    {0, 2, 9, 28, 61, 13}, // 50
    {0, 2, 11, 33, 54, 15}, // 60
    {0, 2, 13, 37, 37, 18}, // 70
    {0, 3, 15, 41, 41, 20}, // 80
    {1, 3, 17, 46, 36, 23}, // 90
    {1, 4, 19, 47, 29, 25}, // 100
    {1, 5, 21, 48, 24, 28}, // 110
    {1, 6, 24, 49, 20, 30}, // 120
    {1, 8, 28, 47, 17, 33}, // 130
    {1, 9, 33, 43, 14, 35}, // 140
    {1, 11, 37, 39, 12, 38}, // 150
    {1, 13, 41, 35, 10, 40}, // 160
    {1, 16, 45, 30, 8, 43}, // 170
    {1, 18, 48, 26, 7, 45}, // 180
    {2, 20, 48, 24, 6, 48}, // 190
    {4, 23, 48, 21, 4, 50}, // 200
    {5, 25, 47, 19, 3, 53}, // 210
    {7, 28, 45, 17, 2, 55}, // 220
    {9, 31, 43, 16, 2, 58}, // 230
    {11, 34, 40, 14, 1, 60}, // 240
    {13, 37, 37, 12, 1, 62}, // 250
    {15, 41, 33, 10, 1, 65}, // 260
    {18, 44, 29, 8, 1, 68}, // 270
    {20, 48, 25, 7, 1, 70}, // 280
    {23, 51, 20, 5, 1, 73}, // 290
    {25, 55, 16, 3, 1, 75} // 300
};

ResistResult CharWarrior::calculatePartialResist(MagicSchool school, int chanceToResist) {
	if (school == SCHOOL_PHYSICAL)
		return RESIST_0;
	else {
		double resistanceChance = chanceToResist;
		ResistanceValues* prev = nullptr;
        ResistanceValues* next = nullptr;
        for (int i = 1; i < 31; ++i) {
            if (resistValues[i].chanceResist >= resistanceChance) {
                prev = &resistValues[i - 1];
                next = &resistValues[i];
                break;
            }
        }
        // assert(next && prev);
        float coeff = float(resistanceChance - prev->chanceResist) / float(next->chanceResist - prev->chanceResist);
        float resist0 = prev->resist0 + (next->resist0 - prev->resist0) * coeff;
        float resist25 = prev->resist25 + (next->resist25 - prev->resist25) * coeff;
        float resist50 = prev->resist50 + (next->resist50 - prev->resist50) * coeff;
        float resist75 = prev->resist75 + (next->resist75 - prev->resist75) * coeff;
        float resist100 = prev->resist100 + (next->resist100 - prev->resist100) * coeff;
        std::uniform_int_distribution<int> uni(0, 99);
        unsigned int ran = uni(*RNG);
        float resistCnt = 0.0f;
        if (ran < resist100 + resist75)
            return RESIST_75;
        else if (ran < resist100 + resist75 + resist50)
            return RESIST_50;
        else if (ran < resist100 + resist75 + resist50 + resist25)
            return RESIST_25;
		else
			return RESIST_0;
	}
}

void CharWarrior::doSpell(const char * spellName /*= "UNKNOWN"*/, MagicSchool school /*= SCHOOL_PHYSICAL*/, const int minVal /*= 0*/, const int maxVal /*= 0*/) {
	RollResult roll = rollYellow((school == SCHOOL_PHYSICAL) ? ABILITY_HAMSTRING : ABILITY_SPELL);	// Using Hamstring as dummy spell for a physical yellow attack. This wouldn't work for paladin spells such as Seal of Command or Judgement of Command, but don't care about that particularly in a warrior simulator.
	if (roll == ROLL_MISS || roll == ROLL_DODGE) {	// Spell resisted or was dodged (in case of a physical damage spell)
		if (writeLog)
			fprintf(stderr, "%.3lf\tYour %s %s.\n", currentTime / 1000.0, spellName, ((roll == ROLL_MISS) ? "missed" : "was dodged"));
	}
	else {	// Spell landed
		ResistResult resist = calculatePartialResist(school, 5);
		if (resist == RESIST_100) {
			if (writeLog)
				fprintf(stderr, "%.3lf\tYour %s was resisted.\n", currentTime / 1000.0, spellName);
		}
		else {
			std::uniform_int_distribution<int> uni(minVal, maxVal);
			int damage = uni(*RNG);
			if (roll == ROLL_CRITICAL) {
				if (school == SCHOOL_PHYSICAL)
					damage *= 2;
				else
					damage *= 1.5;
			}
			damage *= damageMultipliers[school];
			if (resist == RESIST_0) {
				if (writeLog)
					fprintf(stderr, "%.3lf\tYour %s %shits for %d %sdamage.\n", currentTime / 1000.0, spellName, ((roll == ROLL_CRITICAL) ? "critically " : ""), damage, magicSchoolNames[int(school)]);
			}
			else {
				int amountResisted = 0.25 * int(resist) * damage;
				damage -= amountResisted;
				if (writeLog)
					fprintf(stderr, "%.3lf\tYour %s %shits for %d %sdamage. (%d resisted)\n", currentTime / 1000.0, spellName, ((roll == ROLL_CRITICAL) ? "critically " : ""), damage, magicSchoolNames[int(school)], amountResisted);
			}
			totalDamageDealt += damage;
			damagePerComponent[std::string(spellName)] += damage;
		}
	}
}

void CharWarrior::doProcs(RollResult roll, bool isMH /*= true*/, bool isExtraAttack /*= false*/) {
	const std::unordered_set< ProcType, std::hash<int> > & procs = ((isMH) ? MHProcs : OHProcs);
	for (auto proc : procs) {
		if (proc == PROC_TALENT_UNBRIDLEDWRATH && procActivated(4000)) {	// 40% chance
			addRage(1);
			if (writeLog)
				fprintf(stderr, "%.3lf\tYou gain 1 rage from Unbridled Wrath.\n", currentTime / 1000.0);
		}
		else if (proc == PROC_ENCHANTMENT_CRUSADER && procActivated(10000 * 1 * (isMH ? MH.baseWeaponSpeed : OH.baseWeaponSpeed) / 60000.0)) {	// 1 PPM
			bool isPresent = isEventScheduled(isMH ? EVENT_MHCrusaderFaded : EVENT_OHCrusaderFaded);
			if (!isPresent) {
				stats[STAT_STRENGTH] += 100;
				recalculateDamage();
				if (writeLog)
					fprintf(stderr, "%.3lf\tYou gain Holy Strength from %s.\n", currentTime / 1000.0, ((isMH) ? "Main Hand" : "Off Hand"));
			}
			setEventCooldownFromNow((isMH ? EVENT_MHCrusaderFaded : EVENT_OHCrusaderFaded), 15000);
			if (!auraStartLast.count((isMH ? "Crusader MH" : "Crusader OH")))
				auraStartLast[(isMH ? "Crusader MH" : "Crusader OH")] = currentTime;
		}
		else if (proc == PROC_ENCHANTMENT_FIERY && procActivated(10000 * 6 * (isMH ? MH.baseWeaponSpeed : OH.baseWeaponSpeed) / 60000.0)) {	// 6 PPM
			doSpell("Fiery Weapon", SCHOOL_FIRE, 40, 40);
		}
		else if (proc == PROC_ENCHANTMENT_LIFESTEALING && procActivated(10000 * 6 * (isMH ? MH.baseWeaponSpeed : OH.baseWeaponSpeed) / 60000.0)) {	// 6 PPM
			doSpell("Life Steal", SCHOOL_SHADOW, 30, 30);
		}
		else if (proc == PROC_WEAPON_THUNDERFURY && procActivated(2000)) {	// 20% proc chance
			doSpell("Thunderfury", SCHOOL_NATURE, 300, 300);
		}
		else if (proc == PROC_WEAPON_VISKAG && procActivated(10000 * 2 * (isMH ? MH.baseWeaponSpeed : OH.baseWeaponSpeed) / 60000.0)) {	// 2 PPM
			doSpell("Fatal Wound", SCHOOL_PHYSICAL, 240, 240);
		}
		else if (proc == PROC_WEAPON_EMPYREAN && procActivated(10000 * 1.5 * 2.8 / 60.0)) {	// 2 PPM [1 PPM if PTR fix is applied]
			bool isPresent = isEventScheduled(EVENT_EmpyreanDemolisherHasteFaded);
			if (!isPresent) {
				stats[STAT_HASTECOEFFICIENT] *= 1.2;
				recalculateSpeed();
				if (writeLog)
					fprintf(stderr, "%.3lf\tYou gain Haste.\n", currentTime / 1000.0);
			}
			setEventCooldownFromNow(EVENT_EmpyreanDemolisherHasteFaded, 10000);
			if (!auraStartLast.count("Haste"))
				auraStartLast["Haste"] = currentTime;
		}
		else if (proc == PROC_WEAPON_MANSLAYER && procActivated(10000 * 2 * 2.0 / 60.0)) {	// 2 PPM
			doSpell("Drain Life", SCHOOL_SHADOW, 48, 53);
		}
		else if (proc == PROC_WEAPON_DEATHBRINGER && procActivated(750)) {	// 7.5%
			doSpell("Shadow Bolt", SCHOOL_SHADOW, 110, 140);
		}
		else if (proc == PROC_WEAPON_PERDITION && procActivated(450)) {	// 4.5%
			doSpell("Firebolt", SCHOOL_FIRE, 40, 56);
		}
		else if (proc == PROC_WEAPON_SULFURAS && procActivated(10000 * 1.5 * 3.7 / 60.0)) {	// 1.5 PPM
			doSpell("Fireball", SCHOOL_FIRE, 273, 333);
		}
		else if (proc == PROC_WEAPON_SERVOARM && procActivated(1400)) {	// 14%
			doSpell("Electric Discharge", SCHOOL_NATURE, 100, 150);
		}
		else if (proc == PROC_WEAPON_UNTAMEDBLADE && procActivated(10000 * 2 * 3.4 / 60.0)) {	// 2 PPM
			bool isPresent = isEventScheduled(EVENT_UntamedFuryFaded);
			if (!isPresent) {
				stats[STAT_STRENGTH] += 300;
				recalculateDamage();
				if (writeLog)
					fprintf(stderr, "%.3lf\tYou gain Untamed Fury.\n", currentTime / 1000.0);
			}
			setEventCooldownFromNow(EVENT_UntamedFuryFaded, 8000);
			if (!auraStartLast.count("Untamed Fury"))
				auraStartLast["Untamed Fury"] = currentTime;
		}
		else if (proc == PROC_WEAPON_DESTINY && procActivated(10000 * 2 * 2.6 / 60.0)) {	// 2 PPM is fake. We don't know PPM.
			bool isPresent = isEventScheduled(EVENT_DestinyFaded);
			if (!isPresent) {
				stats[STAT_STRENGTH] += 200;
				recalculateDamage();
				if (writeLog)
					fprintf(stderr, "%.3lf\tYou gain Destiny.\n", currentTime / 1000.0);
			}
			setEventCooldownFromNow(EVENT_DestinyFaded, 10000);
			if (!auraStartLast.count("Destiny"))
				auraStartLast["Destiny"] = currentTime;
		}
		else if (proc == PROC_WEAPON_ANNIHILATOR && procActivated(1200)) {	// [TODO: MISSING IMPLEMENTATION; Should decrease armour of the target by 200, up to a stack of 3]
			;
		}
		else if (proc == PROC_ITEM_SWARMGUARD && procActivated(5000)) {	// 50%
			if (badgeOfSwarmguardCharges < 6) {
				armorPenetration += 200;
				if (writeLog)
					fprintf(stderr, "%.3lf\tYou gain Badge of the Swarmguard (%d).\n", currentTime / 1000.0, badgeOfSwarmguardCharges);
			}
		}
		else if (proc == PROC_WEAPON_ESKHANDAR && procActivated(10000 * 1 * 1.5 / 60.0)) {	// 1 PPM after PTR fix
			bool isPresent = isEventScheduled(EVENT_EskhandarHasteFaded);
			if (!isPresent) {
				stats[STAT_HASTECOEFFICIENT] *= 1.3;
				recalculateSpeed();
				if (writeLog)
					fprintf(stderr, "%.3lf\tYou gain Eskhandar's Rage.\n", currentTime / 1000.0);
			}
			setEventCooldownFromNow(EVENT_EskhandarHasteFaded, 5000);
			if (!auraStartLast.count("Eskhandar's Rage"))
				auraStartLast["Eskhandar's Rage"] = currentTime;
		}
		else if (proc == PROC_WEAPON_SWORDOFZEAL && procActivated(550)) {	// 5.5%
			bool isPresent = isEventScheduled(EVENT_ZealFaded);
			if (!isPresent) {
				MH.weaponDamage[0] += 10;
				MH.weaponDamage[1] += 10;
				recalculateDamage();
				if (writeLog)
					fprintf(stderr, "%.3lf\tYou gain Zeal.\n", currentTime / 1000.0);
			}
			setEventCooldownFromNow(EVENT_ZealFaded, 15000);
			if (!auraStartLast.count("Zeal"))
				auraStartLast["Zeal"] = currentTime;
		}
		else if (proc == PROC_WEAPON_FELSTRIKER && procActivated(350)) {	// 3.5%
			bool isPresent = isEventScheduled(EVENT_FelstrikerFaded);
			if (!isPresent) {
				stats[STAT_MELEEHIT] += 10000;
				stats[STAT_MELEECRIT] += 10000;
				recalculateDamage();
				if (writeLog)
					fprintf(stderr, "%.3lf\tYou gain Felstriker.\n", currentTime / 1000.0);
			}
			setEventCooldownFromNow(EVENT_FelstrikerFaded, 3000);
			if (!auraStartLast.count("Felstriker"))
				auraStartLast["Felstriker"] = currentTime;
		}
		else if (proc == PROC_WEAPON_DEMONSHEAR && procActivated(500)) {	// 5%
			doSpell("Shadow Bolt", SCHOOL_SHADOW, 150, 150);
		}
		else if (proc == PROC_WEAPON_MALOWN && procActivated(575)) {	// 5.75%
			bool isPresent = isEventScheduled(EVENT_MalownsSlamFaded);
			if (!isPresent) {
				stats[STAT_STRENGTH] += 50;
				recalculateDamage();
				if (writeLog)
					fprintf(stderr, "%.3lf\tYou gain Malown's Slam.\n", currentTime / 1000.0);
			}
			setEventCooldownFromNow(EVENT_MalownsSlamFaded, 30000);
			if (!auraStartLast.count("Malown's Slam"))
				auraStartLast["Malown's Slam"] = currentTime;
		}
		else if (proc == PROC_WEAPON_DEEPWOODS && procActivated(400)) {	// 4%
			doSpell("Wrath", SCHOOL_NATURE, 90, 126);
		}
		else if (proc == PROC_WEAPON_NIGHTFALL && procActivated(2000)) {	// 20%
			bool isPresent = isEventScheduled(EVENT_NightfallFaded);
			if (!isPresent) {
				for (int i = 0; i < SIZE_MAGICSCHOOL; i++) {
					if (i != SCHOOL_PHYSICAL)
						damageMultipliers[i] *= 1.15;
				}
				if (writeLog)
					fprintf(stderr, "%.3lf\tTarget is afflicted by Spell Vulnerability.\n", currentTime / 1000.0);
			}
			setEventCooldownFromNow(EVENT_NightfallFaded, 5000);
			if (!auraStartLast.count("Spell Vulnerability"))
				auraStartLast["Spell Vulnerability"] = currentTime;
		}
		else if (proc == PROC_ITEM_MAELSTROM && procActivated(10000 * 1.5 * (isMH ? MH.baseWeaponSpeed : OH.baseWeaponSpeed) / 60000.0)) {	// 1.5 PPM
			doSpell("Lightning Strike", SCHOOL_NATURE, 200, 300);
		}
		else if (proc == PROC_ITEM_WYRMTHALAK && procActivated(10000 * 3 * (isMH ? MH.baseWeaponSpeed : OH.baseWeaponSpeed) / 60000.0)) {	// 3 PPM
			doSpell("Flame Lash", SCHOOL_FIRE, 120, 180);
		}
		else if (proc == PROC_TALENT_FLURRY && roll == ROLL_CRITICAL) {
			bool isPresent = isEventScheduled(EVENT_FlurryFaded);
			if (!isPresent) {
				stats[STAT_HASTECOEFFICIENT] *= 1.3;
				recalculateSpeed();
				if (writeLog)
					fprintf(stderr, "%.3lf\tYou gain Flurry.\n", currentTime / 1000.0);
			}
			setEventCooldownFromNow(EVENT_FlurryFaded, 15000);
			flurryCharges = 4;
			if (!auraStartLast.count("Flurry"))
				auraStartLast["Flurry"] = currentTime;
		}
		else if (!isExtraAttack) {	// Extra attacks can't proc additional extra attacks
			if (proc == PROC_ENCHANTMENT_WINDFURY && procActivated(2000)) {	// 20%
				if (writeLog)
					fprintf(stderr, "%.3lf\tYou gain 1 extra attack through Windfury.\n", currentTime / 1000.0);
				bool isPresent = isEventScheduled(EVENT_WindfuryAPFaded);
				if (!isPresent) {
					stats[STAT_AP] += 409.5;
					recalculateDamage();
				}
				setEventCooldownFromNow(EVENT_WindfuryAPFaded, 1500);	// Adds extra attack power for 1.5 seconds
				doMainHandAttack(true);
				if (!auraStartLast.count("Windfury AP"))
					auraStartLast["Windfury AP"] = currentTime;
			}
			else if (proc == PROC_WEAPON_IRONFOE && procActivated(400)) {	// 4%
				if (writeLog)
					fprintf(stderr, "%.3lf\tYou gain 2 extra attacks through Fury of Forgewright.\n", currentTime / 1000.0);
				doMainHandAttack(true);
				doMainHandAttack(true);
			}
			else if (proc == PROC_WEAPON_THRASHBLADE && procActivated(400)) {	// 4%
				if (writeLog)
					fprintf(stderr, "%.3lf\tYou gain 1 extra attack through Thrash Blade.\n", currentTime / 1000.0);
				doMainHandAttack(true);
			}
			else if (proc == PROC_WEAPON_FLURRYAXE && procActivated(300)) {	// 3%
				if (writeLog)
					fprintf(stderr, "%.3lf\tYou gain 1 extra attack through Flurry Axe.\n", currentTime / 1000.0);
				doMainHandAttack(true);
			}
			else if (proc == PROC_ITEM_HOJ && procActivated(200)) {	// 2%
				if (writeLog)
					fprintf(stderr, "%.3lf\tYou gain 1 extra attack through Hand of Justice.\n", currentTime / 1000.0);
				doMainHandAttack(true);
			}
			else if (proc == PROC_TALENT_SWORDSPECIALIZATION && procActivated(500)) {	// Assuming 5%
				if (writeLog)
					fprintf(stderr, "%.3lf\tYou gain 1 extra attack through Sword Specialization.\n", currentTime / 1000.0);
				doMainHandAttack(true);
			}
		}
	}
}

void CharWarrior::enterBattleStance() {
	rage = std::min(rage, maxStanceSwapConservedRage);
	stats[STAT_MELEECRIT] -= 300;	// Assuming that we are switching from berserker stance
	recalculateDamage();
	setEventCooldownFromNow(EVENT_StanceCooldown, globalCooldownDuration);
	// This event may result in a user action; so if GCD is ready, we poke the rotation manager.
	if (!isEventScheduled(EVENT_GlobalCooldown))	
		setEventCooldownFromNow(EVENT_GlobalCooldown, defaultLatency);
	inBattleStance = true;
	if (writeLog)
		fprintf(stderr, "%.3lf\tYou gain Battle Stance.\n", currentTime / 1000.0);
}

void CharWarrior::enterBerserkerStance() {
	rage = std::min(rage, maxStanceSwapConservedRage);
	stats[STAT_MELEECRIT] += 300;
	recalculateDamage();
	setEventCooldownFromNow(EVENT_StanceCooldown, globalCooldownDuration);
	// This event may result in a user action; so if GCD is ready, we poke the rotation manager.
	if (!isEventScheduled(EVENT_GlobalCooldown))	
		setEventCooldownFromNow(EVENT_GlobalCooldown, defaultLatency);
	inBattleStance = false;
	if (writeLog)
		fprintf(stderr, "%.3lf\tYou gain Berserker Stance.\n", currentTime / 1000.0);
}

void CharWarrior::resetCharacter() {
	// Handle pending events and reset them. This should remove all temporary buffs from the character.	
	bool hasAngerManagement = false;
	for (int i = 0; i < SIZE_EVENTTYPE; i++) {
		if (isEventScheduled((EventType)i)) {
			handleEvent((EventType)i);
			cancelEvent((EventType)i);	// While most periodic events should be cancelled by their fading aura equivalents, let's not take chances.
			if ((EventType)i == EVENT_AngerManagementTick) {	// Anger management is the only event that we want running from the start, independently of user actions
				hasAngerManagement = true;
			}
		}
	}
	currentTime = 0;
	setEventReady(EVENT_MHSwing);
	if (OH.type != ITEM_NONE)
		setEventCooldownFromNow(EVENT_OHSwing, 200);
	setEventCooldownFromNow(EVENT_GlobalCooldown, defaultLatency);
	// Assuming that Bloodrage was used 10 seconds before melee combat started
	rage = 20;
	setEventCooldownFromNow(EVENT_BloodrageCooldown, 50000);
	if (hasAngerManagement) {
		std::uniform_int_distribution<int> uni(0, 3000);
		int cd = uni(*RNG);
		setEventCooldownFromNow(EVENT_AngerManagementTick, cd);
	}
	totalDamageDealt = 0;
	rageOverflow = 0;
	badgeOfSwarmguardCharges = 0;
	flurryCharges = 0;
	armorPenetration = 0;
	bonereaversEdgeCharges = 0;
	if (inBattleStance) {
		enterBerserkerStance();
		inBattleStance = false;
	}
	jomGabbarAddedAP = 0;
	auraStartLast.clear();	// Should be cleared by the first loop that handles all events, but just in case.
}

void CharWarrior::handleUseEffect(UseEffect ue) {
	if (ue == USE_ITEM_MIGHTYRAGE) {
		std::uniform_int_distribution<int> uni(45, 75);
		int val = uni(*RNG);
		addRage(val);
		stats[STAT_STRENGTH] += 60;
		recalculateDamage();
		setEventCooldownFromNow(EVENT_MightyRageFaded, 20000);
		setEventCooldownFromNow(EVENT_MightyRagePotionCooldown, 120000);
		if (writeLog) {
			fprintf(stderr, "%.3lf\tYou gain %d rage.\n", currentTime / 1000.0, val);
			fprintf(stderr, "%.3lf\tYou gain Might Rage.\n", currentTime / 1000.0);
		}
		// This event may result in a user action; so if GCD is ready, we poke the rotation manager.
		if (!isEventScheduled(EVENT_GlobalCooldown))	
			setEventCooldownFromNow(EVENT_GlobalCooldown, defaultLatency);
	}
	else if (ue == USE_ITEM_EARTHSTRIKE) {
		stats[STAT_AP] += 280;
		recalculateDamage();
		setEventCooldownFromNow(EVENT_EarthStrikeFaded, 20000);
		setEventCooldownFromNow(EVENT_TrinketCooldown, 20000);
		setEventCooldownFromNow(EVENT_EarthstrikeCooldown, 120000);
		if (writeLog)
			fprintf(stderr, "%.3lf\tYou gain Earthstrike.\n", currentTime / 1000.0);
	}
	else if (ue == USE_ITEM_DIAMONDFLASK) {
		stats[STAT_STRENGTH] += 75;
		recalculateDamage();
		setEventCooldownFromNow(EVENT_DiamondFlaskFaded, 60000);
		setEventCooldownFromNow(EVENT_TrinketCooldown, 60000);
		setEventCooldownFromNow(EVENT_DiamondFlaskCooldown, 300000);
		if (writeLog)
			fprintf(stderr, "%.3lf\tYou gain Diamond Flask.\n", currentTime / 1000.0);
		setEventCooldownFromNow(EVENT_GlobalCooldown, globalCooldownDuration);
	}
	else if (ue == USE_ITEM_SWARMGUARD) {
		MHProcs.insert(PROC_ITEM_SWARMGUARD);
		OHProcs.insert(PROC_ITEM_SWARMGUARD);
		setEventCooldownFromNow(EVENT_BadgeOfSwarmguardFaded, 30000);
		setEventCooldownFromNow(EVENT_BadgeOfSwarmguardCooldown, 180000);
		if (writeLog)
			fprintf(stderr, "%.3lf\tYou gain Badge of the Swarmguard.\n", currentTime / 1000.0);
	}
	else if (ue == USE_ITEM_JOMGABBAR) {
		handleEvent(EVENT_JomGabbarTick);	// Do a tick, event handler will register a new one every 2 seconds until it's canceled in 19.9999 seconds
		setEventCooldownFromNow(EVENT_JomGabbarFaded, 19999);
		setEventCooldownFromNow(EVENT_TrinketCooldown, 20000);
		setEventCooldownFromNow(EVENT_JomGabbarCooldown, 120000);
		if (writeLog)
			fprintf(stderr, "%.3lf\tYou gain Jom Gabbar.\n", currentTime / 1000.0);
	}
	else if (ue == USE_ITEM_CLOUDKEEPER) {
		stats[STAT_AP] += 100;
		recalculateDamage();
		setEventCooldownFromNow(EVENT_CloudkeeperFaded, 30000);
		setEventCooldownFromNow(EVENT_CloudkeeperCooldown, 900000);
		if (writeLog)
			fprintf(stderr, "%.3lf\tYou gain Heaven's Blessing.\n", currentTime / 1000.0);
	}
	else if (ue == USE_ITEM_HARVESTREAPER) {
		setEventCooldownFromNow(EVENT_HarvestReaperHit, 500);
		setEventCooldownFromNow(EVENT_HarvestReaperDespawned, 600000);
	}
	else if (ue == USE_ITEM_BAROV) {
		setEventCooldownFromNow(EVENT_BarovPeasant1Hit, 500);
		setEventCooldownFromNow(EVENT_BarovPeasant2Hit, 500);
		setEventCooldownFromNow(EVENT_BarovPeasant3Hit, 500);
		setEventCooldownFromNow(EVENT_BarovPeasantsDespawned, 20000);
		setEventCooldownFromNow(EVENT_BarovPeasantCallerCooldown, 900000);
	}
	else if (ue == USE_ITEM_KISSOFTHESPIDER) {
		stats[STAT_HASTECOEFFICIENT] *= 1.2;
		recalculateSpeed();
		setEventCooldownFromNow(EVENT_KissOfTheSpiderFaded, 15000);
		setEventCooldownFromNow(EVENT_TrinketCooldown, 15000);
		setEventCooldownFromNow(EVENT_KissOfTheSpiderCooldown, 120000);
		if (writeLog)
			fprintf(stderr, "%.3lf\tYou gain Kiss of the Spider.\n", currentTime / 1000.0);
	}
	else if (ue == USE_ITEM_SLAYERSCREST) {
		stats[STAT_AP] += 260;
		recalculateDamage();
		setEventCooldownFromNow(EVENT_SlayersCrestFaded, 20000);
		setEventCooldownFromNow(EVENT_TrinketCooldown, 20000);
		setEventCooldownFromNow(EVENT_SlayersCrestCooldown, 120000);
		if (writeLog)
			fprintf(stderr, "%.3lf\tYou gain Slayer's Crest.\n", currentTime / 1000.0);
	}
	else if (ue == USE_SPELL_BLOODRAGE) {
		addRage(10);
		if (writeLog)
			fprintf(stderr, "%.3lf\tYou gain 10 rage from Bloodrage.\n", currentTime / 1000.0);
		setEventCooldownFromNow(EVENT_BloodrageTick, 999);
		setEventCooldownFromNow(EVENT_BloodrageFaded, 10000);
		setEventCooldownFromNow(EVENT_BloodrageCooldown, 60000);
	}
	else if (ue == USE_SPELL_BERSERKERRAGE) {
		addRage(rageGainBerserkerRage);
		if (writeLog)
			fprintf(stderr, "%.3lf\tYou gain %d rage from Berserker Rage.\n", currentTime / 1000.0, rageGainBerserkerRage);
		setEventCooldownFromNow(EVENT_BerserkerRageCooldown, 30000);
		setEventCooldownFromNow(EVENT_GlobalCooldown, globalCooldownDuration);
	}
	else if (ue == USE_SPELL_DEATHWISH) {
		rage -= 5;	// Trusting that whatever calls this method will make sure that there is at least 5 rage
		damageMultipliers[SCHOOL_PHYSICAL] *= 1.2;
		recalculateDamage();
		if (writeLog)
			fprintf(stderr, "%.3lf\tYou are afflicted by Death Wish.\n", currentTime / 1000.0);
		setEventCooldownFromNow(EVENT_DeathWishFaded, 30000);
		setEventCooldownFromNow(EVENT_DeathWishCooldown, 180000);
	}
	else if (ue == USE_SPELL_BERSERKING) {
		rage -= 5;	// Trusting that whatever calls this method will make sure that there is at least 5 rage
		berserkingValue = 0.1;	// [TODO] Implement some variation here since the range is 10% - 30%
		stats[STAT_HASTECOEFFICIENT] *= 1 + berserkingValue;
		recalculateSpeed();
		setEventCooldownFromNow(EVENT_BerserkingFaded, 10000);
		setEventCooldownFromNow(EVENT_BerserkingCooldown, 180000);
		if (writeLog)
			fprintf(stderr, "%.3lf\tYou gain Berserking.\n", currentTime / 1000.0);
	}
	else if (ue == USE_SPELL_BLOODFURY) {
		bloodFuryValue = (2 * stats[STAT_STRENGTH] * stats[STAT_STATMULTIPLIER] + 160) / 4.0;
		stats[STAT_AP] += bloodFuryValue;
		recalculateDamage();
		setEventCooldownFromNow(EVENT_BloodFuryFaded, 15000);
		setEventCooldownFromNow(EVENT_BloodFuryCooldown, 120000);
		if (writeLog)
			fprintf(stderr, "%.3lf\tYou gain Blood Fury.\n", currentTime / 1000.0);
		setEventCooldownFromNow(EVENT_GlobalCooldown, globalCooldownDuration);
	}
}

void CharWarrior::printStats() {
	recalculateDamage();
	int currentAP = stats[STAT_AP] + 2 * stats[STAT_STRENGTH] * stats[STAT_STATMULTIPLIER];
	fprintf(stderr, "%20s: %5d\n", "Attack power", (int)currentAP);
	// MH.normalizedDamage[0] = MH.weaponDamage[0] + currentAP / 14.0 * normalizedSpeed;
	// MH.normalizedDamage[1] = MH.weaponDamage[1] + currentAP / 14.0 * normalizedSpeed;
	fprintf(stderr, "%20s: %5d - %5d\n", "MH damage", (int)MH.whiteDamage[0], (int)MH.whiteDamage[1]);
	if (OH.type != ITEM_NONE)
		fprintf(stderr, "%20s: %5d - %5d\n", "OH damage", (int)OH.whiteDamage[0], (int)OH.whiteDamage[1]);
	fprintf(stderr, "%20s: %5d - %5d\n", "Normalized damage", (int)MH.normalizedDamage[0], (int)MH.normalizedDamage[1]);
	fprintf(stderr, "%20s: %5d\n", "Strength", int(stats[STAT_STRENGTH] * stats[STAT_STATMULTIPLIER]));
	fprintf(stderr, "%20s: %5d\n", "Agility", int(stats[STAT_AGILITY] * stats[STAT_STATMULTIPLIER]));
	fprintf(stderr, "%20s: %5d\n", "MH weapon skill", MH.skill);
	if (OH.type != ITEM_NONE)
		fprintf(stderr, "%20s: %5d\n", "OH weapon skill", OH.skill);
	// fprintf(stderr, "%20s: %5d\n", "Hit from items", (int)stats[STAT_MELEEHIT]);
	// fprintf(stderr, "%20s: %5d\n", "MH.miss", MH.miss);
	int miss = std::max(0, std::min(10000, MH.miss + ((OH.type != ITEM_NONE) ? 1900 : 0)));
	fprintf(stderr, "%20s: %5.2lf\n", "MH miss chance", miss / 100.0 );
	int dodge = std::max(0, std::min(10000, MH.dodge));
	// fprintf(stderr, "%20s: %5d\n", "MH.dodge", MH.dodge);
	fprintf(stderr, "%20s: %5.2lf\n", "MH dodge chance", dodge / 100.0 );
	int remaining = 10000 - miss - dodge - 4000;
	int critical = std::max(0, std::min(remaining, MH.critical));
	fprintf(stderr, "%20s: %5.2lf\n", "MH crit chance", critical / 100.0 );
	if (OH.type != ITEM_NONE) {
		miss = std::max(0, std::min(10000, OH.miss + 1900));
		dodge = std::max(0, std::min(10000, OH.dodge));
		remaining = 10000 - miss - dodge - 4000;
		critical = std::max(0, std::min(remaining, OH.critical));
		fprintf(stderr, "%20s: %5.1lf\n", "OH crit chance", critical / 100.0);
	}
	fprintf(stderr, "\n");
}