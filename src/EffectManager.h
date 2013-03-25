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


#pragma once
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

class Effect{
public:
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
	int trigger;
	bool render_above;
	int passive_id;

	Effect()
	 : id(0)
	 , icon(-1)
	 , ticks(0)
	 , duration(-1)
	 , type("")
	 , magnitude(0)
	 , magnitude_max(0)
	 , animation_name("")
	 , animation(NULL)
	 , item(false)
	 , trigger(-1)
	 , render_above(false)
	 , passive_id(0)
	{}

	~Effect() {
	}

};

class EffectManager {
private:
	Animation* loadAnimation(std::string &s);
	void removeEffect(int id);
	void removeAnimation(int id);

public:
	EffectManager();
	~EffectManager();
	EffectManager& operator= (const EffectManager &emSource);
	void clearStatus();
	void logic();
	void addEffect(int id, int icon, int duration, int magnitude, std::string type, std::string animation, bool additive, bool item, int trigger, bool render_above, int passive_id);
	void removeEffectType(std::string type);
	void removeEffectPassive(int id);
	void clearEffects();
	void clearNegativeEffects();
	void clearItemEffects();
	void clearTriggerEffects(int trigger);
	int damageShields(int dmg);

	std::vector<Effect> effect_list;

	int damage;
	int hpot;
	int mpot;
	int speed;
	bool immunity;
	bool stun;
	int forced_speed;
	bool forced_move;
	bool revive;

	int bonus_hp;
	int bonus_hp_regen;
	int bonus_hp_percent;
	int bonus_mp;
	int bonus_mp_regen;
	int bonus_mp_percent;
	int bonus_accuracy;
	int bonus_avoidance;
	int bonus_crit;
	int bonus_offense;
	int bonus_defense;
	int bonus_physical;
	int bonus_mental;
	int bonus_xp;
	int bonus_currency;
	int bonus_item_find;
	int bonus_stealth;
	int bonus_poise;
	std::vector<int> bonus_resist;

	bool triggered_others;
	bool triggered_block;
	bool triggered_hit;
	bool triggered_halfdeath;
	bool triggered_joincombat;
	bool triggered_death;
};

#endif
