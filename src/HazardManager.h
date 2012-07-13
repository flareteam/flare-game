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
 * class HazardManager
 *
 * Holds the collection of hazards (active attacks, spells, etc) and handles group operations
 */
 
#ifndef HAZARD_MANAGER_H
#define HAZARD_MANAGER_H

#include "Utils.h"

class Avatar;
class EnemyManager;
class Hazard;
class MapCollision;
class PowerManager;

class HazardManager {
private:
	Avatar *hero;
	EnemyManager *enemies;
	MapCollision *collider;
	PowerManager *powers;
public:
	HazardManager(PowerManager *_powers, Avatar *_hero, EnemyManager *_enemies);
	~HazardManager();
	void logic();
	void expire(int index);
	void checkNewHazards();
	void handleNewMap(MapCollision *_collider);
	Renderable getRender(int haz_id);
	
	int hazard_count;
	Hazard *h[256];
};

#endif
