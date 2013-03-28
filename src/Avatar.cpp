/*
Copyright © 2011-2012 Clint Bellanger
Copyright © 2012 Igor Paliychuk
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

/**
 * class Avatar
 *
 * Contains logic and rendering routines for the player avatar.
 */


#include "SDL_gfxBlitFunc.h"
#include "Animation.h"
#include "AnimationManager.h"
#include "AnimationSet.h"
#include "Avatar.h"
#include "FileParser.h"
#include "Hazard.h"
#include "MapRenderer.h"
#include "PowerManager.h"
#include "SharedResources.h"
#include "UtilsParsing.h"
#include "UtilsMath.h"

#include <sstream>

using namespace std;

Avatar::Avatar(PowerManager *_powers, MapRenderer *_map)
 : Entity(_map)
 , lockSwing(false)
 , lockCast(false)
 , lockShoot(false)
 , animFwd(false)
 , powers(_powers)
 , hero_stats(NULL)
 , charmed_stats(NULL)
 , act_target()
 , attacking (false)
 , drag_walking(false)
 , respawn(false)
 , close_menus(false)
{

	init();

	// default hero animation data
	stats.cooldown = 4;

	// load the hero's animations from hero definition file
	anim->increaseCount("animations/hero.txt");
	animationSet = anim->getAnimationSet("animations/hero.txt");
	activeAnimation = animationSet->getAnimation();

	loadLayerDefinitions();
}

void Avatar::init() {

	stats.hero_cooldown.resize(POWER_COUNT);

	// name, base, look are set by GameStateNew so don't reset it here

	// other init
	sprites = 0;
	stats.cur_state = AVATAR_STANCE;
	stats.pos.x = map->spawn.x;
	stats.pos.y = map->spawn.y;
	stats.direction = map->spawn_dir;
	current_power = 0;
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
	respawn = false;

	stats.cooldown_ticks = 0;

	haz = NULL;

	body = -1;

	transform_triggered = false;
	setPowers = false;
	revertPowers = false;
	last_transform = "";
	untransform_power = getUntransformPower();

	stats.hero_cooldown = vector<int>(POWER_COUNT, 0);

	for (int i=0; i<4; i++) {
		sound_steps[i] = 0;
	}

	sound_melee = 0;
	sound_mental = 0;
	sound_hit = 0;
	sound_die = 0;
	sound_block = 0;
	level_up = 0;
}

/**
 * Load avatar sprite layer definitions into vector.
 */
void Avatar::loadLayerDefinitions() {
	layer_def = vector<vector<unsigned> >(8, vector<unsigned>());
	layer_reference_order = vector<string>();

	FileParser infile;
	if (infile.open(mods->locate("engine/hero_options.txt"))) {
		while(infile.next()) {
			infile.val = infile.val + ',';

			if (infile.key == "layer") {
				unsigned dir = eatFirstInt(infile.val,',');
				if (dir>7) {
					fprintf(stderr, "direction must be in range [0,7]\n");
					SDL_Quit();
					exit(1);
				}
				string layer = eatFirstString(infile.val,',');
				while (layer != "") {
					// check if already in layer_reference:
					unsigned ref_pos;
					for (ref_pos = 0; ref_pos < layer_reference_order.size(); ++ref_pos)
						if (layer == layer_reference_order[ref_pos])
							break;
					if (ref_pos == layer_reference_order.size())
						layer_reference_order.push_back(layer);
					layer_def[dir].push_back(ref_pos);

					layer = eatFirstString(infile.val,',');
				}
			}
		}
		infile.close();
	} else fprintf(stderr, "Unable to open engine/hero_options.txt!\n");

	// There are the positions of the items relative to layer_reference_order
	// so if layer_reference_order=main,body,head,off
	// and we got a layer=3,off,body,head,main
	// then the layer_def[3] looks like (3,1,2,0)
}

void Avatar::loadGraphics(std::vector<Layer_gfx> _img_gfx) {

	for (unsigned int i=0; i<animsets.size(); i++) {
		if (animsets[i])
			anim->decreaseCount(animsets[i]->getName());
		delete anims[i];
	}
	animsets.clear();
	anims.clear();

	for (unsigned int i=0; i<_img_gfx.size(); i++) {
		if (_img_gfx[i].gfx != "") {
			string name = "animations/avatar/"+stats.base+"/"+_img_gfx[i].gfx+".txt";
			anim->increaseCount(name);
			animsets.push_back(anim->getAnimationSet(name));
			anims.push_back(animsets.back()->getAnimation(activeAnimation->getName()));
			anims.back()->syncTo(activeAnimation);
		} else {
			animsets.push_back(NULL);
			anims.push_back(NULL);
		}
	}
	anim->cleanUp();
}

void Avatar::loadSounds(const string& type_id) {
	// unload any sounds that are common between creatures and the hero
	snd->unload(sound_melee);
	snd->unload(sound_mental);
	snd->unload(sound_hit);
	snd->unload(sound_die);

	if (type_id != "none") {
		sound_melee = snd->load("soundfx/enemies/" + type_id + "_phys.ogg", "Avatar melee attack");
		sound_mental = snd->load("soundfx/enemies/" + type_id + "_ment.ogg", "Avatar mental attack");
		sound_hit = snd->load("soundfx/enemies/" + type_id + "_hit.ogg", "Avatar was hit");
		sound_die = snd->load("soundfx/enemies/" + type_id + "_die.ogg", "Avatar death");
	} else {
		sound_melee = snd->load("soundfx/melee_attack.ogg", "Avatar melee attack");
		sound_mental = 0; // hero does not have this sound
		sound_hit = snd->load("soundfx/" + stats.base + "_hit.ogg", "Avatar was hit");
		sound_die = snd->load("soundfx/" + stats.base + "_die.ogg", "Avatar death");
	}

	sound_block = snd->load("soundfx/powers/block.ogg", "Avatar blocking");
	level_up = snd->load("soundfx/level_up.ogg", "Avatar leveling up");
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
		snd->unload(sound_steps[i]);
	}

	// A literal "NULL" means we don't want to load any new sounds
	// This is used when transforming, since creatures don't have step sound effects
	if (stepname == "NULL") return;

	// load new sounds
	sound_steps[0] = snd->load("soundfx/steps/step_" + filename + "1.ogg", "Avatar loading foot steps");
	sound_steps[1] = snd->load("soundfx/steps/step_" + filename + "2.ogg", "Avatar loading foot steps");
	sound_steps[2] = snd->load("soundfx/steps/step_" + filename + "3.ogg", "Avatar loading foot steps");
	sound_steps[3] = snd->load("soundfx/steps/step_" + filename + "4.ogg", "Avatar loading foot steps");
}


bool Avatar::pressing_move() {
	if (inpt->mouse_emulation) return false;
	if (MOUSE_MOVE) {
		return inpt->pressing[MAIN1];
	} else {
		return inpt->pressing[UP] || inpt->pressing[DOWN] || inpt->pressing[LEFT] || inpt->pressing[RIGHT];
	}
}

void Avatar::set_direction() {
	// handle direction changes
	if (inpt->mouse_emulation) return;
	if (MOUSE_MOVE) {
		Point target = screen_to_map(inpt->mouse.x,  inpt->mouse.y, stats.pos.x, stats.pos.y);
		// if no line of movement to target, use pathfinder
		if (!map->collider.line_of_movement(stats.pos.x, stats.pos.y, target.x, target.y, stats.movement_type)) {
			vector<Point> path;

			// if a path is returned, target first waypoint
			if ( map->collider.compute_path(stats.pos, target, path, 1000, stats.movement_type)) {
				target = path.back();
			}
		}
		stats.direction = face(target.x, target.y);
	} else {
		if (inpt->pressing[UP] && inpt->pressing[LEFT]) stats.direction = 1;
		else if (inpt->pressing[UP] && inpt->pressing[RIGHT]) stats.direction = 3;
		else if (inpt->pressing[DOWN] && inpt->pressing[RIGHT]) stats.direction = 5;
		else if (inpt->pressing[DOWN] && inpt->pressing[LEFT]) stats.direction = 7;
		else if (inpt->pressing[LEFT]) stats.direction = 0;
		else if (inpt->pressing[UP]) stats.direction = 2;
		else if (inpt->pressing[RIGHT]) stats.direction = 4;
		else if (inpt->pressing[DOWN]) stats.direction = 6;
		// Adjust for ORTHO tilesets
		if (TILESET_ORIENTATION == TILESET_ORTHOGONAL &&
				(inpt->pressing[UP] || inpt->pressing[DOWN] ||
				inpt->pressing[LEFT] || inpt->pressing[RIGHT]))
			stats.direction = stats.direction == 7 ? 0 : stats.direction + 1;
	}
}

void Avatar::handlePower(int actionbar_power) {
	if (actionbar_power != 0 && stats.cooldown_ticks == 0) {
		const Power &power = powers->getPower(actionbar_power);
		Point target;
		if (MOUSE_AIM) {
			if (power.aim_assist)
				target = screen_to_map(inpt->mouse.x,  inpt->mouse.y + AIM_ASSIST, stats.pos.x, stats.pos.y);
			else
				target = screen_to_map(inpt->mouse.x,  inpt->mouse.y, stats.pos.x, stats.pos.y);
		} else {
			FPoint ftarget = calcVector(stats.pos, stats.direction, stats.melee_range);
			target.x = static_cast<int>(ftarget.x);
			target.y = static_cast<int>(ftarget.y);
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
				stats.effects.triggered_block = true;
				break;

			case POWSTATE_INSTANT:	// handle instant powers
				powers->activate(current_power, &stats, target);
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
 * @param actionbar_power The actionbar power activated.  0 means no power.
 * @param restrictPowerUse rather or not to allow power usage on mouse1
 */
void Avatar::logic(int actionbar_power, bool restrictPowerUse) {

	// clear current space to allow correct movement
	map->collider.unblock(stats.pos.x, stats.pos.y);

	// turn on all passive powers
	if ((stats.hp > 0 || stats.effects.triggered_death) && !respawn) powers->activatePassives(&stats);

	int stepfx;
	stats.logic();
	if (stats.effects.forced_move) {
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
	if (stats.effects.stun) {

		map->collider.block(stats.pos.x, stats.pos.y);
		return;
	}


	bool allowed_to_move;
	bool allowed_to_use_power;

	// check for revive
	if (stats.hp <= 0 && stats.effects.revive) {
		stats.hp = stats.maxhp;
		stats.alive = true;
		stats.corpse = false;
		stats.cur_state = AVATAR_STANCE;
	}

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
		snd->play(level_up);

		// if the player managed to level up while dead (e.g. via a bleeding creature), restore to life
		if (stats.cur_state == AVATAR_DEAD) {
			stats.cur_state = AVATAR_STANCE;
		}
	}

	// check for bleeding spurt
	if (stats.effects.damage > 0 && stats.hp > 0) {
		comb->addMessage(stats.effects.damage, stats.pos, COMBAT_MESSAGE_TAKEDMG);
	}

	// check for bleeding to death
	if (stats.hp == 0 && !(stats.cur_state == AVATAR_DEAD)) {
		stats.effects.triggered_death = true;
		stats.cur_state = AVATAR_DEAD;
	}

	// assist mouse movement
	if (!inpt->pressing[MAIN1]) {
		drag_walking = false;
		attacking = false;
	} else {
        if(!inpt->lock[MAIN1]) {
            attacking = true;
		}
	}

	// handle animation
	activeAnimation->advanceFrame();
	for (unsigned i=0; i < anims.size(); i++)
		if (anims[i] != NULL)
			anims[i]->advanceFrame();

	// handle transformation
	if (stats.transform_type != "" && stats.transform_type != "untransform" && transform_triggered == false) transform();
	if (stats.transform_type != "" && stats.transform_duration == 0) untransform();

	switch(stats.cur_state) {
		case AVATAR_STANCE:

			setAnimation("stance");

			// allowed to move or use powers?
			if (MOUSE_MOVE) {
				allowed_to_move = restrictPowerUse && (!inpt->lock[MAIN1] || drag_walking) && !lockSwing && !lockShoot && !lockCast;
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

			if (MOUSE_MOVE && !inpt->pressing[MAIN1]) {
				inpt->lock[MAIN1] = false;
				lockSwing = false;
				lockShoot = false;
				lockCast = false;
			}

			// handle power usage
			if (allowed_to_use_power)
				handlePower(actionbar_power);
			break;

		case AVATAR_RUN:

			setAnimation("run");

			stepfx = rand() % 4;

			if (activeAnimation->isFirstFrame() || activeAnimation->isActiveFrame())
				snd->play(sound_steps[stepfx]);

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
			if (allowed_to_use_power)
				handlePower(actionbar_power);
			break;

		case AVATAR_MELEE:

			setAnimation("melee");

			if (MOUSE_MOVE) lockSwing = true;

			if (activeAnimation->isFirstFrame())
				snd->play(sound_melee);

			// do power
			if (activeAnimation->isActiveFrame()) {
				powers->activate(current_power, &stats, act_target);
			}

			if (activeAnimation->getTimesPlayed() >= 1) {
				stats.cur_state = AVATAR_STANCE;
				stats.cooldown_ticks += stats.cooldown;
			}
			break;

		case AVATAR_CAST:

			setAnimation("ment");

			if (MOUSE_MOVE) lockCast = true;

			if (activeAnimation->isFirstFrame())
				snd->play(sound_mental);

			// do power
			if (activeAnimation->isActiveFrame()) {
				powers->activate(current_power, &stats, act_target);
			}

			if (activeAnimation->getTimesPlayed() >= 1) {
				stats.cur_state = AVATAR_STANCE;
				stats.cooldown_ticks += stats.cooldown;
			}
			break;


		case AVATAR_SHOOT:

			setAnimation("ranged");

			if (MOUSE_MOVE) lockShoot = true;

			// do power
			if (activeAnimation->isActiveFrame()) {
				powers->activate(current_power, &stats, act_target);
			}

			if (activeAnimation->getTimesPlayed() >= 1) {
				stats.cur_state = AVATAR_STANCE;
				stats.cooldown_ticks += stats.cooldown;
			}
			break;

		case AVATAR_BLOCK:

			setAnimation("block");

			if (powers->powers[actionbar_power].new_state != POWSTATE_BLOCK) {
				stats.cur_state = AVATAR_STANCE;
				stats.effects.triggered_block = false;
				stats.effects.clearTriggerEffects(TRIGGER_BLOCK);
			}
			break;

		case AVATAR_HIT:

			setAnimation("hit");

			if (activeAnimation->isFirstFrame()) {
				stats.effects.triggered_hit = true;
			}

			if (activeAnimation->getTimesPlayed() >= 1) {
				stats.cur_state = AVATAR_STANCE;
			}

			break;

		case AVATAR_DEAD:
			if (stats.effects.triggered_death) break;

			if (stats.transformed) {
				stats.transform_duration = 0;
				untransform();
			}

			setAnimation("die");

			if (activeAnimation->isFirstFrame() && activeAnimation->getTimesPlayed() < 1) {
				stats.effects.clearEffects();

				// raise the death penalty flag.  Another module will read this and reset.
				stats.death_penalty = true;

				// close menus in GameStatePlay
				close_menus = true;

				snd->play(sound_die);

				if (stats.permadeath) {
					log_msg = msg->get("You are defeated. Game over! Press Enter to exit to Title.");
				}
				else {
					log_msg = msg->get("You are defeated.  You lose half your %s.  Press Enter to continue.", CURRENCY);
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
					respawn = true;

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
		stats.hero_cooldown[i]--;
		if (stats.hero_cooldown[i] < 0) stats.hero_cooldown[i] = 0;
	}

	// make the current square solid
	map->collider.block(stats.pos.x, stats.pos.y);
}

/**
 * Called by HazardManager
 * Return false on a miss
 */
bool Avatar::takeHit(const Hazard &h) {

	if (stats.cur_state != AVATAR_DEAD) {
		CombatText *combat_text = comb;
		// check miss
		int avoidance = stats.avoidance;
		if (stats.effects.triggered_block) avoidance *= 2;
		clampCeil(avoidance, MAX_AVOIDANCE);
		if (percentChance(avoidance - h.accuracy - 25)) {
			combat_text->addMessage(msg->get("miss"), stats.pos, COMBAT_MESSAGE_MISS);
			return false;
		}

		int dmg = randBetween(h.dmg_min, h.dmg_max);

		// apply elemental resistance

		if (h.trait_elemental >= 0 && unsigned(h.trait_elemental) < stats.vulnerable.size()) {
			unsigned i = h.trait_elemental;
			int vulnerable = stats.vulnerable[i];
			if (stats.vulnerable[i] > MAX_RESIST && stats.vulnerable[i] < 100)
				vulnerable = MAX_RESIST;
			dmg = (dmg * vulnerable) / 100;
		}

		if (!h.trait_armor_penetration) { // armor penetration ignores all absorption
			// apply absorption
			int absorption = randBetween(stats.absorb_min, stats.absorb_max);

			if (stats.effects.triggered_block) {
				absorption += absorption + stats.absorb_max; // blocking doubles your absorb amount
			}

			if (absorption > 0 && dmg != 0) {
				if ((absorption*100)/dmg > MAX_BLOCK)
					absorption = (dmg * MAX_BLOCK) /100;
				if ((absorption*100)/dmg > MAX_ABSORB && !stats.effects.triggered_block)
					absorption = (dmg * MAX_ABSORB) /100;

				// Sometimes, the absorb limits cause absorbtion to drop to 1
				// This could be confusing to a player that has something with an absorb of 1 equipped
				// So we round absorption up in this case
				if (absorption == 0) absorption = 1;
			}

			dmg = dmg - absorption;
			if (dmg <= 0) {
				dmg = 0;
				if (h.trait_elemental < 0) {
					if (stats.effects.triggered_block && MAX_BLOCK < 100) dmg = 1;
					else if (!stats.effects.triggered_block && MAX_ABSORB < 100) dmg = 1;
				} else {
					if (MAX_RESIST < 100) dmg = 1;
				}
				snd->play(sound_block);
				activeAnimation->reset(); // shield stutter
				for (unsigned i=0; i < animsets.size(); i++)
					if (anims[i])
						anims[i]->reset();
			}
		}


		int prev_hp = stats.hp;
		combat_text->addMessage(dmg, stats.pos, COMBAT_MESSAGE_TAKEDMG);
		stats.takeDamage(dmg);

		// after effects
		if (stats.hp > 0 && dmg > 0) {

			if (h.mod_power > 0) powers->effect(&stats, h.mod_power);
			powers->effect(&stats, h.power_index);

			if (!stats.effects.immunity) {
				if (stats.effects.forced_move) {
					float theta = powers->calcTheta(h.src_stats->pos.x, h.src_stats->pos.y, stats.pos.x, stats.pos.y);
					stats.forced_speed.x = static_cast<int>(ceil(stats.effects.forced_speed * cos(theta)));
					stats.forced_speed.y = static_cast<int>(ceil(stats.effects.forced_speed * sin(theta)));
				}
				if (h.hp_steal != 0) {
					int steal_amt = (dmg * h.hp_steal) / 100;
					if (steal_amt == 0 && dmg > 0) steal_amt = 1;
					combat_text->addMessage(msg->get("+%d HP",steal_amt), h.src_stats->pos, COMBAT_MESSAGE_BUFF);
					h.src_stats->hp = min(h.src_stats->hp + steal_amt, h.src_stats->maxhp);
				}
			}
			// if (h.mp_steal != 0) { //enemies don't have MP
		}

		// post effect power
		if (h.post_power > 0 && dmg > 0) {
			powers->activate(h.post_power, h.src_stats, stats.pos);
		}

		if (stats.hp <= 0) {
			stats.effects.triggered_death = true;
			stats.cur_state = AVATAR_DEAD;
		}
		else if (prev_hp > stats.hp) { // only interrupt if damage was taken
			snd->play(sound_hit);
			if (!percentChance(stats.poise) && stats.cooldown_hit_ticks == 0) {
				stats.cur_state = AVATAR_HIT;
				stats.cooldown_hit_ticks = stats.cooldown_hit;
			}
		}

		return true;
	}
	return false;
}


void Avatar::transform() {
	// calling a transform power locks the actionbar, so we unlock it here
	inpt->unlockActionBar();

	transform_triggered = true;
	stats.transformed = true;
	setPowers = true;

	delete charmed_stats;
	charmed_stats = new StatBlock();
	charmed_stats->load("enemies/" + stats.transform_type + ".txt");

	// temporary save hero stats
	delete hero_stats;

	hero_stats = new StatBlock();
	*hero_stats = stats;

	// replace some hero stats
	stats.speed = charmed_stats->speed;
	stats.dspeed = charmed_stats->dspeed;
	stats.flying = charmed_stats->flying;
	stats.humanoid = charmed_stats->humanoid;
	stats.animations = charmed_stats->animations;
	stats.powers_list = charmed_stats->powers_list;
	stats.powers_passive = charmed_stats->powers_passive;
	stats.effects.clearEffects();

	string animationname = "animations/enemies/"+charmed_stats->animations + ".txt";
	anim->decreaseCount("animations/hero.txt");
	anim->increaseCount(animationname);
	animationSet = anim->getAnimationSet(animationname);
	delete activeAnimation;
	activeAnimation = animationSet->getAnimation();
	stats.cur_state = AVATAR_STANCE;

	// damage
	clampFloor(stats.dmg_melee_min, charmed_stats->dmg_melee_min);
	clampFloor(stats.dmg_melee_max, charmed_stats->dmg_melee_max);

	clampFloor(stats.dmg_ment_min, charmed_stats->dmg_ment_min);
	clampFloor(stats.dmg_ment_max, charmed_stats->dmg_ment_max);

	clampFloor(stats.dmg_ranged_min, charmed_stats->dmg_ranged_min);
	clampFloor(stats.dmg_ranged_max, charmed_stats->dmg_ranged_max);

	// dexterity
	clampFloor(stats.absorb_min, charmed_stats->absorb_min);
	clampFloor(stats.absorb_max, charmed_stats->absorb_max);

	clampFloor(stats.avoidance, charmed_stats->avoidance);

	clampFloor(stats.accuracy, charmed_stats->accuracy);

	clampFloor(stats.crit, charmed_stats->crit);

	// resistances
	for (unsigned int i=0; i<stats.vulnerable.size(); i++)
		clampCeil(stats.vulnerable[i], charmed_stats->vulnerable[i]);

	loadSounds(charmed_stats->sfx_prefix);
	loadStepFX("NULL");
}

void Avatar::untransform() {
	// calling a transform power locks the actionbar, so we unlock it here
	inpt->unlockActionBar();

	// Only allow untransform when on a valid tile
	if (!map->collider.is_valid_position(stats.pos.x,stats.pos.y,MOVEMENT_NORMAL)) return;

	stats.transformed = false;
	transform_triggered = false;
	stats.transform_type = "";
	revertPowers = true;
	stats.effects.clearEffects();

	// revert some hero stats to last saved
	stats.speed = hero_stats->speed;
	stats.dspeed = hero_stats->dspeed;
	stats.flying = hero_stats->flying;
	stats.humanoid = hero_stats->humanoid;
	stats.animations = hero_stats->animations;
	stats.effects = hero_stats->effects;
	stats.powers_list = hero_stats->powers_list;
	stats.powers_passive = hero_stats->powers_passive;

	anim->increaseCount("animations/hero.txt");
	anim->decreaseCount("animations/enemies/"+charmed_stats->animations + ".txt");
	animationSet = anim->getAnimationSet("animations/hero.txt");
	delete activeAnimation;
	activeAnimation = animationSet->getAnimation();
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

	for (unsigned int i=0; i<stats.vulnerable.size(); i++) {
		stats.vulnerable[i] = hero_stats->vulnerable[i];
	}

	loadSounds();
	loadStepFX(stats.sfx_step);

	delete charmed_stats;
	delete hero_stats;
	charmed_stats = NULL;
	hero_stats = NULL;
}

void Avatar::setAnimation(std::string name) {
	if (name == activeAnimation->getName())
		return;

	Entity::setAnimation(name);
	for (unsigned i=0; i < animsets.size(); i++) {
		delete anims[i];
		if (animsets[i])
			anims[i] = animsets[i]->getAnimation(name);
		else
			anims[i] = 0;
	}
}

/**
 * Find untransform power index to use for manual untransfrom ability
 */
int Avatar::getUntransformPower() {
	for (unsigned id=0; id<powers->powers.size(); id++) {
		if (powers->powers[id].spawn_type == "untransform" && powers->powers[id].requires_item == -1)
			return id;
	}
	return 0;
}

void Avatar::addRenders(vector<Renderable> &r) {
	if (!stats.transformed) {
		for (unsigned i = 0; i < layer_def[stats.direction].size(); ++i) {
			unsigned index = layer_def[stats.direction][i];
			if (anims[index]) {
				Renderable ren = anims[index]->getCurrentFrame(stats.direction);
				ren.map_pos = stats.pos;
				ren.prio = i+1;
				r.push_back(ren);
			}
		}
	} else {
		Renderable ren = activeAnimation->getCurrentFrame(stats.direction);
		ren.map_pos = stats.pos;
		r.push_back(ren);
	}
	// add effects
	for (unsigned i = 0; i < stats.effects.effect_list.size(); ++i) {
		if (stats.effects.effect_list[i].animation && !stats.effects.effect_list[i].animation->isCompleted()) {
			Renderable ren = stats.effects.effect_list[i].animation->getCurrentFrame(0);
			ren.map_pos = stats.pos;
			if (stats.effects.effect_list[i].render_above) ren.prio = layer_def[stats.direction].size()+1;
			else ren.prio = 0;
			r.push_back(ren);
		}
	}
}

Avatar::~Avatar() {

	if (stats.transformed && charmed_stats && charmed_stats->animations != "") {
		anim->decreaseCount("animations/enemies/"+charmed_stats->animations + ".txt");
	} else {
		anim->decreaseCount("animations/hero.txt");
	}

	for (unsigned int i=0; i<animsets.size(); i++) {
		if (animsets[i])
			anim->decreaseCount(animsets[i]->getName());
		delete anims[i];
	}
	anim->cleanUp();

	delete charmed_stats;
	delete hero_stats;

	snd->unload(sound_melee);
	snd->unload(sound_mental);
	snd->unload(sound_hit);
	snd->unload(sound_die);
	snd->unload(sound_block);

	for (int i = 0;i < 4; i++)
		snd->unload(sound_steps[i]);

	snd->unload(level_up);

	delete haz;
}
