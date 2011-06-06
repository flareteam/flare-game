/*
 * class Enemy
 *
 * @author Clint Bellanger
 * @license GPL
 *
 */

#ifndef ENEMY_H
#define ENEMY_H


#include <math.h>
#include <string>
#include "SDL.h"
#include "SDL_image.h"
#include "SDL_mixer.h"

#include "Entity.h"
#include "Utils.h"
#include "InputState.h"
#include "MapIso.h"
#include "StatBlock.h"
#include "Hazard.h"
#include "Settings.h"
#include "PowerManager.h"

// active states
const int ENEMY_STANCE = 0;
const int ENEMY_MOVE = 1;
const int ENEMY_CHARGE = 2;
const int ENEMY_MELEE_PHYS = 3;
const int ENEMY_MELEE_MENT = 4;
const int ENEMY_RANGED_PHYS = 5;
const int ENEMY_RANGED_MENT = 6;

// interrupt states
const int ENEMY_BLOCK = 7;
const int ENEMY_HIT = 8;
const int ENEMY_DEAD = 9;
const int ENEMY_CRITDEAD = 10;

class Enemy : public Entity {
protected:
	PowerManager *powers;
	
public:
	Enemy(PowerManager *_powers, MapIso *_map);
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

	// sound effects flags
	bool sfx_phys;
	bool sfx_ment;
	
	bool sfx_hit;
	bool sfx_die;
	bool sfx_critdie;
	
	// other flags
	bool loot_drop;
	bool reward_xp;
};


#endif

