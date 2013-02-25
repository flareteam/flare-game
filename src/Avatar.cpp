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
 : Entity(_map, new AvatarStatBlock())
 , lockSwing(false)
 , lockCast(false)
 , lockShoot(false)
 , animFwd(false)
 , powers(_powers)
 , hero_stats(NULL)
 , charmed_stats(NULL)
 , act_target(Point())
 , attacking (false)
 , drag_walking(false)
 , respawn(false)
 , close_menus(false)
{

	init();

	// default hero animation data
	statBlock()->cooldown = 4;

	// load the hero's animations from hero definition file
	anim->increaseCount("animations/hero.txt");
	animationSet = anim->getAnimationSet("animations/hero.txt");
	activeAnimation = animationSet->getAnimation();

	loadLayerDefinitions();
}

void Avatar::init() {

	statBlock()->hero_cooldown.resize(POWER_COUNT);

	// name, base, look are set by GameStateNew so don't reset it here

	// other init
	sprites = 0;
	statBlock()->cur_state = AVATAR_STANCE;
	statBlock()->pos.x = map->spawn.x;
	statBlock()->pos.y = map->spawn.y;
	statBlock()->direction = map->spawn_dir;
	current_power = 0;
	newLevelNotification = false;

	lockSwing = false;
	lockCast = false;
	lockShoot = false;

	statBlock()->hero = true;
	statBlock()->humanoid = true;
	statBlock()->level = 1;
	statBlock()->xp = 0;
	statBlock()->physical_character = 1;
	statBlock()->mental_character = 1;
	statBlock()->offense_character = 1;
	statBlock()->defense_character = 1;
	statBlock()->physical_additional = 0;
	statBlock()->mental_additional = 0;
	statBlock()->offense_additional = 0;
	statBlock()->defense_additional = 0;
	statBlock()->speed = 14;
	statBlock()->dspeed = 10;
	statBlock()->recalc();

	log_msg = "";
	respawn = false;

	statBlock()->cooldown_ticks = 0;

	haz = NULL;

	body = -1;

	transform_triggered = false;
	setPowers = false;
	revertPowers = false;
	last_transform = "";
	untransform_power = getUntransformPower();

	statBlock()->hero_cooldown = vector<int>(POWER_COUNT, 0);

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
			string name = "animations/avatar/"+statBlock()->base+"/"+_img_gfx[i].gfx+".txt";
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
		sound_hit = snd->load("soundfx/" + statBlock()->base + "_hit.ogg", "Avatar was hit");
		sound_die = snd->load("soundfx/" + statBlock()->base + "_die.ogg", "Avatar death");
	}

	sound_block = snd->load("soundfx/powers/block.ogg", "Avatar blocking");
	level_up = snd->load("soundfx/level_up.ogg", "Avatar leveling up");
}

/**
 * Walking/running steps sound depends on worn armor
 */
void Avatar::loadStepFX(const string& stepname) {

	string filename = statBlock()->sfx_step;
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
		Point target = screen_to_map(inpt->mouse.x,  inpt->mouse.y, statBlock()->pos.x, statBlock()->pos.y);
		// if no line of movement to target, use pathfinder
		if (!map->collider.line_of_movement(statBlock()->pos.x, statBlock()->pos.y, target.x, target.y, statBlock()->movement_type)) {
			vector<Point> path;

			// if a path is returned, target first waypoint
			if ( map->collider.compute_path(statBlock()->pos, target, path, 1000, statBlock()->movement_type)) {
				target = path.back();
			}
		}
		statBlock()->direction = face(target.x, target.y);
	} else {
		if (inpt->pressing[UP] && inpt->pressing[LEFT]) statBlock()->direction = 1;
		else if (inpt->pressing[UP] && inpt->pressing[RIGHT]) statBlock()->direction = 3;
		else if (inpt->pressing[DOWN] && inpt->pressing[RIGHT]) statBlock()->direction = 5;
		else if (inpt->pressing[DOWN] && inpt->pressing[LEFT]) statBlock()->direction = 7;
		else if (inpt->pressing[LEFT]) statBlock()->direction = 0;
		else if (inpt->pressing[UP]) statBlock()->direction = 2;
		else if (inpt->pressing[RIGHT]) statBlock()->direction = 4;
		else if (inpt->pressing[DOWN]) statBlock()->direction = 6;
		// Adjust for ORTHO tilesets
		if (TILESET_ORIENTATION == TILESET_ORTHOGONAL &&
				(inpt->pressing[UP] || inpt->pressing[DOWN] ||
				inpt->pressing[LEFT] || inpt->pressing[RIGHT]))
			statBlock()->direction = statBlock()->direction == 7 ? 0 : statBlock()->direction + 1;
	}
}

void Avatar::handlePower(int actionbar_power) {
	if (actionbar_power != 0 && statBlock()->cooldown_ticks == 0) {
		const Power &power = powers->getPower(actionbar_power);
		Point target;
		if (MOUSE_AIM) {
			if (power.aim_assist)
				target = screen_to_map(inpt->mouse.x,  inpt->mouse.y + AIM_ASSIST, statBlock()->pos.x, statBlock()->pos.y);
			else
				target = screen_to_map(inpt->mouse.x,  inpt->mouse.y, statBlock()->pos.x, statBlock()->pos.y);
		} else {
			FPoint ftarget = calcVector(statBlock()->pos, statBlock()->direction, statBlock()->melee_range);
			target.x = static_cast<int>(ftarget.x);
			target.y = static_cast<int>(ftarget.y);
		}

		// check requirements
		if (!statBlock()->canUsePower(power, actionbar_power))
			return;
		if (power.requires_los && !map->collider.line_of_sight(statBlock()->pos.x, statBlock()->pos.y, target.x, target.y))
			return;
		if (power.requires_empty_target && !map->collider.is_empty(target.x, target.y))
			return;
		if (statBlock()->hero_cooldown[actionbar_power] > 0)
			return;
		if (!powers->hasValidTarget(actionbar_power, stats, target))
			return;

		statBlock()->hero_cooldown[actionbar_power] = power.cooldown; //set the cooldown timer
		current_power = actionbar_power;
		act_target = target;

		// is this a power that requires changing direction?
		if (power.face) {
			statBlock()->direction = face(target.x, target.y);
		}

		switch (power.new_state) {
			case POWSTATE_SWING:	// handle melee powers
				statBlock()->cur_state = AVATAR_MELEE;
				break;

			case POWSTATE_SHOOT:	// handle ranged powers
				statBlock()->cur_state = AVATAR_SHOOT;
				break;

			case POWSTATE_CAST:		// handle ment powers
				statBlock()->cur_state = AVATAR_CAST;
				break;

			case POWSTATE_BLOCK:	// handle blocking
				statBlock()->cur_state = AVATAR_BLOCK;
				statBlock()->effects.triggered_block = true;
				break;

			case POWSTATE_INSTANT:	// handle instant powers
				powers->activate(current_power, stats, target);
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
	map->collider.unblock(statBlock()->pos.x, statBlock()->pos.y);

	int stepfx;
	statBlock()->logic();
	if (statBlock()->effects.forced_move) {
		move();

		// calc new cam position from player position
		// cam is focused at player position
		map->cam.x = statBlock()->pos.x;
		map->cam.y = statBlock()->pos.y;
		map->hero_tile.x = statBlock()->pos.x / 32;
		map->hero_tile.y = statBlock()->pos.y / 32;

		map->collider.block(statBlock()->pos.x, statBlock()->pos.y);
		return;
	}
	if (statBlock()->effects.stun) {

		map->collider.block(statBlock()->pos.x, statBlock()->pos.y);
		return;
	}


	bool allowed_to_move;
	bool allowed_to_use_power;

	// check for revive
	if (statBlock()->hp <= 0 && statBlock()->effects.revive) {
		statBlock()->hp = statBlock()->maxhp;
		statBlock()->alive = true;
		statBlock()->corpse = false;
		statBlock()->cur_state = AVATAR_STANCE;
	}

	// check level up
	if (statBlock()->xp >= statBlock()->xp_table[statBlock()->level] && statBlock()->level < MAX_CHARACTER_LEVEL) {
		statBlock()->level_up = true;
		statBlock()->level++;
		stringstream ss;
		ss << msg->get("Congratulations, you have reached level %d!", statBlock()->level);
		if (statBlock()->level < statBlock()->max_spendable_stat_points) {
			ss << " " << msg->get("You may increase one attribute through the Character Menu.");
			newLevelNotification = true;
		}
		log_msg = ss.str();
		statBlock()->recalc();
		snd->play(level_up);

		// if the player managed to level up while dead (e.g. via a bleeding creature), restore to life
		if (statBlock()->cur_state == AVATAR_DEAD) {
			statBlock()->cur_state = AVATAR_STANCE;
		}
	}

	// check for bleeding spurt
	if (statBlock()->effects.damage > 0 && statBlock()->hp > 0) {
		comb->addMessage(statBlock()->effects.damage, statBlock()->pos, COMBAT_MESSAGE_TAKEDMG);
	}

	// check for bleeding to death
	if (statBlock()->hp == 0 && !(statBlock()->cur_state == AVATAR_DEAD)) {
		statBlock()->effects.triggered_death = true;
		statBlock()->cur_state = AVATAR_DEAD;
	}

	// assist mouse movement
	if (!inpt->pressing[MAIN1]) {
		drag_walking = false;
		attacking = false;
	} else {
		attacking = true;
	}

	// handle animation
	activeAnimation->advanceFrame();
	for (unsigned i=0; i < anims.size(); i++)
		if (anims[i] != NULL)
			anims[i]->advanceFrame();

	// handle transformation
	if (statBlock()->transform_type != "" && statBlock()->transform_type != "untransform" && transform_triggered == false) transform();
	if (statBlock()->transform_type != "" && statBlock()->transform_duration == 0) untransform();

	switch(statBlock()->cur_state) {
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
					statBlock()->cur_state = AVATAR_RUN;
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
				statBlock()->cur_state = AVATAR_STANCE;
				break;
			}
			else if (!move()) { // collide with wall
				statBlock()->cur_state = AVATAR_STANCE;
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
				powers->activate(current_power, stats, act_target);
			}

			if (activeAnimation->getTimesPlayed() >= 1) {
				statBlock()->cur_state = AVATAR_STANCE;
				if (statBlock()->effects.speed <= 100) statBlock()->cooldown_ticks += statBlock()->cooldown;
			}
			break;

		case AVATAR_CAST:

			setAnimation("ment");

			if (MOUSE_MOVE) lockCast = true;

			if (activeAnimation->isFirstFrame())
				snd->play(sound_mental);

			// do power
			if (activeAnimation->isActiveFrame()) {
				powers->activate(current_power, stats, act_target);
			}

			if (activeAnimation->getTimesPlayed() >= 1) {
				statBlock()->cur_state = AVATAR_STANCE;
				if (statBlock()->effects.speed <= 100) statBlock()->cooldown_ticks += statBlock()->cooldown;
			}
			break;


		case AVATAR_SHOOT:

			setAnimation("ranged");

			if (MOUSE_MOVE) lockShoot = true;

			// do power
			if (activeAnimation->isActiveFrame()) {
				powers->activate(current_power, stats, act_target);
			}

			if (activeAnimation->getTimesPlayed() >= 1) {
				statBlock()->cur_state = AVATAR_STANCE;
				if (statBlock()->effects.speed <= 100) statBlock()->cooldown_ticks += statBlock()->cooldown;
			}
			break;

		case AVATAR_BLOCK:

			setAnimation("block");

			if (powers->powers[actionbar_power].new_state != POWSTATE_BLOCK) {
				statBlock()->cur_state = AVATAR_STANCE;
				statBlock()->effects.triggered_block = false;
				statBlock()->effects.clearTriggerEffects(TRIGGER_BLOCK);
			}
			break;

		case AVATAR_HIT:

			setAnimation("hit");

			if (activeAnimation->isFirstFrame()) {
				statBlock()->effects.triggered_hit = true;
			}

			if (activeAnimation->getTimesPlayed() >= 1) {
				statBlock()->cur_state = AVATAR_STANCE;
			}

			break;

		case AVATAR_DEAD:
			if (statBlock()->effects.triggered_death) break;

			if (statBlock()->transformed) {
				statBlock()->transform_duration = 0;
				untransform();
			}

			setAnimation("die");

			if (activeAnimation->isFirstFrame() && activeAnimation->getTimesPlayed() < 1) {
				statBlock()->effects.clearEffects();

				// raise the death penalty flag.  Another module will read this and reset.
				statBlock()->death_penalty = true;

				// close menus in GameStatePlay
				close_menus = true;

				snd->play(sound_die);

				if (statBlock()->permadeath) {
					log_msg = msg->get("You are defeated. Game over! Press Enter to exit to Title.");
				}
				else {
					log_msg = msg->get("You are defeated.  You lose half your %s.  Press Enter to continue.", CURRENCY);
				}
			}

			if (activeAnimation->getTimesPlayed() >= 1) {
				statBlock()->corpse = true;
			}

			// allow respawn with Accept if not permadeath
			if (inpt->pressing[ACCEPT]) {
				map->teleportation = true;
				map->teleport_mapname = map->respawn_map;
				if (statBlock()->permadeath) {
					// set these positions so it doesn't flash before jumping to Title
					map->teleport_destination.x = statBlock()->pos.x;
					map->teleport_destination.y = statBlock()->pos.y;
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

	// turn on all passive powers
	if ((statBlock()->hp > 0 || statBlock()->effects.triggered_death) && !respawn) powers->activatePassives(stats);

	// calc new cam position from player position
	// cam is focused at player position
	map->cam.x = statBlock()->pos.x;
	map->cam.y = statBlock()->pos.y;
	map->hero_tile.x = statBlock()->pos.x / 32;
	map->hero_tile.y = statBlock()->pos.y / 32;

	// check for map events
	map->checkEvents(statBlock()->pos);

	// decrement all cooldowns
	for (int i = 0; i < POWER_COUNT; i++){
		statBlock()->hero_cooldown[i] -= 1000 / MAX_FRAMES_PER_SEC;
		if (statBlock()->hero_cooldown[i] < 0) statBlock()->hero_cooldown[i] = 0;
	}

	// make the current square solid
	map->collider.block(statBlock()->pos.x, statBlock()->pos.y);
}

/**
 * Called by HazardManager
 * Return false on a miss
 */
bool Avatar::takeHit(const Hazard &h) {

	if (statBlock()->cur_state != AVATAR_DEAD) {
		CombatText *combat_text = comb;
		// check miss
		int avoidance = statBlock()->avoidance;
		if (statBlock()->effects.triggered_block) avoidance *= 2;
		clampCeil(avoidance, MAX_AVOIDANCE);
		if (percentChance(avoidance - h.accuracy - 25)) {
			combat_text->addMessage(msg->get("miss"), statBlock()->pos, COMBAT_MESSAGE_MISS);
			return false;
		}

		int dmg = randBetween(h.dmg_min, h.dmg_max);

		// apply elemental resistance

		if (h.trait_elemental >= 0 && unsigned(h.trait_elemental) < statBlock()->vulnerable.size()) {
			unsigned i = h.trait_elemental;
			int vulnerable = statBlock()->vulnerable[i];
			if (statBlock()->vulnerable[i] > MAX_RESIST && statBlock()->vulnerable[i] < 100)
				vulnerable = MAX_RESIST;
			dmg = (dmg * vulnerable) / 100;
		}

		if (!h.trait_armor_penetration) { // armor penetration ignores all absorption
			// apply absorption
			int absorption = randBetween(statBlock()->absorb_min, statBlock()->absorb_max);

			if (statBlock()->effects.triggered_block) {
				absorption += absorption + statBlock()->absorb_max; // blocking doubles your absorb amount
			}

			if (absorption > 0) {
				if ((dmg*100)/absorption > MAX_BLOCK)
					absorption = (absorption * MAX_BLOCK) /100;
				if ((dmg*100)/absorption > MAX_ABSORB && !statBlock()->effects.triggered_block)
					absorption = (absorption * MAX_ABSORB) /100;

				// Sometimes, the absorb limits cause absorbtion to drop to 1
				// This could be confusing to a player that has something with an absorb of 1 equipped
				// So we round absorption up in this case
				if (absorption == 0) absorption = 1;
			}

			dmg = dmg - absorption;
			if (dmg <= 0) {
				dmg = 0;
				if (h.trait_elemental < 0) {
					if (statBlock()->effects.triggered_block && MAX_BLOCK < 100) dmg = 1;
					else if (!statBlock()->effects.triggered_block && MAX_ABSORB < 100) dmg = 1;
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


		int prev_hp = statBlock()->hp;
		combat_text->addMessage(dmg, statBlock()->pos, COMBAT_MESSAGE_TAKEDMG);
		statBlock()->takeDamage(dmg);

		// after effects
		if (statBlock()->hp > 0 && dmg > 0) {

			if (h.mod_power > 0) powers->effect(stats, h.mod_power);
			powers->effect(stats, h.power_index);

			if (!statBlock()->effects.immunity) {
				if (statBlock()->effects.forced_move) {
					float theta = powers->calcTheta(h.src_stats->pos.x, h.src_stats->pos.y, statBlock()->pos.x, statBlock()->pos.y);
					statBlock()->forced_speed.x = static_cast<int>(ceil(statBlock()->effects.forced_speed * cos(theta)));
					statBlock()->forced_speed.y = static_cast<int>(ceil(statBlock()->effects.forced_speed * sin(theta)));
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
			powers->activate(h.post_power, h.src_stats, statBlock()->pos);
		}

		if (statBlock()->hp <= 0) {
			statBlock()->effects.triggered_death = true;
			statBlock()->cur_state = AVATAR_DEAD;
		}
		else if (prev_hp > statBlock()->hp) { // only interrupt if damage was taken
			snd->play(sound_hit);
			if (!percentChance(statBlock()->poise)) {
				statBlock()->cur_state = AVATAR_HIT;
			}
		}

		return true;
	}
	return false;
}


void Avatar::transform() {

	transform_triggered = true;
	statBlock()->transformed = true;
	setPowers = true;

	delete charmed_stats;
	charmed_stats = new AvatarStatBlock();
	charmed_stats->load("enemies/" + statBlock()->transform_type + ".txt");

	// temporary save hero stats
	delete hero_stats;
	hero_stats = new AvatarStatBlock();

	// replace some hero stats
	statBlock()->speed = charmed_stats->speed;
	statBlock()->dspeed = charmed_stats->dspeed;
	statBlock()->flying = charmed_stats->flying;
	statBlock()->humanoid = charmed_stats->humanoid;
	statBlock()->animations = charmed_stats->animations;
	statBlock()->powers_list = charmed_stats->powers_list;
	statBlock()->effects.clearEffects();

	string animationname = "animations/enemies/"+charmed_stats->animations + ".txt";
	anim->decreaseCount("animations/hero.txt");
	anim->increaseCount(animationname);
	animationSet = anim->getAnimationSet(animationname);
	delete activeAnimation;
	activeAnimation = animationSet->getAnimation();
	statBlock()->cur_state = AVATAR_STANCE;

	// damage
	clampFloor(statBlock()->dmg_melee_min, charmed_stats->dmg_melee_min);
	clampFloor(statBlock()->dmg_melee_max, charmed_stats->dmg_melee_max);

	clampFloor(statBlock()->dmg_ment_min, charmed_stats->dmg_ment_min);
	clampFloor(statBlock()->dmg_ment_max, charmed_stats->dmg_ment_max);

	clampFloor(statBlock()->dmg_ranged_min, charmed_stats->dmg_ranged_min);
	clampFloor(statBlock()->dmg_ranged_max, charmed_stats->dmg_ranged_max);

	// dexterity
	clampFloor(statBlock()->absorb_min, charmed_stats->absorb_min);
	clampFloor(statBlock()->absorb_max, charmed_stats->absorb_max);

	clampFloor(statBlock()->avoidance, charmed_stats->avoidance);

	clampFloor(statBlock()->accuracy, charmed_stats->accuracy);

	clampFloor(statBlock()->crit, charmed_stats->crit);

	// resistances
	for (unsigned int i=0; i<statBlock()->vulnerable.size(); i++)
		clampCeil(statBlock()->vulnerable[i], charmed_stats->vulnerable[i]);

	loadSounds(charmed_stats->sfx_prefix);
	loadStepFX("NULL");
}

void Avatar::untransform() {
	// Only allow untransform when on a valid tile
	if (!map->collider.is_valid_position(statBlock()->pos.x,statBlock()->pos.y,MOVEMENT_NORMAL)) return;

	statBlock()->transformed = false;
	transform_triggered = false;
	statBlock()->transform_type = "";
	revertPowers = true;
	statBlock()->effects.clearEffects();

	// revert some hero stats to last saved
	statBlock()->speed = hero_stats->speed;
	statBlock()->dspeed = hero_stats->dspeed;
	statBlock()->flying = hero_stats->flying;
	statBlock()->humanoid = hero_stats->humanoid;
	statBlock()->animations = hero_stats->animations;
	statBlock()->effects = hero_stats->effects;
	statBlock()->powers_list = hero_stats->powers_list;

	anim->increaseCount("animations/hero.txt");
	anim->decreaseCount("animations/enemies/"+charmed_stats->animations + ".txt");
	animationSet = anim->getAnimationSet("animations/hero.txt");
	delete activeAnimation;
	activeAnimation = animationSet->getAnimation();
	statBlock()->cur_state = AVATAR_STANCE;

	// This is a bit of a hack.
	// In order to switch to the stance animation, we can't already be in a stance animation
	setAnimation("run");

	statBlock()->dmg_melee_min = hero_stats->dmg_melee_min;
	statBlock()->dmg_melee_max = hero_stats->dmg_melee_max;
	statBlock()->dmg_ment_min = hero_stats->dmg_ment_min;
	statBlock()->dmg_ment_max = hero_stats->dmg_ment_max;
	statBlock()->dmg_ranged_min = hero_stats->dmg_ranged_min;
	statBlock()->dmg_ranged_max = hero_stats->dmg_ranged_max;

	statBlock()->absorb_min = hero_stats->absorb_min;
	statBlock()->absorb_max = hero_stats->absorb_max;
	statBlock()->avoidance = hero_stats->avoidance;
	statBlock()->accuracy = hero_stats->accuracy;
	statBlock()->crit = hero_stats->crit;

	for (unsigned int i=0; i<statBlock()->vulnerable.size(); i++) {
		statBlock()->vulnerable[i] = hero_stats->vulnerable[i];
	}

	loadSounds();
	loadStepFX(statBlock()->sfx_step);

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
	if (!statBlock()->transformed) {
		for (unsigned i = 0; i < layer_def[statBlock()->direction].size(); ++i) {
			unsigned index = layer_def[statBlock()->direction][i];
			if (anims[index]) {
				Renderable ren = anims[index]->getCurrentFrame(statBlock()->direction);
				ren.map_pos = statBlock()->pos;
				ren.prio = i+1;
				r.push_back(ren);
			}
		}
	} else {
		Renderable ren = activeAnimation->getCurrentFrame(statBlock()->direction);
		ren.map_pos = statBlock()->pos;
		r.push_back(ren);
	}
	// add effects
	for (unsigned i = 0; i < statBlock()->effects.effect_list.size(); ++i) {
		if (statBlock()->effects.effect_list[i].animation && !statBlock()->effects.effect_list[i].animation->isCompleted()) {
			Renderable ren = statBlock()->effects.effect_list[i].animation->getCurrentFrame(0);
			ren.map_pos = statBlock()->pos;
			if (statBlock()->effects.effect_list[i].render_above) ren.prio = layer_def[statBlock()->direction].size()+1;
			else ren.prio = 0;
			r.push_back(ren);
		}
	}
}

Avatar::~Avatar() {

	if (statBlock()->transformed && charmed_stats && charmed_stats->animations != "") {
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
