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
#include "CampaignManager.h"
#include "CombatText.h"
#include "Enemy.h"
#include "EnemyBehavior.h"
#include "Hazard.h"
#include "LootManager.h"
#include "MapRenderer.h"
#include "PowerManager.h"
#include <sstream>

using namespace std;


Enemy::Enemy(PowerManager *_powers, MapRenderer *_map) : Entity(_map) {
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
			dmg = (dmg * stats.vulnerable_fire) / 100;
		}
		if (h.trait_elemental == ELEMENT_WATER) {
			dmg = (dmg * stats.vulnerable_ice) / 100;
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
			if (h.stun_duration > stats.stun_duration) stats.stun_duration_total = stats.stun_duration = h.stun_duration;
			if (h.slow_duration > stats.slow_duration) stats.slow_duration_total = stats.slow_duration = h.slow_duration;
			if (h.bleed_duration > stats.bleed_duration) stats.bleed_duration_total = stats.bleed_duration = h.bleed_duration;
			if (h.immobilize_duration > stats.immobilize_duration) stats.immobilize_duration_total = stats.immobilize_duration = h.immobilize_duration;
			if (h.forced_move_duration > stats.forced_move_duration) stats.forced_move_duration_total = stats.forced_move_duration = h.forced_move_duration;
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
				map->collider.unblock(stats.pos.x,stats.pos.y);

			}
			else if (stats.hp <= 0) {
				doRewards();
				stats.cur_state = ENEMY_DEAD;
				map->collider.unblock(stats.pos.x,stats.pos.y);

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
	bool loot_drop = false;

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

	if (loot_drop)
		LootManager::getInstance()->addEnemyLoot(this);
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

