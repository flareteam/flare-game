/*
Copyright © 2011-2012 Clint Bellanger
Copyright © 2012 Igor Paliychuk
Copyright © 2012 Stefan Beller

This file is part of FLARE.

FLARE is free software: you can redistribute it and/or modify it under the terms
of the GNU General Public License as published by the Free Software Foundation,
either version 3 of the License, or (at your option) any later version.

FLARE is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
PARTICULAR PURPOSE.  See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with
FLARE.  If not, see http://www.gnu.org/licenses/
*/

/**
 * class StatBlock
 *
 * Character stats and calculations
 */

#include "StatBlock.h"
#include "FileParser.h"
#include "PowerManager.h"
#include "SharedResources.h"
#include "Settings.h"
#include "UtilsParsing.h"
#include "MapCollision.h"
#include "MenuPowers.h"
#include <limits>

using namespace std;


StatBlock::StatBlock() {

	name = "";
	alive = true;
	corpse = false;
	corpse_ticks = 0;
	hero = false;
	humanoid = false;
	hero_pos.x = hero_pos.y = -1;
	hero_alive = true;
	permadeath = false;
	transform_type = "";
	transformed = false;
	refresh_stats = false;

	movement_type = MOVEMENT_NORMAL;
	flying = false;
	intangible = false;
	facing = true;
	suppress_hp = false;

	// core stats
	offense_character = defense_character = physical_character = mental_character = 0;
	offense_additional = defense_additional = physical_additional = mental_additional = 0;
	bonus_per_offense = bonus_per_defense = bonus_per_physical = bonus_per_mental = 0;
	physoff = physdef = mentoff = mentdef = 0;
	physment = offdef = 0;
	character_class="";
	level = 0;
	hp = maxhp = hp_per_minute = hp_ticker = 0;
	mp = maxmp = mp_per_minute = mp_ticker = 0;
	xp = 0;
	accuracy = 75;
	avoidance = 25;
	crit = 0;
	level_up = false;


	// equipment stats
	dmg_melee_min = dmg_melee_min_default = 1;
	dmg_melee_max = dmg_melee_max_default = 4;
	dmg_ment_min = dmg_ment_min_default = 1;
	dmg_ment_max = dmg_ment_max_default = 4;
	dmg_ranged_min = dmg_ranged_min_default = 1;
	dmg_ranged_max = dmg_ranged_max_default = 4;
	absorb_min = absorb_min_default = 0;
	absorb_max = absorb_max_default = 0;
	speed = speed_default = 14;
	dspeed = dspeed_default = 10;
	wielding_physical = false;
	wielding_mental = false;
	wielding_offense = false;

	// buff and debuff stats
	slow_duration = 0;
	slow_duration_total = 0;
	bleed_duration = 0;
	bleed_duration_total = 0;
	stun_duration = 0;
	stun_duration_total = 0;
	immobilize_duration = 0;
	immobilize_duration_total = 0;
	immunity_duration = 0;
	immunity_duration_total = 0;
	transform_duration = 0;
	transform_duration_total = 0;
	manual_untransform = false;
	haste_duration = 0;
	haste_duration_total = 0;
	hot_duration = 0;
	hot_duration_total = 0;
	hot_value = 0;
	forced_move_duration = 0;
	forced_move_duration_total = 0;
	shield_hp = 0;
	shield_hp_total = 0;
	cooldown_ticks = 0;
	blocking = false;
	effects = vector<Effect>();

	// patrol waypoints
	waypoint_pause = 0;
	waypoint_pause_ticks = 0;

	// wandering
	wander= false;
	wander_ticks = 0;
	wander_pause_ticks = 0;

	max_spendable_stat_points = 0;
	max_points_per_stat = 0;

	// xp table
	// default to MAX_INT
	for (int i=0; i<MAX_CHARACTER_LEVEL; i++) {
		xp_table[i] = std::numeric_limits<int>::max();
	}

	loot_chance = 50;
	item_classes = vector<string>();
	item_class_prob = vector<int>();
	item_class_prob_sum = 0;
	teleportation = false;

	for (int i=0; i<POWERSLOT_COUNT; i++) {
		power_chance[i] = 0;
		power_index[i] = 0;
		power_cooldown[i] = 0;
		power_ticks[i] = 0;
	}
	melee_range = 64;

	melee_weapon_power = 0;
	ranged_weapon_power = 0;
	mental_weapon_power = 0;

	vulnerable = std::vector<int>(ELEMENTS.size(), 100);

	currency = 0;
	death_penalty = false;

	// campaign status interaction
	defeat_status = "";
	quest_loot_requires = "";
	quest_loot_not = "";
	quest_loot_id = 0;
	first_defeat_loot = 0;

	// default hero base/option
	base="male";
	head="head_short";
	portrait="male01";

	// default animations
	animations = "";

	// default animation speed
	animationSpeed = 100;

	// default step sound
	sfx_step = "cloth";

	statsLoaded = false;
	// formula numbers. Used only for hero
	hp_base = 10;
	hp_per_level = 2;
	hp_per_physical = 8;
	hp_regen_base = 10;
	hp_regen_per_level = 1;
	hp_regen_per_physical = 4;
	mp_base = 10;
	mp_per_level = 2;
	mp_per_mental = 8;
	mp_regen_base = 10;
	mp_regen_per_level = 1;
	mp_regen_per_mental = 4;
	accuracy_base = 75;
	accuracy_per_level = 1;
	accuracy_per_offense = 5;
	avoidance_base = 25;
	avoidance_per_level = 1;
	avoidance_per_defense = 5;
	crit_base = 5;
	crit_per_level = 1;
}

/**
 * load a statblock, typically for an enemy definition
 */
void StatBlock::load(const string& filename) {
	FileParser infile;
	if (!infile.open(mods->locate(filename))) {
		fprintf(stderr, "Unable to open %s!\n", filename.c_str());
		return;
	}

	int num = 0;
	while (infile.next()) {
		if (isInt(infile.val)) num = toInt(infile.val);

		if (infile.key == "name") name = msg->get(infile.val);
		else if (infile.key == "humanoid") {
			if (infile.val == "true") humanoid = true;
		}
		else if (infile.key == "sfx_prefix") sfx_prefix = infile.val;

		else if (infile.key == "level") level = num;

		// enemy death rewards and events
		else if (infile.key == "xp") xp = num;
		else if (infile.key == "loot_chance") loot_chance = num;
		else if (infile.key == "item_class") {
			string str;
			while ((str = infile.nextValue()) != "") {
				if (!isInt(str)) {
					item_classes.push_back(str);
					item_class_prob.push_back(1);
					item_class_prob_sum++;
				}
				else {
					num = toInt(str);
					item_class_prob[item_classes.size()-1] = num;
					item_class_prob_sum += num - 1; // one was already added, so add one less
				}
			}
		}
		else if (infile.key == "defeat_status") defeat_status = infile.val;
		else if (infile.key == "first_defeat_loot") first_defeat_loot = num;
		else if (infile.key == "quest_loot") {
			quest_loot_requires = infile.nextValue();
			quest_loot_not = infile.nextValue();
			quest_loot_id = toInt(infile.nextValue());
		}
		// combat stats
		else if (infile.key == "hp") {
			hp = num;
			maxhp = num;
		}
		else if (infile.key == "mp") {
			mp = num;
			maxmp = num;
		}
		else if (infile.key == "cooldown") cooldown = num;
		else if (infile.key == "accuracy") accuracy = num;
		else if (infile.key == "avoidance") avoidance = num;
		else if (infile.key == "dmg_melee_min") dmg_melee_min = num;
		else if (infile.key == "dmg_melee_max") dmg_melee_max = num;
		else if (infile.key == "dmg_ment_min") dmg_ment_min = num;
		else if (infile.key == "dmg_ment_max") dmg_ment_max = num;
		else if (infile.key == "dmg_ranged_min") dmg_ranged_min = num;
		else if (infile.key == "dmg_ranged_max") dmg_ranged_max = num;
		else if (infile.key == "absorb_min") absorb_min = num;
		else if (infile.key == "absorb_max") absorb_max = num;

		// behavior stats
		else if (infile.key == "flying") {
			if (num == 1) flying = true;
		}
		else if (infile.key == "intangible") {
			if (num == 1) intangible = true;
		}
		else if (infile.key == "facing") {
			if (num == 0) facing = false;
		}

		else if (infile.key == "waypoint_pause") waypoint_pause = num;

		else if (infile.key == "speed") speed = num;
		else if (infile.key == "dspeed") dspeed = num;
		else if (infile.key == "turn_delay") turn_delay = num;
		else if (infile.key == "chance_pursue") chance_pursue = num;
		else if (infile.key == "chance_flee") chance_flee = num;

		else if (infile.key == "chance_melee_phys") power_chance[MELEE_PHYS] = num;
		else if (infile.key == "chance_melee_ment") power_chance[MELEE_MENT] = num;
		else if (infile.key == "chance_ranged_phys") power_chance[RANGED_PHYS] = num;
		else if (infile.key == "chance_ranged_ment") power_chance[RANGED_MENT] = num;
		else if (infile.key == "power_melee_phys") power_index[MELEE_PHYS] = num;
		else if (infile.key == "power_melee_ment") power_index[MELEE_MENT] = num;
		else if (infile.key == "power_ranged_phys") power_index[RANGED_PHYS] = num;
		else if (infile.key == "power_ranged_ment") power_index[RANGED_MENT] = num;
		else if (infile.key == "power_beacon") power_index[BEACON] = num;
		else if (infile.key == "cooldown_melee_phys") power_cooldown[MELEE_PHYS] = num;
		else if (infile.key == "cooldown_melee_ment") power_cooldown[MELEE_MENT] = num;
		else if (infile.key == "cooldown_ranged_phys") power_cooldown[RANGED_PHYS] = num;
		else if (infile.key == "cooldown_ranged_ment") power_cooldown[RANGED_MENT] = num;
		else if (infile.key == "power_on_hit") power_index[ON_HIT] = num;
		else if (infile.key == "power_on_death") power_index[ON_DEATH] = num;
		else if (infile.key == "power_on_half_dead") power_index[ON_HALF_DEAD] = num;
		else if (infile.key == "power_on_debuff") power_index[ON_DEBUFF] = num;
		else if (infile.key == "power_on_join_combat") power_index[ON_JOIN_COMBAT] = num;
		else if (infile.key == "chance_on_hit") power_chance[ON_HIT] = num;
		else if (infile.key == "chance_on_death") power_chance[ON_DEATH] = num;
		else if (infile.key == "chance_on_half_dead") power_chance[ON_HALF_DEAD] = num;
		else if (infile.key == "chance_on_debuff") power_chance[ON_DEBUFF] = num;
		else if (infile.key == "chance_on_join_combat") power_chance[ON_JOIN_COMBAT] = num;


		else if (infile.key == "melee_range") melee_range = num;
		else if (infile.key == "threat_range") threat_range = num;

		// animation stats
		else if (infile.key == "melee_weapon_power") melee_weapon_power = num;
		else if (infile.key == "mental_weapon_power") mental_weapon_power = num;
		else if (infile.key == "ranged_weapon_power") ranged_weapon_power = num;

		else if (infile.key == "animations") animations = infile.val;
		else if (infile.key == "animation_speed") animationSpeed = num;

		// hide enemy HP bar
		else if (infile.key == "suppress_hp") suppress_hp = num;

		for (unsigned int i=0; i<ELEMENTS.size(); i++) {
			if (infile.key == "vulnerable_" + ELEMENTS[i].name) vulnerable[i] = num;
		}
	}
	infile.close();
}

/**
 * Reduce temphp first, then hp
 */
void StatBlock::takeDamage(int dmg) {
	if (shield_hp > 0) {
		shield_hp -= dmg;
		if (shield_hp < 0) {
			hp += shield_hp;
			shield_hp = 0;
		}
	}
	else {
		hp -= dmg;
	}
	if (hp <= 0) {
		hp = 0;
		alive = false;
	}
}

/**
 * Recalc derived stats from base stats
 * Creatures might skip these formulas.
 */
void StatBlock::recalc() {

	if (!statsLoaded) loadHeroStats();

	refresh_stats = true;

	level = 0;
	for (int i=0; i<MAX_CHARACTER_LEVEL; i++) {
		if (xp >= xp_table[i])
			level=i+1;
	}

	int lev0 = level -1;
	int phys0 = get_physical() -1;
	int ment0 = get_mental() -1;
	int off0 = get_offense() -1;
	int def0 = get_defense() -1;

	hp = maxhp = hp_base + (hp_per_level * lev0) + (hp_per_physical * phys0);
	mp = maxmp = mp_base + (mp_per_level * lev0) + (mp_per_mental * ment0);
	hp_per_minute = hp_regen_base + (hp_regen_per_level * lev0) + (hp_regen_per_physical * phys0);
	mp_per_minute = mp_regen_base + (mp_regen_per_level * lev0) + (mp_regen_per_mental * ment0);
	accuracy = accuracy_base + (accuracy_per_level * lev0) + (accuracy_per_offense * off0);
	avoidance = avoidance_base + (avoidance_per_level * lev0) + (avoidance_per_defense * def0);
	crit = crit_base + (crit_per_level * lev0);

	physoff = get_physical() + get_offense();
	physdef = get_physical() + get_defense();
	mentoff = get_mental() + get_offense();
	mentdef = get_mental() + get_defense();
	physment = get_physical() + get_mental();
	offdef = get_offense() + get_defense();

	int stat_sum = get_physical() + get_mental() + get_offense() + get_defense();

	// TODO: These class names do. not get caught by xgettext, so figure out
	// a way to translate them.

	// determine class
	// if all four stats are max, Grand Master
	if (stat_sum >= 20)
		character_class = msg->get("Grand Master");
	// if three stats are max, Master
	else if (stat_sum >= 16)
		character_class = msg->get("Master");
	// if one attribute is much higher than the others, use the attribute class name
	else if (get_physical() > get_mental()+1 && get_physical() > get_offense()+1 && get_physical() > get_defense()+1)
		character_class = msg->get("Warrior");
	else if (get_mental() > get_physical()+1 && get_mental() > get_offense()+1 && get_mental() > get_defense()+1)
		character_class = msg->get("Wizard");
	else if (get_offense() > get_physical()+1 && get_offense() > get_mental()+1 && get_offense() > get_defense()+1)
		character_class = msg->get("Ranger");
	else if (get_defense() > get_physical()+1 && get_defense() > get_mental()+1 && get_defense() > get_offense()+1)
		character_class = msg->get("Paladin");
	// if there is no dominant attribute, use the dicipline class name
	else if (physoff > physdef && physoff > mentoff && physoff > mentdef && physoff > physment && physoff > offdef)
		character_class = msg->get("Rogue");
	else if (physdef > physoff && physdef > mentoff && physdef > mentdef && physdef > physment && physdef > offdef)
		character_class = msg->get("Knight");
	else if (mentoff > physoff && mentoff > physdef && mentoff > mentdef && mentoff > physment && mentoff > offdef)
		character_class = msg->get("Shaman");
	else if (mentdef > physoff && mentdef > physdef && mentdef > mentoff && mentdef > physment && mentdef > offdef)
		character_class = msg->get("Cleric");
	else if (physment > physoff && physment > physdef && physment > mentoff && physment > mentdef && physment > offdef)
		character_class = msg->get("Battle Mage");
	else if (offdef > physoff && offdef > physdef && offdef > mentoff && offdef > mentdef && offdef > physment)
		character_class = msg->get("Heavy Archer");
	// otherwise, use the generic name
	else character_class = msg->get("Adventurer");

}

/**
 * Process per-frame actions
 */
void StatBlock::logic() {

	// handle cooldowns
	if (cooldown_ticks > 0) cooldown_ticks--; // global cooldown

	for (int i=0; i<POWERSLOT_COUNT; i++) { // NPC/enemy powerslot cooldown
		if (power_ticks[i] > 0) power_ticks[i]--;
	}

	// HP regen
	if (hp_per_minute > 0 && hp < maxhp && hp > 0) {
		hp_ticker++;
		if (hp_ticker >= (60 * MAX_FRAMES_PER_SEC)/hp_per_minute) {
			hp++;
			hp_ticker = 0;
		}
	}

	// MP regen
	if (mp_per_minute > 0 && mp < maxmp && hp > 0) {
		mp_ticker++;
		if (mp_ticker >= (60 * MAX_FRAMES_PER_SEC)/mp_per_minute) {
			mp++;
			mp_ticker = 0;
		}
	}

	// handle buff/debuff durations
	if (slow_duration > 0)
		slow_duration--;
	if (bleed_duration > 0)
		bleed_duration--;
	if (stun_duration > 0)
		stun_duration--;
	if (immobilize_duration > 0)
		immobilize_duration--;
	if (immunity_duration > 0)
		immunity_duration--;
	if (haste_duration > 0)
		haste_duration--;
	if (hot_duration > 0)
		hot_duration--;
	if (forced_move_duration > 0)
		forced_move_duration--;
	if (transform_duration > 0)
		transform_duration--;

	// apply bleed
	if (bleed_duration % MAX_FRAMES_PER_SEC == 1) {
		takeDamage(1);
	}

	// apply healing over time
	if (hot_duration % MAX_FRAMES_PER_SEC == 1) {
		hp += hot_value;
		if (hp > maxhp) hp = maxhp;
	}

	// handle buff/debuff animations
	for (unsigned int i=0; i<effects.size(); i++) {
		effects[i].frame++;
	}
	// shield_frame++;
	// if (shield_frame == 12) shield_frame = 0;

	// set movement type
	// some creatures may shift between movement types
	if (intangible) movement_type = MOVEMENT_INTANGIBLE;
	else if (flying) movement_type = MOVEMENT_FLYING;
	else movement_type = MOVEMENT_NORMAL;

}

/**
 * Remove temporary buffs/debuffs
 */
void StatBlock::clearEffects() {
	immunity_duration = 0;
	immobilize_duration = 0;
	bleed_duration = 0;
	stun_duration = 0;
	shield_hp = 0;
	slow_duration = 0;
	haste_duration = 0;
	forced_move_duration = 0;
}

void StatBlock::addEffect(std::string effect, int icon) {
	for (unsigned int i=0; i<effects.size(); i++) {
		if (effects[i].type == effect) {
			return; // already have this one
		}
	}
	Effect e;
	e.type = effect;
	e.frame = 0;
	e.icon = icon;
	effects.push_back(e);
}

void StatBlock::removeEffect(std::string effect) {
	for (unsigned int i=0; i<effects.size(); i++) {
		if (effects[i].type == effect) {
			effects.erase(effects.begin()+i);
			return;
		}
	}
}

void StatBlock::updateEffects() {
	if (slow_duration == 0) removeEffect("slow");
	if (bleed_duration == 0) removeEffect("bleed");
	if (stun_duration == 0) removeEffect("stun");
	if (immobilize_duration == 0) removeEffect("immobilize");
	if (immunity_duration == 0) removeEffect("immunity");
	if (transform_duration == 0) removeEffect("transform");
	if (haste_duration == 0) removeEffect("haste");
	if (hot_duration == 0) removeEffect("hot");
	if (shield_hp == 0) removeEffect("shield");
	if (!blocking) removeEffect("block");
}

StatBlock::~StatBlock() {
}

bool StatBlock::canUsePower(const Power &power, unsigned powerid) const {
	MenuPowers *menu_powers = MenuPowers::getInstance();

	// needed to unlock shapeshifter powers
	if (transformed) return mp >= power.requires_mp;

	//don't use untransform power if hero is not transformed
	else if (power.spawn_type == "untransform" && !transformed) return false;
	else
	return (!power.requires_mental_weapon || wielding_mental)
		&& (!power.requires_offense_weapon || wielding_offense)
		&& (!power.requires_physical_weapon || wielding_physical)
		&& mp >= power.requires_mp
		&& menu_powers->meetsUsageStats(powerid);

}

void StatBlock::loadHeroStats() {
	// Redefine numbers from config file if present
	FileParser infile;
	if (!infile.open(mods->locate("engine/stats.txt"))) {
		fprintf(stderr, "Unable to open engine/stats.txt!\n");
		return;
	}

	while (infile.next()) {
		int value = toInt(infile.val);

		if (infile.key == "max_points_per_stat") {
			max_points_per_stat = value;
		} else if (infile.key == "hp_base") {
			hp_base = value;
		} else if (infile.key == "hp_per_level") {
			hp_per_level = value;
		} else if (infile.key == "hp_per_physical") {
			hp_per_physical = value;
		} else if (infile.key == "hp_regen_base") {
			hp_regen_base = value;
		} else if (infile.key == "hp_regen_per_level") {
			hp_regen_per_level = value;
		} else if (infile.key == "hp_regen_per_physical") {
			hp_regen_per_physical = value;
		} else if (infile.key == "mp_base") {
			mp_base = value;
		} else if (infile.key == "mp_per_level") {
			mp_per_level = value;
		} else if (infile.key == "mp_per_mental") {
			mp_per_mental = value;
		} else if (infile.key == "mp_regen_base") {
			mp_regen_base = value;
		} else if (infile.key == "mp_regen_per_level") {
			mp_regen_per_level = value;
		} else if (infile.key == "mp_regen_per_mental") {
			mp_regen_per_mental = value;
		} else if (infile.key == "accuracy_base") {
			accuracy_base = value;
		} else if (infile.key == "accuracy_per_level") {
			accuracy_per_level = value;
		} else if (infile.key == "accuracy_per_offense") {
			accuracy_per_offense = value;
		} else if (infile.key == "avoidance_base") {
			avoidance_base = value;
		} else if (infile.key == "avoidance_per_level") {
			avoidance_per_level = value;
		} else if (infile.key == "avoidance_per_defense") {
			avoidance_per_defense = value;
		} else if (infile.key == "crit_base") {
			crit_base = value;
		} else if (infile.key == "crit_per_level") {
			crit_per_level = value;
		} else if (infile.key == "dmg_melee_min") {
			dmg_melee_min = dmg_melee_min_default = value;
		} else if (infile.key == "dmg_melee_max") {
			dmg_melee_max = dmg_melee_max_default = value;
		} else if (infile.key == "dmg_ranged_min") {
			dmg_ranged_min = dmg_ranged_min_default = value;
		} else if (infile.key == "dmg_ranged_max") {
			dmg_ranged_max = dmg_ranged_max_default = value;
		} else if (infile.key == "dmg_ment_min") {
			dmg_ment_min = dmg_ment_min_default = value;
		} else if (infile.key == "dmg_ment_max") {
			dmg_ment_max = dmg_ment_max_default = value;
		} else if (infile.key == "absorb_min") {
			absorb_min = absorb_min_default = value;
		} else if (infile.key == "absorb_max") {
			absorb_max = absorb_max_default = value;
		} else if (infile.key == "speed") {
			speed = speed_default = value;
		} else if (infile.key == "dspeed") {
			dspeed = dspeed_default = value;
		} else if (infile.key == "bonus_per_physical") {
			bonus_per_physical = value;
		} else if (infile.key == "bonus_per_mental") {
			bonus_per_mental = value;
		} else if (infile.key == "bonus_per_offense") {
			bonus_per_offense = value;
		} else if (infile.key == "bonus_per_defense") {
			bonus_per_defense = value;
		} else if (infile.key == "sfx_step") {
			sfx_step = infile.val;
		}
	}
	infile.close();
	if (max_points_per_stat == 0) max_points_per_stat = max_spendable_stat_points / 4 + 1;
	statsLoaded = true;

	// Load the XP table as well
	if (!infile.open(mods->locate("engine/xp_table.txt"))) {
		fprintf(stderr, "Unable to open engine/xp_table.txt!\n");
		return;
	}
	while(infile.next()) {
		xp_table[toInt(infile.key) - 1] = toInt(infile.val);
	}
	max_spendable_stat_points = toInt(infile.key);
	infile.close();
}

void StatBlock::clearNegativeEffects() {
	slow_duration = 0;
	bleed_duration = 0;
	stun_duration = 0;
	immobilize_duration = 0;
}
