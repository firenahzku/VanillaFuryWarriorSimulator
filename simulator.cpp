#include <iostream>
#include <algorithm>
#include <cstdio>
#include <cstring>
#include "items.h"
#include "character.h"

// char talentBuild_fury_Slam[] = "2030501132200000000505000500551005100000000000000000";
// char buffs_alliance_noAQ_noworldbuffs[] = "11111100000011000111";

bool timeFrameFits(const int timeLeft, const int cooldown, const int duration) {
	return (timeLeft % cooldown <= duration);
}

int executeAWarriorFight(CharWarrior & ch, int fightDuration);

int main(int argc, char * argv[]) {
	if (argc < 9) {
		fprintf(stdout, "Usage: ./simulator [gear file] [talentString] [buffString] [fight duration in seconds] [number of experiments] [1 or 0 flag for logging] [race] [MH weapon] [OH weapon]\n\n");
		return 0;
	}
	fprintf(stderr, "%s %s\n", argv[8], argv[9]);
	readItemsFromDB("itemList.txt");
	Item ** gear = new Item * [SIZE_INVENTORYSLOT];
	readGear(argv[1], gear);
	if (!items.count(argv[8])) {
		fprintf(stderr, "<Error> Item %s not found in the item list.", argv[8]);
		exit(0);
	}
	if (!items.count(argv[9])) {
		fprintf(stderr, "<Error> Item %s not found in the item list.", argv[9]);
		exit(0);
	}
	gear[SLOT_MH] = &items[argv[8]];
	gear[SLOT_OH] = &items[argv[9]];
	if (gear[SLOT_OH] == &items["None"]) {
		gear[SLOT_OH_ENCHANTMENT] = &items["None"];
		gear[SLOT_OH_ENCHANTMENT_TEMP] = &items["None"];
	}
	int race;
	sscanf(argv[7], "%d", &race);
	CharWarrior ch(CharRace(race), (const Item **)gear, argv[2], argv[3]);
	ch.printStats();
	int seconds;
	int iterations;
	int log;
	sscanf(argv[4], "%d", &seconds);
	sscanf(argv[5], "%d", &iterations);
	sscanf(argv[6], "%d", &log);
	double averageDPS = 0;
	int lastProgress = 0;
	long long totalRageOverflow = 0;
	for (int i = 0; i < iterations; i++) {
		int progress = 1000.0 * double(i + 1) / double(iterations);
		if (progress > lastProgress) {
			lastProgress = progress;
			fprintf(stderr, "\r%.1lf%%", progress / 10.0);
		}
		ch.writeLog = log;
		double r = executeAWarriorFight(ch, seconds * 1000) / double(seconds);
		// fprintf(stderr, "%.2lf\n", r);
		averageDPS += r;
		totalRageOverflow += ch.rageOverflow;
		ch.resetCharacter();
		// ch.printStats();
	}
	double resultingDPS = averageDPS / double(iterations);
	long long totalDamage = 0;
	for (auto it : ch.damagePerComponent) {
		totalDamage += it.second;
	}
	fprintf(stderr, "\r%20s\t%7.2lf\n\n", "Average DPS", resultingDPS);
	for (auto it : ch.damagePerComponent) {
		fprintf(stderr, "%20s\t%.2lf DPS\t%.1lf%%\n", it.first.c_str(), double(it.second) / double(seconds * iterations), 100.0 * double(it.second) / totalDamage);
	}
	fprintf(stderr, "\n");
	for (auto it : ch.auraUptime) {
		fprintf(stderr, "%20s\t%.1lf%%\n", it.first.c_str(), 100.0 * double(it.second / 1000.0) / double(seconds * iterations));
		// fprintf(stderr, "%20s\t%lld\n", it.first.c_str(), it.second);
	}
	fprintf(stderr, "\r%20s\t%9lld\n\n", "Rage overflow", totalRageOverflow);
	char command[1000];
	sprintf(command, "echo \"%s + %s\t%d\" >> log.txt", argv[8], argv[9], (int)round(resultingDPS));
	system(command);
	return 0;
}

int executeAWarriorFight(CharWarrior & ch, int fightDuration) {
	bool flag = ch.writeLog;
	ch.writeLog = false;
	ch.resetCharacter();
	ch.writeLog = flag;
	while (ch.currentTime < fightDuration) {
		EventType event = ch.getNextEventAndUpdateTime();
		if (ch.writeLog)
			fprintf(stderr, "%d\tRage: %d\n", int(event), ch.rage);
		// if (ch.writeLog) {
		// 	fprintf(stderr, "MH speed: %.3lf\n", ch.MH.speed);
		// 	fprintf(stderr, "MH base speed: %.3lf\n", ch.MH.baseWeaponSpeed);
		// 	for (int i = 0; i < SIZE_STATTYPE; i++) {
		// 		fprintf(stderr, "%.3lf\t", ch.stats[i]);
		// 	}
		// 	fprintf(stderr, "\n");
		// 	// fprintf(stderr, "Haste coeff: %.3lf\n", ch.stats[STAT_HASTECOEFFICIENT]);
		// }
		// Calculate parameters needed to determine the next step
		double timeLeft = fightDuration - ch.currentTime;
		// Trinkets, Cloudkeeper leggings, Mighty rage potion, Deah Wish, Bloodrage and Berserking should be used whenever their duration matches time left, as they are not on global cooldown
		for (UseEffect ue : ch.useEffects) {
			if (ue == USE_ITEM_MIGHTYRAGE && !ch.isEventScheduled(EVENT_MightyRagePotionCooldown) && (timeFrameFits(timeLeft, 120000, 23000) || timeFrameFits(timeLeft, 120000, 25000) && ch.rage <= 25)) {
				// fprintf(stderr, "Mighty Rage Potion\n");
				ch.handleUseEffect(ue);
			}
			else if (ue == USE_ITEM_CLOUDKEEPER && !ch.isEventScheduled(EVENT_CloudkeeperCooldown) && timeFrameFits(timeLeft, 900000, 33000)) {
				// fprintf(stderr, "Cloudkeeper Leggings\n");
				ch.handleUseEffect(ue);
			}
			else if (ue == USE_ITEM_BAROV && !ch.isEventScheduled(EVENT_BarovPeasantCallerCooldown) && timeFrameFits(timeLeft, 600000, 30000)) {// Does not share cooldown with other on-use trinkets
				// fprintf(stderr, "Barov Peasant Caller\n");
				ch.handleUseEffect(ue);
			}
			else if (ue == USE_SPELL_BLOODRAGE && !ch.isEventScheduled(EVENT_BloodrageCooldown) && ch.rage < 90 && timeFrameFits(timeLeft, 60000, 13000)) {
				// fprintf(stderr, "Bloodrage\n");
				ch.handleUseEffect(ue);
			}
			else if (ue == USE_SPELL_DEATHWISH && ch.rage >= 5 && !ch.isEventScheduled(EVENT_DeathWishCooldown) && timeFrameFits(timeLeft, 180000, 30000)) {
				// fprintf(stderr, "Death Wish\n");
				ch.handleUseEffect(ue);
			}
			else if (ue == USE_SPELL_BERSERKING && ch.rage >= 5 && !ch.isEventScheduled(EVENT_BerserkingCooldown) && timeFrameFits(timeLeft, 180000, 13000)) {
				// fprintf(stderr, "Berserking\n");
				ch.handleUseEffect(ue);
			}
			else if (!ch.isEventScheduled(EVENT_TrinketCooldown)) {
				// Order of trinket strength (stronger trinkets need to be used later): KOTS > JG > ES > SC > DF
				if (ue == USE_ITEM_SLAYERSCREST && !ch.isEventScheduled(EVENT_SlayersCrestCooldown) && (ch.numOnUseTrinkets == 2 && ch.useEffects.count(USE_ITEM_KISSOFTHESPIDER) && timeFrameFits(timeLeft, 300000, 38000) || timeFrameFits(timeLeft, 300000, 23000))) {
					// fprintf(stderr, "Slayer's Crest\n");
					ch.handleUseEffect(ue);
				}
				else if (ue == USE_ITEM_EARTHSTRIKE && !ch.isEventScheduled(EVENT_EarthstrikeCooldown) && (ch.numOnUseTrinkets == 2 && ch.useEffects.count(USE_ITEM_KISSOFTHESPIDER) && timeFrameFits(timeLeft, 300000, 38000) || timeFrameFits(timeLeft, 300000, 23000))) {
					// fprintf(stderr, "Earthstrike\n");
					ch.handleUseEffect(ue);
				}
				else if (ue == USE_ITEM_JOMGABBAR && !ch.isEventScheduled(EVENT_JomGabbarCooldown) && (ch.numOnUseTrinkets == 2 && ch.useEffects.count(USE_ITEM_KISSOFTHESPIDER) && timeFrameFits(timeLeft, 300000, 38000) || timeFrameFits(timeLeft, 300000, 23000))) {
					// fprintf(stderr, "Jom Gabbar\n");
					ch.handleUseEffect(ue);
				}
				else if (ue == USE_ITEM_KISSOFTHESPIDER && !ch.isEventScheduled(EVENT_KissOfTheSpiderCooldown) && timeFrameFits(timeLeft, 300000, 18000)) {
					// fprintf(stderr, "Kiss of the Spider\n");
					ch.handleUseEffect(ue);
				}
			}
		}
		double cdBloodthirst = ch.isEventScheduled(EVENT_BloodthirstCooldown) ? ch.getEventCooldown(EVENT_BloodthirstCooldown) : 0;
		double cdWhirlwind = ch.isEventScheduled(EVENT_WhirlwindCooldown) ? ch.getEventCooldown(EVENT_WhirlwindCooldown) : 0;

		double MHSwingTimer = ch.getEventCooldown(EVENT_MHSwing);
		double OHSwingTimer = ch.getEventCooldown(EVENT_OHSwing);
		bool stanceReady = !ch.isEventScheduled(EVENT_StanceCooldown);
		// Stance swap for Overpower
		if (ch.inBattleStance && stanceReady && (std::min(MHSwingTimer, OHSwingTimer) < cdBloodthirst || ch.rage <= ch.maxStanceSwapConservedRage))
			ch.enterBerserkerStance();
		// Handling GCD
		bool is2h = (ch.OH.type == ITEM_NONE);
		if (event == EVENT_GlobalCooldown) {
			if (ch.writeLog)
				fprintf(stderr, "GCD at %d.%03d\n", ch.currentTime / 1000, ch.currentTime % 1000);
			ch.cancelEvent(event);
			// Use Blood Fury if you can
			if (ch.useEffects.count(USE_ITEM_DIAMONDFLASK) && !ch.isEventScheduled(EVENT_DiamondFlaskCooldown) && (ch.numOnUseTrinkets == 2 && (ch.useEffects.count(USE_ITEM_KISSOFTHESPIDER) && timeFrameFits(timeLeft, 300000, 78000) || timeFrameFits(timeLeft, 300000, 83000)) || timeFrameFits(timeLeft, 300000, 63000))) {
				// fprintf(stderr, "Diamond Flask\n");
				ch.handleUseEffect(USE_ITEM_DIAMONDFLASK);
			}
			else if (ch.useEffects.count(USE_SPELL_BLOODFURY) && !ch.isEventScheduled(EVENT_BloodFuryCooldown) && timeFrameFits(timeLeft, 120000, 18000)) {
				// fprintf(stderr, "Blood Fury\n");
				ch.handleUseEffect(USE_SPELL_BLOODFURY);
			}
			// Always execute if you can.
			else if (timeLeft <= 0.12 * fightDuration && ch.rage >= ch.abilityCost[ABILITY_EXECUTE])	// Assuming that the target is at 20% health when there is 12% time left.
				ch.doYellowAttack(ABILITY_EXECUTE);
			// Otherwise, try to Bloodthirst.
			else if (!ch.isEventScheduled(EVENT_BloodthirstCooldown) && ch.rage >= ch.abilityCost[ABILITY_BLOODTHIRST]) {
				// fprintf(stderr, "Bloodthirst\n");
				ch.doYellowAttack(ABILITY_BLOODTHIRST);
			}
			// If we just did an auto attack (or heroic strike), or an extra attack just procced, see if we should Slam in case of using a 2h weapon
			else if (is2h && MHSwingTimer >= ch.globalCooldownDuration && ch.rage >= std::max(40, ch.abilityCost[ABILITY_SLAM]) && cdBloodthirst >= ch.globalCooldownDuration) {
				if (ch.writeLog)
					fprintf(stderr, "Slam queued up at %d.%03d.\n", ch.currentTime / 1000, ch.currentTime % 1000);
				ch.setEventCooldownFromNow(EVENT_GlobalCooldown, ch.globalCooldownDuration);
				ch.spellBeingCast = ABILITY_SLAM;
				ch.setEventCooldownFromNow(EVENT_CastFinished, ch.castTime_Slam);
				// if (ch.writeLog)
				// 	fprintf(stderr, "Will happen in %d (at %d).\n", ch.getEventCooldown(EVENT_CastFinished), ch.events[EVENT_CastFinished].timestamp);
				// ch.doYellowAttack(ABILITY_SLAM);
			}
			// Otherwise, whirlwind if Bloodthirst will not be ready within the global cooldown.
			else if (!ch.isEventScheduled(EVENT_WhirlwindCooldown) && ch.rage >= ch.abilityCost[ABILITY_WHIRLWIND] && cdBloodthirst >= ch.globalCooldownDuration) {
				// fprintf(stderr, "Whirlwind\n");
				ch.doYellowAttack(ABILITY_WHIRLWIND);
			}
			// GCD seems free, so let's try Overpower.
			else if (stanceReady && !ch.isEventScheduled(EVENT_OverpowerCooldown) && ch.isEventScheduled(EVENT_OverpowerWindowClosed) && ch.rage >= ch.abilityCost[ABILITY_OVERPOWER] && ch.rage <= 50 && cdBloodthirst >= ch.globalCooldownDuration) {
				// fprintf(stderr, "Overpower\n");
				ch.enterBattleStance();
				ch.doYellowAttack(ABILITY_OVERPOWER);
			}
			// Berserker rage if we can spare the cooldown, for extra rage.
			else if (ch.useEffects.count(USE_SPELL_BERSERKERRAGE) && !ch.isEventScheduled(EVENT_BerserkerRageCooldown) && timeFrameFits(timeLeft, 30000, 3000) && ch.rage <= 50) {
				// fprintf(stderr, "Berserker rage\n");
				ch.handleUseEffect(USE_SPELL_BERSERKERRAGE);
			}
			// Nothing else to do, so let's hamstring and hope for some procs.
			else if (ch.rage >= std::max(ch.abilityCost[ABILITY_HAMSTRING], 25) && cdBloodthirst >= ch.globalCooldownDuration && (MHSwingTimer >= ch.globalCooldownDuration + ch.defaultLatency)) {
			// else if (ch.rage >= std::max(ch.abilityCost[ABILITY_HAMSTRING], 25) && cdBloodthirst >= ch.globalCooldownDuration && cdWhirlwind >= ch.globalCooldownDuration && (MHSwingTimer >= ch.globalCooldownDuration + ch.defaultLatency)) {
				// fprintf(stderr, "Hamstring\n");
				ch.doYellowAttack(ABILITY_HAMSTRING);
			}
			else {
				// fprintf(stderr, "Doing nothing\n");
				// fprintf(stderr, "Bloodthirst cooldown: %.3lf\n", cdBloodthirst);
			}
		}
		else
			ch.handleEvent(event);	// Let the default event handler take care of the event's default behaviour
		if (ch.writeLog)
			getchar();	// To pause combat log
		// if (ch.stats[STAT_HASTECOEFFICIENT] < 1) {
		// 	fprintf(stderr, "< Error > Stats coefficient is %.3lf\n", ch.stats[STAT_HASTECOEFFICIENT]);
		// 	fprintf(stderr, "\tCurrent time %.3lf\n", ch.currentTime);
		// 	fprintf(stderr, "\tHappened after processing event %d\n", (int)event);
		// 	exit(0);
		// }
	}
	return ch.totalDamageDealt;
}