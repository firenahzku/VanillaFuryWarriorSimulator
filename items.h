#ifndef __ITEMS_H_INCLUDED__
#define __ITEMS_H_INCLUDED__

#include <unordered_map>
#include <cstdio>
#include <cstring>
#include "common.h"

struct Item {
	ItemType type;
	double weaponSpeed;
	int itemStats[SIZE_ITEMSTATTYPE];
	SetBonus setBonus;
	ProcType proc;
	UseEffect use;

	// Item(){}
	// Item(ItemType type, int damageLower, int damageUpper, int elementalLower, int elementalUpper, double weaponSpeed, int weaponAllDamage, int elementalAllDamage, int strength, int agility, int attackPower, int meleeCrit, int meleeHit, int spellCrit, int spellHit, int weaponSkillSwords, int weaponSkill2hSwords, int weaponSkillMaces, int weaponSkill2hMaces, int weaponSkillAxes, int weaponSkill2hAxes, int weaponSkillDaggers, int weaponSkillFists, int weaponSkillStaves, int weaponSkillPolearms, int haste, int armor, SetBonus setBonus, ProcType proc, UseEffect use) : type(type), damageLower(damageLower), damageUpper(damageUpper), elementalLower(elementalLower), elementalUpper(elementalUpper), weaponSpeed(weaponSpeed), weaponAllDamage(weaponAllDamage), elementalAllDamage(elementalAllDamage), strength(strength), agility(agility), attackPower(attackPower), meleeCrit(meleeCrit), meleeHit(meleeHit), spellCrit(spellCrit), spellHit(spellHit), weaponSkillSwords(weaponSkillSwords), weaponSkill(weaponSkill)2hSwords(hSwords), weaponSkillMaces(weaponSkillMaces), weaponSkill(weaponSkill)2hMaces(hMaces), weaponSkillAxes(weaponSkillAxes), weaponSkill(weaponSkill)2hAxes(hAxes), weaponSkillDaggers(weaponSkillDaggers), weaponSkillFists(weaponSkillFists), weaponSkillStaves(weaponSkillStaves), weaponSkillPolearms(weaponSkillPolearms), haste(haste), armor(armor), setBonus(setBonus), proc(proc), use(use) {}

	void readData(char * buffer);

	void debug_print();
};

extern std::unordered_map<std::string, Item> items;

void readItemsFromDB(const char * filename);
void readGear(const char * filename, Item ** gear);

#endif