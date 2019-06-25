# WoW Fury DPS Simulator

`Created by Threewords of the guild "Vanguard", Kronos, Twinstar`

## How to compile
Just run `make`. g++ with C++11 support required.

## How to run
`./runHorde.sh` has examples. You can uncomment them.

## Tuning (with classic beta available)
* The code is currently tuned for Kronos III realm of Twinstar. This means that the hit table formulae, proc chances etc. are all based on data gathered from Kronos III.
* Proc chances can be changed in the character.cpp file (just CTRL+F "untamed fury" for example).
* Dodge formula can be adjusted in the `CharWarrior::CharWarrior` constructor function (lines 189-190).
* `CharWarrior::rollYellow` function needs to be altered to have critical hits on a second roll after the first roll determines that the attack wasn't missed or dodged.
* Miss chance formula can be adjusted in the `CharWarrior::recalculateDamage` function.
* AQ ranks of spells can be toggled on or off by commenting/uncommenting the lines of code in the `CharWarrior::addBuffs` and `CharWarrior::sampleAbilityDamage` (for heroic strike) functions.

## Rotation
* If **Mighty Rage Potion** is not on cooldown, and either (i) there are <= 22 seconds left until the end, or (ii) there are 25 seconds or less left and rage is at most 25; then use it.
* If **Cloudkeeper Legplates** are not on cooldown, and there are <= 32 seconds left until the end; then use it.
* If **Barov Peasant Caller** is not on cooldown, and there are <= 30 seconds left until the end; then use it.
* ASDFGH too lazy to write this down

## Things to note
* Method `CharWarrior::doSpell` assumes 15 resistance to all magic schools.
* Method `CharWarrior::doProcs` is missing implementation for Annihilator debuff effect. Armor penetration isn't used in the code since bosses have their armor reduced to 0 anyway.
* Troll racial Berserking always assumed only 10% haste gain.
