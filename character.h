#ifndef __CHARACTER_H_INCLUDED__
#define __CHARACTER_H_INCLUDED__

#include <cmath>
#include <unordered_set>
#include <unordered_map>
#include <cstring>
#include <random>
#include <iostream>
#include "common.h"
#include "items.h"
#include "heap.h"

struct CharacterEvent {
	int timestamp;
	EventType type;
	
	CharacterEvent() : type(EVENT_None), timestamp(EVENT_MAXTIME) {}
	CharacterEvent(EventType type, int timestamp = EVENT_MAXTIME) : type(type), timestamp(timestamp) {}
	
	bool operator<(const CharacterEvent & Q) const {
		if (timestamp != Q.timestamp) return timestamp < Q.timestamp;
		else return type < Q.type;
	}
	void setTime(int newTimestamp) {
		timestamp = newTimestamp;
	}
};

struct CharWeapon {
	ItemType type;
	int baseWeaponSpeed;
	double speed;
	double weaponDamage[2];	// 0 - lower, 1 - upper
	double elementalDamage[2];	// 0 - lower, 1 - upper
	double whiteDamage[2];	// 0 - lower, 1 - upper
	double normalizedDamage[2];	// 0 - lower, 1 - upper
	MagicSchool elementalSchool;
	double damageMultiplier;	// MH = 1.0, OH = from 0.5 to 0.625
	int skill;
	int miss;
	int dodge;
	int critical;
	int normal;

	CharWeapon() {
		type = ITEM_NONE;
		weaponDamage[0] = weaponDamage[1] = 0;
		elementalDamage[0] = elementalDamage[1] = 0;
		damageMultiplier = 1.0;
	}
};

struct CharWarrior {
	std::random_device rd;
	std::mt19937 * RNG;
	CharRace race;
	double stats[SIZE_STATTYPE];
	double damageMultipliers[SIZE_MAGICSCHOOL];
	double abilityModifiers[SIZE_ABILITYWARRIOR][SIZE_ABILITYMODIFIERS];	// 0 - normal damage multiplier, 1 - crit damage multiplier, 2 - extra crit chance
	int abilityCost[SIZE_ABILITYWARRIOR];
	int addedWeaponCrit[SIZE_ITEMTYPE];
	CharacterEvent events[SIZE_EVENTTYPE];
	Heap<CharacterEvent> * eventQueue;
	std::unordered_set< ProcType, std::hash<int> > MHProcs;
	std::unordered_set< ProcType, std::hash<int> > OHProcs;
	std::unordered_set< UseEffect, std::hash<int> > useEffects;
	std::unordered_map<std::string, long long> damagePerComponent;
	std::unordered_map<std::string, long long> auraUptime;
	std::unordered_map<std::string, long long> auraStartLast;
	long long rageOverflow;
	long long totalDamageDealt;
	CharWeapon MH;
	CharWeapon OH;
	int rage;
	int maxStanceSwapConservedRage;
	int castTime_Slam;
	int rageGainBerserkerRage;
	int currentTime;
	bool writeLog;
	int flurryCharges;
	int badgeOfSwarmguardCharges;
	int bonereaversEdgeCharges;
	int berserkingValue;
	int bloodFuryValue;
	int armorPenetration;
	AbilityWarrior spellBeingCast;
	int defaultLatency;
	int globalCooldownDuration;
	int numOnUseTrinkets;
	bool inBattleStance;
	int jomGabbarAddedAP;
	
	CharWarrior(const CharRace race = RACE_HUMAN, const Item ** itemArray = NULL, const char * talentBuildString = "2030501130200000000505000552501005100000000000000000", const char * buffString = "11111100000000100111");
	bool procActivated(int procChance);
	void doMHProcs(RollResult roll);
	void doOHProcs(RollResult roll);
	void addRage(const int val);
	void doSpell(const char * spellName = "UNKNOWN", MagicSchool school = SCHOOL_PHYSICAL, const int minVal = 0, const int maxVal = 0);
	ResistResult calculatePartialResist(MagicSchool school, int resistance);
	void doProcs(RollResult roll, bool isMH = true, bool isExtraAttack = false);
	RollResult rollSwingPet();
	RollResult rollSwingMainHand();
	RollResult rollSwingOffHand();
	RollResult rollYellow(AbilityWarrior abilityType);
	void reduceFlurryCharges();
	int getGlancingDamage(const int damage, const int weaponSkill);
	void doMainHandAttack(bool isExtraAttack = false);
	void doOffHandAttack();
	void recalculateDamage();
	void recalculateSpeed();
	void addBuffs(const char * buffString = NULL);
	void buildTalentTree(const char * talentBuildString = NULL);
	EventType getNextEventAndUpdateTime();
	bool isEventScheduled(EventType event);
	int getEventCooldown(EventType event);
	void setEventReady(EventType event);
	void cancelEvent(EventType event);
	void setEventCooldownFromNow(EventType event, int cooldownInMiliseconds);
	void delayUntilEvent(EventType event, EventType goal);
	void setDefaultRageCost();
	void addSetBonuses(std::unordered_map< SetBonus, int, std::hash<int> > & bonuses);
	void addRacialStatsAndSpells();
	int sampleAbilityDamage(AbilityWarrior abilityType, RollResult roll);
	void doYellowAttack(AbilityWarrior abilityType);
	void handleEvent(EventType event);
	void enterBattleStance();
	void enterBerserkerStance();
	void resetCharacter();
	void handleUseEffect(UseEffect ue);
	void printStats();
};

#endif