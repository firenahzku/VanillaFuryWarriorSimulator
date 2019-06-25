# WoW Fury DPS Simulator

## Rotation
* If **Mighty Rage Potion** is not on cooldown, and either (i) there are <= 22 seconds left until the end, or (ii) there are 25 seconds or less left and rage is at most 25; then use it.
* If **Cloudkeeper Legplates** are not on cooldown, and there are <= 32 seconds left until the end; then use it.
* If **Barov Peasant Caller** is not on cooldown, and there are <= 30 seconds left until the end; then use it.
* ASDFGH too lazy to write this down

## Things to note
* Method `CharWarrior::doSpell` assumes 15 resistance to all magic schools.
* Method `CharWarrior::doProcs` is missing implementation for Annihilator debuff effect. Armor penetration isn't used in the code since bosses have their armor reduced to 0 anyway.
* Troll racial Berserking always assumed only 10% haste gain.