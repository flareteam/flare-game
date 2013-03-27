/*
Copyright © 2012 Clint Bellanger
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

#include <vector>
#include "Animation.h"
#include "BehaviorStandard.h"
#include "Enemy.h"
#include "MapRenderer.h"
#include "PowerManager.h"
#include "StatBlock.h"
#include "UtilsMath.h"

BehaviorStandard::BehaviorStandard(Enemy *_e) : EnemyBehavior(_e) {
	los = false;
	dist = 0;
	pursue_pos.x = pursue_pos.y = -1;
}

/**
 * One frame of logic for this behavior
 */
void BehaviorStandard::logic() {

	// skip all logic if the enemy is dead and no longer animating
	if (e->stats.corpse) {
		if (e->stats.corpse_ticks > 0)
			e->stats.corpse_ticks--;
		return;
	}
	doUpkeep();
	findTarget();
	checkPower();
	checkMove();
	updateState();

}

/**
 * Various upkeep on stats
 * TODO: some of these actions could be moved to StatBlock::logic()
 */
void BehaviorStandard::doUpkeep() {
	// activate all passive powers
	if (e->stats.hp > 0 || e->stats.effects.triggered_death) e->powers->activatePassives(&e->stats);

	e->stats.logic();

	// heal rapidly while not in combat
	if (!e->stats.in_combat) {
		if (e->stats.alive && e->stats.hero_alive) {
			e->stats.hp++;
			if (e->stats.hp > e->stats.maxhp) e->stats.hp = e->stats.maxhp;
		}
	}

	if (e->stats.effects.forced_move) {
		e->move();
	}

	if (e->stats.waypoint_pause_ticks > 0)
		e->stats.waypoint_pause_ticks--;

	if (e->stats.wander_ticks > 0)
		e->stats.wander_ticks--;

	if (e->stats.wander_pause_ticks > 0)
		e->stats.wander_pause_ticks--;

	// check for revive
	if (e->stats.hp <= 0 && e->stats.effects.revive) {
		e->stats.hp = e->stats.maxhp;
		e->stats.alive = true;
		e->stats.corpse = false;
		e->stats.cur_state = ENEMY_STANCE;
	}

	// check for bleeding to death
	if (e->stats.hp <= 0 && !(e->stats.cur_state == ENEMY_DEAD || e->stats.cur_state == ENEMY_CRITDEAD)) {
		e->doRewards();
		e->stats.effects.triggered_death = true;
		e->stats.cur_state = ENEMY_DEAD;
		e->map->collider.unblock(e->stats.pos.x,e->stats.pos.y);
	}

	// TEMP: check for bleeding spurt
	if (e->stats.effects.damage > 0 && e->stats.hp > 0) {
		comb->addMessage(e->stats.effects.damage, e->stats.pos, COMBAT_MESSAGE_TAKEDMG);
	}

	// check for teleport powers
	if (e->stats.teleportation) {

		e->map->collider.unblock(e->stats.pos.x,e->stats.pos.y);

		e->stats.pos.x = e->stats.teleport_destination.x;
		e->stats.pos.y = e->stats.teleport_destination.y;

		e->map->collider.block(e->stats.pos.x,e->stats.pos.y);

		e->stats.teleportation = false;
	}
}

/**
 * Locate the player and set various targeting info
 */
void BehaviorStandard::findTarget() {
	int stealth_threat_range = (e->stats.threat_range * (100 - e->stats.hero_stealth)) / 100;

	// stunned enemies can't act
	if (e->stats.effects.stun) return;

	// check distance and line of sight between enemy and hero
	if (e->stats.hero_alive)
		dist = e->getDistance(e->stats.hero_pos);
	else
		dist = 0;

	// check line-of-sight
	if (dist < e->stats.threat_range && e->stats.hero_alive)
		los = e->map->collider.line_of_sight(e->stats.pos.x, e->stats.pos.y, e->stats.hero_pos.x, e->stats.hero_pos.y);
	else
		los = false;

	// check entering combat (because the player hit the enemy)
	if (e->stats.join_combat) {
		if (dist <= (stealth_threat_range *2)) {
			e->stats.join_combat = false;
		}
		else {
			e->stats.in_combat = true;
			e->powers->activate(e->stats.power_index[BEACON], &e->stats, e->stats.pos); //emit beacon
		}
	}

	// check entering combat (because the player got too close)
	if (!e->stats.in_combat && los && dist < stealth_threat_range) {

		if (e->stats.in_combat) e->stats.join_combat = true;
		e->stats.in_combat = true;
		e->powers->activate(e->stats.power_index[BEACON], &e->stats, e->stats.pos); //emit beacon
	}

	// check exiting combat (player died or got too far away)
	if (e->stats.in_combat && dist > (e->stats.threat_range *2) && !e->stats.join_combat) {
		e->stats.in_combat = false;
	}

	// check exiting combat (player or enemy died)
	if (!e->stats.alive || !e->stats.hero_alive) {
		e->stats.in_combat = false;
	}

	// if the creature is a wanderer, pick a random point within the wander area to travel to
	if (e->stats.wander && !e->stats.in_combat && e->stats.wander_area.w > 0 && e->stats.wander_area.h > 0) {
		if (e->stats.wander_ticks == 0) {
			pursue_pos.x = e->stats.wander_area.x + (rand() % (e->stats.wander_area.w));
			pursue_pos.y = e->stats.wander_area.y + (rand() % (e->stats.wander_area.h));
			e->stats.wander_ticks = (rand() % 150) + 150;
		}
	} else {
		// by default, the enemy pursues the hero directly
		pursue_pos.x = e->stats.hero_pos.x;
		pursue_pos.y = e->stats.hero_pos.y;

		if (!(e->stats.in_combat || e->stats.waypoints.empty())) {
			Point waypoint = e->stats.waypoints.front();
			pursue_pos.x = waypoint.x;
			pursue_pos.y = waypoint.y;
		}
	}
}


/**
 * Begin using a power if idle, based on behavior % chances.
 * Activate a ready power, if the attack animation has followed through
 */
void BehaviorStandard::checkPower() {

	// stunned enemies can't act
	if (e->stats.effects.stun) return;

	// currently all enemy power use happens during combat
	if (!e->stats.in_combat) return;

	// if the enemy is on global cooldown it cannot act
	if (e->stats.cooldown_ticks > 0) return;

	// Note there are two stages to activating a power.
	// First is the enemy choosing to use a power based on behavioral chance
	// Second is the power actually firing off once the related animation reaches the active frame.
	// (these are separate so that interruptions can take place)

	// Begin Power Animation:
	// standard enemies can begin a power-use animation if they're standing around or moving voluntarily.
	if (los && (e->stats.cur_state == ENEMY_STANCE || e->stats.cur_state == ENEMY_MOVE)) {

		// check half dead power use
		if (!e->stats.on_half_dead_casted && e->stats.hp <= e->stats.maxhp/2) {
			if (percentChance(e->stats.power_chance[ON_HALF_DEAD])) {
				e->newState(ENEMY_POWER);
				e->stats.activated_powerslot = ON_HALF_DEAD;
				return;
			}
		}

		// check ranged power use
		if (dist > e->stats.melee_range) {

			if (percentChance(e->stats.power_chance[RANGED_PHYS]) && e->stats.power_ticks[RANGED_PHYS] == 0) {
				e->newState(ENEMY_POWER);
				e->stats.activated_powerslot = RANGED_PHYS;
				return;
			}
			if (percentChance(e->stats.power_chance[RANGED_MENT]) && e->stats.power_ticks[RANGED_MENT] == 0) {
				e->newState(ENEMY_POWER);
				e->stats.activated_powerslot = RANGED_MENT;
				return;
			}

		}
		else { // check melee power use

			if (percentChance(e->stats.power_chance[MELEE_PHYS]) && e->stats.power_ticks[MELEE_PHYS] == 0) {
				e->newState(ENEMY_POWER);
				e->stats.activated_powerslot = MELEE_PHYS;
				return;
			}
			if (percentChance(e->stats.power_chance[MELEE_MENT]) && e->stats.power_ticks[MELEE_MENT] == 0) {
				e->newState(ENEMY_POWER);
				e->stats.activated_powerslot = MELEE_MENT;
				return;
			}
		}
	}


	// Activate Power:
	// enemy has started the animation to use a power. Activate the power on the Active animation frame
	if (e->stats.cur_state == ENEMY_POWER) {

		// if we're at the active frame of a power animation,
		// activate the power and set the local and global cooldowns
		if (e->activeAnimation->isActiveFrame() || e->instant_power) {
			e->instant_power = false;

			int power_slot =  e->stats.activated_powerslot;
			int power_id = e->stats.power_index[e->stats.activated_powerslot];

			e->powers->activate(power_id, &e->stats, pursue_pos);
			e->stats.power_ticks[power_slot] = e->stats.power_cooldown[power_slot];
			e->stats.cooldown_ticks = e->stats.cooldown;

			if (e->stats.activated_powerslot == ON_HALF_DEAD) {
				e->stats.on_half_dead_casted = true;
			}
		}
	}

}

/**
 * Check state changes related to movement
 */
void BehaviorStandard::checkMove() {

	// dying enemies can't move
	if (e->stats.cur_state == ENEMY_DEAD || e->stats.cur_state == ENEMY_CRITDEAD) return;

	// stunned enemies can't act
	if (e->stats.effects.stun) return;

	// handle not being in combat and (not patrolling waypoints or waiting at waypoint)
	if (!e->stats.in_combat && (e->stats.waypoints.empty() || e->stats.waypoint_pause_ticks > 0) && (!e->stats.wander || e->stats.wander_pause_ticks > 0)) {

		if (e->stats.cur_state == ENEMY_MOVE) {
			e->newState(ENEMY_STANCE);
		}

		// currently enemies only move while in combat or patrolling
		return;
	}

	// clear current space to allow correct movement
	e->map->collider.unblock(e->stats.pos.x, e->stats.pos.y);

	// update direction
	if (e->stats.facing) {
		if (++e->stats.turn_ticks > e->stats.turn_delay) {

			// if blocked, face in pathfinder direction instead
			if (!e->map->collider.line_of_movement(e->stats.pos.x, e->stats.pos.y, e->stats.hero_pos.x, e->stats.hero_pos.y, e->stats.movement_type)) {

				// if a path is returned, target first waypoint
				std::vector<Point> path;

				if ( e->map->collider.compute_path(e->stats.pos, pursue_pos, path, e->stats.movement_type) ) {
					pursue_pos = path.back();
				}
			}

			e->stats.direction = e->face(pursue_pos.x, pursue_pos.y);
			e->stats.turn_ticks = 0;
		}
	}
	int prev_direction = e->stats.direction;

	// try to start moving
	if (e->stats.cur_state == ENEMY_STANCE) {

		if (dist < e->stats.melee_range) {
			// too close, do nothing
		}
		else if (percentChance(e->stats.chance_pursue)) {

			if (e->move()) {
				e->newState(ENEMY_MOVE);
			}
			else {

				// hit an obstacle, try the next best angle
				e->stats.direction = e->faceNextBest(pursue_pos.x, pursue_pos.y);
				if (e->move()) {
					e->newState(ENEMY_MOVE);
				}
				else e->stats.direction = prev_direction;
			}
		}
	}

	// already moving
	else if (e->stats.cur_state == ENEMY_MOVE) {

		// close enough to the hero
		if (dist < e->stats.melee_range) {
			e->newState(ENEMY_STANCE);
		}

		// try to continue moving
		else if (!e->move()) {

			// hit an obstacle.  Try the next best angle
			e->stats.direction = e->faceNextBest(pursue_pos.x, pursue_pos.y);
			if (!e->move()) {
				e->newState(ENEMY_STANCE);
				e->stats.direction = prev_direction;
			}
		}
	}

	// if patrolling waypoints and has reached a waypoint, cycle to the next one
	if (!e->stats.waypoints.empty()) {
		Point waypoint = e->stats.waypoints.front();
		Point pos = e->stats.pos;
		// if the patroller is close to the waypoint
		if (abs(waypoint.x - pos.x) < UNITS_PER_TILE/2 && abs(waypoint.y - pos.y) < UNITS_PER_TILE/2) {
			e->stats.waypoints.pop();
			e->stats.waypoints.push(waypoint);
			e->stats.waypoint_pause_ticks = e->stats.waypoint_pause;
		}
	}

	// if a wandering enemy reaches its destination early, reset wander_ticks
	if (e->stats.wander) {
		Point pos = e->stats.pos;
		if (abs(pursue_pos.x - pos.x) < UNITS_PER_TILE/2 && abs(pursue_pos.y - pos.y) < UNITS_PER_TILE/2) {
			e->stats.wander_ticks = 0;
		}
		if (e->stats.wander_ticks == 0 && e->stats.wander_pause_ticks == 0) {
			e->stats.wander_pause_ticks = rand() % 60;
		}
	}

	// re-block current space to allow correct movement
	e->map->collider.block(e->stats.pos.x, e->stats.pos.y);

}

/**
 * Perform miscellaneous state-based actions.
 * 1) Set animations and sound effects
 * 2) Return to the default state (Stance) when actions are complete
 */
void BehaviorStandard::updateState() {

	// stunned enemies can't act
	if (e->stats.effects.stun) return;

	int power_id;
	int power_state;

	// continue current animations
	e->activeAnimation->advanceFrame();

	switch (e->stats.cur_state) {

		case ENEMY_STANCE:

			e->setAnimation("stance");
			break;

		case ENEMY_MOVE:

			e->setAnimation("run");
			break;

		case ENEMY_POWER:

			power_id = e->stats.power_index[e->stats.activated_powerslot];
			power_state = e->powers->powers[power_id].new_state;

			// animation based on power type
			if (power_state == POWSTATE_SWING) e->setAnimation("melee");
			else if (power_state == POWSTATE_SHOOT) e->setAnimation("ranged");
			else if (power_state == POWSTATE_CAST) e->setAnimation("ment");
			else if (power_state == POWSTATE_INSTANT) e->instant_power = true;

			// sound effect based on power type
			if (e->activeAnimation->isFirstFrame()) {
				if (power_state == POWSTATE_SWING) e->sfx_phys = true;
				else if (power_state == POWSTATE_SHOOT) e->sfx_phys = true;
				else if (power_state == POWSTATE_CAST) e->sfx_ment = true;
			}

			if (e->activeAnimation->isLastFrame()) e->newState(ENEMY_STANCE);
			break;

		case ENEMY_SPAWN:

			e->setAnimation("spawn");
			if (e->activeAnimation->isLastFrame()) e->newState(ENEMY_STANCE);
			break;

		case ENEMY_BLOCK:

			e->setAnimation("block");
			break;

		case ENEMY_HIT:

			e->setAnimation("hit");
			if (e->activeAnimation->isFirstFrame()) {
				e->stats.effects.triggered_hit = true;
			}
			if (e->activeAnimation->isLastFrame()) e->newState(ENEMY_STANCE);
			break;

		case ENEMY_DEAD:
			if (e->stats.effects.triggered_death) break;

			e->setAnimation("die");
			if (e->activeAnimation->isFirstFrame()) {
				e->sfx_die = true;
				e->stats.corpse_ticks = CORPSE_TIMEOUT;
				e->stats.effects.clearEffects();
			}
			if (e->activeAnimation->isSecondLastFrame()) {
				if (percentChance(e->stats.power_chance[ON_DEATH]))
					e->powers->activate(e->stats.power_index[ON_DEATH], &e->stats, e->stats.pos);
			}
			if (e->activeAnimation->isLastFrame()) e->stats.corpse = true; // puts renderable under object layer

			break;

		case ENEMY_CRITDEAD:

			e->setAnimation("critdie");
			if (e->activeAnimation->isFirstFrame()) {
				e->sfx_critdie = true;
				e->stats.corpse_ticks = CORPSE_TIMEOUT;
				e->stats.effects.clearEffects();
			}
			if (e->activeAnimation->isSecondLastFrame()) {
				if (percentChance(e->stats.power_chance[ON_DEATH]))
					e->powers->activate(e->stats.power_index[ON_DEATH], &e->stats, e->stats.pos);
			}
			if (e->activeAnimation->isLastFrame()) e->stats.corpse = true; // puts renderable under object layer

			break;

		default:
			break;
	}
}


