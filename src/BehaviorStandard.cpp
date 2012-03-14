/*
Copyright © 2012 Clint Bellanger

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

#include "BehaviorStandard.h"
#include "Enemy.h"
#include "StatBlock.h"

BehaviorStandard::BehaviorStandard(Enemy *_e) : EnemyBehavior(_e) {
}

/**
 * One frame of logic for this behavior
 */
void BehaviorStandard::logic() {

	checkPower();
	checkMove();
	checkState();

}

/**
 * Begin using a power if idle, based on behavior % chances.
 * Activate a ready power, if the attack animation has followed through
 */
void BehaviorStandard::checkPower() {

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
	if (e->los && (e->stats.cur_state == ENEMY_STANCE || e->stats.cur_state == ENEMY_MOVE)) {
	
		// check ranged power use
		if (e->dist > e->stats.melee_range) {
		
			if ((rand() % 100) < e->stats.power_chance[RANGED_PHYS] && e->stats.power_ticks[RANGED_PHYS] == 0) {
				e->newState(ENEMY_POWER);
				e->stats.activated_powerslot = RANGED_PHYS;
				return;
			}
			if ((rand() % 100) < e->stats.power_chance[RANGED_MENT] && e->stats.power_ticks[RANGED_MENT] == 0) {
				e->newState(ENEMY_POWER);
				e->stats.activated_powerslot = RANGED_MENT;
				return;
			}
	
		}
		else { // check melee power use

			if ((rand() % 100) < e->stats.power_chance[MELEE_PHYS] && e->stats.power_ticks[MELEE_PHYS] == 0) {
				e->newState(ENEMY_POWER);
				e->stats.activated_powerslot = MELEE_PHYS;
				return;
			}
			if ((rand() % 100) < e->stats.power_chance[MELEE_MENT] && e->stats.power_ticks[MELEE_MENT] == 0) {
				e->newState(ENEMY_POWER);
				e->stats.activated_powerslot = MELEE_MENT;
				return;
			}	
		}
	}
	
	int power_id;
	int power_slot;
	
	// Activate Power:
	// enemy has started the animation to use a power. Activate the power on the Active animation frame
	if (e->stats.cur_state == ENEMY_POWER) {
	
		power_id = e->stats.power_index[e->stats.activated_powerslot];
		power_slot = e->stats.activated_powerslot;
		
		// if we're at the active frame of a power animation,
		// activate the power and set the local and global cooldowns
		if (e->activeAnimation->isActiveFrame()) {
			e->powers->activate(power_id, &e->stats, e->pursue_pos);
			e->stats.power_ticks[power_slot] = e->stats.power_cooldown[power_slot];
			e->stats.cooldown_ticks = e->stats.cooldown;
		}
	}
	
}

/**
 * Check state changes related to movement
 */
void BehaviorStandard::checkMove() {

}

/**
 * Perform miscellaneous state-based actions.
 * 1) Set animations and sound effects
 * 2) Return to the default state (Stance) when actions are complete
 */
void BehaviorStandard::checkState() {

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
			if (power_state == POWSTATE_SWING) e->setAnimation("swing");
			else if (power_state == POWSTATE_SHOOT) e->setAnimation("shoot");
			else if (power_state == POWSTATE_CAST) e->setAnimation("cast");
		
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
			if (e->activeAnimation->isFirstFrame()) e->sfx_hit = true;
			if (e->activeAnimation->isLastFrame()) e->newState(ENEMY_STANCE);
			break;

		case ENEMY_DEAD:
		
			e->setAnimation("die");
			if (e->activeAnimation->isFirstFrame()) e->sfx_die = true;
			break;
			
		case ENEMY_CRITDEAD:

			e->setAnimation("critdie");
			if (e->activeAnimation->isFirstFrame()) e->sfx_critdie = true;
			break;

		default:
			break;
	}
}


