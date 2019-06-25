# WoW Fury DPS Simulator

`Created by Threewords of the guild "Vanguard", Kronos, Twinstar`

## How to compile
Just run `make`. g++ with C++11 support required.

## How to run
`./runHorde.sh` has examples. You can uncomment them to run the weapon combination listed at the end of the line.

`./simulator gear/hordePreMC.txt 2030501130200000000505000552501005200000000000000000 11111000000000100111 60 200000 0 4 "Annihilator" "Frostbite"`

Running the above line would start the simulator with:
* `hordePreMC.txt` gear equipped.
* Using talent built given by the string `2030501130200000000505000552501005200000000000000000` (every number represents number of talent points used in the respective talent slot, from the left to right, top to bottom).
* Buffs on the character represented by the string `11111000000000100111` (see common.h for which buff is represented by which index).
* `60` seconds fight duration
* `200000` fights will be ran one after the other, and results will be presented as the average of the observed numbers.
* `0` flag tells the simulator not to save full combat log (for all the fights) in `combatLog.txt` file. If you want to output combat log, change this to 1 (but make sure you set number of fights to just 1).
* `4` is the number code for Orc race (see common.h for all race codes). This affects racial abilities and base stats.
* `"Annihilator" "Frostbite"` represents the MH and OH weapon equipped.

If you want to use an item that is missing in the `itemList.txt` file, then you have to add it, following the same format as all other items there have.

## Tuning
* The code is currently tuned for Kronos III realm of Twinstar. This means that proc chances etc. are all based on data gathered from Kronos III.
* Proc chances can be changed in the character.cpp file (just CTRL+F "untamed fury" for example).
* AQ ranks of spells can be toggled on or off by commenting/uncommenting the lines of code in the `CharWarrior::addBuffs` and `CharWarrior::sampleAbilityDamage` (for heroic strike) functions.

## Rotation
* See `simulator.cpp` for the exact rotation.

## Things to note
* Method `CharWarrior::doSpell` assumes 15 resistance to all magic schools.
* Method `CharWarrior::doProcs` is missing implementation for Annihilator debuff effect. Armor penetration isn't used in the code since bosses have their armor reduced to 0 anyway.
* Troll racial Berserking always assumed only 10% haste gain.
