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
#ifndef AVATAR_H
#define AVATAR_H


#include "Entity.h"
#include "SharedResources.h"
#include "Utils.h"

#include <SDL.h>
#include <SDL_image.h>

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

	std::string img_main;
	std::string img_armor;
	std::string img_off;

public:
	Avatar(PowerManager *_powers, MapIso *_map);
	~Avatar();

	void init();
	void loadGraphics(const std::string& img_main, std::string img_armor, const std::string& img_off);
	void loadSounds();
	void loadStepFX(const std::string& stepname);

	void logic(int actionbar_power, bool restrictPowerUse);
	bool pressing_move();
	void set_direction();
	bool takeHit(Hazard h);
	std::string log_msg;

	virtual Renderable getRender();

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

