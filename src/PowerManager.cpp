/**
 * class PowerManager
 *
 * @author Clint Bellanger
 * @license GPL
 */


#include "PowerManager.h"

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
		
	powers[POWER_FREEZE].name = "Freeze";
	powers[POWER_FREEZE].type = POWTYPE_GROUNDRAY;
	powers[POWER_FREEZE].icon = 14;
	powers[POWER_FREEZE].description = "Create a ray of piercing cold that slows enemies";
	powers[POWER_FREEZE].new_state = POWSTATE_CAST;
	powers[POWER_FREEZE].face = true;
	powers[POWER_FREEZE].requires_mp = 1;
	
	powers[POWER_VENGEANCE].name = "Vengeance";
	powers[POWER_VENGEANCE].type = POWTYPE_SINGLE;
	powers[POWER_VENGEANCE].icon = 17;
	powers[POWER_VENGEANCE].description = "After blocking, unlease a deadly and accurate counter-attack";
	powers[POWER_VENGEANCE].new_state = POWSTATE_SWING;
	powers[POWER_VENGEANCE].face = true;
	powers[POWER_VENGEANCE].requires_mp = 1;
	
	used_item=-1;
	
	loadGraphics();
	loadSounds();
	loadPowers();
}

/**
 * All powers are defined in powers/powers.txt
 */
void PowerManager::loadPowers() {

	ifstream infile;
	string line;
	string key;
	string val;
	string starts_with;
	int input_id = 0;
	
	infile.open("powers/powers.txt", ios::in);

	if (infile.is_open()) {
		while (!infile.eof()) {
			line = getLine(infile);

			if (line.length() > 0) {
				starts_with = line.at(0);
				
				if (starts_with == "#") {
					// skip comments
				}
				if (starts_with == "[") {
					// not actually necessary.  We know we're at a new power when
					// we see a new id key.
				}
				else { // this is data.  treatment depends on key
					parse_key_pair(line, key, val);          
					key = trim(key, ' ');
					val = trim(val, ' ');
				
					// id needs to be the first component of each power.  That is how we write
					// data to the correct power.
					if (key == "id") {
						input_id = atoi(val.c_str());
					}
					else if (key == "type") {
						if (val == "single") powers[input_id].type = POWTYPE_SINGLE;
						else if (val == "effect") powers[input_id].type = POWTYPE_EFFECT;
						else if (val == "missile") powers[input_id].type = POWTYPE_MISSILE;
						else if (val == "groundray") powers[input_id].type = POWTYPE_GROUNDRAY;
					}
					else if (key == "name") {
						powers[input_id].name = val;
					}
					else if (key == "description") {
						powers[input_id].description = val;
					}
					else if (key == "icon") {
						powers[input_id].icon = atoi(val.c_str());
					}
					else if (key == "new_state") {
						if (val == "swing") powers[input_id].new_state = POWSTATE_SWING;
						else if (val == "shoot") powers[input_id].new_state = POWSTATE_SHOOT;
						else if (val == "cast") powers[input_id].new_state = POWSTATE_CAST;
						else if (val == "block") powers[input_id].new_state = POWSTATE_BLOCK;
					}
					else if (key == "face") {
						if (val == "true") powers[input_id].face = true;
					}
					
					// power requirements
					else if (key == "requires_physical_weapon") {
						if (val == "true") powers[input_id].requires_physical_weapon = true;
					}
					else if (key == "requires_mental_weapon") {
						if (val == "true") powers[input_id].requires_mental_weapon = true;
					}
					else if (key == "requires_offense_weapon") {
						if (val == "true") powers[input_id].requires_offense_weapon = true;
					}
					else if (key == "requires_mp") {
						powers[input_id].requires_mp = atoi(val.c_str());
					}
					else if (key == "requires_los") {
						if (val == "true") powers[input_id].requires_los = true;
					}
					else if (key == "requires_empty_target") {
						if (val == "true") powers[input_id].requires_empty_target = true;
					}
					else if (key == "requires_item") {
						powers[input_id].requires_item = atoi(val.c_str());
					}
					
					// animation info
					else if (key == "gfx") {
						powers[input_id].gfx_index = loadGFX(val);
					}
					else if (key == "sfx") {
						powers[input_id].sfx_index = loadSFX(val);
					}
					else if (key == "rendered") {
						if (val == "true") powers[input_id].rendered = true;				
					}
					else if (key == "directional") {
						if (val == "true") powers[input_id].directional = true;
					}
					else if (key == "visual_random") {
						powers[input_id].visual_random = atoi(val.c_str());
					}
					else if (key == "visual_option") {
						powers[input_id].visual_option = atoi(val.c_str());
					}
					else if (key == "aim_assist") {
						powers[input_id].aim_assist = atoi(val.c_str());
					}
					else if (key == "speed") {
						powers[input_id].speed = atoi(val.c_str());
					}
					else if (key == "lifespan") {
						powers[input_id].lifespan = atoi(val.c_str());
					}
					else if (key == "frame_loop") {
						powers[input_id].frame_loop = atoi(val.c_str());
					}
					else if (key == "frame_duration") {
						powers[input_id].frame_duration = atoi(val.c_str());
					}
					else if (key == "frame_size") {
						val = val + ",";
						powers[input_id].frame_size.x = eatFirstInt(val, ',');										
						powers[input_id].frame_size.y = eatFirstInt(val, ',');				
					}
					else if (key == "frame_offset") {
						val = val + ",";
						powers[input_id].frame_offset.x = eatFirstInt(val, ',');										
						powers[input_id].frame_offset.y = eatFirstInt(val, ',');				
					}
					else if (key == "floor") {
						if (val == "true") powers[input_id].floor = true;
					}
					else if (key == "active_frame") {
						powers[input_id].active_frame = atoi(val.c_str());
					}
					
					// hazard traits
					else if (key == "use_hazard") {
						if (val == "true") powers[input_id].use_hazard = true;
					}
					else if (key == "no_attack") {
						if (val == "true") powers[input_id].no_attack = true;
					}
					else if (key == "radius") {
						powers[input_id].radius = atoi(val.c_str());
					}
					else if (key == "base_damage") {
						if (val == "none")
							powers[input_id].base_damage = BASE_DAMAGE_NONE;
						else if (val == "melee")
							powers[input_id].base_damage = BASE_DAMAGE_MELEE;
						else if (val == "ranged")
							powers[input_id].base_damage = BASE_DAMAGE_RANGED;
						else if (val == "ment")
							powers[input_id].base_damage = BASE_DAMAGE_MENT;
					}
					else if (key == "damage_multiplier") {
						powers[input_id].damage_multiplier = atoi(val.c_str());
					}
					else if (key == "starting_pos") {
						if (val == "source")
							powers[input_id].starting_pos = STARTING_POS_SOURCE;
						else if (val == "target")
							powers[input_id].starting_pos = STARTING_POS_TARGET;
						else if (val == "melee")
							powers[input_id].starting_pos = STARTING_POS_MELEE;
					}
					else if (key == "multitarget") {
						if (val == "true") powers[input_id].multitarget = true;
					}
					else if (key == "trait_armor_penetration") {
						if (val == "true") powers[input_id].trait_armor_penetration = true;
					}
					else if (key == "trait_crits_impaired") {
						powers[input_id].trait_crits_impaired = atoi(val.c_str());
					}
					else if (key == "trait_elemental") {
						if (val == "wood") powers[input_id].trait_elemental = ELEMENT_WOOD;
						else if (val == "metal") powers[input_id].trait_elemental = ELEMENT_METAL;
						else if (val == "wind") powers[input_id].trait_elemental = ELEMENT_WIND;
						else if (val == "water") powers[input_id].trait_elemental = ELEMENT_WATER;
						else if (val == "earth") powers[input_id].trait_elemental = ELEMENT_EARTH;
						else if (val == "fire") powers[input_id].trait_elemental = ELEMENT_FIRE;
						else if (val == "shadow") powers[input_id].trait_elemental = ELEMENT_SHADOW;
						else if (val == "light") powers[input_id].trait_elemental = ELEMENT_LIGHT;
					}
					//missile modifiers
					else if (key == "missile_num") {
						powers[input_id].missile_num = atoi(val.c_str());
					}
					else if (key == "missile_angle") {
						powers[input_id].missile_angle = atoi(val.c_str());
					}
					else if (key == "angle_variance") {
						powers[input_id].angle_variance = atoi(val.c_str());
					}
					else if (key == "speed_variance") {
						powers[input_id].speed_variance = atoi(val.c_str());
					}
					// buff/debuff durations
					else if (key == "bleed_duration") {
						powers[input_id].bleed_duration = atoi(val.c_str());
					}
					else if (key == "stun_duration") {
						powers[input_id].stun_duration = atoi(val.c_str());
					}
					else if (key == "slow_duration") {
						powers[input_id].slow_duration = atoi(val.c_str());
					}
					else if (key == "immobilize_duration") {
						powers[input_id].immobilize_duration = atoi(val.c_str());
					}
					else if (key == "immunity_duration") {
						powers[input_id].immunity_duration = atoi(val.c_str());
					}
					else if (key == "haste_duration") {
						powers[input_id].haste_duration = atoi(val.c_str());
					}
					else if (key == "hot_duration") {
						powers[input_id].hot_duration = atoi(val.c_str());
					}
					else if (key == "hot_value") {
						powers[input_id].hot_value = atoi(val.c_str());
					}
					
					// buffs
					else if (key == "buff_heal") {
						if (val == "true") powers[input_id].buff_heal = true;
					}
					else if (key == "buff_shield") {
						if (val == "true") powers[input_id].buff_shield = true;
					}
					else if (key == "buff_teleport") {
						if (val == "true") powers[input_id].buff_teleport = true;
					}
					else if (key == "buff_immunity") {
						if (val == "true") powers[input_id].buff_immunity = true;
					}
					else if (key == "buff_restore_hp") {
						powers[input_id].buff_restore_hp = atoi(val.c_str());
					}
					else if (key == "buff_restore_mp") {
						powers[input_id].buff_restore_mp = atoi(val.c_str());
					}
					
					// pre and post power effects
					else if (key == "post_power") {
						powers[input_id].post_power = atoi(val.c_str());
					}
					else if (key == "wall_power") {
						powers[input_id].wall_power = atoi(val.c_str());
					}
					else if (key == "allow_power_mod") {
						if (val == "true") powers[input_id].allow_power_mod = true;
					}
				}
			}
		}
	}
	infile.close();
}

/**
 * Load the specified graphic for this power
 *
 * @param filename The .png file containing sprites for this power, assumed to be in images/powers/
 * @return The gfx[] array index for this graphic, or -1 upon load failure
 */
int PowerManager::loadGFX(string filename) {
	
	// currently we restrict the total number of unique power sprite sets
	if (gfx_count == POWER_MAX_GFX) return -1;
	
	// first check to make sure the sprite isn't already loaded
	for (int i=0; i<gfx_count; i++) {
		if (gfx_filenames[i] == filename) {
			return i; // already have this one
		}
	}

	// we don't already have this sprite loaded, so load it
	gfx[gfx_count] = IMG_Load(("images/powers/" + filename).c_str());
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
int PowerManager::loadSFX(string filename) {
	
	// currently we restrict the total number of unique power sounds
	if (sfx_count == POWER_MAX_SFX) return -1;
	
	// first check to make sure the sound isn't already loaded
	for (int i=0; i<sfx_count; i++) {
		if (sfx_filenames[i] == filename) {
			return i; // already have this one
		}
	}

	// we don't already have this sound loaded, so load it
	sfx[sfx_count] = Mix_LoadWAV(("soundfx/powers/" + filename).c_str());
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

	freeze = IMG_Load("images/powers/freeze.png");
	runes = IMG_Load("images/powers/runes.png");
	
	if(!freeze || !runes) {
		fprintf(stderr, "Couldn't load image: %s\n", IMG_GetError());
		SDL_Quit();
	}
}

void PowerManager::loadSounds() {
	sfx_freeze = Mix_LoadWAV("soundfx/powers/freeze.ogg");	
}

/**
 * Set new collision object
 */
void PowerManager::handleNewMap(MapCollision *_collider) {
	collider = _collider;
}

/**
 * Change direction to face the target map location
 */
int PowerManager::calcDirection(int origin_x, int origin_y, int target_x, int target_y) {

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

	// the source (hero/enemy/neutral) determines what is a valid target
	// i.e. no friendly fire between enemies
	if (src_stats->hero)
		haz->source = SRC_HERO;
	else
		haz->source = SRC_ENEMY;

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
	haz->dmg_min = ceil(haz->dmg_min * powers[power_index].damage_multiplier / 100.0);
	haz->dmg_max = ceil(haz->dmg_max * powers[power_index].damage_multiplier / 100.0);
	
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
		int heal_max = ceil(src_stats->dmg_ment_max * powers[power_index].damage_multiplier / 100.0);
		int heal_min = ceil(src_stats->dmg_ment_min * powers[power_index].damage_multiplier / 100.0);
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
		src_stats->shield_hp = ceil(src_stats->dmg_ment_max * powers[power_index].damage_multiplier / 100.0);	
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
	if (powers[power_index].requires_mp > 0) {
		src_stats->mp -= powers[power_index].requires_mp;
	}
	used_item = powers[power_index].requires_item;

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

	Hazard *haz[powers[power_index].missile_num];

	// calculate base angle
	float dx = (float)target.x - (float)src_stats->pos.x;
	float dy = (float)target.y - (float)src_stats->pos.y;
	float theta = atan(dy/dx);
	if (dx > 0) theta += pi; //theta corrector

	//generate hazards
	for (int i=0; i < powers[power_index].missile_num; i++) {
		haz[i] = new Hazard();
		Point rot_target;

		//calculate individual missile angle
		float offset_angle = ((1.0 - powers[power_index].missile_num)/2 + i) * (powers[power_index].missile_angle * pi / 180.0);
		float variance = 0;
		if (powers[power_index].angle_variance != 0)
			variance = pow(-1, (rand() % 2) - 1) * (rand() % powers[power_index].angle_variance) * pi / 180.0; //random between 0 and angle_variance away
		float alpha = theta + offset_angle + variance;
		while (alpha >= 2 * pi) alpha -= 2 * pi;
		while (alpha < 0) alpha += 2 * pi;

		//calculate animation direction (the UNITS_PER_TILE just reduces round-off error)
		rot_target.x = src_stats->pos.x - UNITS_PER_TILE * cos(alpha);
		rot_target.y = src_stats->pos.y - UNITS_PER_TILE * sin(alpha);

		initHazard(power_index, src_stats, rot_target, haz[i]);

		//calculate the missile velocity
		int speed_var = 0;
		if (powers[power_index].speed_variance != 0)
			speed_var = pow(-1, (rand() % 2) - 1) * (rand() % powers[power_index].speed_variance + 1) - 1;
		haz[i]->speed.x = (haz[0]->base_speed + speed_var) * -cos(alpha);
		haz[i]->speed.y = (haz[0]->base_speed + speed_var) * -sin(alpha);
		hazards.push(haz[i]);
	}

	// pay costs
	if (powers[power_index].requires_mp>0) {
		src_stats->mp-=powers[power_index].requires_mp;
	}
	used_item = powers[power_index].requires_item;

	playSound(power_index, src_stats);
	return true;
}


/**
 * Ground Rays are multiple hazards that spawn in a straight line
 */
bool PowerManager::groundRay(int power_index, StatBlock *src_stats, Point target) {

	// pay costs
	src_stats->mp--;
	used_item = powers[power_index].requires_item;

	Hazard *haz[10];
	FPoint location_iterator;
	FPoint speed;
	int delay_iterator;
	int map_speed = 64;

	// calculate speed
	float dx = (float)(target.x - src_stats->pos.x);
	float dy = (float)(target.y - src_stats->pos.y);
	float theta = atan(dy/dx);
	speed.x = (float)map_speed * cos(theta);
	speed.y = (float)map_speed * sin(theta);
	if (dx > 0.0 && speed.x < 0.0 || dx < 0.0 && speed.x > 0.0)
		speed.x *= -1.0;
	if (dy > 0.0 && speed.y < 0.0 || dy < 0.0 && speed.y > 0.0)
		speed.y *= -1.0;

	location_iterator.x = (float)src_stats->pos.x;
	location_iterator.y = (float)src_stats->pos.y;
	delay_iterator = 0;

	if (power_index == POWER_FREEZE) {
		Mix_PlayChannel(-1, sfx_freeze, 0);
	}

	for (int i=0; i<10; i++) {

		location_iterator.x += speed.x;
		location_iterator.y += speed.y;

		// only travels until it hits a wall
		if (collider->is_wall((int)location_iterator.x, (int)location_iterator.y)) {
			break; // no more hazards
		}

		haz[i] = new Hazard();	
		haz[i]->pos.x = location_iterator.x;
		haz[i]->pos.y = location_iterator.y;
		
		
		haz[i]->delay_frames = delay_iterator;
		delay_iterator += 3;
		
		haz[i]->crit_chance = src_stats->crit;
		haz[i]->accuracy = src_stats->accuracy;
		if (src_stats->hero)
			haz[i]->source = SRC_HERO;
		else
			haz[i]->source = SRC_ENEMY;
		haz[i]->rendered = true;
		haz[i]->frame_offset.y = 48;
		
		// specific powers have different stats here
		if (power_index == POWER_FREEZE) {
			haz[i]->lifespan = 20;
			haz[i]->active_frame = 0;
			haz[i]->frame = 16; // start at bottom frame
			haz[i]->frame_loop = 20;
			haz[i]->frame_duration = 3;
			haz[i]->radius = 64;
			haz[i]->dmg_min = src_stats->dmg_ment_min;
			haz[i]->dmg_max = src_stats->dmg_ment_max;
			haz[i]->sprites = freeze;
			haz[i]->direction = rand() % 3;
			haz[i]->complete_animation = true;
			haz[i]->slow_duration = 90;
			haz[i]->trait_elemental = ELEMENT_WATER;
		}
		
		hazards.push(haz[i]);
	}
	
	// Hazard memory is now the responsibility of HazardManager
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
	if (src_stats->hero)
		haz->source = SRC_HERO;
	else
		haz->source = SRC_ENEMY;

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

	// logic for different types of powers are very different.  We allow these
	// separate functions to handle the details.
	if (powers[power_index].type == POWTYPE_SINGLE)
		return single(power_index, src_stats, target);
	else if (powers[power_index].type == POWTYPE_MISSILE)
		return missile(power_index, src_stats, target);
	else if (powers[power_index].type == POWTYPE_GROUNDRAY)
		return groundRay(power_index, src_stats, target);
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

