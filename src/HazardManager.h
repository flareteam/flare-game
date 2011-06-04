/**
 * class HazardManager
 *
 * Holds the collection of hazards (active attacks, spells, etc) and handles group operations
 *
 * @author Clint Bellanger
 * @license GPL
 */
 
#ifndef HAZARD_MANAGER_H
#define HAZARD_MANAGER_H

#include "Avatar.h"
#include "EnemyManager.h"
#include "Utils.h"
#include "Hazard.h"
#include "MapCollision.h"
#include "PowerManager.h"

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
