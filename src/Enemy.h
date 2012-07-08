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

/*
 * class Enemy
 */

#ifndef ENEMY_H
#define ENEMY_H


#include <math.h>
#include <string>
#include <queue>
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_mixer.h>

#include "Entity.h"
#include "Utils.h"

class EnemyBehavior;
class Hazard;
class PowerManager;
class StatBlock;
class MapRenderer;

class Enemy : public Entity {

public:
	Enemy(PowerManager *_powers, MapRenderer *_map);
	~Enemy();
	bool lineOfSight();
	void logic();
	int faceNextBest(int mapx, int mapy);
	void newState(int state);
	int getDistance(Point dest);
	bool takeHit(Hazard h);
	void doRewards();

	virtual Renderable getRender();

	Hazard *haz;
	EnemyBehavior *eb;
	PowerManager *powers;

	// sound effects flags
	bool sfx_phys;
	bool sfx_ment;

	bool sfx_hit;
	bool sfx_die;
	bool sfx_critdie;

	// other flags
	bool loot_drop;
	bool reward_xp;

	// common behavior vars
	//int dist;
	//int prev_direction;
	//bool los;
	//Point pursue_pos;
};


#endif

