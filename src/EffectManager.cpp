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
 */

#include "Animation.h"
#include "AnimationSet.h"
#include "EffectManager.h"

using namespace std;

EffectManager::EffectManager()
	: bleed_dmg(0)
	, hpot(0)
	, forced_speed(0)
	, immunity(false)
	, slow(false)
	, stun(false)
	, immobilize(false)
	, haste(false)
	, forced_move(false)
{
}

EffectManager::~EffectManager() {
	for (unsigned i=0; i<effect_list.size(); i++) {
		removeAnimation(i);
	}
}

void EffectManager::logic() {
	bleed_dmg = 0;
	hpot = 0;
	forced_speed = 0;
	immunity = false;
	slow = false;
	stun = false;
	immobilize = false;
	haste = false;
	forced_move = false;

	for (unsigned i=0; i<effect_list.size(); i++) {
		if (effect_list[i].duration > 0) {
			if (effect_list[i].type == "bleed" && effect_list[i].ticks % 30 == 1) bleed_dmg += effect_list[i].magnitude;
			else if (effect_list[i].type == "hpot" && effect_list[i].ticks % 30 == 1) hpot += effect_list[i].magnitude;
			else if (effect_list[i].type == "immunity") immunity = true;
			else if (effect_list[i].type == "slow") slow = true;
			else if (effect_list[i].type == "stun") stun = true;
			else if (effect_list[i].type == "immobilize") immobilize = true;
			else if (effect_list[i].type == "haste") haste = true;
			else if (effect_list[i].type == "forced_move") {
				forced_move = true;
				forced_speed = effect_list[i].magnitude;
			}

			if (effect_list[i].ticks > 0) effect_list[i].ticks--;
			if (effect_list[i].ticks == 0) removeEffect(i);
		}
		if (effect_list[i].shield_maxhp > 0) {
			if (effect_list[i].shield_hp == 0) removeEffect(i);
		}
		if (effect_list[i].animation) {
			if (!effect_list[i].animation->isCompleted())
				effect_list[i].animation->advanceFrame();
		}
	}
}

void EffectManager::addEffect(int _id, int _icon, int _duration, int _shield_hp, int _magnitude, std::string _type, std::string _animation) {
	// if we're already immune, don't add negative effects
	if (immunity) {
		if (_type == "bleed") return;
		else if (_type == "slow") return;
		else if (_type == "stun") return;
		else if (_type == "immobilize") return;
	}

	// only allow one forced_move effect
	// TODO remove this limitation
	if (forced_move) {
		if (_type == "forced_move") return;
	}

	for (unsigned i=0; i<effect_list.size(); i++) {
		if (effect_list[i].id == _id) {
			if (effect_list[i].duration <= _duration) {
				effect_list[i].ticks = effect_list[i].duration = _duration;
				if (effect_list[i].animation) effect_list[i].animation->reset();
			}
			if (effect_list[i].shield_maxhp <= _shield_hp) {
				effect_list[i].shield_hp = effect_list[i].shield_hp = _shield_hp;
				if (effect_list[i].animation) effect_list[i].animation->reset();
			}
			if (effect_list[i].magnitude <= _magnitude) {
				effect_list[i].magnitude = _magnitude;
				if (effect_list[i].animation) effect_list[i].animation->reset();
			}
			return; // we already have this effect
		}
		// if we're adding an immunity effect, remove all negative effects
		if (_type == "immunity") {
			clearNegativeEffects();
		}
	}

	Effect e;

	e.id = _id;
	e.icon = _icon;
	e.ticks = e.duration = _duration;
	e.shield_hp = e.shield_maxhp = _shield_hp;
	e.magnitude = _magnitude;
	e.type = _type;

	if (_animation != "") {
		anim->increaseCount(_animation);
		e.animation = loadAnimation(_animation);
		e.animation_name = _animation;
	}

	effect_list.push_back(e);
}

void EffectManager::removeEffect(int _id) {
	removeAnimation(_id);
	effect_list.erase(effect_list.begin()+_id);
}

void EffectManager::removeAnimation(int _id) {
	if (effect_list[_id].animation && effect_list[_id].animation_name != "") {
		anim->decreaseCount(effect_list[_id].animation_name);
		delete effect_list[_id].animation;
		effect_list[_id].animation = NULL;
		effect_list[_id].animation_name = "";
	}
}

void EffectManager::removeEffectType(std::string _type) {
	for (unsigned i=0; i<effect_list.size(); i++) {
		if (effect_list[i].type == _type) removeEffect(i);
	}
}

void EffectManager::clearEffects() {
	for (unsigned i=0; i<effect_list.size(); i++) {
		removeEffect(i);
	}
}

void EffectManager::clearNegativeEffects() {
	for (unsigned i=0; i<effect_list.size(); i++) {
		if (effect_list[i].type == "bleed") removeEffect(i);
		else if (effect_list[i].type == "slow") removeEffect(i);
		else if (effect_list[i].type == "stun") removeEffect(i);
		else if (effect_list[i].type == "immobilize") removeEffect(i);
	}
}

int EffectManager::damageShields(int _dmg) {
	int over_dmg = _dmg;

	for (unsigned i=0; i<effect_list.size(); i++) {
		if (effect_list[i].shield_maxhp > 0) {
			effect_list[i].shield_hp -= _dmg;
			if (effect_list[i].shield_hp < 0) {
				if (abs(effect_list[i].shield_hp) < over_dmg) over_dmg = abs(effect_list[i].shield_hp);
				effect_list[i].shield_hp = 0;
			} else {
				over_dmg = 0;
			}
		}
	}

	return over_dmg;
}

Animation* EffectManager::loadAnimation(std::string &s) {
	if (s != "") {
		AnimationSet *animationSet = anim->getAnimationSet(s);
		return animationSet->getAnimation(animationSet->starting_animation);
	}
	return NULL;
}

