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
 * activate a Power based on current state and random chance
 */
void BehaviorStandard::checkPower() {

	// currently all enemy power use happens during combat
	if (!e->stats.in_combat) return;
	
	// if the enemy is on global cooldown it cannot act
	if (e->stats.cooldown_ticks > 0) return;
	
	// standard creatures can user a power if they're standing around or moving
	// voluntarily.
	if (!(e->stats.cur_state == ENEMY_STANCE || e->stats.cur_state == ENEMY_MOVE)) return;
	
	// power use requires line of sight
	if (!e->los) return;
	
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

/**
 * Check state changes related to movement
 */
void BehaviorStandard::checkMove() {

}

/**
 * Perform standard state-based actions
 * Return to the default state (Stance) when other actions are complete
 */
void BehaviorStandard::checkState() {

	e->activeAnimation->advanceFrame();

	switch (e->stats.cur_state) {
		case ENEMY_POWER:
		
			// if the first frame of a power animation, play the associated sound effect
			if (e->activeAnimation->isFirstFrame()) {
				
			}
			
			// if we're at the active frame of a power animation, activate
			if (e->activeAnimation->isActiveFrame()) {
				e->powers->activate(e->stats.power_index[e->stats.activated_powerslot], &e->stats, e->pursue_pos);
				e->stats.power_ticks[e->stats.activated_powerslot] = e->stats.power_cooldown[e->stats.activated_powerslot];
				e->stats.cooldown_ticks = e->stats.cooldown;
			}

			// standard enemies revert to Stance after completing a power
			if (e->activeAnimation->isLastFrame()) {
				e->newState(ENEMY_STANCE);
			}
			break;

		case ENEMY_SPAWN:
		// perhaps combine passive animations with no additional effect
		
			// standard enemies revert to Stance after completing a power
			if (e->activeAnimation->isLastFrame()) {
				e->newState(ENEMY_STANCE);
			}
			break;


		default:
			break;
	}
}


