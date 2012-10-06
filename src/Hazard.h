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
 * class Hazard
 *
 * Stand-alone object that can harm the hero or creatures
 * These are generated whenever something makes any attack
 */

#ifndef HAZARD_H
#define HAZARD_H

class Entity;

#include <vector>
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_mixer.h>
#include "Utils.h"

class Animation;
class StatBlock;
class MapCollision;

// the spell/power's source type: eg. which team did it come from?
const int SOURCE_TYPE_HERO = 0;
const int SOURCE_TYPE_NEUTRAL = 1;
const int SOURCE_TYPE_ENEMY = 2;

class Hazard {
private:
	MapCollision *collider;
	// Keeps track of entities already hit
	std::vector<Entity*> entitiesCollided;

public:
	Hazard();

	Hazard(const Hazard&); // not implemented! If you implement this, take care to create a real copy of the pointers, such as Animation.
	Hazard & operator= (const Hazard & other); // same as copy constructor!

	~Hazard();

	StatBlock *src_stats;

	void setCollision(MapCollision *_collider);
	void logic();

	bool hasEntity(Entity*);

	void addEntity(Entity*);

	int dmg_min;
	int dmg_max;
	int crit_chance;
	int accuracy;
	int source_type;

	FPoint pos;
	FPoint speed;
	int base_speed;
	int lifespan; // ticks down to zero
	int radius;
	int power_index;

	Animation *activeAnimation;
	int animationKind;	// direction or other, it is a specific value according to
						// some hazard animations are 8-directional
						// some hazard animations have random/varietal options

	Renderable getRenderable();

	bool floor; // rendererable goes on the floor layer
	int delay_frames;
	bool complete_animation; // if not multitarget but hitting a creature, still complete the animation?

	// these work in conjunction
	// if the attack is not multitarget, set active=false
	// only process active hazards for collision
	bool multitarget;
	bool active;

	bool remove_now;
	bool hit_wall;

	// after effects of various powers
	int stun_duration;
	int immobilize_duration;
	int slow_duration;
	int bleed_duration;
	int forced_move_speed;
	int forced_move_duration;
	int hp_steal;
	int mp_steal;

	bool trait_armor_penetration;
	int trait_crits_impaired;
	int trait_elemental;

	// pre/post power effects
	int post_power;
	int wall_power;

	bool equipment_modified;

};

#endif
