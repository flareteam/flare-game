/*
Copyright © 2011-2012 Clint Bellanger
Copyright © 2012 Igor Paliychuk

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


#pragma once
#ifndef STAT_BLOCK_H
#define STAT_BLOCK_H

#include "EffectManager.h"
#include "Utils.h"
#include <string>
#include <queue>

class Power;

const int POWERSLOT_COUNT = 10;
const int MELEE_PHYS = 0;
const int MELEE_MENT = 1;
const int RANGED_PHYS = 2;
const int RANGED_MENT = 3;
const int BEACON = 4;
const int ON_HIT = 5;
const int ON_DEATH = 6;
const int ON_HALF_DEAD = 7;
const int ON_DEBUFF = 8;
const int ON_JOIN_COMBAT = 9;

// active states
const int ENEMY_STANCE = 0;
const int ENEMY_MOVE = 1;
const int ENEMY_CHARGE = 2;
const int ENEMY_MELEE_PHYS = 3;
const int ENEMY_MELEE_MENT = 4;
const int ENEMY_RANGED_PHYS = 5;
const int ENEMY_RANGED_MENT = 6;
const int ENEMY_SPAWN = 7;
// interrupt states
const int ENEMY_BLOCK = 9;
const int ENEMY_HIT = 10;
const int ENEMY_DEAD = 11;
const int ENEMY_CRITDEAD = 12;
const int ENEMY_HALF_DEAD = 13;
const int ENEMY_JOIN_COMBAT = 14;

// final shared states
const int ENEMY_POWER = 15; // enemy performing a power. anim/sfx based on power


const int MAX_CHARACTER_LEVEL = 32;

class EnemyLoot {
public:
	int id;
	int chance;
	int count_min;
	int count_max;

	EnemyLoot()
		: id(0)
		, chance(0)
		, count_min(1)
		, count_max(1)
	{}
};

class StatBlock {
private:
	void loadHeroStats();
	bool statsLoaded;

public:
	StatBlock();
	~StatBlock();

	void load(const std::string& filename);
	void takeDamage(int dmg);
	void recalc();
	void recalc_alt();
	void calcBaseDmgAndAbs();
	void logic();

	bool alive;
	bool corpse; // creature is dead and done animating
	int corpse_ticks;
	bool hero; // else, enemy or other
	bool humanoid; // true for human, sceleton...; false for wyvern, snake...
	bool permadeath;
	bool transformed;
	bool refresh_stats;

	int movement_type;
	bool flying;
	bool intangible;
	bool facing; // does this creature turn to face the hero

	std::string name;
	std::string sfx_prefix;

	int level;
	int xp;
	int xp_table[MAX_CHARACTER_LEVEL+1];
	bool level_up;
	bool check_title;
	int stat_points_per_level;
	int power_points_per_level;

	// base stats ("attributes")
	int offense_character;
	int defense_character;
	int physical_character;
	int mental_character;

	// additional values to base stats, given by items
	int offense_additional;
	int defense_additional;
	int physical_additional;
	int mental_additional;

	// bonuses for base stats
	int bonus_per_physical;
	int bonus_per_mental;
	int bonus_per_offense;
	int bonus_per_defense;

	// getters for full base stats (character + additional)
	int get_offense()  const { return offense_character + offense_additional; }
	int get_defense()  const { return defense_character + defense_additional; }
	int get_physical() const { return physical_character + physical_additional; }
	int get_mental()   const { return mental_character + mental_additional; }

	// derived stats ("disciplines")
	int physoff() { return get_physical() + get_offense(); }
	int physdef() { return get_physical() + get_defense(); }
	int mentoff() { return get_mental() + get_offense(); }
	int mentdef() { return get_mental() + get_defense(); }
	int physment() { return get_physical() + get_mental(); }
	int offdef() { return get_offense() + get_defense(); }

	// in Flare there are no distinct character classes.
	// instead each class is given a descriptor based on their base stat builds
	std::string character_class;

	// physical stats
	int hp;
	int maxhp;
	int hp_per_minute;
	int hp_ticker;

	// mental stats
	int mp;
	int maxmp;
	int mp_per_minute;
	int mp_ticker;

	// offense stats
	int accuracy;

	// defense stats
	int avoidance;

	// overall stats
	int crit;

	// default equipment stats
	int dmg_melee_min_default;
	int dmg_melee_max_default;
	int dmg_ment_min_default;
	int dmg_ment_max_default;
	int dmg_ranged_min_default;
	int dmg_ranged_max_default;
	int absorb_min_default;
	int absorb_max_default;

	int speed_default;
	int dspeed_default;

	// equipment stats
	int dmg_melee_min;
	int dmg_melee_max;
	int dmg_ment_min;
	int dmg_ment_max;
	int dmg_ranged_min;
	int dmg_ranged_max;
	int absorb_min;
	int absorb_max;

	int speed;
	int dspeed;

	bool wielding_physical;
	bool wielding_mental;
	bool wielding_offense;
	std::vector<int> vulnerable;
	std::vector<int> vulnerable_base;

	// buff and debuff stats
	int transform_duration;
	int transform_duration_total;
	bool manual_untransform;
	bool transform_with_equipment;
	EffectManager effects;

	Point pos;
	Point forced_speed;
	char direction;
	std::vector<int> hero_cooldown;

	int poise;
	int poise_base;

	int cooldown_hit;
	int cooldown_hit_ticks;

	// state
	int cur_state;

	// waypoint patrolling
	std::queue<Point> waypoints;
	int waypoint_pause;
	int waypoint_pause_ticks;

	// wandering area
	bool wander;
	SDL_Rect wander_area;
	int wander_ticks;
	int wander_pause_ticks;

	// enemy behavioral stats
	int chance_pursue;
	int chance_flee;

	std::vector<int> powers_list;
	std::vector<int> powers_list_items;
	std::vector<int> powers_passive;
	std::vector<int> power_chance;
	std::vector<int> power_index;
	std::vector<int> power_cooldown;
	std::vector<int> power_ticks;

	bool canUsePower(const Power &power, unsigned powerid) const;

	int melee_range;
	int threat_range;
	Point hero_pos;
	bool hero_alive;
	int hero_stealth;
	Point last_seen;
	int turn_delay;
	int turn_ticks;
	bool in_combat;
	bool join_combat;
	int cooldown_ticks;
	int cooldown; // min. # of frames between abilities
	int activated_powerslot;
	bool on_half_dead_casted;
	bool suppress_hp; // hide an enemy HP bar

	std::vector<EnemyLoot> loot;

	// for the teleport spell
	bool teleportation;
	Point teleport_destination;

	// weapons can modify spells
	int melee_weapon_power;
	int mental_weapon_power;
	int ranged_weapon_power;

	// for purchasing tracking
	int currency;

	// marked for death
	bool death_penalty;

	// Campaign event interaction
	std::string defeat_status;
	std::string quest_loot_requires;
	std::string quest_loot_not;
	int quest_loot_id;
	int first_defeat_loot;

	// player look options
	std::string base; // folder in /images/avatar
	std::string head; // png in /images/avatar/[base]
	std::string portrait; // png in /images/portraits
	std::string transform_type;

	std::string animations;

	// default sounds
	std::string sfx_step;

	// formula numbers
	int max_spendable_stat_points;
	int max_points_per_stat;
	int hp_base;
	int hp_per_level;
	int hp_per_physical;
	int hp_regen_base;
	int hp_regen_per_level;
	int hp_regen_per_physical;
	int mp_base;
	int mp_per_level;
	int mp_per_mental;
	int mp_regen_base;
	int mp_regen_per_level;
	int mp_regen_per_mental;
	int accuracy_base;
	int accuracy_per_level;
	int accuracy_per_offense;
	int avoidance_base;
	int avoidance_per_level;
	int avoidance_per_defense;
	int crit_base;
	int crit_per_level;
};

#endif

