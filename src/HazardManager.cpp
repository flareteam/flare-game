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

/**
 * class HazardManager
 *
 * Holds the collection of hazards (active attacks, spells, etc) and handles group operations
 */

#include "Avatar.h"
#include "Animation.h"
#include "EnemyManager.h"
#include "Hazard.h"
#include "HazardManager.h"
#include "PowerManager.h"

using namespace std;

HazardManager::HazardManager(PowerManager *_powers, Avatar *_hero, EnemyManager *_enemies) {
	powers = _powers;
	hero = _hero;
	enemies = _enemies;
}

void HazardManager::logic() {

	// remove all hazards with lifespan 0.  Most hazards still display their last frame.
	for (int i=h.size()-1; i>=0; i--) {
		if (h[i]->lifespan == 0)
			expire(i);
	}

	checkNewHazards();

	// handle single-frame transforms
	for (int i=h.size()-1; i>=0; i--) {
		h[i]->logic();

		// remove all hazards that need to die immediately (e.g. exit the map)
		if (h[i]->remove_now) {
			expire(i);
			continue;
		}


		// if a moving hazard hits a wall, check for an after-effect
		if (h[i]->hit_wall && h[i]->wall_power > 0) {
			Point target;
			target.x = (int)(h[i]->pos.x);
			target.y = (int)(h[i]->pos.y);

			powers->activate(h[i]->wall_power, h[i]->src_stats, target);
			if (powers->powers[h[i]->wall_power].directional) powers->hazards.back()->animationKind = h[i]->animationKind;

		}

	}

	bool hit;

	// handle collisions
	for (unsigned int i=0; i<h.size(); i++) {
		if (h[i]->isDangerousNow()) {

			// process hazards that can hurt enemies
			if (h[i]->source_type != SOURCE_TYPE_ENEMY) { //hero or neutral sources
				for (unsigned int eindex = 0; eindex < enemies->enemies.size(); eindex++) {

					// only check living enemies
					if (enemies->enemies[eindex]->stats.hp > 0 && h[i]->active) {
						if (isWithin(round(h[i]->pos), h[i]->radius, enemies->enemies[eindex]->stats.pos)) {
							if (!h[i]->hasEntity(enemies->enemies[eindex])) {
								h[i]->addEntity(enemies->enemies[eindex]);
								// hit!
								hit = enemies->enemies[eindex]->takeHit(*h[i]);
								if (!h[i]->multitarget && hit) {
									h[i]->active = false;
									if (!h[i]->complete_animation) h[i]->lifespan = 0;
								}
							}
						}
					}

				}
			}

			// process hazards that can hurt the hero
			if (h[i]->source_type != SOURCE_TYPE_HERO) { //enemy or neutral sources
				if (hero->stats.hp > 0 && h[i]->active) {
					if (isWithin(round(h[i]->pos), h[i]->radius, hero->stats.pos)) {
						if (!h[i]->hasEntity(hero)) {
							h[i]->addEntity(hero);
							// hit!
							hit = hero->takeHit(*h[i]);
							if (!h[i]->multitarget && hit) {
								h[i]->active = false;
								if (!h[i]->complete_animation) h[i]->lifespan = 0;
							}
						}
					}
				}
			}

		}
	}
}

/**
 * Look for hazards generated this frame
 * TODO: all these hazards will originate from PowerManager instead
 */
void HazardManager::checkNewHazards() {

	// check PowerManager for hazards
	while (!powers->hazards.empty()) {
		Hazard *new_haz = powers->hazards.front();
		powers->hazards.pop();
		//new_haz->setCollision(collider);

		h.push_back(new_haz);
	}

	// check hero hazards
	if (hero->haz != NULL) {
		h.push_back(hero->haz);
		hero->haz = NULL;
	}

	// check monster hazards
	for (unsigned int eindex = 0; eindex < enemies->enemies.size(); eindex++) {
		if (enemies->enemies[eindex]->haz != NULL) {
			h.push_back(enemies->enemies[eindex]->haz);
			enemies->enemies[eindex]->haz = NULL;
		}
	}
}

void HazardManager::expire(int index) {
	delete h[index];
	h.erase(h.begin()+index);
}

/**
 * Reset all hazards and get new collision object
 */
void HazardManager::handleNewMap() {
	for (unsigned int i = 0; i < h.size(); i++)
		delete h[i];
	h.clear();
}

/**
 * addRenders()
 * Map objects need to be drawn in Z order, so we allow a parent object (GameEngine)
 * to collect all mobile sprites each frame.
 */
void HazardManager::addRenders(vector<Renderable> &r, vector<Renderable> &r_dead) {
	for (unsigned int i=0; i<h.size(); i++)
		h[i]->addRenderable(r, r_dead);
}

HazardManager::~HazardManager() {
	for (unsigned int i = 0; i < h.size(); i++)
		delete h[i];
	// h.clear(); not needed in destructor
}
