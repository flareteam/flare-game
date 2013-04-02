/*
Copyright © 2011-2012 Clint Bellanger and kitano
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
 * class Entity
 *
 * An Entity represents any character in the game - the player, allies, enemies
 * This base class handles logic common to all of these child classes
 */

#include "Animation.h"
#include "AnimationManager.h"
#include "AnimationSet.h"
#include "Entity.h"
#include "MapRenderer.h"
#include "SharedResources.h"

#include <iostream>

using namespace std;

Entity::Entity(MapRenderer* _map)
 : sprites(NULL)
 , activeAnimation(NULL)
 , animationSet(NULL)
 , map(_map)
{
}

Entity::Entity(const Entity &e)
 : sprites(e.sprites)
 , activeAnimation(new Animation(*e.activeAnimation))
 , animationSet(e.animationSet)
 , map(e.map)
 , stats(StatBlock(e.stats))
{
}

/**
 * move()
 * Apply speed to the direction faced.
 *
 * @return Returns false if wall collision, otherwise true.
 */
bool Entity::move() {

	if (stats.effects.forced_move) {
		return map->collider.move(stats.pos.x, stats.pos.y, stats.forced_speed.x, stats.forced_speed.y, 1, stats.movement_type);
	}

	if (stats.effects.speed == 0) return false;

	int speed_diagonal = stats.dspeed;
	int speed_straight = stats.speed;

	speed_diagonal = (speed_diagonal * stats.effects.speed) / 100;
	speed_straight = (speed_straight * stats.effects.speed) / 100;

	bool full_move = false;

	switch (stats.direction) {
		case 0:
			full_move = map->collider.move(stats.pos.x, stats.pos.y, -1, 1, speed_diagonal, stats.movement_type);
			break;
		case 1:
			full_move =  map->collider.move(stats.pos.x, stats.pos.y, -1, 0, speed_straight, stats.movement_type);
			break;
		case 2:
			full_move =  map->collider.move(stats.pos.x, stats.pos.y, -1, -1, speed_diagonal, stats.movement_type);
			break;
		case 3:
			full_move =  map->collider.move(stats.pos.x, stats.pos.y, 0, -1, speed_straight, stats.movement_type);
			break;
		case 4:
			full_move =  map->collider.move(stats.pos.x, stats.pos.y, 1, -1, speed_diagonal, stats.movement_type);
			break;
		case 5:
			full_move =  map->collider.move(stats.pos.x, stats.pos.y, 1, 0, speed_straight, stats.movement_type);
			break;
		case 6:
			full_move =  map->collider.move(stats.pos.x, stats.pos.y, 1, 1, speed_diagonal, stats.movement_type);
			break;
		case 7:
			full_move =  map->collider.move(stats.pos.x, stats.pos.y, 0, 1, speed_straight, stats.movement_type);
			break;
	}

	return full_move;
}

/**
 * Set the entity's current animation by name
 */
bool Entity::setAnimation(const string& animationName) {

	// if the animation is already the requested one do nothing
	if (activeAnimation != NULL && activeAnimation->getName() == animationName)
		return true;

	delete activeAnimation;
	activeAnimation = animationSet->getAnimation(animationName);

	if (activeAnimation == NULL)
		fprintf(stderr, "Entity::setAnimation(%s): not found\n", animationName.c_str());

	return activeAnimation == NULL;
}

Entity::~Entity () {

	delete activeAnimation;
}

