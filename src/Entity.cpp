/**
 * class Entity
 *
 * An Entity represents any character in the game - the player, allies, enemies
 * This base class handles logic common to all of these child classes
 *
 * @author Clint Bellanger
 * @author kitano
 * @license GPL
 */

#include "Entity.h"

Entity::Entity(MapIso* _map) {
	map = _map;
}

/**
 * move()
 * Apply speed to the direction faced.
 *
 * @return Returns false if wall collision, otherwise true.
 */
bool Entity::move() {
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
	
	switch (stats.direction) {
		case 0:
			return map->collider.move(stats.pos.x, stats.pos.y, -1, 1, speed_diagonal);
		case 1:
			return map->collider.move(stats.pos.x, stats.pos.y, -1, 0, speed_straight);
		case 2:
			return map->collider.move(stats.pos.x, stats.pos.y, -1, -1, speed_diagonal);
		case 3:
			return map->collider.move(stats.pos.x, stats.pos.y, 0, -1, speed_straight);
		case 4:
			return map->collider.move(stats.pos.x, stats.pos.y, 1, -1, speed_diagonal);
		case 5:
			return map->collider.move(stats.pos.x, stats.pos.y, 1, 0, speed_straight);
		case 6:
			return map->collider.move(stats.pos.x, stats.pos.y, 1, 1, speed_diagonal);
		case 7:
			return map->collider.move(stats.pos.x, stats.pos.y, 0, 1, speed_straight);
	}
	return true;
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
	if (2 <= slope || -2 >= slope) {
		if (dy > 0) return 3;
		else return 7;
	}
	return stats.direction;
}
  
void Entity::logic() {
}

