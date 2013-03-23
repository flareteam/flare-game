/*
Copyright © 2011-2012 Clint Bellanger
Copyright © 2012 Stefan Beller
Copyright © 2013 Henrik Andersson

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

#include "EnemyManager.h"

#include "AnimationManager.h"
#include "AnimationSet.h"
#include "Animation.h"
#include "SharedResources.h"
#include "EnemyBehavior.h"
#include "BehaviorStandard.h"

#include <iostream>
#include <algorithm>

using namespace std;

EnemyManager::EnemyManager(PowerManager *_powers, MapRenderer *_map)
	: map(_map)
	, powers(_powers)
	, enemies()
	, hero_alive(true)
	, hero_stealth(0)
{
	hero_pos.x = hero_pos.y = -1;
	handleNewMap();
}

void EnemyManager::loadSounds(const string& type_id) {

	// first check to make sure the sfx isn't already loaded
	if (find(sfx_prefixes.begin(), sfx_prefixes.end(), type_id) != sfx_prefixes.end())
		return;

	if (type_id != "none") {
		sound_phys.push_back(snd->load("soundfx/enemies/" + type_id + "_phys.ogg", "EnemyManager physical attack sound"));
		sound_ment.push_back(snd->load("soundfx/enemies/" + type_id + "_ment.ogg", "EnemyManager mental attack sound"));
		sound_hit.push_back(snd->load("soundfx/enemies/" + type_id + "_hit.ogg", "EnemyManager physical hit sound"));
		sound_die.push_back(snd->load("soundfx/enemies/" + type_id + "_die.ogg", "EnemyManager die sound"));
		sound_critdie.push_back(snd->load("soundfx/enemies/" + type_id + "_critdie.ogg", "EnemyManager critdeath sound"));
	} else {
		sound_phys.push_back(0);
		sound_ment.push_back(0);
		sound_hit.push_back(0);
		sound_die.push_back(0);
		sound_critdie.push_back(0);
	}

	sfx_prefixes.push_back(type_id);
}

void EnemyManager::loadAnimations(Enemy *e) {
	string animationsname = "animations/enemies/"+e->stats.animations + ".txt";
	anim->increaseCount(animationsname);
	e->animationSet = anim->getAnimationSet(animationsname);
	e->activeAnimation = e->animationSet->getAnimation();
}

Enemy *EnemyManager::getEnemyPrototype(const string& type_id) {
	for (size_t i = 0; i < prototypes.size(); i++)
		if (prototypes[i].type == type_id) {
			string animationsname = "animations/enemies/"+prototypes[i].stats.animations + ".txt";
			anim->increaseCount(animationsname);
			return new Enemy(prototypes[i]);
		}

	Enemy e = Enemy(powers, map);

	e.eb = new BehaviorStandard(&e);
	e.stats.load("enemies/" + type_id + ".txt");
	e.type = type_id;

	if (e.stats.animations == "")
		cerr << "Warning: no animation file specified for entity: " << type_id << endl;
	if (e.stats.sfx_prefix == "")
		cerr << "Warning: no sfx_prefix specified for entity: " << type_id << endl;

	loadAnimations(&e);
	loadSounds(e.stats.sfx_prefix);

	prototypes.push_back(e);

	return new Enemy(prototypes.back());
}

/**
 * When loading a new map, we eliminate existing enemies and load the new ones.
 * The map will have loaded Entity blocks into an array; retrieve the Enemies and init them
 */
void EnemyManager::handleNewMap () {

	Map_Enemy me;

	// delete existing enemies
	for (unsigned int i=0; i < enemies.size(); i++) {
		anim->decreaseCount(enemies[i]->animationSet->getName());
		delete enemies[i];
	}
	enemies.clear();

	for (unsigned j=0; j<sound_phys.size(); j++) {
		snd->unload(sound_phys[j]);
		snd->unload(sound_ment[j]);
		snd->unload(sound_hit[j]);
		snd->unload(sound_die[j]);
		snd->unload(sound_critdie[j]);
	}
	sfx_prefixes.clear();
	sound_phys.clear();
	sound_ment.clear();
	sound_hit.clear();
	sound_die.clear();
	sound_critdie.clear();

	prototypes.clear();

	// load new enemies
	while (!map->enemies.empty()) {
		me = map->enemies.front();
		map->enemies.pop();

		Enemy *e = getEnemyPrototype(me.type);

		e->stats.waypoints = me.waypoints;
		e->stats.pos.x = me.pos.x;
		e->stats.pos.y = me.pos.y;
		e->stats.direction = me.direction;
		e->stats.wander = me.wander;
		e->stats.wander_area = me.wander_area;

		enemies.push_back(e);

		map->collider.block(me.pos.x, me.pos.y);
	}
	anim->cleanUp();
}

/**
 * Powers can cause new enemies to spawn
 * Check PowerManager for any new queued enemies
 */
void EnemyManager::handleSpawn() {

	Map_Enemy espawn;

	while (!powers->enemies.empty()) {
		espawn = powers->enemies.front();
		powers->enemies.pop();

		Enemy *e = new Enemy(powers, map);
		// factory
		e->eb = new BehaviorStandard(e);

		e->stats.pos.x = espawn.pos.x;
		e->stats.pos.y = espawn.pos.y;
		e->stats.direction = espawn.direction;
		e->stats.load("enemies/" + espawn.type + ".txt");
		if (e->stats.animations != "") {
			// load the animation file if specified
			string animationname = "animations/enemies/"+e->stats.animations + ".txt";
			anim->increaseCount(animationname);
			e->animationSet = anim->getAnimationSet(animationname);
			if (e->animationSet)
				e->activeAnimation = e->animationSet->getAnimation();
			else
				cout << "Warning: animations file could not be loaded for " << espawn.type << endl;
		}
		else {
			cout << "Warning: no animation file specified for entity: " << espawn.type << endl;
		}
		loadSounds(e->stats.sfx_prefix);

		// special animation state for spawning enemies
		e->stats.cur_state = ENEMY_SPAWN;
		enemies.push_back(e);

		map->collider.block(espawn.pos.x, espawn.pos.y);
	}
}

/**
 * perform logic() for all enemies
 */
void EnemyManager::logic() {

	handleSpawn();

	vector<Enemy*>::iterator it;
	for (it = enemies.begin(); it != enemies.end(); ++it) {
		// hazards are processed after Avatar and Enemy[]
		// so process and clear sound effects from previous frames
		// check sound effects
		if (AUDIO) {
			vector<string>::iterator found = find (sfx_prefixes.begin(), sfx_prefixes.end(), (*it)->stats.sfx_prefix);
			unsigned pref_id = distance(sfx_prefixes.begin(), found);

			if (pref_id >= sfx_prefixes.size()) {
				cerr << "ERROR: enemy sfx_prefix doesn't match registered prefixes (enemy: '"
					 << (*it)->stats.name << "', sfx_prefix: '"
					 << (*it)->stats.sfx_prefix << "')" << endl;
			} else {
				if ((*it)->sfx_phys)
					snd->play(sound_phys[pref_id], GLOBAL_VIRTUAL_CHANNEL, (*it)->stats.pos, false);
				if ((*it)->sfx_ment)
					snd->play(sound_ment[pref_id], GLOBAL_VIRTUAL_CHANNEL, (*it)->stats.pos, false);
				if ((*it)->sfx_hit)
					snd->play(sound_hit[pref_id], GLOBAL_VIRTUAL_CHANNEL, (*it)->stats.pos, false);
				if ((*it)->sfx_die)
					snd->play(sound_die[pref_id], GLOBAL_VIRTUAL_CHANNEL, (*it)->stats.pos, false);
				if ((*it)->sfx_critdie)
					snd->play(sound_critdie[pref_id], GLOBAL_VIRTUAL_CHANNEL, (*it)->stats.pos, false);
			}

			// clear sound flags
			(*it)->sfx_hit = false;
			(*it)->sfx_phys = false;
			(*it)->sfx_ment = false;
			(*it)->sfx_die = false;
			(*it)->sfx_critdie = false;
		}

		// new actions this round
		(*it)->stats.hero_pos = hero_pos;
		(*it)->stats.hero_alive = hero_alive;
		(*it)->stats.hero_stealth = hero_stealth;
		(*it)->logic();

	}
}

Enemy* EnemyManager::enemyFocus(Point mouse, Point cam, bool alive_only) {
	Point p;
	SDL_Rect r;
	for(unsigned int i = 0; i < enemies.size(); i++) {
		if(alive_only && (enemies[i]->stats.cur_state == ENEMY_DEAD || enemies[i]->stats.cur_state == ENEMY_CRITDEAD)) {
			continue;
		}
		p = map_to_screen(enemies[i]->stats.pos.x, enemies[i]->stats.pos.y, cam.x, cam.y);

		r.w = enemies[i]->getRender().src.w;
		r.h = enemies[i]->getRender().src.h;
		r.x = p.x - enemies[i]->getRender().offset.x;
		r.y = p.y - enemies[i]->getRender().offset.y;

		if (isWithin(r, mouse)) {
			Enemy *enemy = enemies[i];
			return enemy;
		}
	}
	return NULL;
}

/**
 * If an enemy has died, reward the hero with experience points
 */
void EnemyManager::checkEnemiesforXP(CampaignManager *camp) {
	for (unsigned int i=0; i < enemies.size(); i++) {
		if (enemies[i]->reward_xp) {
			camp->rewardXP(enemies[i]->stats.xp, false);
			enemies[i]->reward_xp = false; // clear flag
		}
	}
}

bool EnemyManager::isCleared() {
	if (enemies.empty()) return true;

	for (unsigned int i=0; i < enemies.size(); i++) {
		if (enemies[i]->stats.alive) return false;
	}

	return true;
}

/**
 * addRenders()
 * Map objects need to be drawn in Z order, so we allow a parent object (GameEngine)
 * to collect all mobile sprites each frame.
 */
void EnemyManager::addRenders(vector<Renderable> &r, vector<Renderable> &r_dead) {
	vector<Enemy*>::iterator it;
	for (it = enemies.begin(); it != enemies.end(); ++it) {
		bool dead = (*it)->stats.corpse;
		if (!dead || (dead && (*it)->stats.corpse_ticks > 0)) {
			Renderable re = (*it)->getRender();
			re.prio = 1;

			// draw corpses below objects so that floor loot is more visible
			(dead ? r_dead : r).push_back(re);

			// add effects
			for (unsigned i = 0; i < (*it)->stats.effects.effect_list.size(); ++i) {
				if ((*it)->stats.effects.effect_list[i].animation) {
					Renderable ren = (*it)->stats.effects.effect_list[i].animation->getCurrentFrame(0);
					ren.map_pos = (*it)->stats.pos;
					if ((*it)->stats.effects.effect_list[i].render_above) ren.prio = 2;
					else ren.prio = 0;
					r.push_back(ren);
				}
			}
		}
	}
}

EnemyManager::~EnemyManager() {
	for (unsigned int i=0; i < enemies.size(); i++) {
		anim->decreaseCount(enemies[i]->animationSet->getName());
		delete enemies[i];
	}

	for (unsigned i=0; i<sound_phys.size(); i++) {
		snd->unload(sound_phys[i]);
		snd->unload(sound_ment[i]);
		snd->unload(sound_hit[i]);
		snd->unload(sound_die[i]);
		snd->unload(sound_critdie[i]);
	}
}
