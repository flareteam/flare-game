#ifndef ENTITY_H
#define ENTITY_H

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

#include "MapIso.h"
#include "Animation.h"
#include "Utils.h"
#include <vector>

class Entity {
protected:
	MapIso* map;
	vector<Animation*> animations;
	Animation *activeAnimation;
	SDL_Surface *sprites;

public:
	Entity(MapIso*);
	virtual ~Entity();

	bool move();
	int face(int, int);

	// Logic common to all entities goes here
	virtual void logic();

	// Each child of Entity defines its own rendering method
	virtual Renderable getRender() = 0;

	void loadAnimations(std::string filename);

	bool setAnimation(std::string animation);

	StatBlock stats;
};

#endif

