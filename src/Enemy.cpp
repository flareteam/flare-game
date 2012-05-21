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

#include "Animation.h"
#include "Enemy.h"
#include "EnemyBehavior.h"
#include <sstream>

using namespace std;


Enemy::Enemy(PowerManager *_powers, MapIso *_map) : Entity(_map) {
	powers = _powers;

	stats.cur_state = ENEMY_STANCE;
	stats.turn_ticks = FRAMES_PER_SEC;
	//stats.patrol_ticks = 0; //no longer needed due to A*
	stats.cooldown = 0;
	stats.last_seen.x = -1;
	stats.last_seen.y = -1;
	stats.in_combat = false;
    stats.join_combat = false;
	
	haz = NULL;
	
	sfx_phys = false;
	sfx_ment = false;
	sfx_hit = false;
	sfx_die = false;
	sfx_critdie = false;
	loot_drop = false;
	reward_xp = false;

	eb = NULL;
}

/**
 * The current direction leads to a wall.  Try the next best direction, if one is available.
 */
int Enemy::faceNextBest(int mapx, int mapy) {
	int dx = abs(mapx - stats.pos.x);
	int dy = abs(mapy - stats.pos.y);
	switch (stats.direction) {
		case 0:
			if (dy > dx) return 7;
			else return 1;
		case 1:
			if (mapy > stats.pos.y) return 0;
			else return 2;
		case 2:
			if (dx > dy) return 1;
			else return 3;
		case 3:
			if (mapx < stats.pos.x) return 2;
			else return 4;
		case 4:
			if (dy > dx) return 3;
			else return 5;
		case 5:
			if (mapy < stats.pos.y) return 4;
			else return 6;
		case 6:
			if (dx > dy) return 5;
			else return 7;
		case 7:
			if (mapx > stats.pos.x) return 6;
			else return 0;
	}
	return 0;
}

/**
 * Calculate distance between the enemy and the hero
 */
int Enemy::getDistance(Point dest) {
	int dx = dest.x - stats.pos.x;
	int dy = dest.y - stats.pos.y;
	double step1 = (double)dx * (double)dx + (double)dy * (double)dy;
	double step2 = sqrt(step1);
	return int(step2);
}

void Enemy::newState(int state) {
	
	stats.cur_state = state;
}
	
/**
 * logic()
 * Handle a single frame.  This includes:
 * - move the enemy based on AI % chances
 * - calculate the next frame of animation
 */
void Enemy::logic() {

	eb->logic();
	return;

/*
	stats.logic();
	if (stats.forced_move_duration > 0) {
		move(); return;
	}
	if (stats.stun_duration > 0) return;
	// check for bleeding to death
	if (stats.hp <= 0 && !(stats.cur_state == ENEMY_DEAD || stats.cur_state == ENEMY_CRITDEAD)) {
		doRewards();
		stats.cur_state = ENEMY_DEAD;
	}
	// check for bleeding spurt
	if (stats.bleed_duration % 30 == 1) {
	    CombatText::Instance()->addMessage(1, stats.pos, DISPLAY_DAMAGE);
		powers->activate(POWER_SPARK_BLOOD, &stats, stats.pos);
	}
	
	// check for teleport powers
	if (stats.teleportation) {
		
		stats.pos.x = stats.teleport_destination.x;
		stats.pos.y = stats.teleport_destination.y;	
		
		stats.teleportation = false;	
	}
	

	// set a default pursue_pos, all else failing (used in targeting)
	pursue_pos.x = stats.hero_pos.x;
	pursue_pos.y = stats.hero_pos.y;
	los = false;	
	
	
	// SECTION 1: Steering and Vision
	// ------------------------------
	
	// check distance and line of sight between enemy and hero
	if (stats.hero_alive)
		dist = getDistance(stats.hero_pos);
	else
		dist = 0;
	
	// if the hero is too far away or dead, abandon combat and do nothing
	if (dist > stats.threat_range+stats.threat_range || !stats.hero_alive) {
		stats.in_combat = false;
		stats.patrol_ticks = 0;
		stats.last_seen.x = -1;
		stats.last_seen.y = -1;
		
		// heal rapidly if the hero has left range
		if (stats.alive && stats.hero_alive) {
			stats.hp++;
			if (stats.hp > stats.maxhp) stats.hp = stats.maxhp;
		}
	}

	if (dist < stats.threat_range && stats.hero_alive)
		los = map->collider.line_of_sight(stats.pos.x, stats.pos.y, stats.hero_pos.x, stats.hero_pos.y);
	else
		los = false;

	// if the enemy can see the hero, it pursues.
	// otherwise, it will head towards where it last saw the hero
	if (los && dist < stats.threat_range) {
		if (!stats.in_combat)
            stats.join_combat = true;
        stats.in_combat = true;
		stats.last_seen.x = stats.hero_pos.x;
		stats.last_seen.y = stats.hero_pos.y;
		powers->activate(stats.power_index[BEACON], &stats, stats.pos); //emit beacon
	}
	else if (stats.last_seen.x >= 0 && stats.last_seen.y >= 0) {
		if (getDistance(stats.last_seen) <= (stats.speed+stats.speed) && stats.patrol_ticks == 0) {
			stats.last_seen.x = -1;
			stats.last_seen.y = -1;
			stats.patrol_ticks = 8; // start patrol; see note on "patrolling" below
		}		
	}


	// where is the creature heading?
	// TODO: add fleeing for X ticks
	if (los) {
		pursue_pos.x = stats.last_seen.x = stats.hero_pos.x;
		pursue_pos.y = stats.last_seen.y = stats.hero_pos.y;
		stats.patrol_ticks = 0;
	}
	else if (stats.in_combat) {
	
		// "patrolling" is a simple way to help steering.
		// When the enemy arrives at where he last saw the hero, it continues
		// walking a few steps.  This gives a better chance of re-establishing
		// line of sight around corners.
		
		if (stats.patrol_ticks > 0) {
			stats.patrol_ticks--;
			if (stats.patrol_ticks == 0) {
				stats.in_combat = false;
			}			
		}
		pursue_pos.x = stats.last_seen.x;
		pursue_pos.y = stats.last_seen.y;
	}

	
	// SECTION 2: States
	// -----------------
	
	activeAnimation->advanceFrame();

	switch(stats.cur_state) {
	
		case ENEMY_STANCE:
		
			setAnimation("stance");
			
			if (stats.in_combat) {

				// update direction to face the target
				if (++stats.turn_ticks > stats.turn_delay && stats.patrol_ticks == 0) {
					stats.direction = face(pursue_pos.x, pursue_pos.y);				
					stats.turn_ticks = 0;
				}
		
                // perform action upon joining combat
                if ((rand() % 100) < stats.power_chance[ON_JOIN_COMBAT] && stats.join_combat == true) {
                    newState(ENEMY_JOIN_COMBAT);
                    break;
                }

                // perform action if half health or lower
                if ((rand() % 100) < stats.power_chance[ON_HALF_DEAD] && stats.hp <= stats.maxhp/2) {
                    newState(ENEMY_HALF_DEAD);
                    break;
                }
                // perform action if debuffed
                else if ((rand() % 100) < stats.power_chance[ON_DEBUFF] && (stats.stun_duration > 0 || stats.immobilize_duration > 0 || stats.slow_duration > 0 || stats.bleed_duration > 0)) {
                    newState(ENEMY_DEBUFF);
                    break;
                }

				// performed ranged actions
				else if (dist > stats.melee_range && stats.cooldown_ticks == 0) {

					// CHECK: ranged physical!
					//if (!powers->powers[stats.power_index[RANGED_PHYS]].requires_los || los) {
					if (los) {
						if ((rand() % 100) < stats.power_chance[RANGED_PHYS] && stats.power_ticks[RANGED_PHYS] == 0) {
							
							newState(ENEMY_RANGED_PHYS);
							break;
						}
					}
					// CHECK: ranged spell!
					//if (!powers->powers[stats.power_index[RANGED_MENT]].requires_los || los) {
					if (los) {			
						if ((rand() % 100) < stats.power_index[RANGED_MENT] && stats.power_ticks[RANGED_MENT] == 0) {
							
							newState(ENEMY_RANGED_MENT);
							break;
						}
					}
				
					// CHECK: flee!
					
					// CHECK: pursue!
					if ((rand() % 100) < stats.chance_pursue) {
						if (move()) { // no collision
							newState(ENEMY_MOVE);
						}
						else {
							// hit an obstacle, try the next best angle
							prev_direction = stats.direction;
							stats.direction = faceNextBest(pursue_pos.x, pursue_pos.y);
							if (move()) {
								newState(ENEMY_MOVE);
								break;
							}
							else stats.direction = prev_direction;
						}
					}
					
				}
				// perform melee actions
				else if (dist <= stats.melee_range && stats.cooldown_ticks == 0) {
				
					// CHECK: melee attack!
					//if (!powers->powers[stats.power_index[MELEE_PHYS]].requires_los || los) {
					if (los) {
						if ((rand() % 100) < stats.power_chance[MELEE_PHYS] && stats.power_ticks[MELEE_PHYS] == 0) {
							
							newState(ENEMY_MELEE_PHYS);
							break;
						}
					}
					// CHECK: melee ment!
					//if (!powers->powers[stats.power_index[MELEE_MENT]].requires_los || los) {
					if (los) {
						if ((rand() % 100) < stats.power_chance[MELEE_MENT] && stats.power_ticks[MELEE_MENT] == 0) {
													
							newState(ENEMY_MELEE_MENT);
							break;
						}
					}
				}
			}
			
			break;
		
		case ENEMY_MOVE:
		
			setAnimation("run");
	
			if (stats.in_combat) {

				if (++stats.turn_ticks > stats.turn_delay && stats.patrol_ticks == 0) {
					// if no line of movement to target, use pathfinder
					if ( !map->collider.line_of_movement(stats.pos.x, stats.pos.y, pursue_pos.x, pursue_pos.y)) {
						vector<Point> path;
						// if a path is returned, target first waypoint
						if ( map->collider.compute_path(stats.pos,pursue_pos,path) ) {
							pursue_pos = path.back();
						}
					}
					stats.direction = face(pursue_pos.x, pursue_pos.y);
					stats.turn_ticks = 0;
				}
				
				if (dist > stats.melee_range && stats.cooldown_ticks == 0) {
				
					// check ranged physical!
					//if (!powers->powers[stats.power_index[RANGED_PHYS]].requires_los || los) {
					if (los) {
						if ((rand() % 100) < stats.power_chance[RANGED_PHYS] && stats.power_ticks[RANGED_PHYS] == 0) {
							
							newState(ENEMY_RANGED_PHYS);
							break;
						}
					}
					// check ranged spell!
					// if (!powers->powers[stats.power_index[RANGED_MENT]].requires_los || los) {
					if (los) {
						if ((rand() % 100) < stats.power_chance[RANGED_MENT] && stats.power_ticks[RANGED_MENT] == 0) {
							
							newState(ENEMY_RANGED_MENT);
							break;
						}
					}
				
					if (!move()) {
						// hit an obstacle.  Try the next best angle
						prev_direction = stats.direction;
						stats.direction = faceNextBest(pursue_pos.x, pursue_pos.y);
						if (!move()) {
							newState(ENEMY_STANCE);
							stats.direction = prev_direction;
						}
					}
				}
				else {
					newState(ENEMY_STANCE);
				}
			}
			else {
				newState(ENEMY_STANCE);
			}
			break;
			
		case ENEMY_MELEE_PHYS:
			
			setAnimation("melee");

			if (activeAnimation->getCurFrame() == 1) {
				sfx_phys = true;
			}

			// the attack hazard is alive for a single frame
			if (activeAnimation->getCurFrame() == activeAnimation->getMaxFrame()/2 && haz == NULL) {
				powers->activate(stats.power_index[MELEE_PHYS], &stats, pursue_pos);
				stats.power_ticks[MELEE_PHYS] = stats.power_cooldown[MELEE_PHYS];
			}

			if (activeAnimation->getCurFrame() == activeAnimation->getMaxFrame()-1) {
				newState(ENEMY_STANCE);
				stats.cooldown_ticks = stats.cooldown;
			}
			break;

		case ENEMY_RANGED_PHYS:

			setAnimation("ranged");
	
			// monsters turn to keep aim at the hero
			stats.direction = face(pursue_pos.x, pursue_pos.y);
			
			if (activeAnimation->getCurFrame() == 1) {
				sfx_phys = true;
			}
			
			// the attack hazard is alive for a single frame
			if (activeAnimation->getCurFrame() == activeAnimation->getMaxFrame()/2 && haz == NULL) {
				powers->activate(stats.power_index[RANGED_PHYS], &stats, pursue_pos);
				stats.power_ticks[RANGED_PHYS] = stats.power_cooldown[RANGED_PHYS];
			}
			
			if (activeAnimation->getCurFrame() == activeAnimation->getMaxFrame()-1) {
				newState(ENEMY_STANCE);
				stats.cooldown_ticks = stats.cooldown;
			}
			break;

		
		case ENEMY_MELEE_MENT:
	
			setAnimation("ment");

			if (activeAnimation->getCurFrame() == 1) {
				sfx_ment = true;
			}
			
			// the attack hazard is alive for a single frame
			if (activeAnimation->getCurFrame() == activeAnimation->getMaxFrame()/2 && haz == NULL) {
				powers->activate(stats.power_index[MELEE_MENT], &stats, pursue_pos);
				stats.power_ticks[MELEE_MENT] = stats.power_cooldown[MELEE_MENT];
			}
			
			if (activeAnimation->getCurFrame() == activeAnimation->getMaxFrame()-1) {
				newState(ENEMY_STANCE);
				stats.cooldown_ticks = stats.cooldown;
			}
			break;

		case ENEMY_RANGED_MENT:

			setAnimation("ment");
		
			// monsters turn to keep aim at the hero
			stats.direction = face(pursue_pos.x, pursue_pos.y);
	
			if (activeAnimation->getCurFrame() == 1) {
				sfx_ment = true;
			}
			
			// the attack hazard is alive for a single frame
			if (activeAnimation->getCurFrame() == activeAnimation->getMaxFrame()/2 && haz == NULL) {
			
				powers->activate(stats.power_index[RANGED_MENT], &stats, pursue_pos);
				stats.power_ticks[RANGED_MENT] = stats.power_cooldown[RANGED_MENT];
			}
			
			if (activeAnimation->getCurFrame() == activeAnimation->getMaxFrame()-1) {
				newState(ENEMY_STANCE);
				stats.cooldown_ticks = stats.cooldown;
			}
			break;
			
		case ENEMY_SPAWN:
			// enemy is appearing out of nowhere

			setAnimation("spawn");
			if (activeAnimation->getCurFrame() == 1) {
				sfx_ment = true;
			}

			if (activeAnimation->getCurFrame() == activeAnimation->getMaxFrame()-1) {
				newState(ENEMY_STANCE);
			}
			
			break;

	
		case ENEMY_HIT:
			// enemy has taken damage (but isn't dead)

			setAnimation("hit");
			if (activeAnimation->getCurFrame() == 1) {
				sfx_hit = true;
			}

            if ((rand() % 100) < stats.power_chance[ON_HIT]) {
                if (activeAnimation->getCurFrame() == activeAnimation->getMaxFrame()/2 && haz == NULL) {
                        powers->activate(stats.power_index[ON_HIT], &stats, stats.pos);
                        stats.power_ticks[ON_HIT] = stats.power_cooldown[ON_HIT];
                }
            }

			if (activeAnimation->getCurFrame() == activeAnimation->getMaxFrame()-1) {
				newState(ENEMY_STANCE);
			}

			break;


		case ENEMY_DEAD:

			// corpse means the creature is dead and done animating		
			if (!stats.corpse) {
				setAnimation("die");
				
				if (activeAnimation->getCurFrame() == 1) {
					sfx_die = true;
				}

                if ((rand() % 100) < stats.power_chance[ON_DEATH]) {
                    if (activeAnimation->getCurFrame() == activeAnimation->getMaxFrame()/2 && haz == NULL) {
                        powers->activate(stats.power_index[ON_DEATH], &stats, stats.pos);
                        stats.power_ticks[ON_DEATH] = stats.power_cooldown[ON_DEATH];
                    }
                }
			}

			break;


		case ENEMY_CRITDEAD:
			// critdead is an optional, more gruesome death animation
		
			// corpse means the creature is dead and done animating
			if (!stats.corpse) {
				setAnimation("critdie");
				
				if (activeAnimation->getCurFrame() == 1) {
					sfx_critdie = true;
				}

                if ((rand() % 100) < stats.power_chance[ON_DEATH]) {
                    if (activeAnimation->getCurFrame() == activeAnimation->getMaxFrame()/2 && haz == NULL) {
                        powers->activate(stats.power_index[ON_DEATH], &stats, stats.pos);
                        stats.power_ticks[ON_DEATH] = stats.power_cooldown[ON_DEATH];
                    }
                }
			}
			
			break;


        case ENEMY_HALF_DEAD:
            // enemy is at half health or lower

            if (activeAnimation->getCurFrame() == activeAnimation->getMaxFrame()/2 && haz == NULL) {
                    powers->activate(stats.power_index[ON_HALF_DEAD], &stats, stats.pos);
                    stats.power_ticks[ON_HALF_DEAD] = stats.power_cooldown[ON_HALF_DEAD];
            }

			if (activeAnimation->getCurFrame() == activeAnimation->getMaxFrame()-1) {
				newState(ENEMY_STANCE);
			}

            break;


        case ENEMY_DEBUFF:
            // enemy is stunned, bleeding, etc

            if (activeAnimation->getCurFrame() == activeAnimation->getMaxFrame()/2 && haz == NULL) {
                    powers->activate(stats.power_index[ON_DEBUFF], &stats, stats.pos);
                    stats.power_ticks[ON_DEBUFF] = stats.power_cooldown[ON_DEBUFF];
            }

			if (activeAnimation->getCurFrame() == activeAnimation->getMaxFrame()-1) {
				newState(ENEMY_STANCE);
			}

            break;


        case ENEMY_JOIN_COMBAT:
            // enemy joins combat

            if (activeAnimation->getCurFrame() == activeAnimation->getMaxFrame()/2 && haz == NULL) {
                    powers->activate(stats.power_index[ON_JOIN_COMBAT], &stats, stats.pos);
                    stats.power_ticks[ON_JOIN_COMBAT] = stats.power_cooldown[ON_JOIN_COMBAT];
            }

			if (activeAnimation->getCurFrame() == activeAnimation->getMaxFrame()-1) {
				newState(ENEMY_STANCE);
                stats.join_combat = false;
			}

            break;
	}
*/

}

/**
 * Whenever a hazard collides with an enemy, this function resolves the effect
 * Called by HazardManager
 *
 * Returns false on miss
 */
bool Enemy::takeHit(Hazard h) {
	if (stats.cur_state != ENEMY_DEAD && stats.cur_state != ENEMY_CRITDEAD) 
	{   
		if (!stats.in_combat) {
			stats.join_combat = true;
            stats.in_combat = true;
			stats.last_seen.x = stats.hero_pos.x;
			stats.last_seen.y = stats.hero_pos.y;
			powers->activate(stats.power_index[BEACON], &stats, stats.pos); //emit beacon
		}

		// exit if it was a beacon (to prevent stats.targeted from being set)
		if (powers->powers[h.power_index].beacon) return false;

        // prepare the combat text
	    CombatText *combat_text = CombatText::Instance();
	    
		// if it's a miss, do nothing
		if (rand() % 100 > (h.accuracy - stats.avoidance + 25)) {
		    combat_text->addMessage("miss", stats.pos, DISPLAY_MISS);
		    return false; 
		}
		
		// calculate base damage
		int dmg;
		if (h.dmg_max > h.dmg_min) dmg = rand() % (h.dmg_max - h.dmg_min + 1) + h.dmg_min;
		else dmg = h.dmg_min;

		// apply elemental resistance
		// TODO: make this generic
		if (h.trait_elemental == ELEMENT_FIRE) {
			dmg = (dmg * stats.attunement_fire) / 100;
		}
		if (h.trait_elemental == ELEMENT_WATER) {
			dmg = (dmg * stats.attunement_ice) / 100;			
		}
		
		// substract absorption from armor
		int absorption;
		if (!h.trait_armor_penetration) { // armor penetration ignores all absorption
			if (stats.absorb_min == stats.absorb_max) absorption = stats.absorb_min;
			else absorption = stats.absorb_min + (rand() % (stats.absorb_max - stats.absorb_min + 1));
			dmg = dmg - absorption;
			if (dmg < 1 && h.dmg_min >= 1) dmg = 1; // TODO: when blocking, dmg can be reduced to 0
			if (dmg < 0) dmg = 0;
		}

		// check for crits
		int true_crit_chance = h.crit_chance;
		if (stats.stun_duration > 0 || stats.immobilize_duration > 0 || stats.slow_duration > 0)
			true_crit_chance += h.trait_crits_impaired;
			
		bool crit = (rand() % 100) < true_crit_chance;
		if (crit) {
			dmg = dmg + h.dmg_max;
			map->shaky_cam_ticks = FRAMES_PER_SEC/2;
			
			// show crit damage
		    combat_text->addMessage(dmg, stats.pos, DISPLAY_CRIT);
		}
		else {
		    // show normal damage
		    combat_text->addMessage(dmg, stats.pos, DISPLAY_DAMAGE);
		}
		
		// apply damage
		stats.takeDamage(dmg);
		
		// damage always breaks stun
		if (dmg > 0) stats.stun_duration=0;
		
		// after effects
		if (stats.hp > 0) {
			if (h.stun_duration > stats.stun_duration) stats.stun_duration = h.stun_duration;
			if (h.slow_duration > stats.slow_duration) stats.slow_duration = h.slow_duration;
			if (h.bleed_duration > stats.bleed_duration) stats.bleed_duration = h.bleed_duration;
			if (h.immobilize_duration > stats.immobilize_duration) stats.immobilize_duration = h.immobilize_duration;
			if (h.forced_move_duration > stats.forced_move_duration) stats.forced_move_duration = h.forced_move_duration;
			if (h.forced_move_speed != 0) {
				float theta = powers->calcTheta(stats.hero_pos.x, stats.hero_pos.y, stats.pos.x, stats.pos.y);
				stats.forced_speed.x = ceil((float)h.forced_move_speed * cos(theta));
				stats.forced_speed.y = ceil((float)h.forced_move_speed * sin(theta));
			}
			if (h.hp_steal != 0) {
			    int heal_amt = (int)ceil((float)dmg * (float)h.hp_steal / 100.0);
    			combat_text->addMessage(heal_amt, h.src_stats->pos, DISPLAY_HEAL);
				h.src_stats->hp += heal_amt;
				if (h.src_stats->hp > h.src_stats->maxhp) h.src_stats->hp = h.src_stats->maxhp;
			}
			if (h.mp_steal != 0) {
				h.src_stats->mp += (int)ceil((float)dmg * (float)h.mp_steal / 100.0);
				if (h.src_stats->mp > h.src_stats->maxmp) h.src_stats->mp = h.src_stats->maxmp;
			}
		}
		
		// post effect power
		if (h.post_power >= 0 && dmg > 0) {
			powers->activate(h.post_power, h.src_stats, stats.pos);
		}
		
		// interrupted to new state
		if (dmg > 0) {
			
			if (stats.hp <= 0 && crit) {
				doRewards();
				stats.cur_state = ENEMY_CRITDEAD;
			}
			else if (stats.hp <= 0) {
				doRewards();
				stats.cur_state = ENEMY_DEAD;		
			}
			// don't go through a hit animation if stunned
			else if (h.stun_duration == 0) {
				stats.cur_state = ENEMY_HIT;
			}
		}
		
		return true;
	}
	return false;
}

/**
 * Upon enemy death, handle rewards (gold, xp, loot)
 */
void Enemy::doRewards() {

	int roll = rand() % 100;
	if (roll < stats.loot_chance) {
		loot_drop = true;
	}
	reward_xp = true;
	
	// some creatures create special loot if we're on a quest
	if (stats.quest_loot_requires != "") {
	
		// the loot manager will check quest_loot_id
		// if set (not zero), the loot manager will 100% generate that loot.
		if (map->camp->checkStatus(stats.quest_loot_requires) && !map->camp->checkStatus(stats.quest_loot_not)) {
			loot_drop = true;
		}
		else {
			stats.quest_loot_id = 0;
		}
	}
	
	// some creatures drop special loot the first time they are defeated
	// this must be done in conjunction with defeat status
	if (stats.first_defeat_loot > 0) {
		if (!map->camp->checkStatus(stats.defeat_status)) {
			loot_drop = true;
			stats.quest_loot_id = stats.first_defeat_loot;
		}
	}
	
	// defeating some creatures (e.g. bosses) affects the story
	if (stats.defeat_status != "") {
		map->camp->setStatus(stats.defeat_status);
	}

}

/**
 * getRender()
 * Map objects need to be drawn in Z order, so we allow a parent object (GameEngine)
 * to collect all mobile sprites each frame.
 */
Renderable Enemy::getRender() {
	Renderable r = activeAnimation->getCurrentFrame(stats.direction);
	r.map_pos.x = stats.pos.x;
	r.map_pos.y = stats.pos.y;

	// draw corpses below objects so that floor loot is more visible
	r.object_layer = !stats.corpse;

	return r;	
}

Enemy::~Enemy() {
	delete haz;
	delete eb;
}

