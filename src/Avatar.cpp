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
 * class Avatar
 *
 * Contains logic and rendering routines for the player avatar.
 */

#include "Avatar.h"
#include "ModManager.h"

Avatar::Avatar(PowerManager *_powers, InputState *_inp, MapIso *_map) : Entity(_map), powers(_powers), inp(_inp) {
	
	init();
	
	// default hero animation data
	stats.cooldown = 4;

	// load the hero's animations from hero definition file
	loadAnimations("animations/hero.txt");
}

void Avatar::init() {

	// name, base, look are set by GameStateNew so don't reset it here

	// other init
	sprites = 0;
	stats.cur_state = AVATAR_STANCE;
	stats.pos.x = map->spawn.x;
	stats.pos.y = map->spawn.y;
	stats.direction = map->spawn_dir;
	current_power = -1;
	newLevelNotification = false;
		
	lockSwing = false;
	lockCast = false;
	lockShoot = false;
	
	stats.hero = true;
	stats.level = 1;
	stats.xp = 0;
	stats.physical_character = 1;
	stats.mental_character = 1;
	stats.offense_character = 1;
	stats.defense_character = 1;
	stats.physical_additional = 0;
	stats.mental_additional = 0;
	stats.offense_additional = 0;
	stats.defense_additional = 0;
	stats.speed = 14;
	stats.dspeed = 10;
	stats.recalc();
	
	log_msg = "";

	stats.cooldown_ticks = 0;
	
	haz = NULL;

	img_main = "";
	img_armor = "";
	img_off = "";

	for (int i = 0; i < POWER_COUNT; i++) {
		stats.hero_cooldown[i] = 0;
	}
}

void Avatar::loadGraphics(string _img_main, string _img_armor, string _img_off) {
	SDL_Surface *gfx_main = NULL;
	SDL_Surface *gfx_off = NULL;
	SDL_Surface *gfx_head = NULL;
	SDL_Rect src;
	SDL_Rect dest;
	
	// Default appearance
	if (_img_armor == "") _img_armor = "clothes";
	
	// Check if we really need to change the graphics
	if (_img_main != img_main || _img_armor != img_armor || _img_off != img_off) {
		img_main = _img_main;
		img_armor = _img_armor;
		img_off = _img_off;
	
		// composite the hero graphic
		if (sprites) SDL_FreeSurface(sprites);
		sprites = IMG_Load(mods->locate("images/avatar/" + stats.base + "/" + img_armor + ".png").c_str());
		if (img_main != "") gfx_main = IMG_Load(mods->locate("images/avatar/" + stats.base + "/" + img_main + ".png").c_str());
		if (img_off != "") gfx_off = IMG_Load(mods->locate("images/avatar/" + stats.base + "/" + img_off + ".png").c_str());
		gfx_head = IMG_Load(mods->locate("images/avatar/" + stats.base + "/" + stats.head + ".png").c_str());

		SDL_SetColorKey( sprites, SDL_SRCCOLORKEY, SDL_MapRGB(sprites->format, 255, 0, 255) ); 
		if (gfx_main) SDL_SetColorKey( gfx_main, SDL_SRCCOLORKEY, SDL_MapRGB(gfx_main->format, 255, 0, 255) ); 
		if (gfx_off) SDL_SetColorKey( gfx_off, SDL_SRCCOLORKEY, SDL_MapRGB(gfx_off->format, 255, 0, 255) ); 
		if (gfx_head) SDL_SetColorKey( gfx_head, SDL_SRCCOLORKEY, SDL_MapRGB(gfx_head->format, 255, 0, 255) ); 
		
		// assuming the hero is right-handed, we know the layer z-order
		// copy the furthest hand first
		src.w = dest.w = 4096;
		src.h = dest.h = 256;
		src.x = dest.x = 0;
		src.y = dest.y = 0;
		if (gfx_main) SDL_BlitSurface(gfx_main, &src, sprites, &dest); // row 0,1 main hand
		src.y = dest.y = 768;
		if (gfx_main) SDL_BlitSurface(gfx_main, &src, sprites, &dest); // row 6,7 main hand
		src.h = dest.h = 512;
		src.y = dest.y = 256;
		if (gfx_off) SDL_BlitSurface(gfx_off, &src, sprites, &dest); // row 2-5 off hand
		
		// copy the head in the middle
		src.h = dest.h = 1024;
		src.y = dest.y = 0;
		if (gfx_head) SDL_BlitSurface(gfx_head, &src, sprites, &dest); // head
		
		// copy the closest hand last
		src.w = dest.w = 4096;
		src.h = dest.h = 256;
		src.x = dest.x = 0;
		src.y = dest.y = 0;
		if (gfx_off) SDL_BlitSurface(gfx_off, &src, sprites, &dest); // row 0,1 off hand
		src.y = dest.y = 768;
		if (gfx_off) SDL_BlitSurface(gfx_off, &src, sprites, &dest); // row 6,7 off hand
		src.h = dest.h = 512;
		src.y = dest.y = 256;
		if (gfx_main) SDL_BlitSurface(gfx_main, &src, sprites, &dest); // row 2-5 main hand
		
		if (gfx_main) SDL_FreeSurface(gfx_main);
		if (gfx_off) SDL_FreeSurface(gfx_off);
		if (gfx_head) SDL_FreeSurface(gfx_head);
		
		// optimize
		SDL_Surface *cleanup = sprites;
		sprites = SDL_DisplayFormatAlpha(sprites);
		SDL_FreeSurface(cleanup);
	}
}

void Avatar::loadSounds() {
	sound_melee = Mix_LoadWAV(mods->locate("soundfx/melee_attack.ogg").c_str());
	sound_hit = Mix_LoadWAV(mods->locate("soundfx/" + stats.base + "_hit.ogg").c_str());
	sound_die = Mix_LoadWAV(mods->locate("soundfx/" + stats.base + "_die.ogg").c_str());
	sound_block = Mix_LoadWAV(mods->locate("soundfx/powers/block.ogg").c_str());
	sound_steps[0] = Mix_LoadWAV(mods->locate("soundfx/step_echo1.ogg").c_str());
	sound_steps[1] = Mix_LoadWAV(mods->locate("soundfx/step_echo2.ogg").c_str());
	sound_steps[2] = Mix_LoadWAV(mods->locate("soundfx/step_echo3.ogg").c_str());
	sound_steps[3] = Mix_LoadWAV(mods->locate("soundfx/step_echo4.ogg").c_str());
	level_up = Mix_LoadWAV(mods->locate("soundfx/level_up.ogg").c_str());
				
	if (!sound_melee || !sound_hit || !sound_die || !sound_steps[0] || !level_up) {
		printf("Mix_LoadWAV: %s\n", Mix_GetError());
	}
	
}

bool Avatar::pressing_move() {
	if(MOUSE_MOVE) {
		return inp->pressing[MAIN1];
	} else {
		return inp->pressing[UP] || inp->pressing[DOWN] || inp->pressing[LEFT] || inp->pressing[RIGHT];
	}	
}

void Avatar::set_direction() {
	// handle direction changes
	if(MOUSE_MOVE) {
		Point target = screen_to_map(inp->mouse.x,  inp->mouse.y, stats.pos.x, stats.pos.y);
		stats.direction = face(target.x, target.y);
	} else {
		if(inp->pressing[UP] && inp->pressing[LEFT]) stats.direction = 1;
		else if(inp->pressing[UP] && inp->pressing[RIGHT]) stats.direction = 3;
		else if(inp->pressing[DOWN] && inp->pressing[RIGHT]) stats.direction = 5;
		else if(inp->pressing[DOWN] && inp->pressing[LEFT]) stats.direction = 7;
		else if(inp->pressing[LEFT]) stats.direction = 0;
		else if(inp->pressing[UP]) stats.direction = 2;
		else if(inp->pressing[RIGHT]) stats.direction = 4;
		else if(inp->pressing[DOWN]) stats.direction = 6;
	}
}

/**
 * logic()
 * Handle a single frame.  This includes:
 * - move the avatar based on buttons pressed
 * - calculate the next frame of animation
 * - calculate camera position based on avatar position
 *
 * @param power_index The actionbar power activated.  -1 means no power.
 */
void Avatar::logic(int actionbar_power, bool restrictPowerUse) {

	Point target;
	int stepfx;
	stats.logic();
	if (stats.stun_duration > 0) return;
	bool allowed_to_move;
	bool allowed_to_use_power;
	
	// check level up
	int max_spendable_stat_points = 16;
	if (stats.xp >= stats.xp_table[stats.level] && stats.level < MAX_CHARACTER_LEVEL) {
		stats.level++;
		stringstream ss;
		ss << msg->get("Congratulations, you have reached level %d!", stats.level);
		if (stats.level < max_spendable_stat_points) {
			ss << " " << msg->get("You may increase one attribute through the Character Menu.");
			newLevelNotification = true;
		}
		log_msg = ss.str();
		stats.recalc();
		Mix_PlayChannel(-1, level_up, 0);
	}

	// check for bleeding spurt
	if (stats.bleed_duration % 30 == 1) {
		powers->activate(POWER_SPARK_BLOOD, &stats, stats.pos);
	}
	// check for bleeding to death
	if (stats.hp == 0 && !(stats.cur_state == AVATAR_DEAD)) {
		stats.cur_state = AVATAR_DEAD;
	}
	
	// assist mouse movement
	if (!inp->pressing[MAIN1]) drag_walking = false;
	
	// handle animation
	activeAnimation->advanceFrame();
			
	switch(stats.cur_state) {
		case AVATAR_STANCE:

			setAnimation("stance");
		
			// allowed to move or use powers?
			if (MOUSE_MOVE) {
				allowed_to_move = restrictPowerUse && (!inp->lock[MAIN1] || drag_walking);
				allowed_to_use_power = !allowed_to_move;
			}
			else {
				allowed_to_move = true;
				allowed_to_use_power = true;
			}

			// handle transitions to RUN
			if (allowed_to_move)
				set_direction();
			
			if (pressing_move() && allowed_to_move) {
				if (MOUSE_MOVE && inp->pressing[MAIN1]) {
					inp->lock[MAIN1] = true;
					drag_walking = true;
				}
				
				if (move()) { // no collision
					stats.cur_state = AVATAR_RUN;
				}

			}
			// handle power usage
			if (allowed_to_use_power && actionbar_power != -1 && stats.cooldown_ticks == 0) {				
				target = screen_to_map(inp->mouse.x,  inp->mouse.y + powers->powers[actionbar_power].aim_assist, stats.pos.x, stats.pos.y);
			
				// check requirements
				if (powers->powers[actionbar_power].requires_mp > stats.mp)
					break;
				if (powers->powers[actionbar_power].requires_physical_weapon && !stats.wielding_physical)
					break;
				if (powers->powers[actionbar_power].requires_mental_weapon && !stats.wielding_mental)
					break;
				if (powers->powers[actionbar_power].requires_offense_weapon && !stats.wielding_offense)
					break;
				if (powers->powers[actionbar_power].requires_los && !map->collider.line_of_sight(stats.pos.x, stats.pos.y, target.x, target.y))
					break;
				if (powers->powers[actionbar_power].requires_empty_target && !map->collider.is_empty(target.x, target.y))
					break;
				if (stats.hero_cooldown[actionbar_power] > 0)
					break;

				stats.hero_cooldown[actionbar_power] = powers->powers[actionbar_power].cooldown; //set the cooldown timer
				current_power = actionbar_power;
				act_target.x = target.x;
				act_target.y = target.y;
			
				// is this a power that requires changing direction?
				if (powers->powers[current_power].face) {
					stats.direction = face(target.x, target.y);
				}
			
				// handle melee powers
				if (powers->powers[current_power].new_state == POWSTATE_SWING) {
					stats.cur_state = AVATAR_MELEE;
					break;
				}
				// handle ranged powers
				if (powers->powers[current_power].new_state == POWSTATE_SHOOT) {
					stats.cur_state = AVATAR_SHOOT;
					break;
				}
				// handle ment powers
				if (powers->powers[current_power].new_state == POWSTATE_CAST) {
					stats.cur_state = AVATAR_CAST;
					break;
				}
				if (powers->powers[current_power].new_state == POWSTATE_BLOCK) {
					stats.cur_state = AVATAR_BLOCK;
					stats.blocking = true;
					break;
				}
			}
			
			break;
			
		case AVATAR_RUN:

			setAnimation("run");
		
			stepfx = rand() % 4;
			
			if (activeAnimation->getCurFrame() == 1 || activeAnimation->getCurFrame() == activeAnimation->getMaxFrame()/2) {
				Mix_PlayChannel(-1, sound_steps[stepfx], 0);
			}

			// allowed to move or use powers?
			if (MOUSE_MOVE) {
				allowed_to_use_power = !(restrictPowerUse && !inp->lock[MAIN1]);
			}
			else {
				allowed_to_use_power = true;
			}
			
			// handle direction changes
			set_direction();
			
			// handle transition to STANCE
			if (!pressing_move()) {
				stats.cur_state = AVATAR_STANCE;
				break;
			} 
			else if (!move()) { // collide with wall
				stats.cur_state = AVATAR_STANCE;
				break;
			}
						
			// handle power usage
			if (allowed_to_use_power && actionbar_power != -1 && stats.cooldown_ticks == 0) {

				target = screen_to_map(inp->mouse.x,  inp->mouse.y + powers->powers[actionbar_power].aim_assist, stats.pos.x, stats.pos.y);
			
				// check requirements
				if (powers->powers[actionbar_power].requires_mp > stats.mp)
					break;
				if (powers->powers[actionbar_power].requires_physical_weapon && !stats.wielding_physical)
					break;
				if (powers->powers[actionbar_power].requires_mental_weapon && !stats.wielding_mental)
					break;
				if (powers->powers[actionbar_power].requires_offense_weapon && !stats.wielding_offense)
					break;
				if (powers->powers[actionbar_power].requires_los && !map->collider.line_of_sight(stats.pos.x, stats.pos.y, target.x, target.y))
					break;
				if (powers->powers[actionbar_power].requires_empty_target && !map->collider.is_empty(target.x, target.y))
					break;
				if (stats.hero_cooldown[actionbar_power] > 0)
					break;

				stats.hero_cooldown[actionbar_power] = powers->powers[actionbar_power].cooldown; //set the cooldown timer
				current_power = actionbar_power;
				act_target.x = target.x;
				act_target.y = target.y;
			
				// is this a power that requires changing direction?
				if (powers->powers[current_power].face) {
					stats.direction = face(target.x, target.y);
				}
			
				// handle melee powers
				if (powers->powers[current_power].new_state == POWSTATE_SWING) {
					stats.cur_state = AVATAR_MELEE;
					break;
				}
				// handle ranged powers
				if (powers->powers[current_power].new_state == POWSTATE_SHOOT) {
					stats.cur_state = AVATAR_SHOOT;
					break;
				}
				// handle ment powers
				if (powers->powers[current_power].new_state == POWSTATE_CAST) {
					stats.cur_state = AVATAR_CAST;
					break;
				}
				if (powers->powers[current_power].new_state == POWSTATE_BLOCK) {
					stats.cur_state = AVATAR_BLOCK;
					stats.blocking = true;
					break;
				}				
			}
							
			break;
			
		case AVATAR_MELEE:

			setAnimation("melee");

			if (activeAnimation->getCurFrame() == 1) {
				Mix_PlayChannel(-1, sound_melee, 0);
			}
			
			// do power
			if (activeAnimation->getCurFrame()  == activeAnimation->getMaxFrame()/2) {
				powers->activate(current_power, &stats, act_target);
			}
			
			if (activeAnimation->getTimesPlayed() >= 1) {
				stats.cur_state = AVATAR_STANCE;
				if (stats.haste_duration == 0) stats.cooldown_ticks += stats.cooldown;
			}
			break;

		case AVATAR_CAST:

			setAnimation("ment");

			// do power
			if (activeAnimation->getCurFrame() == activeAnimation->getMaxFrame()/2) {
				powers->activate(current_power, &stats, act_target);
			}

			if (activeAnimation->getTimesPlayed() >= 1) {
				stats.cur_state = AVATAR_STANCE;
				if (stats.haste_duration == 0) stats.cooldown_ticks += stats.cooldown;
			}
			break;

			
		case AVATAR_SHOOT:
		
			setAnimation("ranged");

			// do power
			if (activeAnimation->getCurFrame() == activeAnimation->getMaxFrame()/2) {
				powers->activate(current_power, &stats, act_target);
			}

			if (activeAnimation->getTimesPlayed() >= 1) {
				stats.cur_state = AVATAR_STANCE;
				if (stats.haste_duration == 0) stats.cooldown_ticks += stats.cooldown;
			}
			break;

		case AVATAR_BLOCK:
		
			setAnimation("block");

			if (powers->powers[actionbar_power].new_state != POWSTATE_BLOCK) {
				stats.cur_state = AVATAR_STANCE;
				stats.blocking = false;
			}
			break;
			
		case AVATAR_HIT:

			setAnimation("hit");
						 
			if (activeAnimation->getTimesPlayed() >= 1) {
				stats.cur_state = AVATAR_STANCE;
			}
			
			break;
			
		case AVATAR_DEAD:

			setAnimation("die");
				
			if (activeAnimation->getCurFrame() == 1 && activeAnimation->getTimesPlayed() < 1) {
				Mix_PlayChannel(-1, sound_die, 0);
				log_msg = msg->get("You are defeated.  You lose half your gold.  Press Enter to continue.");
			}

			if (activeAnimation->getTimesPlayed() >= 1) {
				stats.corpse = true;
			}
			
			// allow respawn with Accept
			if (inp->pressing[ACCEPT]) {
				stats.hp = stats.maxhp;
				stats.mp = stats.maxmp;
				stats.alive = true;
				stats.corpse = false;
				stats.cur_state = AVATAR_STANCE;
				
				// remove temporary effects
				stats.clearEffects();
				
				// set teleportation variables.  GameEngine acts on these.
				map->teleportation = true;
				map->teleport_mapname = map->respawn_map;
				map->teleport_destination.x = map->respawn_point.x;
				map->teleport_destination.y = map->respawn_point.y;
			}
			
			break;
		
		default:
			break;
	}
	
	// calc new cam position from player position
	// cam is focused at player position
	map->cam.x = stats.pos.x;
	map->cam.y = stats.pos.y;
	map->hero_tile.x = stats.pos.x / 32;
	map->hero_tile.y = stats.pos.y / 32;
	
	// check for map events
	map->checkEvents(stats.pos);

	// decrement all cooldowns
	for (int i = 0; i < POWER_COUNT; i++){
		stats.hero_cooldown[i] -= 1000 / FRAMES_PER_SEC;
		if (stats.hero_cooldown[i] < 0) stats.hero_cooldown[i] = 0;
	}
}

/**
 * Called by HazardManager
 * Return false on a miss
 */
bool Avatar::takeHit(Hazard h) {

	if (stats.cur_state != AVATAR_DEAD) {
		// check miss
		int avoidance = stats.avoidance;
		if (stats.blocking) avoidance *= 2;
		if (rand() % 100 > (h.accuracy - avoidance + 25)) return false; 
	
		int dmg;
		if (h.dmg_min == h.dmg_max) dmg = h.dmg_min;
		else dmg = h.dmg_min + (rand() % (h.dmg_max - h.dmg_min + 1));
	
		// apply elemental resistance
		// TODO: make this generic
		if (h.trait_elemental == ELEMENT_FIRE) {
			dmg = (dmg * stats.attunement_fire) / 100;
		}
		if (h.trait_elemental == ELEMENT_WATER) {
			dmg = (dmg * stats.attunement_ice) / 100;			
		}
	
		// apply absorption
		int absorption;
		if (!h.trait_armor_penetration) { // armor penetration ignores all absorption
			if (stats.absorb_min == stats.absorb_max) absorption = stats.absorb_min;
			else absorption = stats.absorb_min + (rand() % (stats.absorb_max - stats.absorb_min + 1));
			
			if (stats.blocking) absorption += absorption + stats.absorb_max; // blocking doubles your absorb amount
			
			dmg = dmg - absorption;
			if (dmg < 1 && !stats.blocking) dmg = 1; // when blocking, dmg can be reduced to 0
			if (dmg <= 0) {
				dmg = 0;
				Mix_PlayChannel(-1, sound_block, 0);
				activeAnimation->reset(); // shield stutter
			}
		}
	
		
		int prev_hp = stats.hp;
		stats.takeDamage(dmg);
		
		// after effects
		if (stats.hp > 0 && stats.immunity_duration == 0 && dmg > 0) {
			if (h.stun_duration > stats.stun_duration) stats.stun_duration = h.stun_duration;
			if (h.slow_duration > stats.slow_duration) stats.slow_duration = h.slow_duration;
			if (h.bleed_duration > stats.bleed_duration) stats.bleed_duration = h.bleed_duration;
			if (h.immobilize_duration > stats.immobilize_duration) stats.immobilize_duration = h.immobilize_duration;
			if (h.hp_steal != 0) {
				h.src_stats->hp += (int)ceil((float)dmg * (float)h.hp_steal / 100.0);
				if (h.src_stats->hp > h.src_stats->maxhp) h.src_stats->hp = h.src_stats->maxhp;
			}
			// if (h.mp_steal != 0) { //enemies don't have MP
		}
		
		// post effect power
		if (h.post_power >= 0 && dmg > 0) {
			powers->activate(h.post_power, h.src_stats, stats.pos);
		}
		
		// Power-specific: Vengeance gains stacks when blocking
		if (stats.blocking && stats.physdef >= 9) {
			if (stats.vengeance_stacks < 3)
				stats.vengeance_stacks++;
		}
		
		
		if (stats.hp <= 0) {
			stats.cur_state = AVATAR_DEAD;
			
			// raise the death penalty flag.  Another module will read this and reset.
			stats.death_penalty = true;
		}
		else if (prev_hp > stats.hp) { // only interrupt if damage was taken
			Mix_PlayChannel(-1, sound_hit, 0);
			stats.cur_state = AVATAR_HIT;
		}
		
		return true;
	}
	return false;
}

/**
 * getRender()
 * Map objects need to be drawn in Z order, so we allow a parent object (GameEngine)
 * to collect all mobile sprites each frame.
 */
Renderable Avatar::getRender() {
	Renderable r = activeAnimation->getCurrentFrame(stats.direction);
	r.sprite = sprites;
	r.map_pos.x = stats.pos.x;
	r.map_pos.y = stats.pos.y;
	return r;
}

Avatar::~Avatar() {

	SDL_FreeSurface(sprites);
	Mix_FreeChunk(sound_melee);
	Mix_FreeChunk(sound_hit);
	Mix_FreeChunk(sound_die);
	Mix_FreeChunk(sound_block);
	Mix_FreeChunk(sound_steps[0]);
	Mix_FreeChunk(sound_steps[1]);
	Mix_FreeChunk(sound_steps[2]);
	Mix_FreeChunk(sound_steps[3]);
	Mix_FreeChunk(level_up);
			
	delete haz;
}
