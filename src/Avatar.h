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
#ifndef AVATAR_H
#define AVATAR_H


#include <sstream>
#include "SDL.h"
#include "SDL_image.h"
#include "SDL_mixer.h"

#include "Entity.h"
#include "Utils.h"
#include "InputState.h"
#include "MapIso.h"
#include "StatBlock.h"
#include "Hazard.h"
#include "PowerManager.h"
#include "MessageEngine.h"

// AVATAR State enum
const int AVATAR_STANCE = 0;
const int AVATAR_RUN = 1;
const int AVATAR_MELEE = 2;
const int AVATAR_BLOCK = 3;
const int AVATAR_HIT = 4;
const int AVATAR_DEAD = 5;
const int AVATAR_CAST = 6;
const int AVATAR_SHOOT = 7;

class Avatar : public Entity {
private:
	
	PowerManager *powers;
	InputState *inp;

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

	string img_main;
	string img_armor;
	string img_off;

public:
	Avatar(PowerManager *_powers, InputState *_inp, MapIso *_map);
	~Avatar();
	
	void init();
	void loadGraphics(string img_main, string img_armor, string img_off);
	void loadSounds();
	
	void logic(int actionbar_power, bool restrictPowerUse);
	bool pressing_move();	
	void set_direction();
	bool takeHit(Hazard h);
	string log_msg;

	virtual Renderable getRender();

	// vars
	Hazard *haz;
	int current_power;
	Point act_target;
	bool drag_walking;
};

#endif

