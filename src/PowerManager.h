/*
Copyright 2011 Clint Bellanger

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
 * class PowerManager
 *
 * Special code for handling spells, special powers, item effects, etc.
 */

#include <string>
#include <queue>
#include "SDL.h"
#include "SDL_image.h"
#include "SDL_mixer.h"
#include "Utils.h"
#include "StatBlock.h"
#include "Hazard.h"
#include "MapCollision.h"

#ifndef POWER_MANAGER_H
#define POWER_MANAGER_H

using namespace std;

const int POWER_COUNT = 1024;
const int POWER_MAX_GFX = 64;
const int POWER_MAX_SFX = 64;

const int POWTYPE_SINGLE = 2;
const int POWTYPE_MISSILE = 3;
const int POWTYPE_REPEATER = 4;
const int POWTYPE_EFFECT = 5;

const int POWSTATE_SWING = 0;
const int POWSTATE_CAST = 1;
const int POWSTATE_SHOOT = 2;
const int POWSTATE_BLOCK = 3;

const int BASE_DAMAGE_NONE = 0;
const int BASE_DAMAGE_MELEE = 1;
const int BASE_DAMAGE_RANGED = 2;
const int BASE_DAMAGE_MENT = 3;

// this elemental list covers the western 4, eastern 5, and classic rpg light vs. shadow
// TODO: user-defined element list?
const int ELEMENT_WOOD = 0;
const int ELEMENT_METAL = 1;
const int ELEMENT_WIND = 2;
const int ELEMENT_WATER = 3;
const int ELEMENT_EARTH = 4;
const int ELEMENT_FIRE = 5;
const int ELEMENT_SHADOW = 6;
const int ELEMENT_LIGHT = 7;

// when casting a spell/power, the hazard starting position is
// either the source (the avatar or enemy), the target (mouse click position),
// or melee range in the direction that the source is facing
const int STARTING_POS_SOURCE = 0;
const int STARTING_POS_TARGET = 1;
const int STARTING_POS_MELEE = 2;

// first 20 powers coincide with power tree
// TODO: remove this restriction
const int POWER_SHIELD = 11;
const int POWER_VENGEANCE = 17;
const int POWER_SPARK_BLOOD = 127;

struct Power {

	// base info
	int type; // what kind of activate() this is
	string name;
	string description;
	int icon; // just the number.  The caller menu will have access to the surface.
	int new_state; // when using this power the user (avatar/enemy) starts a new state
	bool face; // does the user turn to face the mouse cursor when using this power?
	int source_type; //hero, neutral, or enemy
	bool beacon; //true if it's just an ememy calling its allies

	// power requirements
	bool requires_physical_weapon;
	bool requires_offense_weapon;
	bool requires_mental_weapon;
	int requires_mp;
	bool requires_los; // line of sight
	bool requires_empty_target; // target square must be empty
	int requires_item;
	bool consumable;
	bool requires_targeting; // power only makes sense when using click-to-target
	int cooldown; // milliseconds before you can use the power again
	
	// animation info
	int gfx_index;
	int sfx_index;
	bool rendered;
	bool directional; // sprite sheet contains options for 8 directions, one per row
	int visual_random; // sprite sheet contains rows of random options
	int visual_option; // sprite sheet contains rows of similar effects.  use a specific option
	int aim_assist;
	int speed; // for missile hazards, map units per frame
	int lifespan; // how long the hazard/animation lasts
	int frame_loop;
	int frame_duration;
	Point frame_size;
	Point frame_offset;
	bool floor; // the hazard is drawn between the background and object layers
	int active_frame;
	bool complete_animation;

	// hazard traits
	bool use_hazard;
	bool no_attack;
	int radius;
	int base_damage; // enum.  damage is powered by melee, ranged, mental weapon
	int damage_multiplier; // % of base damage done by power (eg. 200 doubles damage and 50 halves it)
	int starting_pos; // enum. (source, target, or melee)
	bool multitarget;

	//steal effects (in %, eg. hp_steal=50 turns 50% damage done into HP regain.)
	int hp_steal;
	int mp_steal;
	
	//missile traits
	int missile_num;
	int missile_angle;
	int angle_variance;
	int speed_variance;

	//repeater traits
	int delay;
	int start_frame;
	int repeater_num;

	int trait_elemental; // enum. of elements
	bool trait_armor_penetration;
	int trait_crits_impaired; // crit bonus vs. movement impaired enemies (slowed, immobilized, stunned)
	
	int bleed_duration;
	int stun_duration;
	int slow_duration;
	int immobilize_duration;
	int immunity_duration;
	int haste_duration;
	int hot_duration;
	int hot_value;
	
	// special effects
	bool buff_heal;
	bool buff_shield;
	bool buff_teleport;
	bool buff_immunity;
	int buff_restore_hp;
	int buff_restore_mp;
	
	int post_power;
	int wall_power;
	bool allow_power_mod;
	
	Power() {
		type = -1;
		name = "";
		description = "";
		icon = -1;
		new_state = -1;
		face=false;
		source_type=-1;
		beacon=false;
		
		requires_physical_weapon = false;
		requires_offense_weapon = false;
		requires_mental_weapon = false;
		
		requires_mp = 0;
		requires_los = false;
		requires_empty_target = false;
		requires_item = -1;
		requires_targeting=false;
		cooldown = 0;
		
		gfx_index = -1;
		sfx_index = -1;
		rendered = false;
		directional = false;
		visual_random = 0;
		visual_option = 0;
		aim_assist = 0;
		speed = 0;
		lifespan = 0;
		frame_loop = 1;
		frame_duration = 1;
		frame_size.x = frame_size.y = 0;
		frame_offset.x = frame_offset.y = 0;
		floor = false;
		active_frame = -1;
		complete_animation = false;

		use_hazard = false;
		no_attack = false;
		radius = 0;
		starting_pos = STARTING_POS_SOURCE;
		base_damage = BASE_DAMAGE_NONE;
		damage_multiplier = 100;
		multitarget = false;

		hp_steal = 0;
		mp_steal = 0;

		missile_num = 1;
		missile_angle = 0;
		angle_variance = 0;
		speed_variance = 0;

		delay = 0;
		start_frame = 0;
		repeater_num = 1;
		
		trait_elemental = -1;
		trait_armor_penetration = false;
		trait_crits_impaired = 0;
		
		bleed_duration = 0;
		stun_duration = 0;
		slow_duration = 0;
		immobilize_duration = 0;
		immunity_duration = 0;
		haste_duration = 0;
		hot_duration = 0;
		hot_value = 0;
		
		buff_heal = false;
		buff_shield = false;
		buff_teleport = false;
		buff_immunity = false;
		buff_restore_hp = 0;
		buff_restore_mp = 0;
		
		post_power = -1;
		wall_power = -1;
		
		allow_power_mod = false;
	}	
	
};

class PowerManager {
private:
	
	MapCollision *collider;

	void loadPowers();
	void loadGraphics();
	
	int loadGFX(string filename);
	int loadSFX(string filename);
	string gfx_filenames[POWER_MAX_GFX];
	string sfx_filenames[POWER_MAX_SFX];
	int gfx_count;
	int sfx_count;
	float calcTheta(int x1, int y1, int x2, int y2);

	int calcDirection(int origin_x, int origin_y, int target_x, int target_y);
	void initHazard(int powernum, StatBlock *src_stats, Point target, Hazard *haz);
	void buff(int power_index, StatBlock *src_stats, Point target);
	void playSound(int power_index, StatBlock *src_stats);

	bool effect(int powernum, StatBlock *src_stats, Point target);
	bool missile(int powernum, StatBlock *src_stats, Point target);
	bool repeater(int powernum, StatBlock *src_stats, Point target);
	bool single(int powernum, StatBlock *src_stats, Point target);

public:
	PowerManager();
	~PowerManager();

	void handleNewMap(MapCollision *_collider);
	bool activate(int power_index, StatBlock *src_stats, Point target);

	StatBlock *src_stats;
	Power powers[POWER_COUNT];
	queue<Hazard *> hazards; // output; read by HazardManager

	// shared images/sounds for power special effects
	SDL_Surface *gfx[POWER_MAX_GFX];
	Mix_Chunk *sfx[POWER_MAX_SFX];
	
	SDL_Surface *freeze;
	SDL_Surface *runes;
	
	int used_item;
	
	Mix_Chunk *sfx_freeze;
};

#endif
