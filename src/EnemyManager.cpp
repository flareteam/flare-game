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

#include "EnemyManager.h"

#include "AnimationManager.h"
#include "AnimationSet.h"
#include "Animation.h"
#include "StatBlock.h"
#include "SharedResources.h"
#include "EnemyBehavior.h"
#include "BehaviorStandard.h"

#include <iostream>
#include <algorithm>

using namespace std;

EnemyManager::EnemyManager(PowerManager *_powers, MapRenderer *_map) {
	powers = _powers;
	map = _map;
	enemies = vector<Enemy*>();
	hero_pos.x = hero_pos.y = -1;
	hero_alive = true;
	handleNewMap();
}

bool EnemyManager::loadSounds(const string& type_id) {

	// first check to make sure the sfx isn't already loaded
	if (find(sfx_prefixes.begin(), sfx_prefixes.end(), type_id) != sfx_prefixes.end())
		return true;

	if (audio && SOUND_VOLUME) {
		sound_phys.push_back(Mix_LoadWAV(mods->locate("soundfx/enemies/" + type_id + "_phys.ogg").c_str()));
		sound_ment.push_back(Mix_LoadWAV(mods->locate("soundfx/enemies/" + type_id + "_ment.ogg").c_str()));
		sound_hit.push_back(Mix_LoadWAV(mods->locate("soundfx/enemies/" + type_id + "_hit.ogg").c_str()));
		sound_die.push_back(Mix_LoadWAV(mods->locate("soundfx/enemies/" + type_id + "_die.ogg").c_str()));
		sound_critdie.push_back(Mix_LoadWAV(mods->locate("soundfx/enemies/" + type_id + "_critdie.ogg").c_str()));
	} else {
		sound_phys.push_back(NULL);
		sound_ment.push_back(NULL);
		sound_hit.push_back(NULL);
		sound_die.push_back(NULL);
		sound_critdie.push_back(NULL);
	}

	sfx_prefixes.push_back(type_id);

	return true;
}

bool EnemyManager::loadAnimations(Enemy *e) {

	string animationsname = "animations/enemies/"+e->stats.animations + ".txt";
	AnimationManager::instance()->increaseCount(animationsname);
	e->animationSet = AnimationManager::instance()->getAnimationSet(animationsname);
	e->activeAnimation = e->animationSet->getAnimation(e->animationSet->starting_animation);

	return true;
}

Enemy *EnemyManager::getEnemyPrototype(const string& type_id) {
	for (size_t i = 0; i < prototypes.size(); i++)
		if (prototypes[i].type == type_id) {
			string animationsname = "animations/enemies/"+prototypes[i].stats.animations + ".txt";
			AnimationManager::instance()->increaseCount(animationsname);
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

	if (!loadAnimations(&e)) {
		cerr << "Warning: could not load animations for enemy type" << e.stats.animations << endl;
	}
	if (!loadSounds(e.stats.sfx_prefix)) {
		cerr << "Warning: could not load sounds prefix: " << e.stats.sfx_prefix << endl;
	}

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
		AnimationManager::instance()->decreaseCount(enemies[i]->animationSet->getName());
		delete enemies[i];
	}
	enemies.clear();

	for (unsigned j=0; j<sound_phys.size(); j++) {
		Mix_FreeChunk(sound_phys[j]);
		Mix_FreeChunk(sound_ment[j]);
		Mix_FreeChunk(sound_hit[j]);
		Mix_FreeChunk(sound_die[j]);
		Mix_FreeChunk(sound_critdie[j]);
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
	AnimationManager::instance()->cleanUp();
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
			AnimationManager::instance()->increaseCount(animationname);
			e->animationSet = AnimationManager::instance()->getAnimationSet(animationname);
			if (e->animationSet)
				e->activeAnimation = e->animationSet->getAnimation(e->animationSet->starting_animation);
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

	for (unsigned int i=0; i < enemies.size(); i++) {

		// hazards are processed after Avatar and Enemy[]
		// so process and clear sound effects from previous frames
		// check sound effects
		if (audio == true) {
			vector<string>::iterator found = find (sfx_prefixes.begin(), sfx_prefixes.end(), enemies[i]->stats.sfx_prefix);
			unsigned pref_id = distance(sfx_prefixes.begin(), found);

			if (pref_id >= sfx_prefixes.size()) {
				cerr << "ERROR: enemy sfx_prefix doesn't match registered prefixes (enemy: '"
					 << enemies[i]->stats.name << "', sfx_prefix: '"
					 << enemies[i]->stats.sfx_prefix << "')" << endl;
			} else {
				if (enemies[i]->sfx_phys) Mix_PlayChannel(-1, sound_phys[pref_id], 0);
				if (enemies[i]->sfx_ment) Mix_PlayChannel(-1, sound_ment[pref_id], 0);
				if (enemies[i]->sfx_hit) Mix_PlayChannel(-1, sound_hit[pref_id], 0);
				if (enemies[i]->sfx_die) Mix_PlayChannel(-1, sound_die[pref_id], 0);
				if (enemies[i]->sfx_critdie) Mix_PlayChannel(-1, sound_critdie[pref_id], 0);
			}

			// clear sound flags
			enemies[i]->sfx_hit = false;
			enemies[i]->sfx_phys = false;
			enemies[i]->sfx_ment = false;
			enemies[i]->sfx_die = false;
			enemies[i]->sfx_critdie = false;
		}

		// new actions this round
		enemies[i]->stats.hero_pos = hero_pos;
		enemies[i]->stats.hero_alive = hero_alive;
		enemies[i]->logic();

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

			// draw corpses below objects so that floor loot is more visible
			(dead ? r_dead : r).push_back(re);

			(*it)->stats.updateEffects();
			re = powers->renderEffects(&(*it)->stats);
			(dead ? r_dead : r).push_back(re);
		}
	}
}

EnemyManager::~EnemyManager() {
	for (unsigned int i=0; i < enemies.size(); i++) {
		AnimationManager::instance()->decreaseCount(enemies[i]->animationSet->getName());
		delete enemies[i];
	}

	for (unsigned i=0; i<sound_phys.size(); i++) {
		Mix_FreeChunk(sound_phys[i]);
		Mix_FreeChunk(sound_ment[i]);
		Mix_FreeChunk(sound_hit[i]);
		Mix_FreeChunk(sound_die[i]);
		Mix_FreeChunk(sound_critdie[i]);
	}
}
