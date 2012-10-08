/*
Copyright © 2011-2012 Clint Bellanger
Copyright © 2012 Igor Paliychuk

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
#ifndef AVATAR_H
#define AVATAR_H


#include "Entity.h"
#include "SharedResources.h"
#include "Utils.h"

#include <SDL.h>
#include <SDL_image.h>
#include <SDL_mixer.h>

#include <math.h>
#include <string>

class Entity;
class Hazard;
class PowerManager;
class StatBlock;

/**
 * Avatar State enum
 */
enum AvatarState {
	AVATAR_STANCE = 0,
	AVATAR_RUN = 1,
	AVATAR_MELEE = 2,
	AVATAR_BLOCK = 3,
	AVATAR_HIT = 4,
	AVATAR_DEAD = 5,
	AVATAR_CAST = 6,
	AVATAR_SHOOT = 7
};

struct Layer_gfx {
	std::string gfx;
	std::string type;
	Layer_gfx() {
		gfx = "";
		type = "";
	}
};

class Avatar : public Entity {
private:

	PowerManager *powers;

	bool lockSwing;
	bool lockCast;
	bool lockShoot;
	bool animFwd;

	Mix_Chunk *sound_melee;
	Mix_Chunk *sound_hit;
	Mix_Chunk *sound_die;
	Mix_Chunk *sound_block;
	Mix_Chunk *sound_steps[4];
	Mix_Chunk *level_up;

	void setAnimation(std::string name);
	std::vector<AnimationSet*> animsets; // hold the animations for all equipped items in the right order of drawing.
	std::vector<Animation*> anims; // hold the animations for all equipped items in the right order of drawing.

	short body;

	bool transform_triggered;
	std::string last_transform;
	int getUntransformPower();

public:
	Avatar(PowerManager *_powers, MapRenderer *_map);
	~Avatar();

	void init();
	void loadLayerDefinitions();
	std::vector<std::string> layer_reference_order;
	std::vector<std::vector<unsigned> > layer_def;
	void loadGraphics(std::vector<Layer_gfx> _img_gfx);
	void loadSounds();
	void loadStepFX(const std::string& stepname);

	void logic(int actionbar_power, bool restrictPowerUse);
	bool pressing_move();
	void set_direction();
	bool takeHit(const Hazard &h);
	std::string log_msg;

	// transformation handling
	void transform();
	void untransform();
	bool setPowers;
	bool revertPowers;
	int untransform_power;
	StatBlock *hero_stats;
	StatBlock *charmed_stats;

	virtual Renderable getRender() { return Renderable(); }
	void addRenders(std::vector<Renderable> &r);

	// vars
	Hazard *haz;
	int current_power;
	Point act_target;
	bool drag_walking;
	bool newLevelNotification;

private:
	void handlePower(int actionbar_power);
};

#endif

