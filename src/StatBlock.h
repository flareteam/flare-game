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

#ifndef STAT_BLOCK_H
#define STAT_BLOCK_H

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
const int ENEMY_DEBUFF = 14;
const int ENEMY_JOIN_COMBAT = 15;

// final shared states
const int ENEMY_POWER = 16; // enemy performing a power. anim/sfx based on power


const int MAX_CHARACTER_LEVEL = 32;

class StatBlock {
private:
	void loadHeroStats();
	bool statsLoaded;

public:
	StatBlock();
	~StatBlock();

	struct Effect {
		std::string type;
		int frame;
		int icon;
	};

	void load(const std::string& filename);
	void takeDamage(int dmg);
	void recalc();
	void logic();
	void clearEffects();
	void addEffect(std::string effect, int icon);
	void removeEffect(std::string effect);
	void updateEffects();

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
	int physoff;
	int physdef;
	int mentoff;
	int mentdef;
	int physment;
	int offdef;

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
	bool wielding_physical;
	bool wielding_mental;
	bool wielding_offense;
	bool ammo_arrows;
	std::vector<int> vulnerable;

	// buff and debuff stats
	int slow_duration;
	int slow_duration_total;
	int bleed_duration;
	int bleed_duration_total;
	int stun_duration;
	int stun_duration_total;
	int immobilize_duration;
	int immobilize_duration_total;

	void clearNegativeEffects();

	int immunity_duration;
	int immunity_duration_total;
	int transform_duration;
	int transform_duration_total;
	bool manual_untransform;
	int haste_duration;
	int haste_duration_total;
	int hot_duration;
	int hot_duration_total;
	int hot_value;
	int forced_move_duration;
	int forced_move_duration_total;

	int shield_hp; // shield
	int shield_hp_total;
	bool blocking;
	std::vector<Effect> effects;

	int speed;
	int dspeed;
	Point pos;
	Point forced_speed;
	char direction;
	std::vector<int> hero_cooldown;

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

	int power_chance[POWERSLOT_COUNT];
	int power_index[POWERSLOT_COUNT];
	int power_cooldown[POWERSLOT_COUNT];
	int power_ticks[POWERSLOT_COUNT];

	bool canUsePower(const Power &power, unsigned powerid) const;

	int melee_range;
	int threat_range;
	Point hero_pos;
	bool hero_alive;
	Point last_seen;
	int turn_delay;
	int turn_ticks;
	bool in_combat;
    bool join_combat;
	int cooldown_ticks;
	int cooldown; // min. # of frames between abilities
	int activated_powerslot;
	bool suppress_hp; // hide an enemy HP bar

	int loot_chance;
	std::vector<std::string> item_classes; // which kind of loot is able to be dropped
	// the strings given in item_class correspond to the item class
	std::vector<int> item_class_prob;      // weights for each kind of drop.
	int item_class_prob_sum;               // sum of all loot_prob entries.

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
	int animationSpeed;

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

