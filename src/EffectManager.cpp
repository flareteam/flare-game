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
#include "Settings.h"

using namespace std;

EffectManager::EffectManager() {
	bonus_resist = std::vector<int>(ELEMENTS.size(), 0);
	clearStatus();
}

EffectManager::~EffectManager() {
	for (unsigned i=0; i<effect_list.size(); i++) {
		removeAnimation(i);
	}
}

void EffectManager::clearStatus() {
	bleed_dmg = 0;
	hpot = 0;
	mpot = 0;
	speed = 100;
	immunity = false;
	stun = false;
	forced_speed = 0;
	forced_move = false;

	bonus_hp = 0;
	bonus_hp_regen = 0;
	bonus_mp = 0;
	bonus_mp_regen = 0;
	bonus_accuracy = 0;
	bonus_avoidance = 0;
	bonus_crit = 0;
	bonus_offense = 0;
	bonus_defense = 0;
	bonus_physical = 0;
	bonus_mental = 0;

	bonus_xp = 0;
	bonus_currency = 0;

	for (unsigned i=0; i<bonus_resist.size(); i++) {
		bonus_resist[i] = 0;
	}
}

void EffectManager::logic() {
	clearStatus();

	for (unsigned i=0; i<effect_list.size(); i++) {
		// expire timed effects and total up magnitudes of active effects
		if (effect_list[i].duration >= 0) {
			if (effect_list[i].type == "bleed" && effect_list[i].ticks % MAX_FRAMES_PER_SEC == 1) bleed_dmg += effect_list[i].magnitude;
			else if (effect_list[i].type == "hpot" && effect_list[i].ticks % MAX_FRAMES_PER_SEC == 1) hpot += effect_list[i].magnitude;
			else if (effect_list[i].type == "mpot" && effect_list[i].ticks % MAX_FRAMES_PER_SEC == 1) mpot += effect_list[i].magnitude;
			else if (effect_list[i].type == "speed") speed = (effect_list[i].magnitude * speed) / 100;
			else if (effect_list[i].type == "immunity") immunity = true;
			else if (effect_list[i].type == "stun") stun = true;
			else if (effect_list[i].type == "forced_move") {
				forced_move = true;
				forced_speed = effect_list[i].magnitude;
			}
			else if (effect_list[i].type == "hp") bonus_hp += effect_list[i].magnitude;
			else if (effect_list[i].type == "hp_regen") bonus_hp_regen += effect_list[i].magnitude;
			else if (effect_list[i].type == "mp") bonus_mp += effect_list[i].magnitude;
			else if (effect_list[i].type == "mp_regen") bonus_mp_regen += effect_list[i].magnitude;
			else if (effect_list[i].type == "accuracy") bonus_accuracy += effect_list[i].magnitude;
			else if (effect_list[i].type == "avoidance") bonus_avoidance += effect_list[i].magnitude;
			else if (effect_list[i].type == "crit") bonus_crit += effect_list[i].magnitude;
			else if (effect_list[i].type == "offense") bonus_offense += effect_list[i].magnitude;
			else if (effect_list[i].type == "defense") bonus_defense += effect_list[i].magnitude;
			else if (effect_list[i].type == "physical") bonus_physical += effect_list[i].magnitude;
			else if (effect_list[i].type == "mental") bonus_mental += effect_list[i].magnitude;
			else if (effect_list[i].type == "xp") bonus_xp += effect_list[i].magnitude;
			else if (effect_list[i].type == "currency") bonus_currency += effect_list[i].magnitude;
			else {
				for (unsigned j=0; j<bonus_resist.size(); j++) {
					if (effect_list[i].type == ELEMENTS[j].name + "_resist")
						bonus_resist[j] += effect_list[i].magnitude;
				}
			}

			if (effect_list[i].duration > 0) {
				if (effect_list[i].ticks > 0) effect_list[i].ticks--;
				if (effect_list[i].ticks == 0) removeEffect(i);
			}
		}
		// expire shield effects
		if (effect_list[i].magnitude_max > 0 && effect_list[i].magnitude == 0) {
			if (effect_list[i].type == "shield") removeEffect(i);
		}
		// expire effects based on animations
		if ((effect_list[i].animation && effect_list[i].animation->isLastFrame()) || !effect_list[i].animation) {
			if (effect_list[i].type == "heal") removeEffect(i);
		}

		// animate
		if (effect_list[i].animation) {
			if (!effect_list[i].animation->isCompleted())
				effect_list[i].animation->advanceFrame();
		}
	}
}

void EffectManager::addEffect(int _id, int _icon, int _duration, int _magnitude, std::string _type, std::string _animation, bool _additive, bool _item) {
	// if we're already immune, don't add negative effects
	if (immunity) {
		if (_type == "bleed") return;
		else if (_type == "speed" && _magnitude < 100) return;
		else if (_type == "stun") return;
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
			if (_additive) {
				effect_list[i].magnitude += _magnitude;
				effect_list[i].magnitude_max += _magnitude;
			} else if (effect_list[i].magnitude_max <= _magnitude) {
				effect_list[i].magnitude = effect_list[i].magnitude_max = _magnitude;
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
	e.magnitude = e.magnitude_max = _magnitude;
	e.type = _type;
	e.item = _item;

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
		else if (effect_list[i].type == "speed" && effect_list[i].magnitude_max < 100) removeEffect(i);
		else if (effect_list[i].type == "stun") removeEffect(i);
	}
}

void EffectManager::clearItemEffects() {
	for (unsigned i=0; i<effect_list.size(); i++) {
		if (effect_list[i].item) removeEffect(i);
	}
}

int EffectManager::damageShields(int _dmg) {
	int over_dmg = _dmg;

	for (unsigned i=0; i<effect_list.size(); i++) {
		if (effect_list[i].magnitude_max > 0 && effect_list[i].type == "shield") {
			effect_list[i].magnitude -= _dmg;
			if (effect_list[i].magnitude < 0) {
				if (abs(effect_list[i].magnitude) < over_dmg) over_dmg = abs(effect_list[i].magnitude);
				effect_list[i].magnitude = 0;
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

