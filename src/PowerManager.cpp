/*
Copyright 2011 Clint Bellanger

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
#include "FileParser.h"
#include "SharedResources.h"
#include "UtilsFileSystem.h"

using namespace std;


/**
 * PowerManager constructor
 */
PowerManager::PowerManager() {

	gfx_count = 0;
	sfx_count = 0;
	for (int i=0; i<POWER_MAX_GFX; i++) {
		gfx[i] = NULL;
	}
	for (int i=0; i<POWER_MAX_SFX; i++) {
		sfx[i] = NULL;
	}
	
	powers[POWER_VENGEANCE].name = "Vengeance";
	powers[POWER_VENGEANCE].type = POWTYPE_SINGLE;
	powers[POWER_VENGEANCE].icon = 17;
	powers[POWER_VENGEANCE].description = "After blocking, unlease a deadly and accurate counter-attack";
	powers[POWER_VENGEANCE].new_state = POWSTATE_SWING;
	powers[POWER_VENGEANCE].face = true;
	powers[POWER_VENGEANCE].requires_mp = 1;

	used_item=-1;
	
	loadGraphics();
	loadAll();
}

/**
 * Load all powers files in all mods
 */
void PowerManager::loadAll() {

	string test_path;

	// load each items.txt file. Individual item IDs can be overwritten with mods.
	for (unsigned int i = 0; i < mods->mod_list.size(); i++) {

		test_path = PATH_DATA + "mods/" + mods->mod_list[i] + "/powers/powers.txt";

		if (fileExists(test_path)) {
			this->loadPowers(test_path);
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
	int input_id = 0;
	
	if (infile.open(filename.c_str())) {
		while (infile.next()) {
			// id needs to be the first component of each power.  That is how we write
			// data to the correct power.
			if (infile.key == "id") {
				input_id = atoi(infile.val.c_str());
			}
			else if (infile.key == "type") {
				if (infile.val == "single") powers[input_id].type = POWTYPE_SINGLE;
				else if (infile.val == "effect") powers[input_id].type = POWTYPE_EFFECT;
				else if (infile.val == "missile") powers[input_id].type = POWTYPE_MISSILE;
				else if (infile.val == "repeater") powers[input_id].type = POWTYPE_REPEATER;
			}
			else if (infile.key == "name") {
				powers[input_id].name = msg->get(infile.val);
			}
			else if (infile.key == "description") {
				powers[input_id].description = msg->get(infile.val);
			}
			else if (infile.key == "icon") {
				powers[input_id].icon = atoi(infile.val.c_str());
			}
			else if (infile.key == "new_state") {
				if (infile.val == "swing") powers[input_id].new_state = POWSTATE_SWING;
				else if (infile.val == "shoot") powers[input_id].new_state = POWSTATE_SHOOT;
				else if (infile.val == "cast") powers[input_id].new_state = POWSTATE_CAST;
				else if (infile.val == "block") powers[input_id].new_state = POWSTATE_BLOCK;
			}
			else if (infile.key == "face") {
				if (infile.val == "true") powers[input_id].face = true;
			}
			else if (infile.key == "source_type") {
				if (infile.val == "hero") powers[input_id].source_type = SOURCE_TYPE_HERO;
				else if (infile.val == "neutral") powers[input_id].source_type = SOURCE_TYPE_NEUTRAL;
				else if (infile.val == "enemy") powers[input_id].source_type = SOURCE_TYPE_ENEMY;
			}
			else if (infile.key == "beacon") {
				if (infile.val == "true") powers[input_id].beacon = true;
			}
			// power requirements
			else if (infile.key == "requires_physical_weapon") {
				if (infile.val == "true") powers[input_id].requires_physical_weapon = true;
			}
			else if (infile.key == "requires_mental_weapon") {
				if (infile.val == "true") powers[input_id].requires_mental_weapon = true;
			}
			else if (infile.key == "requires_offense_weapon") {
				if (infile.val == "true") powers[input_id].requires_offense_weapon = true;
			}
			else if (infile.key == "requires_mp") {
				powers[input_id].requires_mp = atoi(infile.val.c_str());
			}
			else if (infile.key == "requires_los") {
				if (infile.val == "true") powers[input_id].requires_los = true;
			}
			else if (infile.key == "requires_empty_target") {
				if (infile.val == "true") powers[input_id].requires_empty_target = true;
			}
			else if (infile.key == "requires_item") {
				powers[input_id].requires_item = atoi(infile.val.c_str());
			}
			else if (infile.key == "requires_targeting") {
				if (infile.val == "true") powers[input_id].requires_targeting = true;
			}
			else if (infile.key == "cooldown") {
				powers[input_id].cooldown = atoi(infile.val.c_str());
			}
			
			// animation info
			else if (infile.key == "gfx") {
				powers[input_id].gfx_index = loadGFX(infile.val);
			}
			else if (infile.key == "sfx") {
				powers[input_id].sfx_index = loadSFX(infile.val);
			}
			else if (infile.key == "rendered") {
				if (infile.val == "true") powers[input_id].rendered = true;
			}
			else if (infile.key == "directional") {
				if (infile.val == "true") powers[input_id].directional = true;
			}
			else if (infile.key == "visual_random") {
				powers[input_id].visual_random = atoi(infile.val.c_str());
			}
			else if (infile.key == "visual_option") {
				powers[input_id].visual_option = atoi(infile.val.c_str());
			}
			else if (infile.key == "aim_assist") {
				powers[input_id].aim_assist = atoi(infile.val.c_str());
			}
			else if (infile.key == "speed") {
				powers[input_id].speed = atoi(infile.val.c_str());
			}
			else if (infile.key == "lifespan") {
				powers[input_id].lifespan = atoi(infile.val.c_str());
			}
			else if (infile.key == "frame_loop") {
				powers[input_id].frame_loop = atoi(infile.val.c_str());
			}
			else if (infile.key == "frame_duration") {
				powers[input_id].frame_duration = atoi(infile.val.c_str());
			}
			else if (infile.key == "frame_size") {
				powers[input_id].frame_size.x = atoi(infile.nextValue().c_str());
				powers[input_id].frame_size.y = atoi(infile.nextValue().c_str());
			}
			else if (infile.key == "frame_offset") {
				powers[input_id].frame_offset.x = atoi(infile.nextValue().c_str());
				powers[input_id].frame_offset.y = atoi(infile.nextValue().c_str());
			}
			else if (infile.key == "floor") {
				if (infile.val == "true") powers[input_id].floor = true;
			}
			else if (infile.key == "active_frame") {
				powers[input_id].active_frame = atoi(infile.val.c_str());
			}
			else if (infile.key == "complete_animation") {
				if (infile.val == "true") powers[input_id].complete_animation = true;
			}
			
			// hazard traits
			else if (infile.key == "use_hazard") {
				if (infile.val == "true") powers[input_id].use_hazard = true;
			}
			else if (infile.key == "no_attack") {
				if (infile.val == "true") powers[input_id].no_attack = true;
			}
			else if (infile.key == "radius") {
				powers[input_id].radius = atoi(infile.val.c_str());
			}
			else if (infile.key == "base_damage") {
				if (infile.val == "none")
					powers[input_id].base_damage = BASE_DAMAGE_NONE;
				else if (infile.val == "melee")
					powers[input_id].base_damage = BASE_DAMAGE_MELEE;
				else if (infile.val == "ranged")
					powers[input_id].base_damage = BASE_DAMAGE_RANGED;
				else if (infile.val == "ment")
					powers[input_id].base_damage = BASE_DAMAGE_MENT;
			}
			else if (infile.key == "damage_multiplier") {
				powers[input_id].damage_multiplier = atoi(infile.val.c_str());
			}
			else if (infile.key == "starting_pos") {
				if (infile.val == "source")
					powers[input_id].starting_pos = STARTING_POS_SOURCE;
				else if (infile.val == "target")
					powers[input_id].starting_pos = STARTING_POS_TARGET;
				else if (infile.val == "melee")
					powers[input_id].starting_pos = STARTING_POS_MELEE;
			}
			else if (infile.key == "multitarget") {
				if (infile.val == "true") powers[input_id].multitarget = true;
			}
			else if (infile.key == "trait_armor_penetration") {
				if (infile.val == "true") powers[input_id].trait_armor_penetration = true;
			}
			else if (infile.key == "trait_crits_impaired") {
				powers[input_id].trait_crits_impaired = atoi(infile.val.c_str());
			}
			else if (infile.key == "trait_elemental") {
				if (infile.val == "wood") powers[input_id].trait_elemental = ELEMENT_WOOD;
				else if (infile.val == "metal") powers[input_id].trait_elemental = ELEMENT_METAL;
				else if (infile.val == "wind") powers[input_id].trait_elemental = ELEMENT_WIND;
				else if (infile.val == "water") powers[input_id].trait_elemental = ELEMENT_WATER;
				else if (infile.val == "earth") powers[input_id].trait_elemental = ELEMENT_EARTH;
				else if (infile.val == "fire") powers[input_id].trait_elemental = ELEMENT_FIRE;
				else if (infile.val == "shadow") powers[input_id].trait_elemental = ELEMENT_SHADOW;
				else if (infile.val == "light") powers[input_id].trait_elemental = ELEMENT_LIGHT;
			}
			//steal effects
			else if (infile.key == "hp_steal") {
				powers[input_id].hp_steal = atoi(infile.val.c_str());
			}
			else if (infile.key == "mp_steal") {
				powers[input_id].mp_steal = atoi(infile.val.c_str());
			}
			//missile modifiers
			else if (infile.key == "missile_num") {
				powers[input_id].missile_num = atoi(infile.val.c_str());
			}
			else if (infile.key == "missile_angle") {
				powers[input_id].missile_angle = atoi(infile.val.c_str());
			}
			else if (infile.key == "angle_variance") {
				powers[input_id].angle_variance = atoi(infile.val.c_str());
			}
			else if (infile.key == "speed_variance") {
				powers[input_id].speed_variance = atoi(infile.val.c_str());
			}
			//repeater modifiers
			else if (infile.key == "delay") {
				powers[input_id].delay = atoi(infile.val.c_str());
			}
			else if (infile.key == "start_frame") {
				powers[input_id].start_frame = atoi(infile.val.c_str());
			}
			else if (infile.key == "repeater_num") {
				powers[input_id].repeater_num = atoi(infile.val.c_str());
			}
			// buff/debuff durations
			else if (infile.key == "bleed_duration") {
				powers[input_id].bleed_duration = atoi(infile.val.c_str());
			}
			else if (infile.key == "stun_duration") {
				powers[input_id].stun_duration = atoi(infile.val.c_str());
			}
			else if (infile.key == "slow_duration") {
				powers[input_id].slow_duration = atoi(infile.val.c_str());
			}
			else if (infile.key == "immobilize_duration") {
				powers[input_id].immobilize_duration = atoi(infile.val.c_str());
			}
			else if (infile.key == "immunity_duration") {
				powers[input_id].immunity_duration = atoi(infile.val.c_str());
			}
			else if (infile.key == "haste_duration") {
				powers[input_id].haste_duration = atoi(infile.val.c_str());
			}
			else if (infile.key == "hot_duration") {
				powers[input_id].hot_duration = atoi(infile.val.c_str());
			}
			else if (infile.key == "hot_value") {
				powers[input_id].hot_value = atoi(infile.val.c_str());
			}
			
			// buffs
			else if (infile.key == "buff_heal") {
				if (infile.val == "true") powers[input_id].buff_heal = true;
			}
			else if (infile.key == "buff_shield") {
				if (infile.val == "true") powers[input_id].buff_shield = true;
			}
			else if (infile.key == "buff_teleport") {
				if (infile.val == "true") powers[input_id].buff_teleport = true;
			}
			else if (infile.key == "buff_immunity") {
				if (infile.val == "true") powers[input_id].buff_immunity = true;
			}
			else if (infile.key == "buff_restore_hp") {
				powers[input_id].buff_restore_hp = atoi(infile.val.c_str());
			}
			else if (infile.key == "buff_restore_mp") {
				powers[input_id].buff_restore_mp = atoi(infile.val.c_str());
			}
			
			// pre and post power effects
			else if (infile.key == "post_power") {
				powers[input_id].post_power = atoi(infile.val.c_str());
			}
			else if (infile.key == "wall_power") {
				powers[input_id].wall_power = atoi(infile.val.c_str());
			}
			else if (infile.key == "allow_power_mod") {
				if (infile.val == "true") powers[input_id].allow_power_mod = true;
			}
		}
		infile.close();
	}
}

/**
 * Load the specified graphic for this power
 *
 * @param filename The .png file containing sprites for this power, assumed to be in images/powers/
 * @return The gfx[] array index for this graphic, or -1 upon load failure
 */
int PowerManager::loadGFX(const string& filename) {
	
	// currently we restrict the total number of unique power sprite sets
	if (gfx_count == POWER_MAX_GFX) return -1;
	
	// first check to make sure the sprite isn't already loaded
	for (int i=0; i<gfx_count; i++) {
		if (gfx_filenames[i] == filename) {
			return i; // already have this one
		}
	}

	// we don't already have this sprite loaded, so load it
	gfx[gfx_count] = IMG_Load(mods->locate("images/powers/" + filename).c_str());
	if(!gfx[gfx_count]) {
		fprintf(stderr, "Couldn't load power sprites: %s\n", IMG_GetError());
		return -1;
	}
	
	// optimize
	SDL_Surface *cleanup = gfx[gfx_count];
	gfx[gfx_count] = SDL_DisplayFormatAlpha(gfx[gfx_count]);
	SDL_FreeSurface(cleanup);	

	// success; perform record-keeping
	gfx_filenames[gfx_count] = filename;
	gfx_count++;
	return gfx_count-1;
}

/**
 * Load the specified sound effect for this power
 *
 * @param filename The .ogg file containing the sound for this power, assumed to be in soundfx/powers/
 * @return The sfx[] array index for this mix chunk, or -1 upon load failure
 */
int PowerManager::loadSFX(const string& filename) {
	
	// currently we restrict the total number of unique power sounds
	if (sfx_count == POWER_MAX_SFX) return -1;
	
	// first check to make sure the sound isn't already loaded
	for (int i=0; i<sfx_count; i++) {
		if (sfx_filenames[i] == filename) {
			return i; // already have this one
		}
	}

	// we don't already have this sound loaded, so load it
	sfx[sfx_count] = Mix_LoadWAV(mods->locate("soundfx/powers/" + filename).c_str());
	if(!sfx[sfx_count]) {
		fprintf(stderr, "Couldn't load power soundfx: %s\n", filename.c_str());
		return -1;
	}
	
	// success; perform record-keeping
	sfx_filenames[sfx_count] = filename;
	sfx_count++;
	return sfx_count-1;
}


void PowerManager::loadGraphics() {

	runes = IMG_Load(mods->locate("images/powers/runes.png").c_str());
	
	if(!runes) {
		fprintf(stderr, "Couldn't load image: %s\n", IMG_GetError());
		SDL_Quit();
	}
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
	
	// Hazard damage depends on equipped weapons and the power's optional damage_multiplier
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
	//apply the multiplier
	haz->dmg_min = (int)ceil(haz->dmg_min * powers[power_index].damage_multiplier / 100.0);
	haz->dmg_max = (int)ceil(haz->dmg_max * powers[power_index].damage_multiplier / 100.0);
	
	// Only apply stats from powers that are not defaults
	// If we do this, we can init with multiple power layers
	// (e.g. base spell plus weapon type)
	
	if (powers[power_index].gfx_index != -1) {
		haz->sprites = gfx[powers[power_index].gfx_index];
	}
	if (powers[power_index].rendered) {
		haz->rendered = powers[power_index].rendered;
	}
	if (powers[power_index].lifespan != 0) {
		haz->lifespan = powers[power_index].lifespan;
	}
	if (powers[power_index].frame_loop != 1) {
		haz->frame_loop = powers[power_index].frame_loop;
	}
	if (powers[power_index].frame_duration != 1) {
		haz->frame_duration = powers[power_index].frame_duration;
	}
	if (powers[power_index].frame_size.x != 0) {
		haz->frame_size.x = powers[power_index].frame_size.x;
	}
	if (powers[power_index].frame_size.y != 0) {
		haz->frame_size.y = powers[power_index].frame_size.y;
	}
	if (powers[power_index].frame_offset.x != 0) {
		haz->frame_offset.x = powers[power_index].frame_offset.x;
	}
	if (powers[power_index].frame_offset.y != 0) {
		haz->frame_offset.y = powers[power_index].frame_offset.y;
	}
	if (powers[power_index].directional) {
		haz->direction = calcDirection(src_stats->pos.x, src_stats->pos.y, target.x, target.y);
	}
	else if (powers[power_index].visual_random != 0) {
		haz->visual_option = rand() % powers[power_index].visual_random;
	}
	else if (powers[power_index].visual_option != 0) {
		haz->visual_option = powers[power_index].visual_option;
	}
	haz->floor = powers[power_index].floor;
	if (powers[power_index].speed > 0) {
		haz->base_speed = powers[power_index].speed;
	}
	if (powers[power_index].complete_animation) {
		haz->complete_animation = true;
	}
	
	// combat traits
	if (powers[power_index].no_attack) {
		haz->active = false;
	}
	if (powers[power_index].multitarget) {
		haz->multitarget = true;
	}
	if (powers[power_index].active_frame != -1) {
		haz->active_frame = powers[power_index].active_frame;
	}
	if (powers[power_index].radius != 0) {
		haz->radius = powers[power_index].radius;
	}
	if (powers[power_index].trait_armor_penetration) {
		haz->trait_armor_penetration = true;
	}
	haz->trait_crits_impaired = powers[power_index].trait_crits_impaired;
	if (powers[power_index].trait_elemental) {
		haz->trait_elemental = powers[power_index].trait_elemental;
	}
	
	// status effect durations
	// durations stack when combining powers (e.g. base power and weapon/ammo type)
	haz->bleed_duration += powers[power_index].bleed_duration;
	haz->stun_duration += powers[power_index].stun_duration;
	haz->slow_duration += powers[power_index].slow_duration;
	haz->immobilize_duration += powers[power_index].immobilize_duration;
	// steal effects
	haz->hp_steal += powers[power_index].hp_steal;
	haz->mp_steal += powers[power_index].mp_steal;
	
	// hazard starting position
	if (powers[power_index].starting_pos == STARTING_POS_SOURCE) {
		haz->pos.x = (float)src_stats->pos.x;
		haz->pos.y = (float)src_stats->pos.y;
	}
	else if (powers[power_index].starting_pos == STARTING_POS_TARGET) {
		haz->pos.x = (float)target.x;
		haz->pos.y = (float)target.y;	
	}
	else if (powers[power_index].starting_pos == STARTING_POS_MELEE) {
		haz->pos = calcVector(src_stats->pos, src_stats->direction, src_stats->melee_range);
	}
	
	// pre/post power effects
	if (powers[power_index].post_power != -1) {
		haz->post_power = powers[power_index].post_power;
	}
	if (powers[power_index].wall_power != -1) {
		haz->wall_power = powers[power_index].wall_power;
	}
	
	// if equipment has special powers, apply it here (if it hasn't already been applied)
	if (!haz->equipment_modified && powers[power_index].allow_power_mod) {
		if (powers[power_index].base_damage == BASE_DAMAGE_MELEE && src_stats->melee_weapon_power != -1) {
			haz->equipment_modified = true;
			initHazard(src_stats->melee_weapon_power, src_stats, target, haz);
		}
		else if (powers[power_index].base_damage == BASE_DAMAGE_MENT && src_stats->mental_weapon_power != -1) {
			haz->equipment_modified = true;
			initHazard(src_stats->mental_weapon_power, src_stats, target, haz);
		}
		else if (powers[power_index].base_damage == BASE_DAMAGE_RANGED && src_stats->ranged_weapon_power != -1) {
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
		int heal_max = (int)ceil(src_stats->dmg_ment_max * powers[power_index].damage_multiplier / 100.0);
		int heal_min = (int)ceil(src_stats->dmg_ment_min * powers[power_index].damage_multiplier / 100.0);
		if (heal_max > heal_min)
			heal_amt = rand() % (heal_max - heal_min) + heal_min;
		else // avoid div by 0
			heal_amt = heal_min;
		src_stats->hp += heal_amt;
		if (src_stats->hp > src_stats->maxhp) src_stats->hp = src_stats->maxhp;
	}
	
	// hp restore
	if (powers[power_index].buff_restore_hp > 0) {
		src_stats->hp += powers[power_index].buff_restore_hp;
		if (src_stats->hp > src_stats->maxhp) src_stats->hp = src_stats->maxhp;
	}

	// mp restore
	if (powers[power_index].buff_restore_mp > 0) {
		src_stats->mp += powers[power_index].buff_restore_mp;
		if (src_stats->mp > src_stats->maxmp) src_stats->mp = src_stats->maxmp;
	}
	
	// charge shield to max ment weapon damage * damage multiplier
	if (powers[power_index].buff_shield) {
		src_stats->shield_hp = (int)ceil(src_stats->dmg_ment_max * powers[power_index].damage_multiplier / 100.0);	
	}
	
	// teleport to the target location
	if (powers[power_index].buff_teleport) {
		src_stats->teleportation = true;
		src_stats->teleport_destination.x = target.x;
		src_stats->teleport_destination.y = target.y;
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
		src_stats->immunity_duration = powers[power_index].immunity_duration;
	}
	
	// haste doubles run speed and removes power cooldowns
	if (src_stats->haste_duration < powers[power_index].haste_duration) {
		src_stats->haste_duration = powers[power_index].haste_duration;
	}
	
	// hot is healing over time
	if (src_stats->hot_duration < powers[power_index].hot_duration) {
		src_stats->hot_duration = powers[power_index].hot_duration;
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
		if (powers[power_index].base_damage == BASE_DAMAGE_MELEE && src_stats->melee_weapon_power != -1 
				&& powers[src_stats->melee_weapon_power].sfx_index != -1) {
			Mix_PlayChannel(-1,sfx[powers[src_stats->melee_weapon_power].sfx_index],0);
		}
		else if (powers[power_index].base_damage == BASE_DAMAGE_MENT && src_stats->mental_weapon_power != -1 
				&& powers[src_stats->mental_weapon_power].sfx_index != -1) {
			Mix_PlayChannel(-1,sfx[powers[src_stats->mental_weapon_power].sfx_index],0);
		}
		else if (powers[power_index].base_damage == BASE_DAMAGE_RANGED && src_stats->ranged_weapon_power != -1 
				&& powers[src_stats->ranged_weapon_power].sfx_index != -1) {
			Mix_PlayChannel(-1,sfx[powers[src_stats->ranged_weapon_power].sfx_index],0);
		}
		else play_base_sound = true;
	}
	else play_base_sound = true;

	if (play_base_sound && powers[power_index].sfx_index != -1) {
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
		Hazard *haz = new Hazard();
		initHazard(power_index, src_stats, target, haz);
		
		// Hazard memory is now the responsibility of HazardManager
		hazards.push(haz);
	}

	buff(power_index, src_stats, target);
	
	// If there's a sound effect, play it here
	playSound(power_index, src_stats);

	// if all else succeeded, pay costs
	if (src_stats->hero && powers[power_index].requires_mp > 0) src_stats->mp -= powers[power_index].requires_mp;
	if (src_stats->hero && powers[power_index].requires_item != -1) used_item = powers[power_index].requires_item;
	
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
	
	//generate hazards
	for (int i=0; i < powers[power_index].missile_num; i++) {
		haz = new Hazard();

		//calculate individual missile angle
		float offset_angle = ((1.0 - powers[power_index].missile_num)/2 + i) * (powers[power_index].missile_angle * pi / 180.0);
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
			haz->direction = calcDirection(src.x, src.y, src.x + UNITS_PER_TILE * haz->speed.x, src.y + UNITS_PER_TILE * haz->speed.y);
		
		hazards.push(haz);
	}

	// if all else succeeded, pay costs
	if (src_stats->hero && powers[power_index].requires_mp > 0) src_stats->mp -= powers[power_index].requires_mp;
	if (src_stats->hero && powers[power_index].requires_item != -1) used_item = powers[power_index].requires_item;

	playSound(power_index, src_stats);
	return true;
}

/**
 * Repeaters are multiple hazards that spawn in a straight line
 */
bool PowerManager::repeater(int power_index, StatBlock *src_stats, Point target) {

	
	// pay costs up front
	if (src_stats->hero && powers[power_index].requires_mp > 0) src_stats->mp -= powers[power_index].requires_mp;
	if (src_stats->hero && powers[power_index].requires_item != -1) used_item = powers[power_index].requires_item;
	
	//initialize variables
	Hazard *haz[10];
	FPoint location_iterator;
	FPoint speed;
	int delay_iterator;
	int map_speed = 64;

	// calculate polar coordinates angle
	float theta = calcTheta(src_stats->pos.x, src_stats->pos.y, target.x, target.y);

	speed.x = (float)map_speed * cos(theta);
	speed.y = (float)map_speed * sin(theta);

	location_iterator.x = (float)src_stats->pos.x;
	location_iterator.y = (float)src_stats->pos.y;
	delay_iterator = 0;

	playSound(power_index, src_stats);

	for (int i=0; i<powers[power_index].repeater_num; i++) {

		location_iterator.x += speed.x;
		location_iterator.y += speed.y;

		// only travels until it hits a wall
		if (collider->is_wall((int)location_iterator.x, (int)location_iterator.y)) {
			break; // no more hazards
		}
		
		haz[i] = new Hazard();
		initHazard(power_index, src_stats, target, haz[i]);

		haz[i]->pos.x = location_iterator.x;
		haz[i]->pos.y = location_iterator.y;
		haz[i]->delay_frames = delay_iterator;
		delay_iterator += powers[power_index].delay;
		
		haz[i]->frame = powers[power_index].start_frame; // start at bottom frame
		
		hazards.push(haz[i]);
	}

	return true;
	
}


/**
 * Basic single-frame area hazard
 */
bool PowerManager::single(int power_index, StatBlock *src_stats, Point target) {
	
	Hazard *haz = new Hazard();
	
	// common to all singles
	haz->pos.x = (float)target.x;
	haz->pos.y = (float)target.y;
	haz->lifespan = 1;
	haz->crit_chance = src_stats->crit;
	haz->accuracy = src_stats->accuracy;
	haz->src_stats = src_stats;

	// specific powers have different stats here
	if (power_index == POWER_VENGEANCE) {
		haz->pos = calcVector(src_stats->pos, src_stats->direction, src_stats->melee_range);
		haz->dmg_min = src_stats->dmg_melee_min;
		haz->dmg_max = src_stats->dmg_melee_max;
		haz->radius = 64;
		src_stats->mp--;
		
		// use vengeance stacks
		haz->accuracy += src_stats->vengeance_stacks * 25;
		haz->crit_chance += src_stats->vengeance_stacks * 25;
		src_stats->vengeance_stacks = 0;
	}
	
	hazards.push(haz);

	// Hazard memory is now the responsibility of HazardManager
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
	if (powers[power_index].type == POWTYPE_SINGLE)
		return single(power_index, src_stats, target);
	else if (powers[power_index].type == POWTYPE_MISSILE)
		return missile(power_index, src_stats, target);
	else if (powers[power_index].type == POWTYPE_REPEATER)
		return repeater(power_index, src_stats, target);
	else if (powers[power_index].type == POWTYPE_EFFECT)
		return effect(power_index, src_stats, target);
	
	return false;
}

PowerManager::~PowerManager() {

	for (int i=0; i<gfx_count; i++) {
		if (gfx[i] != NULL)
			SDL_FreeSurface(gfx[i]);
	}
	for (int i=0; i<sfx_count; i++) {
		if (sfx[i] != NULL)
			Mix_FreeChunk(sfx[i]);
	}

	SDL_FreeSurface(freeze);
	SDL_FreeSurface(runes);	
	Mix_FreeChunk(sfx_freeze);
	
}

