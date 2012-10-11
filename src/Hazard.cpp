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

/**
 * class Hazard
 *
 * Stand-alone object that can harm the hero or creatures
 * These are generated whenever something makes any attack
 */

#include "Animation.h"
#include "AnimationSet.h"
#include "AnimationManager.h"
#include "FileParser.h"
#include "Hazard.h"
#include "MapCollision.h"
#include "SharedResources.h"
#include "Settings.h"
#include "UtilsParsing.h"

using namespace std;

Hazard::Hazard()
	: src_stats(NULL)
	, dmg_min(0)
	, dmg_max(0)
	, crit_chance(0)
	, accuracy(0)
	, source_type(0)
	, pos(FPoint())
	, speed(FPoint())
	, base_speed(0)
	, lifespan(1)
	, radius(0)
	, power_index(0)
	, activeAnimation(NULL)
	, animationKind(0)
	, floor(false)
	, delay_frames(0)
	, complete_animation(false)
	, multitarget(false)
	, active(true)
	, remove_now(false)
	, hit_wall(false)
	, stun_duration(0)
	, immobilize_duration(0)
	, slow_duration(0)
	, bleed_duration(0)
	, forced_move_speed(0)
	, forced_move_duration(0)
	, hp_steal(0)
	, mp_steal(0)
	, trait_armor_penetration(false)
	, trait_crits_impaired(0)
	, trait_elemental(-1)
	, post_power(0)
	, wall_power(0)
	, equipment_modified(false)
{
}

Hazard::~Hazard() {
	delete activeAnimation;
}

void Hazard::setCollision(MapCollision *_collider) {
	collider = _collider;
}

void Hazard::logic() {

	// if the hazard is on delay, take no action
	if (delay_frames > 0) {
		delay_frames--;
		return;
	}

	// handle tickers
	if (lifespan > 0) lifespan--;

	if (activeAnimation)
		activeAnimation->advanceFrame();

	// handle movement
	if (!(round(speed.x) == 0 && round(speed.y) == 0)) {
		pos.x += speed.x;
		pos.y += speed.y;

		// very simplified collider, could skim around corners
		// or even pass through thin walls if speed > tilesize
		if (collider->is_wall(round(pos.x), round(pos.y))) {
			lifespan = 0;
			hit_wall = true;

			if (collider->outsideMap(round(pos.x) >> TILE_SHIFT, round(pos.y) >> TILE_SHIFT))
				remove_now = true;
		}
	}

}

bool Hazard::hasEntity(Entity *ent)
{
	for(vector<Entity*>::iterator it = entitiesCollided.begin(); it != entitiesCollided.end(); ++it)
		if(*it == ent) return true;
	return false;
}

void Hazard::addEntity(Entity *ent)
{
	entitiesCollided.push_back(ent);
}

Renderable Hazard::getRenderable()
{
	Renderable re = activeAnimation->getCurrentFrame(animationKind);
	re.map_pos.x = round(pos.x);
	re.map_pos.y = round(pos.y);
	return re;
}
