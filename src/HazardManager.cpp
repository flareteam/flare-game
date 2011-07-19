/**
 * class HazardManager
 *
 * Holds the collection of hazards (active attacks, spells, etc) and handles group operations
 *
 * @author Clint Bellanger
 * @license GPL
 */

#include "HazardManager.h"

HazardManager::HazardManager(PowerManager *_powers, Avatar *_hero, EnemyManager *_enemies) {
	powers = _powers;
	hero = _hero;
	enemies = _enemies;
	hazard_count = 0;
}

void HazardManager::logic() {

	// remove all hazards with lifespan 0.  Most hazards still display their last frame.
	for (int i=hazard_count-1; i>=0; i--) {
		if (h[i]->lifespan == 0)
			expire(i);
	}
	
	checkNewHazards();
	
	// handle single-frame transforms
	for (int i=hazard_count-1; i>=0; i--) {
		h[i]->logic();
		
		// remove all hazards that need to die immediately (e.g. exit the map)
		if (h[i]->remove_now)
			expire(i);
		
		
		// if a moving hazard hits a wall, check for an after-effect
		if (h[i]->hit_wall && h[i]->wall_power >= 0) {
			Point target;
			target.x = (int)(h[i]->pos.x);
			target.y = (int)(h[i]->pos.y);
			
			powers->activate(h[i]->wall_power, h[i]->src_stats, target);
			if (powers->powers[h[i]->wall_power].directional) powers->hazards.back()->direction = h[i]->direction;
			
		}
		
	}
	
	bool hit;
	
	// handle collisions
	for (int i=0; i<hazard_count; i++) {
		if (h[i]->active && h[i]->delay_frames==0 && (h[i]->active_frame == -1 || h[i]->active_frame == h[i]->frame)) {
	
			// process hazards that can hurt enemies
			if (h[i]->source_type != SOURCE_TYPE_ENEMY) { //hero or neutral sources
				for (int eindex = 0; eindex < enemies->enemy_count; eindex++) {
			
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

	Hazard *new_haz;

	// check PowerManager for hazards
	while (!powers->hazards.empty()) {
		new_haz = powers->hazards.front();		
		powers->hazards.pop();		
		new_haz->setCollision(collider);

		h[hazard_count] = new_haz;
		hazard_count++;
	}

	// check hero hazards
	if (hero->haz != NULL) {
		h[hazard_count] = hero->haz;
		hazard_count++;
		hero->haz = NULL;
	}
	
	// check monster hazards
	for (int eindex = 0; eindex < enemies->enemy_count; eindex++) {
		if (enemies->enemies[eindex]->haz != NULL) {
			h[hazard_count] = enemies->enemies[eindex]->haz;
			hazard_count++;
			enemies->enemies[eindex]->haz = NULL;
		}
	}
}

void HazardManager::expire(int index) {
	// TODO: assert this instead?
	if (index >= 0 && index < hazard_count) {
		delete(h[index]);
		for (int i=index; i<hazard_count-1; i++) {
			h[i] = h[i+1];
		}
		hazard_count--;
	}
}

/**
 * Reset all hazards and get new collision object
 */
void HazardManager::handleNewMap(MapCollision *_collider) {
	hazard_count = 0;
	collider = _collider;
}

/**
 * getRender()
 * Map objects need to be drawn in Z order, so we allow a parent object (GameEngine)
 * to collect all mobile sprites each frame.
 */
Renderable HazardManager::getRender(int haz_id) {

	Renderable r;
	r.map_pos.x = round(h[haz_id]->pos.x);
	r.map_pos.y = round(h[haz_id]->pos.y);
	r.sprite = h[haz_id]->sprites;
	r.src.x = h[haz_id]->frame_size.x * (h[haz_id]->frame / h[haz_id]->frame_duration);
	r.src.w = h[haz_id]->frame_size.x;
	r.src.h = h[haz_id]->frame_size.y;
	r.offset.x = h[haz_id]->frame_offset.x;
	r.offset.y = h[haz_id]->frame_offset.y;
	r.object_layer = !h[haz_id]->floor;

	if (h[haz_id]->direction > 0)
		r.src.y = h[haz_id]->frame_size.y * h[haz_id]->direction;
	else if (h[haz_id]->visual_option > 0)
		r.src.y = h[haz_id]->frame_size.y * h[haz_id]->visual_option;
	else
		r.src.y = 0;
	
	return r;
}

HazardManager::~HazardManager() {
	for (int i=0; i<hazard_count; i++) {
		delete h[i];
	}
}
