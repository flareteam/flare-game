/*
Copyright © 2011-2012 Clint Bellanger and kitano

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

#include "Entity.h"
#include "FileParser.h"
#include "SharedResources.h"

using namespace std;

Entity::Entity(MapIso* _map) : sprites(NULL), activeAnimation(NULL), map(_map) {
}

/**
 * move()
 * Apply speed to the direction faced.
 *
 * @return Returns false if wall collision, otherwise true.
 */
bool Entity::move() {
	if (stats.forced_move_duration > 0) {
		return map->collider.move(stats.pos.x, stats.pos.y, stats.forced_speed.x, stats.forced_speed.y, 1);
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
	if (2.0 <= slope || -2.0 >= slope) {
		if (dy > 0) return 3;
		else return 7;
	}
	return stats.direction;
}

/**
 * Load the entity's animation from animation definition file
 */
void Entity::loadAnimations(const string& filename) {

	FileParser parser;

	if (!parser.open(mods->locate(filename).c_str())) {
		cout << "Error loading animation definition file: " << filename << endl;
		SDL_Quit();
		exit(1);
	}

	string name = "";
	int position = 0;
	int frames = 0;
	int duration = 0;
	Point render_size;
	Point render_offset;
	string type = "";
	string firstAnimation = "";
	int active_frame = 0;

	// Parse the file and on each new section create an animation object from the data parsed previously

	parser.next();
	parser.new_section = false; // do not create the first animation object until parser has parsed first section

	do {
		// create the animation if finished parsing a section
		if (parser.new_section) {
			animations.push_back(new Animation(name, render_size, render_offset,  position, frames, duration, type, active_frame));
		}

		if (parser.key == "position") {
			if (isInt(parser.val)) {
				position = atoi(parser.val.c_str());
			}
		}
		else if (parser.key == "frames") {
			if (isInt(parser.val)) {
				frames = atoi(parser.val.c_str());
			}
		}
		else if (parser.key == "duration") {
			if (isInt(parser.val)) {
				int ms_per_frame = atoi(parser.val.c_str());
				
				duration = (int)round((float)ms_per_frame / (1000.0 / (float)FRAMES_PER_SEC));

				// adjust duration according to the entity's animation speed
				duration = (duration * 100) / stats.animationSpeed;
				
				// TEMP: if an animation is too fast, display one frame per fps anyway
				if (duration < 1) duration=1;
			}
		}
		else if (parser.key == "type") {
			type = parser.val;
		}
		else if (parser.key == "render_size_x") {
			if (isInt(parser.val)) {
				render_size.x = atoi(parser.val.c_str());
			}
		}
		else if (parser.key == "render_size_y") {
			if (isInt(parser.val)) {
				render_size.y = atoi(parser.val.c_str());
			}
		}
		else if (parser.key == "render_offset_x") {
			if (isInt(parser.val)) {
				render_offset.x = atoi(parser.val.c_str());
			}
		}
		else if (parser.key == "render_offset_y") {
			if (isInt(parser.val)) {
				render_offset.y = atoi(parser.val.c_str());
			}
		}
		else if (parser.key == "active_frame") {
			active_frame = atoi(parser.val.c_str());
		}

		if (name == "") {
			// This is the first animation
			firstAnimation = parser.section;
		}
		name = parser.section;
	}
	while (parser.next());

	// add final animation
	animations.push_back(new Animation(name, render_size, render_offset, position, frames, duration, type, active_frame));


	// set the default animation
	if (firstAnimation != "") {
		setAnimation(firstAnimation);
	}
}

/**
 * Set the entity's current animation by name
*/
bool Entity::setAnimation(const string& animationName) {

	// if the animation is already the requested one do nothing
	if (activeAnimation != NULL && activeAnimation->getName() == animationName) {
		return true;
	}

	// search animations for the requested animation and set the active animation to it if found
	for (vector<Animation*>::iterator it = animations.begin(); it!=animations.end(); it++) {
		if ((*it) != NULL && (*it)->getName() == animationName) {
			activeAnimation = *it;
			activeAnimation->reset();
			return true;
		}
	}

	return false;
}

Entity::~Entity () {

	// delete all loaded animations
	for (vector<Animation*>::const_iterator it = animations.begin(); it != animations.end(); it++)
	{
		delete *it;
	}
	animations.clear();
}

