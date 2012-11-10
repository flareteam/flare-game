/*
Copyright © 2012 Justin Jacobs

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
 * class EffectManager
 *
 * Holds the collection of hazards (active attacks, spells, etc) and handles group operations
 */

#ifndef EFFECT_MANAGER_H
#define EFFECT_MANAGER_H

#include "Animation.h"
#include "SharedResources.h"
#include "Utils.h"

#include <string>
#include <vector>

class Avatar;
class EnemyManager;
class Hazard;
class MapCollision;
class PowerManager;

struct Effect{
	int id;
	int icon;
	int ticks;
	int duration;
	std::string type;
	int magnitude;
	int magnitude_max;
	std::string animation_name;
	Animation* animation;
	bool item;

	Effect() {
		id = 0;
		icon = -1;
		ticks = 0;
		duration = -1;
		type = "";
		magnitude = 0;
		magnitude_max = 0;
		animation_name = "";
		animation = NULL;
		item = false;
	}

	~Effect() {
	}

};

class EffectManager {
private:
	Animation* loadAnimation(std::string &s);
	void removeEffect(int _id);
	void removeAnimation(int _id);

public:
	EffectManager();
	~EffectManager();
	void clearStatus();
	void logic();
	void addEffect(int _id, int _icon, int _duration, int _magnitude, std::string _type, std::string _animation, bool _additive, bool _item);
	void removeEffectType(std::string _type);
	void clearEffects();
	void clearNegativeEffects();
	void clearItemEffects();
	int damageShields(int _dmg);

	std::vector<Effect> effect_list;

	int bleed_dmg;
	int hpot;
	int mpot;
	int speed;
	bool immunity;
	bool stun;
	int forced_speed;
	bool forced_move;

	int bonus_hp;
	int bonus_hp_regen;
	int bonus_mp;
	int bonus_mp_regen;
	int bonus_accuracy;
	int bonus_avoidance;
	int bonus_crit;
	int bonus_offense;
	int bonus_defense;
	int bonus_physical;
	int bonus_mental;
	std::vector<int> bonus_resist;
};

#endif
