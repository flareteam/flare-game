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

	if (stats.forced_move_duration > 0) {
		return map->collider.move(stats.pos.x, stats.pos.y, stats.forced_speed.x, stats.forced_speed.y, 1, stats.movement_type);
	}
	if (stats.immobilize_duration > 0) return false;

	int speed_diagonal = stats.dspeed;
	int speed_straight = stats.speed;

	if (stats.slow_duration > 0) {
		speed_diagonal /= 2;
		speed_straight /= 2;
	}
	else if (stats.haste_duration > 0) {
		speed_diagonal *= 2;
		speed_straight *= 2;
	}

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
 * Change direction to face the target map location
 */
int Entity::face(int mapx, int mapy) {
	// inverting Y to convert map coordinates to standard cartesian coordinates
	int dx = mapx - stats.pos.x;
	int dy = stats.pos.y - mapy;

	// avoid div by zero
	if (dx == 0) {
		if (dy > 0) return 3;
		else return 7;
	}

	float slope = ((float)dy)/((float)dx);
	if (0.5 <= slope && slope <= 2.0) {
		if (dy > 0) return 4;
		else return 0;
	}
	if (-0.5 <= slope && slope <= 0.5) {
		if (dx > 0) return 5;
		else return 1;
	}
	if (-2.0 <= slope && slope <= -0.5) {
		if (dx > 0) return 6;
		else return 2;
	}
	if (2.0 <= slope || -2.0 >= slope) {
		if (dy > 0) return 3;
		else return 7;
	}
	return stats.direction;
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

