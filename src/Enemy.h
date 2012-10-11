/*
Copyright © 2011-2012 Clint Bellanger
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
	Enemy(const Enemy& e);
	~Enemy();
	bool lineOfSight();
	void logic();
	int faceNextBest(int mapx, int mapy);
	void newState(int state);
	int getDistance(Point dest);
	bool takeHit(const Hazard &h);
	void doRewards();

	std::string type;

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
	bool reward_xp;
	bool instant_power;
};


#endif

