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
 * class Avatar
 *
 * Contains logic and rendering routines for the player avatar.
 */


#include "SDL_gfxBlitFunc.h"
#include "Animation.h"
#include "Avatar.h"
#include "CombatText.h"
#include "Hazard.h"
#include "MapRenderer.h"
#include "PowerManager.h"
#include "SharedResources.h"

#include <sstream>

using namespace std;


Avatar::Avatar(PowerManager *_powers, MapRenderer *_map) : Entity(_map), powers(_powers) {

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
	transformed_sprites = 0;
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
	stats.humanoid = true;
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

	transform_triggered = false;
	untransform_triggered = false;
	setPowers = false;
	revertPowers = false;
	last_transform = "";

	for (int i = 0; i < POWER_COUNT; i++) {
		stats.hero_cooldown[i] = 0;
	}

	for (int i=0; i<4; i++) {
		sound_steps[i] = NULL;
	}

	sound_melee = NULL;
	sound_hit = NULL;
	sound_die = NULL;
	sound_block = NULL;
	level_up = NULL;
}

void Avatar::loadGraphics(const string& _img_main, string _img_armor, const string& _img_off) {
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


		// assuming the hero is right-handed, we know the layer z-order
		// copy the furthest hand first
		src.w = dest.w = 4096;
		src.h = dest.h = 256;
		src.x = dest.x = 0;
		src.y = dest.y = 0;
		if (gfx_main) SDL_gfxBlitRGBA(gfx_main, &src, sprites, &dest); // row 0,1 main hand
		src.y = dest.y = 768;
		if (gfx_main) SDL_gfxBlitRGBA(gfx_main, &src, sprites, &dest); // row 6,7 main hand
		src.h = dest.h = 512;
		src.y = dest.y = 256;
		if (gfx_off) SDL_gfxBlitRGBA(gfx_off, &src, sprites, &dest); // row 2-5 off hand

		// copy the head in the middle
		src.h = dest.h = 1024;
		src.y = dest.y = 0;
		if (gfx_head) SDL_gfxBlitRGBA(gfx_head, &src, sprites, &dest); // head

		// copy the closest hand last
		src.w = dest.w = 4096;
		src.h = dest.h = 256;
		src.x = dest.x = 0;
		src.y = dest.y = 0;
		if (gfx_off) SDL_gfxBlitRGBA(gfx_off, &src, sprites, &dest); // row 0,1 off hand
		src.y = dest.y = 768;
		if (gfx_off) SDL_gfxBlitRGBA(gfx_off, &src, sprites, &dest); // row 6,7 off hand
		src.h = dest.h = 512;
		src.y = dest.y = 256;
		if (gfx_main) SDL_gfxBlitRGBA(gfx_main, &src, sprites, &dest); // row 2-5 main hand

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
	if (audio && SOUND_VOLUME) {
		Mix_FreeChunk(sound_melee);
		Mix_FreeChunk(sound_hit);
		Mix_FreeChunk(sound_die);
		Mix_FreeChunk(sound_block);
		Mix_FreeChunk(level_up);

		sound_melee = Mix_LoadWAV(mods->locate("soundfx/melee_attack.ogg").c_str());
		sound_hit = Mix_LoadWAV(mods->locate("soundfx/" + stats.base + "_hit.ogg").c_str());
		sound_die = Mix_LoadWAV(mods->locate("soundfx/" + stats.base + "_die.ogg").c_str());
		sound_block = Mix_LoadWAV(mods->locate("soundfx/powers/block.ogg").c_str());
		level_up = Mix_LoadWAV(mods->locate("soundfx/level_up.ogg").c_str());

		if (!sound_melee || !sound_hit || !sound_die || !level_up) {
			printf("Mix_LoadWAV: %s\n", Mix_GetError());
		}
	}
}

/**
 * Walking/running steps sound depends on worn armor
 */
void Avatar::loadStepFX(const string& stepname) {

	string filename = stats.sfx_step;
	if (stepname != "") {
		filename = stepname;
	}

	// clear previous sounds
	for (int i=0; i<4; i++) {
		Mix_FreeChunk(sound_steps[i]);
		sound_steps[i] = NULL;
	}

	// load new sounds
	if (audio && SOUND_VOLUME) {
		sound_steps[0] = Mix_LoadWAV(mods->locate("soundfx/steps/step_" + filename + "1.ogg").c_str());
		sound_steps[1] = Mix_LoadWAV(mods->locate("soundfx/steps/step_" + filename + "2.ogg").c_str());
		sound_steps[2] = Mix_LoadWAV(mods->locate("soundfx/steps/step_" + filename + "3.ogg").c_str());
		sound_steps[3] = Mix_LoadWAV(mods->locate("soundfx/steps/step_" + filename + "4.ogg").c_str());
	} else {
		sound_steps[0] = NULL;
		sound_steps[1] = NULL;
		sound_steps[2] = NULL;
		sound_steps[3] = NULL;
	}
}


bool Avatar::pressing_move() {
	if(MOUSE_MOVE) {
		return inpt->pressing[MAIN1];
	} else {
		return inpt->pressing[UP] || inpt->pressing[DOWN] || inpt->pressing[LEFT] || inpt->pressing[RIGHT];
	}
}

void Avatar::set_direction() {
	// handle direction changes
	if(MOUSE_MOVE) {
		Point target = screen_to_map(inpt->mouse.x,  inpt->mouse.y, stats.pos.x, stats.pos.y);
		// if no line of movement to target, use pathfinder
		if( !map->collider.line_of_movement(stats.pos.x, stats.pos.y, target.x, target.y, stats.movement_type) ) {
			vector<Point> path;

			// if a path is returned, target first waypoint

			if ( map->collider.compute_path(stats.pos, target, path, 1000, stats.movement_type) ) {
				target = path.back();
			}


		}
		stats.direction = face(target.x, target.y);
	} else {
		if(inpt->pressing[UP] && inpt->pressing[LEFT]) stats.direction = 1;
		else if(inpt->pressing[UP] && inpt->pressing[RIGHT]) stats.direction = 3;
		else if(inpt->pressing[DOWN] && inpt->pressing[RIGHT]) stats.direction = 5;
		else if(inpt->pressing[DOWN] && inpt->pressing[LEFT]) stats.direction = 7;
		else if(inpt->pressing[LEFT]) stats.direction = 0;
		else if(inpt->pressing[UP]) stats.direction = 2;
		else if(inpt->pressing[RIGHT]) stats.direction = 4;
		else if(inpt->pressing[DOWN]) stats.direction = 6;
		// Adjust for ORTHO tilesets
		if (TILESET_ORIENTATION == TILESET_ORTHOGONAL &&
				(inpt->pressing[UP] || inpt->pressing[DOWN] ||
				inpt->pressing[LEFT] || inpt->pressing[RIGHT]))
			stats.direction = stats.direction == 7 ? 0 : stats.direction + 1;
	}
}

void Avatar::handlePower(int actionbar_power) {
	if (actionbar_power != -1 && stats.cooldown_ticks == 0) {
		const Power &power = powers->getPower(actionbar_power);
		Point target = screen_to_map(inpt->mouse.x,  inpt->mouse.y + power.aim_assist, stats.pos.x, stats.pos.y);
		if (ENABLE_JOYSTICK) {
			switch (stats.direction) {
				case 0:
					target = screen_to_map(VIEW_W_HALF-128, VIEW_H_HALF, stats.pos.x, stats.pos.y);
					break;
				case 1:
					target = screen_to_map(VIEW_W_HALF-128, VIEW_H_HALF-64, stats.pos.x, stats.pos.y);
					break;
				case 2:
					target = screen_to_map(VIEW_W_HALF, VIEW_H_HALF-128, stats.pos.x, stats.pos.y);
					break;
				case 3:
					target = screen_to_map(VIEW_W_HALF+128, VIEW_H_HALF-64, stats.pos.x, stats.pos.y);
					break;
				case 4:
					target = screen_to_map(VIEW_W_HALF+128, VIEW_H_HALF, stats.pos.x, stats.pos.y);
					break;
				case 5:
					target = screen_to_map(VIEW_W_HALF+128, VIEW_H_HALF+64, stats.pos.x, stats.pos.y);
					break;
				case 6:
					target = screen_to_map(VIEW_W_HALF, VIEW_H_HALF+128, stats.pos.x, stats.pos.y);
					break;
				case 7:
					target = screen_to_map(VIEW_W_HALF-128, VIEW_H_HALF+64, stats.pos.x, stats.pos.y);
					break;
				default:
					break;
			}
		}

		// check requirements
		if (!stats.canUsePower(power, actionbar_power))
			return;
		if (power.requires_los && !map->collider.line_of_sight(stats.pos.x, stats.pos.y, target.x, target.y))
			return;
		if (power.requires_empty_target && !map->collider.is_empty(target.x, target.y))
			return;
		if (stats.hero_cooldown[actionbar_power] > 0)
			return;
		if (!powers->hasValidTarget(actionbar_power,&stats,target))
			return;

		stats.hero_cooldown[actionbar_power] = power.cooldown; //set the cooldown timer
		current_power = actionbar_power;
		act_target = target;

		// is this a power that requires changing direction?
		if (power.face) {
			stats.direction = face(target.x, target.y);
		}

		switch (power.new_state) {
			case POWSTATE_SWING:	// handle melee powers
				stats.cur_state = AVATAR_MELEE;
				break;

			case POWSTATE_SHOOT:	// handle ranged powers
				stats.cur_state = AVATAR_SHOOT;
				break;

			case POWSTATE_CAST:		// handle ment powers
				stats.cur_state = AVATAR_CAST;
				break;

			case POWSTATE_BLOCK:	// handle blocking
				stats.cur_state = AVATAR_BLOCK;
				stats.blocking = true;
				break;
		}
	}
}

/**
 * logic()
 * Handle a single frame.  This includes:
 * - move the avatar based on buttons pressed
 * - calculate the next frame of animation
 * - calculate camera position based on avatar position
 *
 * @param actionbar_power The actionbar power activated.  -1 means no power.
 * @param restrictPowerUse rather or not to allow power usage on mouse1
 */
void Avatar::logic(int actionbar_power, bool restrictPowerUse) {

	// clear current space to allow correct movement
	map->collider.unblock(stats.pos.x, stats.pos.y);

	int stepfx;
	stats.logic();
	if (stats.forced_move_duration > 0) {
		move();

		// calc new cam position from player position
		// cam is focused at player position
		map->cam.x = stats.pos.x;
		map->cam.y = stats.pos.y;
		map->hero_tile.x = stats.pos.x / 32;
		map->hero_tile.y = stats.pos.y / 32;

		map->collider.block(stats.pos.x, stats.pos.y);
		return;
	}
	if (stats.stun_duration > 0) {

		map->collider.block(stats.pos.x, stats.pos.y);
		return;
	}


	bool allowed_to_move;
	bool allowed_to_use_power;

	// check level up
	if (stats.xp >= stats.xp_table[stats.level] && stats.level < MAX_CHARACTER_LEVEL) {
		stats.level_up = true;
		stats.level++;
		stringstream ss;
		ss << msg->get("Congratulations, you have reached level %d!", stats.level);
		if (stats.level < stats.max_spendable_stat_points) {
			ss << " " << msg->get("You may increase one attribute through the Character Menu.");
			newLevelNotification = true;
		}
		log_msg = ss.str();
		stats.recalc();
		if (level_up)
			Mix_PlayChannel(-1, level_up, 0);
	}

	// check for bleeding spurt
	if (stats.bleed_duration % 30 == 1) {
		CombatText::Instance()->addMessage(1, stats.pos, COMBAT_MESSAGE_DAMAGE);
		powers->activate(POWER_SPARK_BLOOD, &stats, stats.pos);
	}
	// check for bleeding to death
	if (stats.hp == 0 && !(stats.cur_state == AVATAR_DEAD)) {
		stats.cur_state = AVATAR_DEAD;
	}

	// assist mouse movement
	if (!inpt->pressing[MAIN1]) drag_walking = false;

	// handle animation
	activeAnimation->advanceFrame();

	// handle transformation
	if (stats.transform_type != "" && stats.transform_type != "untransform" && transform_triggered == false) transform();
	if (stats.transform_type != "" && stats.transform_duration == 0) untransform();

	switch(stats.cur_state) {
		case AVATAR_STANCE:

			setAnimation("stance");

			// allowed to move or use powers?
			if (MOUSE_MOVE) {
				allowed_to_move = restrictPowerUse && (!inpt->lock[MAIN1] || drag_walking);
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
				if (MOUSE_MOVE && inpt->pressing[MAIN1]) {
					inpt->lock[MAIN1] = true;
					drag_walking = true;
				}

				if (move()) { // no collision
					stats.cur_state = AVATAR_RUN;
				}

			}

			// handle power usage
			if(allowed_to_use_power)
				handlePower(actionbar_power);
			break;

		case AVATAR_RUN:

			setAnimation("run");

			stepfx = rand() % 4;

			if (activeAnimation->getCurFrame() == 1 || activeAnimation->getCurFrame() == activeAnimation->getMaxFrame()/2) {
				if (sound_steps[stepfx])
					Mix_PlayChannel(-1, sound_steps[stepfx], 0);
			}

			// allowed to move or use powers?
			if (MOUSE_MOVE) {
				allowed_to_use_power = !(restrictPowerUse && !inpt->lock[MAIN1]);
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
			if(allowed_to_use_power)
				handlePower(actionbar_power);
			break;

		case AVATAR_MELEE:

			setAnimation("melee");

			if (activeAnimation->getCurFrame() == 1) {
				if (sound_melee)
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
			if (stats.transformed) {
				stats.transform_duration = 0;
				untransform();
			}

			setAnimation("die");

			if (activeAnimation->getCurFrame() == 1 && activeAnimation->getTimesPlayed() < 1) {
				if (sound_die)
					Mix_PlayChannel(-1, sound_die, 0);
				if (stats.permadeath) {
					log_msg = msg->get("You are defeated. Game over! Press Enter to exit to Title.");
				}
				else {
					log_msg = msg->get("You are defeated.  You lose half your gold.  Press Enter to continue.");
				}
			}

			if (activeAnimation->getTimesPlayed() >= 1) {
				stats.corpse = true;
			}

			// allow respawn with Accept if not permadeath
			if (inpt->pressing[ACCEPT]) {
				map->teleportation = true;
				map->teleport_mapname = map->respawn_map;
				if (stats.permadeath) {
					// set these positions so it doesn't flash before jumping to Title
					map->teleport_destination.x = stats.pos.x;
					map->teleport_destination.y = stats.pos.y;
				}
				else {
					stats.hp = stats.maxhp;
					stats.mp = stats.maxmp;
					stats.alive = true;
					stats.corpse = false;
					stats.cur_state = AVATAR_STANCE;

					// remove temporary effects
					stats.clearEffects();

					// set teleportation variables.  GameEngine acts on these.
					map->teleport_destination.x = map->respawn_point.x;
					map->teleport_destination.y = map->respawn_point.y;
				}
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

	// make the current square solid
	map->collider.block(stats.pos.x, stats.pos.y);
}

/**
 * Called by HazardManager
 * Return false on a miss
 */
bool Avatar::takeHit(Hazard h) {

	if (stats.cur_state != AVATAR_DEAD) {
		CombatText *combat_text = CombatText::Instance();
		// check miss
		int avoidance = stats.avoidance;
		if (stats.blocking) avoidance *= 2;
		if (MAX_AVOIDANCE < avoidance) avoidance = MAX_AVOIDANCE;
		if (rand() % 100 > (h.accuracy - avoidance + 25)) {
			combat_text->addMessage("miss", stats.pos, COMBAT_MESSAGE_MISS);
			return false;
		}

		int dmg;
		if (h.dmg_min == h.dmg_max) dmg = h.dmg_min;
		else dmg = h.dmg_min + (rand() % (h.dmg_max - h.dmg_min + 1));

		// apply elemental resistance
		// TODO: make this generic
		int vulnerable;
		if (h.trait_elemental == ELEMENT_FIRE) {
			if (MAX_RESIST < stats.vulnerable_fire) vulnerable = MAX_RESIST;
			else vulnerable = stats.vulnerable_fire;
			dmg = (dmg * vulnerable) / 100;
		}
		if (h.trait_elemental == ELEMENT_WATER) {
			if (MAX_RESIST < stats.vulnerable_ice) vulnerable = MAX_RESIST;
			else vulnerable = stats.vulnerable_ice;
			dmg = (dmg * vulnerable) / 100;
		}

		// apply absorption
		int absorption;
		if (!h.trait_armor_penetration) { // armor penetration ignores all absorption
			if (stats.absorb_min == stats.absorb_max) absorption = stats.absorb_min;
			else absorption = stats.absorb_min + (rand() % (stats.absorb_max - stats.absorb_min + 1));

			if (stats.blocking) {
				absorption += absorption + stats.absorb_max; // blocking doubles your absorb amount
				if (absorption > 0) {
					if ((dmg*100)/absorption > MAX_BLOCK) absorption = (float)absorption * (MAX_BLOCK/100.0);
				}
			} else {
				if (absorption > 0) {
					if ((dmg*100)/absorption > MAX_ABSORB) absorption = (float)absorption * (MAX_ABSORB/100.0);
				}
			}

			dmg = dmg - absorption;
			if (dmg <= 0) {
				dmg = 0;
				if (h.trait_elemental < 0) {
					if (stats.blocking && MAX_BLOCK < 100) dmg = 1;
					else if (!stats.blocking && MAX_ABSORB < 100) dmg = 1;
				} else {
					if (MAX_RESIST < 100) dmg = 1;
				}
				if (sound_block)
					Mix_PlayChannel(-1, sound_block, 0);
				activeAnimation->reset(); // shield stutter
			}
		}


		int prev_hp = stats.hp;
		combat_text->addMessage(dmg, stats.pos, COMBAT_MESSAGE_DAMAGE);
		stats.takeDamage(dmg);

		// after effects
		if (stats.hp > 0 && stats.immunity_duration == 0 && dmg > 0) {
			if (h.stun_duration > stats.stun_duration) stats.stun_duration_total = stats.stun_duration = h.stun_duration;
			if (h.slow_duration > stats.slow_duration) stats.slow_duration_total = stats.slow_duration = h.slow_duration;
			if (h.bleed_duration > stats.bleed_duration) stats.bleed_duration_total = stats.bleed_duration = h.bleed_duration;
			if (h.immobilize_duration > stats.immobilize_duration) stats.immobilize_duration_total = stats.immobilize_duration = h.immobilize_duration;
			if (h.forced_move_duration > stats.forced_move_duration) stats.forced_move_duration_total = stats.forced_move_duration = h.forced_move_duration;
			if (h.forced_move_speed != 0) {
				float theta = powers->calcTheta(h.src_stats->pos.x, h.src_stats->pos.y, stats.pos.x, stats.pos.y);
				stats.forced_speed.x = ceil((float)h.forced_move_speed * cos(theta));
				stats.forced_speed.y = ceil((float)h.forced_move_speed * sin(theta));
			}
			if (h.hp_steal != 0) {
				int steal_amt = (int)ceil((float)dmg * (float)h.hp_steal / 100.0);
				combat_text->addMessage(steal_amt, h.src_stats->pos, COMBAT_MESSAGE_BUFF);
				h.src_stats->hp += steal_amt;
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
			if (sound_hit)
				Mix_PlayChannel(-1, sound_hit, 0);
			stats.cur_state = AVATAR_HIT;
		}

		return true;
	}
	return false;
}


void Avatar::transform() {

	transform_triggered = true;
	stats.transformed = true;
	setPowers = true;

	charmed_stats = new StatBlock();
	charmed_stats->load("enemies/" + stats.transform_type + ".txt");

	img_armor = charmed_stats->gfx_prefix;

	// transform the hero graphic
	if (last_transform != charmed_stats->gfx_prefix) {
		if (transformed_sprites) SDL_FreeSurface(transformed_sprites);

		transformed_sprites = IMG_Load(mods->locate("images/enemies/" + charmed_stats->gfx_prefix + ".png").c_str());

		if(!transformed_sprites) {
			fprintf(stderr, "Couldn't load image: %s\n", IMG_GetError());
			SDL_Quit();
		}
		last_transform = charmed_stats->gfx_prefix;
	}

	SDL_SetColorKey( transformed_sprites, SDL_SRCCOLORKEY, SDL_MapRGB(transformed_sprites->format, 255, 0, 255) );

	// optimize
	SDL_Surface *cleanup = transformed_sprites;
	transformed_sprites = SDL_DisplayFormatAlpha(transformed_sprites);
	SDL_FreeSurface(cleanup);

	// temporary save hero stats
	hero_stats = new StatBlock();
	*hero_stats = stats;

	// replace some hero stats
	stats.speed = charmed_stats->speed;
	stats.dspeed = charmed_stats->dspeed;
	stats.flying = charmed_stats->flying;
	stats.humanoid = charmed_stats->humanoid;
	stats.animations = charmed_stats->animations;
	stats.animationSpeed = charmed_stats->animationSpeed;
	loadAnimations("animations/" + charmed_stats->animations + ".txt");
	stats.cur_state = AVATAR_STANCE;

	// damage
	if (charmed_stats->dmg_melee_min > stats.dmg_melee_min)
	stats.dmg_melee_min = charmed_stats->dmg_melee_min;

	if (charmed_stats->dmg_melee_max > stats.dmg_melee_max)
	stats.dmg_melee_max = charmed_stats->dmg_melee_max;

	if (charmed_stats->dmg_ment_min > stats.dmg_ment_min)
	stats.dmg_ment_min = charmed_stats->dmg_ment_min;

	if (charmed_stats->dmg_ment_max > stats.dmg_ment_max)
	stats.dmg_ment_max = charmed_stats->dmg_ment_max;

	if (charmed_stats->dmg_ranged_min > stats.dmg_ranged_min)
	stats.dmg_ranged_min = charmed_stats->dmg_ranged_min;

	if (charmed_stats->dmg_ranged_max > stats.dmg_ranged_max)
	stats.dmg_ranged_max = charmed_stats->dmg_ranged_max;

	// dexterity
	if (charmed_stats->absorb_min > stats.absorb_min)
	stats.absorb_min = charmed_stats->absorb_min;

	if (charmed_stats->absorb_max > stats.absorb_max)
	stats.absorb_max = charmed_stats->absorb_max;

	if (charmed_stats->avoidance > stats.avoidance)
	stats.avoidance = charmed_stats->avoidance;

	if (charmed_stats->accuracy > stats.accuracy)
	stats.accuracy = charmed_stats->accuracy;

	if (charmed_stats->crit > stats.crit)
	stats.crit = charmed_stats->crit;

	// resistances
	if (charmed_stats->vulnerable_fire < stats.vulnerable_fire)
	stats.vulnerable_fire = charmed_stats->vulnerable_fire;

	if (charmed_stats->vulnerable_ice < stats.vulnerable_ice)
	stats.vulnerable_ice = charmed_stats->vulnerable_ice;

	loadStepFX("NULL");
}

void Avatar::untransform() {
	// Only allow untransform when on a valid tile
	if (!map->collider.valid_position(stats.pos.x,stats.pos.y,MOVEMENT_NORMAL)) return;

	stats.transformed = false;
	transform_triggered = false;
	untransform_triggered = true;
	stats.transform_type = "";
	revertPowers = true;

	// revert some hero stats to last saved
	stats.speed = hero_stats->speed;
	stats.dspeed = hero_stats->dspeed;
	stats.flying = hero_stats->flying;
	stats.humanoid = hero_stats->humanoid;
	stats.animations = hero_stats->animations;
	stats.animationSpeed = hero_stats->animationSpeed;
	loadAnimations("animations/hero.txt");
	stats.cur_state = AVATAR_STANCE;

	// This is a bit of a hack.
	// In order to switch to the stance animation, we can't already be in a stance animation
	setAnimation("run");

	stats.dmg_melee_min = hero_stats->dmg_melee_min;
	stats.dmg_melee_max = hero_stats->dmg_melee_max;
	stats.dmg_ment_min = hero_stats->dmg_ment_min;
	stats.dmg_ment_max = hero_stats->dmg_ment_max;
	stats.dmg_ranged_min = hero_stats->dmg_ranged_min;
	stats.dmg_ranged_max = hero_stats->dmg_ranged_max;

	stats.absorb_min = hero_stats->absorb_min;
	stats.absorb_max = hero_stats->absorb_max;
	stats.avoidance = hero_stats->avoidance;
	stats.accuracy = hero_stats->accuracy;
	stats.crit = hero_stats->crit;

	stats.vulnerable_fire = hero_stats->vulnerable_fire;
	stats.vulnerable_ice = hero_stats->vulnerable_ice;

	delete charmed_stats;
	delete hero_stats;
}

/**
 * getRender()
 * Map objects need to be drawn in Z order, so we allow a parent object (GameEngine)
 * to collect all mobile sprites each frame.
 */
Renderable Avatar::getRender() {
	Renderable r = activeAnimation->getCurrentFrame(stats.direction);
	if (stats.transformed)
		r.sprite = transformed_sprites;
	else
		r.sprite = sprites;
	r.map_pos = stats.pos;
	return r;
}

Avatar::~Avatar() {

	SDL_FreeSurface(sprites);
	if (transformed_sprites) SDL_FreeSurface(transformed_sprites);

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
