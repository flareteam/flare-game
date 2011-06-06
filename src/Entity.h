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

class Entity {
protected:
	MapIso* map;

public:
	Entity(MapIso*);

	bool move();
	int face(int, int);

	// Logic common to all entities goes here
	virtual void logic();

	// Each child of Entity defines its own rendering method
	virtual Renderable getRender() = 0;

	StatBlock stats;
};

#endif

