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


#pragma once
#ifndef ENTITY_H
#define ENTITY_H

#include "StatBlock.h"
#include <vector>

class Animation;
class AnimationSet;
class MapRenderer;

class Entity {
protected:
	SDL_Surface *sprites;

public:
	Entity(MapRenderer*);
	Entity(const Entity&);
	virtual ~Entity();

	bool move();

	// Each child of Entity defines its own rendering method
	virtual Renderable getRender() = 0;

	bool setAnimation(const std::string& animation);
	Animation *activeAnimation;
	AnimationSet *animationSet;

	MapRenderer* map;
	StatBlock stats;
};

#endif

