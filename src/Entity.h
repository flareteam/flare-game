/*
Copyright 2011 Clint Bellanger and kitano

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

#ifndef ENTITY_H
#define ENTITY_H

#include "MapIso.h"
#include "Animation.h"
#include "Utils.h"
#include <vector>

class Entity {
protected:
	SDL_Surface *sprites;
	Animation *activeAnimation;
	MapIso* map;
	vector<Animation*> animations;

public:
	Entity(MapIso*);
	virtual ~Entity();

	bool move();
	int face(int, int);

	// Logic common to all entities goes here
	virtual void logic();

	// Each child of Entity defines its own rendering method
	virtual Renderable getRender() = 0;

	void loadAnimations(const std::string& filename);

	bool setAnimation(const std::string& animation);

	StatBlock stats;
};

#endif

