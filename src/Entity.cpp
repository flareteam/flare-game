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

Entity::Entity(MapIso* _map) : sprites(NULL), activeAnimation(NULL), map(_map) {
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
	if (2.0 <= slope || -2.0 >= slope) {
		if (dy > 0) return 3;
		else return 7;
	}
	return stats.direction;
}
  
/**
 * Load the entity's animation from animation definition file
 */
void Entity::loadAnimations(std::string filename) {

	FileParser parser;

	if (!parser.open(filename)) {
		cout << "Error loading animation definition file: " << filename << endl;
		exit(1);
	}

	std::string name = "";
	int position = 0;
	int frames = 0;
	int duration = 0;
	std::string type = "";
	std::string firstAnimation = "";

	// Parse the file and on each new section create an animation object from the data parsed previously

	parser.next();
	parser.new_section = false; // do not create the first animation object until parser has parsed first section

	do {
		if (parser.new_section) {
			animations.push_back(new Animation(name, sprites, 128, position, frames, duration, type));
		}

		if (parser.key == "position") {
			if (isInt(parser.val)) {
				position = atoi(parser.val.c_str());
			}
		}	
		if (parser.key == "frames") {
			if (isInt(parser.val)) {
				frames = atoi(parser.val.c_str());
			}
		}	
		if (parser.key == "duration") {
			if (isInt(parser.val)) {
				int ms_per_frame = atoi(parser.val.c_str());
				
				duration = round((float)ms_per_frame / (1000.0 / (float)FRAMES_PER_SEC));
				
				// TEMP: if an animation is too fast, display one frame per fps anyway
				if (duration < 1) duration=1;
			}
		}	
		if (parser.key == "type") {
			type = parser.val;
		}	

		if (name == "") {
			// This is the first animation
			firstAnimation = parser.section;
		}
		name = parser.section;
	}
	while (parser.next());

	// add final animation
	animations.push_back(new Animation(name, sprites, 128, position, frames, duration, type));


	// set the default animation
	if (firstAnimation != "") {
		setAnimation(firstAnimation);
	}
}

/**
 * Set the entity's current animation by name
*/
bool Entity::setAnimation(std::string animationName) {

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

void Entity::logic() {
}

Entity::~Entity () {
	// delete all loaded animations
	for (vector<Animation*>::const_iterator it = animations.begin(); it != animations.end(); it++)
	{
	    delete *it;
	} 
	animations.clear();
}

