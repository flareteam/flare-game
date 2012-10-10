/*
Copyright © 2011-2012 Clint Bellanger
Copyright © 2012 Igor Paliychuk
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
 * class PowerManager
 */

#include "PowerManager.h"
#include "Animation.h"
#include "AnimationSet.h"
#include "AnimationManager.h"
#include "CombatText.h"
#include "FileParser.h"
#include "Hazard.h"
#include "SharedResources.h"
#include "Settings.h"
#include "StatBlock.h"
#include "MapCollision.h"
#include "UtilsFileSystem.h"
#include "UtilsParsing.h"

#include <cmath>
#include <iostream>
#include <climits>
using namespace std;


/**
 * PowerManager constructor
 */
PowerManager::PowerManager() {

	used_item=-1;

	log_msg = "";

	loadAll();
}

/**
 * Load all powers files in all mods
 */
void PowerManager::loadAll() {

	string test_path;

	// load each config file
	for (unsigned int i = 0; i < mods->mod_list.size(); i++) {

		test_path = PATH_DATA + "mods/" + mods->mod_list[i] + "/powers/powers.txt";

		if (fileExists(test_path)) {
			this->loadPowers(test_path);
		}

		test_path = PATH_DATA + "mods/" + mods->mod_list[i] + "/engine/effects.txt";

		if (fileExists(test_path)) {
			this->loadEffects(test_path);
		}
	}

}


/**
 * Powers are defined in [mod]/powers/powers.txt
 *
 * @param filename The full path and filename to this powers.txt file
 */
void PowerManager::loadPowers(const std::string& filename) {
	FileParser infile;
	if (!infile.open(filename)) {
		fprintf(stderr, "Unable to open %s!\n", filename.c_str());
		return;
	}

	int input_id = 0;
	bool skippingEntry = false;

	while (infile.next()) {
		// id needs to be the first component of each power.  That is how we write
		// data to the correct power.
		if (infile.key == "id") {
			input_id = toInt(infile.val);
			skippingEntry = input_id < 1;
			if (skippingEntry)
				fprintf(stderr, "Power index out of bounds 1-%d, skipping\n", INT_MAX);
			if (static_cast<int>(powers.size()) < input_id + 1)
				powers.resize(input_id + 1);
			continue;
		}
		if (skippingEntry)
			continue;

		if (infile.key == "type") {
			if (infile.val == "effect") powers[input_id].type = POWTYPE_EFFECT;
			else if (infile.val == "missile") powers[input_id].type = POWTYPE_MISSILE;
			else if (infile.val == "repeater") powers[input_id].type = POWTYPE_REPEATER;
			else if (infile.val == "spawn") powers[input_id].type = POWTYPE_SPAWN;
			else if (infile.val == "transform") powers[input_id].type = POWTYPE_TRANSFORM;
			else fprintf(stderr, "unknown type %s\n", infile.val.c_str());
		}
		else if (infile.key == "name")
			powers[input_id].name = msg->get(infile.val);
		else if (infile.key == "description")
			powers[input_id].description = msg->get(infile.val);
		else if (infile.key == "icon")
			powers[input_id].icon = toInt(infile.val);
		else if (infile.key == "new_state") {
			if (infile.val == "swing") powers[input_id].new_state = POWSTATE_SWING;
			else if (infile.val == "shoot") powers[input_id].new_state = POWSTATE_SHOOT;
			else if (infile.val == "cast") powers[input_id].new_state = POWSTATE_CAST;
			else if (infile.val == "block") powers[input_id].new_state = POWSTATE_BLOCK;
			else if (infile.val == "instant") powers[input_id].new_state = POWSTATE_INSTANT;
			else fprintf(stderr, "unknown new_state %s\n", infile.val.c_str());
		}
		else if (infile.key == "face")
			powers[input_id].face = toBool(infile.val);
		else if (infile.key == "source_type") {
			if (infile.val == "hero") powers[input_id].source_type = SOURCE_TYPE_HERO;
			else if (infile.val == "neutral") powers[input_id].source_type = SOURCE_TYPE_NEUTRAL;
			else if (infile.val == "enemy") powers[input_id].source_type = SOURCE_TYPE_ENEMY;
			else fprintf(stderr, "unknown source_type %s\n", infile.val.c_str());
		}
		else if (infile.key == "beacon")
			powers[input_id].beacon = toBool(infile.val);
		else if (infile.key == "count")
			powers[input_id].count = toInt(infile.val);
		// power requirements
		else if (infile.key == "requires_physical_weapon")
			powers[input_id].requires_physical_weapon = toBool(infile.val);
		else if (infile.key == "requires_mental_weapon")
			powers[input_id].requires_mental_weapon = toBool(infile.val);
		else if (infile.key == "requires_offense_weapon")
			powers[input_id].requires_offense_weapon = toBool(infile.val);
		else if (infile.key == "requires_mp")
			powers[input_id].requires_mp = toInt(infile.val);
		else if (infile.key == "requires_los")
			powers[input_id].requires_los = toBool(infile.val);
		else if (infile.key == "requires_empty_target")
			powers[input_id].requires_empty_target = toBool(infile.val);
		else if (infile.key == "requires_item")
			powers[input_id].requires_item = toInt(infile.val);
		else if (infile.key == "requires_targeting")
			powers[input_id].requires_targeting =toBool(infile.val);
		else if (infile.key == "cooldown")
			powers[input_id].cooldown = toInt(infile.val);
		// animation info
		else if (infile.key == "animation") {
			string animation_name = "animations/powers/" + infile.val;
			AnimationManager::instance()->increaseCount(animation_name);
			powers[input_id].animationSet = AnimationManager::instance()->getAnimationSet(animation_name);
		}
		else if (infile.key == "sfx")
			powers[input_id].sfx_index = loadSFX(infile.val);
		else if (infile.key == "directional")
			powers[input_id].directional = toBool(infile.val);
		else if (infile.key == "visual_random")
			powers[input_id].visual_random = toInt(infile.val);
		else if (infile.key == "visual_option")
			powers[input_id].visual_option = toInt(infile.val);
		else if (infile.key == "aim_assist")
			powers[input_id].aim_assist = toInt(infile.val);
		else if (infile.key == "speed")
			powers[input_id].speed = toInt(infile.val);
		else if (infile.key == "lifespan")
			powers[input_id].lifespan = toInt(infile.val);
		else if (infile.key == "frame_loop")
			powers[input_id].frame_loop = toInt(infile.val);
		else if (infile.key == "frame_duration")
			powers[input_id].frame_duration = toInt(infile.val);
		else if (infile.key == "frame_size") {
			powers[input_id].frame_size.x = toInt(infile.nextValue());
			powers[input_id].frame_size.y = toInt(infile.nextValue());
		}
		else if (infile.key == "frame_offset") {
			powers[input_id].frame_offset.x = toInt(infile.nextValue());
			powers[input_id].frame_offset.y = toInt(infile.nextValue());
		}
		else if (infile.key == "floor")
			powers[input_id].floor = toBool(infile.val);
		else if (infile.key == "active_frame")
			powers[input_id].active_frame = toInt(infile.val);
		else if (infile.key == "complete_animation")
			powers[input_id].complete_animation = toBool(infile.val);
		// hazard traits
		else if (infile.key == "use_hazard")
			powers[input_id].use_hazard = toBool(infile.val);
		else if (infile.key == "no_attack")
			powers[input_id].no_attack = toBool(infile.val);
		else if (infile.key == "radius")
			powers[input_id].radius = toInt(infile.val);
		else if (infile.key == "base_damage") {
			if (infile.val == "none")        powers[input_id].base_damage = BASE_DAMAGE_NONE;
			else if (infile.val == "melee")  powers[input_id].base_damage = BASE_DAMAGE_MELEE;
			else if (infile.val == "ranged") powers[input_id].base_damage = BASE_DAMAGE_RANGED;
			else if (infile.val == "ment")   powers[input_id].base_damage = BASE_DAMAGE_MENT;
			else fprintf(stderr, "unknown base_damage %s\n", infile.val.c_str());
		}
		else if (infile.key == "damage_multiplier")
			powers[input_id].damage_multiplier = toInt(infile.val);
		else if (infile.key == "starting_pos") {
			if (infile.val == "source")      powers[input_id].starting_pos = STARTING_POS_SOURCE;
			else if (infile.val == "target") powers[input_id].starting_pos = STARTING_POS_TARGET;
			else if (infile.val == "melee")  powers[input_id].starting_pos = STARTING_POS_MELEE;
			else fprintf(stderr, "unknown starting_pos %s\n", infile.val.c_str());
		}
		else if (infile.key == "multitarget")
			powers[input_id].multitarget = toBool(infile.val);
		else if (infile.key == "trait_armor_penetration")
			powers[input_id].trait_armor_penetration = toBool(infile.val);
		else if (infile.key == "trait_crits_impaired")
			powers[input_id].trait_crits_impaired = toInt(infile.val);
		else if (infile.key == "trait_elemental") {
			for (unsigned int i=0; i<ELEMENTS.size(); i++) {
				if (infile.val == ELEMENTS[i].name) powers[input_id].trait_elemental = i;
			}
		}
		else if (infile.key == "forced_move") {
			powers[input_id].forced_move_speed = toInt(infile.nextValue());
			powers[input_id].forced_move_duration = toInt(infile.nextValue());
		}
		else if (infile.key == "range")
			powers[input_id].range = toInt(infile.nextValue());
		//steal effects
		else if (infile.key == "hp_steal")
			powers[input_id].hp_steal = toInt(infile.val);
		else if (infile.key == "mp_steal")
			powers[input_id].mp_steal = toInt(infile.val);
		//missile modifiers
		else if (infile.key == "missile_angle")
			powers[input_id].missile_angle = toInt(infile.val);
		else if (infile.key == "angle_variance")
			powers[input_id].angle_variance = toInt(infile.val);
		else if (infile.key == "speed_variance")
			powers[input_id].speed_variance = toInt(infile.val);
		//repeater modifiers
		else if (infile.key == "delay")
			powers[input_id].delay = toInt(infile.val);
		else if (infile.key == "start_frame")
			powers[input_id].start_frame = toInt(infile.val);
		// buff/debuff durations
		else if (infile.key == "bleed_duration")
			powers[input_id].bleed_duration = toInt(infile.val);
		else if (infile.key == "stun_duration")
			powers[input_id].stun_duration = toInt(infile.val);
		else if (infile.key == "slow_duration")
			powers[input_id].slow_duration = toInt(infile.val);
		else if (infile.key == "immobilize_duration")
			powers[input_id].immobilize_duration = toInt(infile.val);
		else if (infile.key == "immunity_duration")
			powers[input_id].immunity_duration = toInt(infile.val);
		else if (infile.key == "transform_duration")
			powers[input_id].transform_duration = toInt(infile.val);
		else if (infile.key == "manual_untransform")
			powers[input_id].manual_untransform = toBool(infile.val);
		else if (infile.key == "haste_duration")
			powers[input_id].haste_duration = toInt(infile.val);
		else if (infile.key == "hot_duration")
			powers[input_id].hot_duration = toInt(infile.val);
		else if (infile.key == "hot_value")
			powers[input_id].hot_value = toInt(infile.val);
		// buffs
		else if (infile.key == "buff_heal")
			powers[input_id].buff_heal = toBool(infile.val);
		else if (infile.key == "buff_shield")
			powers[input_id].buff_shield = toBool(infile.val);
		else if (infile.key == "buff_teleport")
			powers[input_id].buff_teleport = toBool(infile.val);
		else if (infile.key == "buff_immunity")
			powers[input_id].buff_immunity = toBool(infile.val);
		else if (infile.key == "buff_restore_hp")
			powers[input_id].buff_restore_hp = toInt(infile.val);
		else if (infile.key == "buff_restore_mp")
			powers[input_id].buff_restore_mp = toInt(infile.val);
		// pre and post power effects
		else if (infile.key == "post_power")
			powers[input_id].post_power = toInt(infile.val);
		else if (infile.key == "wall_power")
			powers[input_id].wall_power = toInt(infile.val);
		else if (infile.key == "allow_power_mod")
			powers[input_id].allow_power_mod = toBool(infile.val);
		// spawn info
		else if (infile.key == "spawn_type")
			powers[input_id].spawn_type = infile.val;
		else if (infile.key == "target_neighbor")
			powers[input_id].target_neighbor = toInt(infile.val);
		else
			fprintf(stderr, "ignoring unknown key %s set to %s\n", infile.key.c_str(), infile.val.c_str());
	}
	infile.close();
}

/**
 * Effects are defined in [mod]/engine/effects.txt
 *
 * @param filename The full path and filename to this engine.txt file
 */
void PowerManager::loadEffects(const std::string& filename) {
	FileParser infile;
	if (!infile.open(filename)) {
		fprintf(stderr, "Unable to open %s!\n", filename.c_str());
		return;
	}

	int input_id = 0;
	bool skippingEntry = false;
	while (infile.next()) {
		// id needs to be the first component of each effect.  That is how we write
		// data to the correct effect.
		if (infile.key == "id") {
			input_id = toInt(infile.val);
			skippingEntry = input_id < 1;
			if (skippingEntry)
				fprintf(stderr, "Effect index out of bounds 1-%d, skipping\n", INT_MAX);
			if (static_cast<int>(effects.size()) < input_id + 1)
				effects.resize(input_id + 1);
			continue;
		}
		if (skippingEntry)
			continue;

		infile.val = infile.val + ',';

		if (infile.key == "type") {
			effects[input_id].type = eatFirstString(infile.val,',');
		} else if (infile.key == "icon") {
			effects[input_id].icon = eatFirstInt(infile.val,',');
		} else if (infile.key == "gfx") {
			effects[input_id].gfx = NULL;
			SDL_Surface *surface = IMG_Load(mods->locate("images/powers/" + eatFirstString(infile.val,',')).c_str());
			if(!surface) {
				fprintf(stderr, "Couldn't load effect sprites: %s\n", IMG_GetError());
			} else {
				effects[input_id].gfx = SDL_DisplayFormatAlpha(surface);
				SDL_FreeSurface(surface);
			}
		} else if (infile.key == "size") {
			effects[input_id].frame_size.x = eatFirstInt(infile.val, ',');
			effects[input_id].frame_size.y = eatFirstInt(infile.val, ',');
			effects[input_id].frame_size.w = eatFirstInt(infile.val, ',');
			effects[input_id].frame_size.h = eatFirstInt(infile.val, ',');
		} else if (infile.key == "offset") {
			effects[input_id].frame_offset.x = eatFirstInt(infile.val, ',');
			effects[input_id].frame_offset.y = eatFirstInt(infile.val, ',');
		} else if (infile.key == "frame_total") {
			effects[input_id].frame_total = eatFirstInt(infile.val, ',');
		} else if (infile.key == "ticks_per_frame") {
			effects[input_id].ticks_per_frame = eatFirstInt(infile.val, ',');
		}
	}
	infile.close();
}

/**
 * Load the specified sound effect for this power
 *
 * @param filename The .ogg file containing the sound for this power, assumed to be in soundfx/powers/
 * @return The sfx[] array index for this mix chunk, or -1 upon load failure
 */
int PowerManager::loadSFX(const string& filename) {

		// first check to make sure the sound isn't already loaded
		for (unsigned i=0; i<sfx_filenames.size(); i++) {
			if (sfx_filenames[i] == filename) {
				return i; // already have this one
			}
		}

		// we don't already have this sound loaded, so load it
		Mix_Chunk* sound;
		if (audio && SOUND_VOLUME) {
			sound = Mix_LoadWAV(mods->locate("soundfx/powers/" + filename).c_str());
			if(!sound) {
				cerr << "Couldn't load power soundfx: " << filename << endl;
				return -1;
			}
		} else {
			sound = NULL;
		}

		// success; perform record-keeping
		sfx_filenames.push_back(filename);
		sfx.push_back(sound);
		return sfx.size() - 1;
}


/**
 * Set new collision object
 */
void PowerManager::handleNewMap(MapCollision *_collider) {
	collider = _collider;
}

// convert cartesian to polar theta where (x1,x2) is the origin
float PowerManager::calcTheta(int x1, int y1, int x2, int y2) {

	float pi = 3.1415926535898;

	// calculate base angle
	float dx = (float)x2 - (float)x1;
	float dy = (float)y2 - (float)y1;
	int exact_dx = x2 - x1;
	float theta;

	// convert cartesian to polar coordinates
	if (exact_dx == 0) {
		if (dy > 0.0) theta = pi/2.0;
		else theta = -pi/2.0;
	}
	else {
		theta = atan(dy/dx);
		if (dx < 0.0 && dy >= 0.0) theta += pi;
		if (dx < 0.0 && dy < 0.0) theta -= pi;
	}
	return theta;
}

/**
 * Change direction to face the target map location
 */
int PowerManager::calcDirection(int origin_x, int origin_y, int target_x, int target_y) {

	// TODO: use calcTheta instead and check for the areas between -PI and PI

	// inverting Y to convert map coordinates to standard cartesian coordinates
	int dx = target_x - origin_x;
	int dy = origin_y - target_y;

	// avoid div by zero
	if (dx == 0) {
		if (dy > 0) return 3;
		else return 7;
	}

	float slope = ((float)dy)/((float)dx);
	if (0.5 <= slope && slope <= 2.0) {
		if (dy > 0) return 4;
		else return 0;
	}
	if (-0.5 <= slope && slope <= 0.5) {
		if (dx > 0) return 5;
		else return 1;
	}
	if (-2.0 <= slope && slope <= -0.5) {
		if (dx > 0) return 6;
		else return 2;
	}
	if (2.0 <= slope || -2.0 >= slope) {
		if (dy > 0) return 3;
		else return 7;
	}
	return 0;
}

/**
 * Keep two points within a certain range
 */
Point PowerManager::limitRange(int range, Point src, Point target) {
	if (range > 0) {
		if (src.x+range < target.x)
			target.x = src.x+range;
		if (src.x-range > target.x)
			target.x = src.x-range;
		if (src.y+range < target.y)
			target.y = src.y+range;
		if (src.y-range > target.y)
			target.y = src.y-range;
	}

	return target;
}

/**
 * Check if the target is valid (not an empty area or a wall)
 */
bool PowerManager::hasValidTarget(int power_index, StatBlock *src_stats, Point target) {

	target = limitRange(powers[power_index].range,src_stats->pos,target);

	if (!collider->is_empty(target.x, target.y) || collider->is_wall(target.x,target.y)) {
		if (powers[power_index].buff_teleport) {
			return false;
		}
	}

	return true;
}

Point PowerManager::targetNeighbor(Point target, int range) {
	return targetNeighbor(target,range,false);
}

/**
 * Try to retarget the power to one of the 8 adjacent tiles
 * Returns the retargeted position on success, returns the original position on failure
 */
Point PowerManager::targetNeighbor(Point target, int range, bool ignore_blocked) {
	Point new_target = target;
	std::vector<Point> valid_tiles;

	for (int i=-range; i<=range; i++) {
		for (int j=-range; j<=range; j++) {
			if (i == 0 && j == 0) continue; // skip the middle tile
			new_target.x = target.x+UNITS_PER_TILE*i;
			new_target.y = target.y+UNITS_PER_TILE*j;
			if (collider->valid_position(new_target.x,new_target.y,MOVEMENT_NORMAL) || ignore_blocked)
				valid_tiles.push_back(new_target);
		}
	}

	if (!valid_tiles.empty())
		return valid_tiles[rand() % valid_tiles.size()];
	else
		return target;
}

/**
 * Apply basic power info to a new hazard.
 *
 * This can be called several times to combine powers.
 * Typically done when a base power can be modified by equipment
 * (e.g. ammo type affects the traits of powers that shoot)
 *
 * @param power_index The activated power ID
 * @param src_stats The StatBlock of the power activator
 * @param target Aim position in map coordinates
 * @param haz A newly-initialized hazard
 */
void PowerManager::initHazard(int power_index, StatBlock *src_stats, Point target, Hazard *haz) {

	//the hazard holds the statblock of its source
	haz->src_stats = src_stats;

	haz->power_index = power_index;

	if (powers[power_index].source_type == -1){
		if (src_stats->hero) haz->source_type = SOURCE_TYPE_HERO;
		else haz->source_type = SOURCE_TYPE_ENEMY;
	}
	else {
		haz->source_type = powers[power_index].source_type;
	}

	// Hazard attributes based on power source
	haz->crit_chance = src_stats->crit;
	haz->accuracy = src_stats->accuracy;

	// If the hazard's damage isn't default (0), we are applying an item-based power mod.
	// We don't allow equipment power mods to alter damage (mainly to preserve the base power's multiplier).
	if (haz->dmg_max == 0) {

		// base damage is by equipped item
		if (powers[power_index].base_damage == BASE_DAMAGE_MELEE) {
			haz->dmg_min = src_stats->dmg_melee_min;
			haz->dmg_max = src_stats->dmg_melee_max;
		}
		else if (powers[power_index].base_damage == BASE_DAMAGE_RANGED) {
			haz->dmg_min = src_stats->dmg_ranged_min;
			haz->dmg_max = src_stats->dmg_ranged_max;
		}
		else if (powers[power_index].base_damage == BASE_DAMAGE_MENT) {
			haz->dmg_min = src_stats->dmg_ment_min;
			haz->dmg_max = src_stats->dmg_ment_max;
		}

		//apply stat bonuses
		if (powers[power_index].base_damage == BASE_DAMAGE_MELEE) {
			haz->dmg_min += src_stats->get_physical() * src_stats->bonus_per_physical;
			haz->dmg_max += src_stats->get_physical() * src_stats->bonus_per_physical;
		}
		else if (powers[power_index].base_damage == BASE_DAMAGE_RANGED) {
			haz->dmg_min += src_stats->get_offense() * src_stats->bonus_per_offense;
			haz->dmg_max += src_stats->get_offense() * src_stats->bonus_per_offense;
		}
		else if (powers[power_index].base_damage == BASE_DAMAGE_MENT) {
			haz->dmg_min += src_stats->get_mental() * src_stats->bonus_per_mental;
			haz->dmg_max += src_stats->get_mental() * src_stats->bonus_per_mental;
		}

		// some powers have a damage multiplier, default 100 (percent)
		haz->dmg_min = (int)ceil((haz->dmg_min * powers[power_index].damage_multiplier) / 100.0);
		haz->dmg_max = (int)ceil((haz->dmg_max * powers[power_index].damage_multiplier) / 100.0);
	}

	// Only apply stats from powers that are not defaults
	// If we do this, we can init with multiple power layers
	// (e.g. base spell plus weapon type)

	if (powers[power_index].animationSet != NULL) {
		delete haz->activeAnimation;
		haz->activeAnimation = powers[power_index].animationSet->getAnimation(powers[power_index].animationSet->starting_animation);
	}
	if (powers[power_index].lifespan != 0)
		haz->lifespan = powers[power_index].lifespan;
	if (powers[power_index].directional)
		haz->animationKind = calcDirection(src_stats->pos.x, src_stats->pos.y, target.x, target.y);
	else if (powers[power_index].visual_random)
		haz->animationKind = rand() % powers[power_index].visual_random;
	else if (powers[power_index].visual_option)
		haz->animationKind = powers[power_index].visual_option;

	haz->floor = powers[power_index].floor;
	haz->base_speed = powers[power_index].speed;
	haz->complete_animation = powers[power_index].complete_animation;

	// combat traits
	if (powers[power_index].no_attack) {
		haz->active = false;
	}
	if (powers[power_index].multitarget) {
		haz->multitarget = true;
	}
	if (powers[power_index].radius != 0) {
		haz->radius = powers[power_index].radius;
	}
	if (powers[power_index].trait_armor_penetration) {
		haz->trait_armor_penetration = true;
	}
	haz->trait_crits_impaired = powers[power_index].trait_crits_impaired;
	if (powers[power_index].trait_elemental != -1) {
		haz->trait_elemental = powers[power_index].trait_elemental;
	}

	// status effect durations
	// durations stack when combining powers (e.g. base power and weapon/ammo type)
	haz->bleed_duration += powers[power_index].bleed_duration;
	haz->stun_duration += powers[power_index].stun_duration;
	haz->slow_duration += powers[power_index].slow_duration;
	haz->immobilize_duration += powers[power_index].immobilize_duration;
	// forced move
	haz->forced_move_speed += powers[power_index].forced_move_speed;
	haz->forced_move_duration += powers[power_index].forced_move_duration;
	// steal effects
	haz->hp_steal += powers[power_index].hp_steal;
	haz->mp_steal += powers[power_index].mp_steal;

	// hazard starting position
	if (powers[power_index].starting_pos == STARTING_POS_SOURCE) {
		haz->pos.x = (float)src_stats->pos.x;
		haz->pos.y = (float)src_stats->pos.y;
	}
	else if (powers[power_index].starting_pos == STARTING_POS_TARGET) {
		target = limitRange(powers[power_index].range,src_stats->pos,target);
		haz->pos.x = (float)target.x;
		haz->pos.y = (float)target.y;
	}
	else if (powers[power_index].starting_pos == STARTING_POS_MELEE) {
		haz->pos = calcVector(src_stats->pos, src_stats->direction, src_stats->melee_range);
	}
	if (powers[power_index].target_neighbor > 0) {
		Point new_target = targetNeighbor(src_stats->pos,powers[power_index].target_neighbor,true);
		haz->pos.x = (float)new_target.x;
		haz->pos.y = (float)new_target.y;
	}

	// pre/post power effects
	if (powers[power_index].post_power != 0) {
		haz->post_power = powers[power_index].post_power;
	}
	if (powers[power_index].wall_power != 0) {
		haz->wall_power = powers[power_index].wall_power;
	}

	// if equipment has special powers, apply it here (if it hasn't already been applied)
	if (!haz->equipment_modified && powers[power_index].allow_power_mod) {
		if (powers[power_index].base_damage == BASE_DAMAGE_MELEE && src_stats->melee_weapon_power != 0) {
			haz->equipment_modified = true;
			initHazard(src_stats->melee_weapon_power, src_stats, target, haz);
		}
		else if (powers[power_index].base_damage == BASE_DAMAGE_MENT && src_stats->mental_weapon_power != 0) {
			haz->equipment_modified = true;
			initHazard(src_stats->mental_weapon_power, src_stats, target, haz);
		}
		else if (powers[power_index].base_damage == BASE_DAMAGE_RANGED && src_stats->ranged_weapon_power != 0) {
			haz->equipment_modified = true;
			initHazard(src_stats->ranged_weapon_power, src_stats, target, haz);
		}
	}
}

/**
 * Any attack-based effects are handled by hazards.
 * Self-enhancements (buffs) are handled by this function.
 */
void PowerManager::buff(int power_index, StatBlock *src_stats, Point target) {

	// heal for ment weapon damage * damage multiplier
	if (powers[power_index].buff_heal) {
		int heal_amt = 0;
		int heal_max = (int)ceil(src_stats->dmg_ment_max * powers[power_index].damage_multiplier / 100.0) + (src_stats->get_mental()*src_stats->bonus_per_mental);
		int heal_min = (int)ceil(src_stats->dmg_ment_min * powers[power_index].damage_multiplier / 100.0) + (src_stats->get_mental()*src_stats->bonus_per_mental);
		if (heal_max > heal_min)
			heal_amt = rand() % (heal_max - heal_min) + heal_min;
		else // avoid div by 0
			heal_amt = heal_min;
		if (src_stats->hero)
			CombatText::Instance()->addMessage(msg->get("+%d HP",heal_amt), src_stats->pos, COMBAT_MESSAGE_BUFF, true);
		else
			CombatText::Instance()->addMessage(msg->get("+%d HP",heal_amt), src_stats->pos, COMBAT_MESSAGE_BUFF, false);
		src_stats->hp += heal_amt;
		if (src_stats->hp > src_stats->maxhp) src_stats->hp = src_stats->maxhp;
	}

	// hp restore
	if (powers[power_index].buff_restore_hp > 0) {
		int hp_amt = powers[power_index].buff_restore_hp;
		if (src_stats->hero)
			CombatText::Instance()->addMessage(msg->get("+%d HP",hp_amt), src_stats->pos, COMBAT_MESSAGE_BUFF, true);
		else
			CombatText::Instance()->addMessage(msg->get("+%d HP",hp_amt), src_stats->pos, COMBAT_MESSAGE_BUFF, false);
		src_stats->hp += hp_amt;
		if (src_stats->hp > src_stats->maxhp) src_stats->hp = src_stats->maxhp;
	}

	// mp restore
	if (powers[power_index].buff_restore_mp > 0) {
		int mp_amt = powers[power_index].buff_restore_mp;
		if (src_stats->hero)
			CombatText::Instance()->addMessage(msg->get("+%d MP",mp_amt), src_stats->pos, COMBAT_MESSAGE_BUFF, true);
		else
			CombatText::Instance()->addMessage(msg->get("+%d MP",mp_amt), src_stats->pos, COMBAT_MESSAGE_BUFF, false);
		src_stats->mp += mp_amt;
		if (src_stats->mp > src_stats->maxmp) src_stats->mp = src_stats->maxmp;
	}

	// charge shield to max ment weapon damage * damage multiplier
	if (powers[power_index].buff_shield) {
		int shield_amt = (int)ceil(src_stats->dmg_ment_max * powers[power_index].damage_multiplier / 100.0) + (src_stats->get_mental()*src_stats->bonus_per_mental);
		if (src_stats->hero)
			CombatText::Instance()->addMessage(msg->get("+%d Shield",shield_amt), src_stats->pos, COMBAT_MESSAGE_BUFF, true);
		else
			CombatText::Instance()->addMessage(msg->get("+%d Shield",shield_amt), src_stats->pos, COMBAT_MESSAGE_BUFF, false);
		src_stats->shield_hp = src_stats->shield_hp_total = shield_amt;
		src_stats->addEffect("shield",getEffectIcon("shield"));
	}

	// teleport to the target location
	if (powers[power_index].buff_teleport) {
		target = limitRange(powers[power_index].range,src_stats->pos,target);
		if (powers[power_index].target_neighbor > 0) {
			Point new_target = targetNeighbor(target,powers[power_index].target_neighbor);
			if (new_target.x == target.x && new_target.y == target.y) {
				src_stats->teleportation = false;
			} else {
				src_stats->teleportation = true;
				src_stats->teleport_destination.x = new_target.x;
				src_stats->teleport_destination.y = new_target.y;
			}
		} else {
			src_stats->teleportation = true;
			src_stats->teleport_destination.x = target.x;
			src_stats->teleport_destination.y = target.y;
		}
	}

	// buff_immunity removes all existing debuffs
	if (powers[power_index].buff_immunity) {
		src_stats->slow_duration = 0;
		src_stats->immobilize_duration = 0;
		src_stats->stun_duration = 0;
		src_stats->bleed_duration = 0;
	}

	// immunity_duration makes one immune to new debuffs
	if (src_stats->immunity_duration < powers[power_index].immunity_duration) {
		src_stats->addEffect("immunity",getEffectIcon("immunity"));
		src_stats->immunity_duration = src_stats->immunity_duration_total = powers[power_index].immunity_duration;
	}

	// transform_duration causes hero to be transformed
	if (src_stats->transform_duration < powers[power_index].transform_duration &&
		src_stats->transform_duration !=-1) {
		src_stats->addEffect("transform",getEffectIcon("transform"));
		src_stats->transform_duration = src_stats->transform_duration_total = powers[power_index].transform_duration;
	}

	// haste doubles run speed and removes power cooldowns
	if (src_stats->haste_duration < powers[power_index].haste_duration) {
		src_stats->addEffect("haste",getEffectIcon("haste"));
		src_stats->haste_duration = src_stats->haste_duration_total = powers[power_index].haste_duration;
	}

	// hot is healing over time
	if (src_stats->hot_duration < powers[power_index].hot_duration) {
		src_stats->addEffect("hot",getEffectIcon("hot"));
		src_stats->hot_duration = src_stats->hot_duration_total = powers[power_index].hot_duration;
		src_stats->hot_value = powers[power_index].hot_value;
	}
}

/**
 * Play the sound effect for this power
 * Equipped items may have unique sounds
 */
void PowerManager::playSound(int power_index, StatBlock *src_stats) {
	bool play_base_sound = false;

	if (powers[power_index].allow_power_mod) {
		if (powers[power_index].base_damage == BASE_DAMAGE_MELEE && src_stats->melee_weapon_power != 0
				&& powers[src_stats->melee_weapon_power].sfx_index != -1) {
			if (sfx[powers[src_stats->melee_weapon_power].sfx_index])
				Mix_PlayChannel(-1,sfx[powers[src_stats->melee_weapon_power].sfx_index],0);
		}
		else if (powers[power_index].base_damage == BASE_DAMAGE_MENT && src_stats->mental_weapon_power != 0
				&& powers[src_stats->mental_weapon_power].sfx_index != -1) {
			if (sfx[powers[src_stats->mental_weapon_power].sfx_index])
				Mix_PlayChannel(-1,sfx[powers[src_stats->mental_weapon_power].sfx_index],0);
		}
		else if (powers[power_index].base_damage == BASE_DAMAGE_RANGED && src_stats->ranged_weapon_power != 0
				&& powers[src_stats->ranged_weapon_power].sfx_index != -1) {
			if (sfx[powers[src_stats->ranged_weapon_power].sfx_index])
				Mix_PlayChannel(-1,sfx[powers[src_stats->ranged_weapon_power].sfx_index],0);
		}
		else play_base_sound = true;
	}
	else play_base_sound = true;

	if (play_base_sound && powers[power_index].sfx_index != -1) {
		if (sfx[powers[power_index].sfx_index])
			Mix_PlayChannel(-1,sfx[powers[power_index].sfx_index],0);
	}
}


/**
 * The activated power creates a static effect (not a moving hazard)
 *
 * @param power_index The activated power ID
 * @param src_stats The StatBlock of the power activator
 * @param target The mouse cursor position in map coordinates
 * return boolean true if successful
 */
bool PowerManager::effect(int power_index, StatBlock *src_stats, Point target) {

	if (powers[power_index].use_hazard) {
		int delay_iterator = 0;
		for (int i=0; i < powers[power_index].count; i++) {
			Hazard *haz = new Hazard();
			initHazard(power_index, src_stats, target, haz);

			// add optional delay
			haz->delay_frames = delay_iterator;
			delay_iterator += powers[power_index].delay;

			// Hazard memory is now the responsibility of HazardManager
			hazards.push(haz);
		}
	}

	buff(power_index, src_stats, target);

	// If there's a sound effect, play it here
	playSound(power_index, src_stats);

	payPowerCost(power_index, src_stats);
	return true;
}

/**
 * The activated power creates a group of missile hazards (e.g. arrow, thrown knife, firebolt).
 * Each individual missile is a single animated hazard that travels from the caster position to the
 * mouse target position.
 *
 * @param power_index The activated power ID
 * @param src_stats The StatBlock of the power activator
 * @param target The mouse cursor position in map coordinates
 * return boolean true if successful
 */
bool PowerManager::missile(int power_index, StatBlock *src_stats, Point target) {
	float pi = 3.1415926535898;

	Point src;
	if (powers[power_index].starting_pos == STARTING_POS_TARGET) {
		src.x = target.x;
		src.y = target.y;
	}
	else {
		src.x = src_stats->pos.x;
		src.y = src_stats->pos.y;
	}

	Hazard *haz;

	// calculate polar coordinates angle
	float theta = calcTheta(src.x, src.y, target.x, target.y);

	int delay_iterator = 0;

	//generate hazards
	for (int i=0; i < powers[power_index].count; i++) {
		haz = new Hazard();

		//calculate individual missile angle
		float offset_angle = ((1.0 - powers[power_index].count)/2 + i) * (powers[power_index].missile_angle * pi / 180.0);
		float variance = 0;
		if (powers[power_index].angle_variance != 0)
			variance = pow(-1.0f, (rand() % 2) - 1) * (rand() % powers[power_index].angle_variance) * pi / 180.0; //random between 0 and angle_variance away
		float alpha = theta + offset_angle + variance;
		while (alpha >= pi+pi) alpha -= pi+pi;
		while (alpha < 0.0) alpha += pi+pi;

		initHazard(power_index, src_stats, target, haz);

		//calculate the missile velocity
		int speed_var = 0;
		if (powers[power_index].speed_variance != 0)
			speed_var = (int)(pow(-1.0f, (rand() % 2) - 1) * (rand() % powers[power_index].speed_variance + 1) - 1);
		haz->speed.x = (haz->base_speed + speed_var) * cos(alpha);
		haz->speed.y = (haz->base_speed + speed_var) * sin(alpha);

		//calculate direction based on trajectory, not actual target (UNITS_PER_TILE reduces round off error)
		if (powers[power_index].directional)
			haz->animationKind = calcDirection(
					src.x, src.y,
					static_cast<int>(src.x + UNITS_PER_TILE * haz->speed.x),
					static_cast<int>(src.y + UNITS_PER_TILE * haz->speed.y));

		// add optional delay
		haz->delay_frames = delay_iterator;
		delay_iterator += powers[power_index].delay;

		hazards.push(haz);
	}

	payPowerCost(power_index, src_stats);

	playSound(power_index, src_stats);
	return true;
}

/**
 * Repeaters are multiple hazards that spawn in a straight line
 */
bool PowerManager::repeater(int power_index, StatBlock *src_stats, Point target) {

	payPowerCost(power_index, src_stats);

	//initialize variables
	Hazard *haz;
	FPoint location_iterator;
	FPoint speed;
	int delay_iterator = 0;
	int map_speed = 64;

	// calculate polar coordinates angle
	float theta = calcTheta(src_stats->pos.x, src_stats->pos.y, target.x, target.y);

	speed.x = (float)map_speed * cos(theta);
	speed.y = (float)map_speed * sin(theta);

	location_iterator.x = (float)src_stats->pos.x;
	location_iterator.y = (float)src_stats->pos.y;

	playSound(power_index, src_stats);

	for (int i=0; i<powers[power_index].count; i++) {

		location_iterator.x += speed.x;
		location_iterator.y += speed.y;

		// only travels until it hits a wall
		if (collider->is_wall((int)location_iterator.x, (int)location_iterator.y)) {
			break; // no more hazards
		}

		haz = new Hazard();
		initHazard(power_index, src_stats, target, haz);

		haz->pos.x = location_iterator.x;
		haz->pos.y = location_iterator.y;
		haz->delay_frames = delay_iterator;
		delay_iterator += powers[power_index].delay;

		hazards.push(haz);
	}

	return true;

}


/**
 * Spawn a creature. Does not create a hazard
 */
bool PowerManager::spawn(int power_index, StatBlock *src_stats, Point target) {

	// apply any buffs
	buff(power_index, src_stats, target);

	// If there's a sound effect, play it here
	playSound(power_index, src_stats);

	Map_Enemy espawn;
	espawn.type = powers[power_index].spawn_type;

	// enemy spawning position
	if (powers[power_index].starting_pos == STARTING_POS_SOURCE) {
		espawn.pos.x = src_stats->pos.x;
		espawn.pos.y = src_stats->pos.y;
	}
	else if (powers[power_index].starting_pos == STARTING_POS_TARGET) {
		espawn.pos.x = target.x;
		espawn.pos.y = target.y;
	}
	else if (powers[power_index].starting_pos == STARTING_POS_MELEE) {
		FPoint fpos = calcVector(src_stats->pos, src_stats->direction, src_stats->melee_range);
		espawn.pos.x = static_cast<int>(fpos.x);
		espawn.pos.y = static_cast<int>(fpos.y);
	}
	if (powers[power_index].target_neighbor > 0) {
		espawn.pos = targetNeighbor(src_stats->pos,powers[power_index].target_neighbor);
	}

	espawn.direction = calcDirection(src_stats->pos.x, src_stats->pos.y, target.x, target.y);
	for (int i=0; i < powers[power_index].count; i++) {
		enemies.push(espawn);
	}
	payPowerCost(power_index, src_stats);

	return true;
}

/**
 * A simpler spawn routine for map events
 */
bool PowerManager::spawn(const std::string& enemy_type, Point target) {

	Map_Enemy espawn;

	espawn.type = enemy_type;
	espawn.pos = target;

	// quick spawns start facing a random direction
	espawn.direction = rand() % 8;

	enemies.push(espawn);
	return true;
}

/**
 * Transform into a creature. Fully replaces entity characteristics
 */
bool PowerManager::transform(int power_index, StatBlock *src_stats, Point target) {

	if (src_stats->transformed && powers[power_index].spawn_type != "untransform") {
		log_msg = msg->get("You are already transformed, untransform first.");
		return false;
	}

	// apply any buffs
	buff(power_index, src_stats, target);

	src_stats->manual_untransform = powers[power_index].manual_untransform;

	// If there's a sound effect, play it here
	playSound(power_index, src_stats);

	// execute untransform powers
	if (powers[power_index].spawn_type == "untransform" && src_stats->transformed) {
		src_stats->transform_duration = 0;
		src_stats->transform_type = "untransform"; // untransform() is called only if type !=""
	}
	else {
		// permanent transformation
		if (powers[power_index].transform_duration == 0) src_stats->transform_duration = -1;

		src_stats->transform_type = powers[power_index].spawn_type;
	}

	payPowerCost(power_index, src_stats);

	return true;
}


/**
 * Activate is basically a switch/redirect to the appropriate function
 */
bool PowerManager::activate(int power_index, StatBlock *src_stats, Point target) {

	if (src_stats->hero) {
		if (powers[power_index].requires_mp > src_stats->mp)
			return false;
	}

	// logic for different types of powers are very different.  We allow these
	// separate functions to handle the details.
	switch(powers[power_index].type) {
		case POWTYPE_EFFECT:    return effect(power_index, src_stats, target);
		case POWTYPE_MISSILE:   return missile(power_index, src_stats, target);
		case POWTYPE_REPEATER:  return repeater(power_index, src_stats, target);
		case POWTYPE_SPAWN:     return spawn(power_index, src_stats, target);
		case POWTYPE_TRANSFORM: return transform(power_index, src_stats, target);
	}

	return false;
}

/**
 * pay costs, i.e. remove mana or items.
 */
void PowerManager::payPowerCost(int power_index, StatBlock *src_stats) {
	if (src_stats && src_stats->hero) {
		if (powers[power_index].requires_mp > 0) src_stats->mp -= powers[power_index].requires_mp;
		if (powers[power_index].requires_item != -1) used_item = powers[power_index].requires_item;
	}
}

/**
 * Render various status effects (buffs/debuffs)
 *
 * @param src_stats The StatBlock of the power activator
 */
Renderable PowerManager::renderEffects(StatBlock *src_stats) {
	Renderable r;
	r.map_pos.x = src_stats->pos.x;
	r.map_pos.y = src_stats->pos.y;
	r.sprite = NULL;

	for (unsigned int j=0; j<src_stats->effects.size(); j++) {
		for (unsigned int i=0; i<effects.size(); i++) {
			if (src_stats->effects[j].type == effects[i].type && effects[i].gfx != NULL) {

				// TODO: frame reset belogs in the logic phase, e.g. StatBlock::logic
				if (src_stats->effects[j].frame >= effects[i].frame_total)
					src_stats->effects[j].frame = 0;

				r.src.x = (src_stats->effects[j].frame / effects[i].ticks_per_frame) * effects[i].frame_size.w;

				r.src.y = effects[i].frame_size.y;
				r.src.w = effects[i].frame_size.w;
				r.src.h = effects[i].frame_size.h;
				r.offset.x = effects[i].frame_offset.x;
				r.offset.y = effects[i].frame_offset.y;
				r.sprite = effects[i].gfx;
				return r;
			}
		}
	}
	return r;
}

int PowerManager::getEffectIcon(std::string type) {
	for (unsigned int i=0; i<effects.size(); i++) {
		if (effects[i].type == type)
			return effects[i].icon;
	}
	return -1;
}

PowerManager::~PowerManager() {

	gfx_filenames.clear();

	for (unsigned i=0; i<sfx.size(); i++) {
		Mix_FreeChunk(sfx[i]);
	}
	sfx.clear();
	sfx_filenames.clear();

	for (unsigned i=0; i<effects.size(); i++) {
		SDL_FreeSurface(effects[i].gfx);
	}
}

