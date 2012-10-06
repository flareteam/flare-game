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
 * class PowerManager
 *
 * Special code for handling spells, special powers, item effects, etc.
 */

#ifndef POWER_MANAGER_H
#define POWER_MANAGER_H

#include "MapRenderer.h"
#include "Utils.h"

#include <SDL.h>
#include <SDL_image.h>
#include <SDL_mixer.h>

#include <string>
#include <queue>
#include <cassert>
#include <vector>

class Animation;
class AnimationSet;
class Hazard;
class MapCollision;
class StatBlock;

const int POWER_COUNT = 1024;

const int POWTYPE_EFFECT = 0;
const int POWTYPE_MISSILE = 1;
const int POWTYPE_REPEATER = 2;
const int POWTYPE_SPAWN = 3;
const int POWTYPE_TRANSFORM = 4;

const int POWSTATE_SWING = 0;
const int POWSTATE_CAST = 1;
const int POWSTATE_SHOOT = 2;
const int POWSTATE_BLOCK = 3;
const int POWSTATE_INSTANT = 4;

const int BASE_DAMAGE_NONE = 0;
const int BASE_DAMAGE_MELEE = 1;
const int BASE_DAMAGE_RANGED = 2;
const int BASE_DAMAGE_MENT = 3;

// when casting a spell/power, the hazard starting position is
// either the source (the avatar or enemy), the target (mouse click position),
// or melee range in the direction that the source is facing
const int STARTING_POS_SOURCE = 0;
const int STARTING_POS_TARGET = 1;
const int STARTING_POS_MELEE = 2;

// TODO: maybe move this to an effect?
const int POWER_SPARK_BLOOD = 127;

class Power {
public:
	// base info
	int type; // what kind of activate() this is
	std::string name;
	std::string description;
	int icon; // just the number.  The caller menu will have access to the surface.
	int new_state; // when using this power the user (avatar/enemy) starts a new state
	bool face; // does the user turn to face the mouse cursor when using this power?
	int source_type; //hero, neutral, or enemy
	bool beacon; //true if it's just an ememy calling its allies
	int count; // number of hazards/effects or spawns created

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
	AnimationSet *animationSet;
	int sfx_index;
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
	int forced_move_speed;
	int forced_move_duration;
	int range;

	//steal effects (in %, eg. hp_steal=50 turns 50% damage done into HP regain.)
	int hp_steal;
	int mp_steal;

	//missile traits
	int missile_angle;
	int angle_variance;
	int speed_variance;

	//repeater traits
	int delay;
	int start_frame;

	int trait_elemental; // enum. of elements
	bool trait_armor_penetration;
	int trait_crits_impaired; // crit bonus vs. movement impaired enemies (slowed, immobilized, stunned)

	int bleed_duration;
	int stun_duration;
	int slow_duration;
	int immobilize_duration;
	int immunity_duration;
	int transform_duration;
	bool manual_untransform; // true binds to the power another recurrence power
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

	// spawn info
	std::string spawn_type;
	int target_neighbor;

	Power()
		: type(-1)
		, name("")
		, description("")
		, icon(-1)
		, new_state(-1)
		, face(false)
		, source_type(-1)
		, beacon(false)
		, count(1)

		, requires_physical_weapon(false)
		, requires_offense_weapon(false)
		, requires_mental_weapon(false)

		, requires_mp(0)
		, requires_los(false)
		, requires_empty_target(false)
		, requires_item(-1)
		, consumable(false)
		, requires_targeting(false)
		, cooldown(0)

		, animationSet(NULL)
		, sfx_index(-1)
		, directional(false)
		, visual_random(0)
		, visual_option(0)
		, aim_assist(0)
		, speed(0)
		, lifespan(0)
		, frame_loop(1)
		, frame_duration(1)
		, frame_size(Point())
		, frame_offset(Point())
		, floor(false)
		, active_frame(-1)
		, complete_animation(false)

		, use_hazard(false)
		, no_attack(false)
		, radius(0)
		, base_damage(BASE_DAMAGE_NONE)
		, damage_multiplier(100)
		, starting_pos(STARTING_POS_SOURCE)
		, multitarget(false)
		, forced_move_speed(0)
		, forced_move_duration(0)
		, range(0)

		, hp_steal(0)
		, mp_steal(0)

		, missile_angle(0)
		, angle_variance(0)
		, speed_variance(0)

		, delay(0)
		, start_frame(0)

		, trait_elemental(-1)
		, trait_armor_penetration(false)
		, trait_crits_impaired(0)

		, bleed_duration(0)
		, stun_duration(0)
		, slow_duration(0)
		, immobilize_duration(0)
		, immunity_duration(0)
		, transform_duration(0)
		, manual_untransform(false)
		, haste_duration(0)
		, hot_duration(0)
		, hot_value(0)

		, buff_heal(false)
		, buff_shield(false)
		, buff_teleport(false)
		, buff_immunity(false)
		, buff_restore_hp(0)
		, buff_restore_mp(0)

		, post_power(0)
		, wall_power(0)

		, allow_power_mod(false)
		, spawn_type("")
		, target_neighbor(0)
	{}

};

class PowerManager {
private:

	struct Effect {
		std::string type;
		int icon;
		SDL_Surface *gfx;
		SDL_Rect frame_size;
		Point frame_offset;
		int frame_total;
		int ticks_per_frame;

		Effect() {
			type = "";
			icon = -1;
			gfx = NULL;
			frame_size.x = 0;
			frame_size.y = 0;
			frame_size.w = 1;
			frame_size.h = 1;
			frame_offset.x = 0;
			frame_offset.y = 0;
			frame_total = 1;
			ticks_per_frame = 1;
		}
	};

	MapCollision *collider;

	void loadAll();
	void loadPowers(const std::string& filename);
	void loadEffects(const std::string& filename);

	int loadSFX(const std::string& filename);
	std::vector<std::string> gfx_filenames;
	std::vector<std::string> sfx_filenames;
	std::vector<Effect> effects;

	int calcDirection(int origin_x, int origin_y, int target_x, int target_y);
	Point limitRange(int range, Point src, Point target);
	Point targetNeighbor(Point target, int range);
	Point targetNeighbor(Point target, int range, bool ignore_blocked);
	void initHazard(int powernum, StatBlock *src_stats, Point target, Hazard *haz);
	void buff(int power_index, StatBlock *src_stats, Point target);
	void playSound(int power_index, StatBlock *src_stats);

	bool effect(int powernum, StatBlock *src_stats, Point target);
	bool missile(int powernum, StatBlock *src_stats, Point target);
	bool repeater(int powernum, StatBlock *src_stats, Point target);
	bool spawn(int powernum, StatBlock *src_stats, Point target);
	bool transform(int powernum, StatBlock *src_stats, Point target);

	void payPowerCost(int power_index, StatBlock *src_stats);

public:
	PowerManager();
	~PowerManager();

	std::string log_msg;

	void handleNewMap(MapCollision *_collider);
	bool activate(int power_index, StatBlock *src_stats, Point target);
	float calcTheta(int x1, int y1, int x2, int y2);
	const Power &getPower(unsigned id) 	{assert(id < powers.size()); return powers[id];}
	bool canUsePower(unsigned id) const;
	bool hasValidTarget(int power_index, StatBlock *src_stats, Point target);
	bool spawn(const std::string& enemy_type, Point target);
	Renderable renderEffects(StatBlock *src_stats);
	int getEffectIcon(std::string type);

	std::vector<Power> powers;
	std::queue<Hazard *> hazards; // output; read by HazardManager
	std::queue<Map_Enemy> enemies; // output; read by PowerManager

	// shared sounds for power special effects
	std::vector<Mix_Chunk*> sfx;

	int used_item;
};

#endif
