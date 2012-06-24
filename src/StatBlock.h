/*
Copyright © 2011-2012 Clint Bellanger

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

const int STAT_EFFECT_SHIELD = 0;
const int STAT_EFFECT_VENGEANCE = 1;

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

public:
	StatBlock();
	~StatBlock();

	void load(const std::string& filename);
	void takeDamage(int dmg);
	void recalc();
	void logic();
	void clearEffects();
	Renderable getEffectRender(int effect_type);

	bool alive;
	bool corpse; // creature is dead and done animating
	bool hero; // else, enemy or other
	bool permadeath;
	bool transformed;

	int movement_type;
	bool flying;
	bool intangible;

	std::string name;
	std::string sfx_prefix;
	std::string gfx_prefix;

	int level;
	int xp;
	int xp_table[MAX_CHARACTER_LEVEL+1];

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
	int attunement_fire;
	int attunement_ice;

	// buff and debuff stats
	int slow_duration;
	int bleed_duration;
	int stun_duration;
	int immobilize_duration;
	int immunity_duration;
	int transform_duration;
	bool manual_untransform;
	int haste_duration;
	int hot_duration;
	int hot_value;
	int forced_move_duration;

	int shield_hp; // shield
	int shield_frame;
	bool blocking;
	int vengeance_stacks;
	int vengeance_frame;

	int speed;
	int dspeed;
	Point pos;
	Point forced_speed;
	int direction;
	int hero_cooldown[1024]; //TODO: fix this to use POWER_COUNT... right now it can't #include "PowerManager.h"

	// state
	int cur_state;

    // waypoint patrolling
    std::queue<Point> waypoints;
    int waypoint_pause;
    int waypoint_pause_ticks;

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

	int loot_chance;

	// for the teleport spell
	bool teleportation;
	Point teleport_destination;

	// weapons can modify spells
	int melee_weapon_power;
	int mental_weapon_power;
	int ranged_weapon_power;

	// for purchasing tracking
	int gold;

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
};

#endif

