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

EffectManager::EffectManager() {
}

EffectManager::~EffectManager() {
}

void EffectManager::logic() {
	for (unsigned i=0; i<effect_list.size(); i++) {
		if (effect_list[i].duration > 0) {
			if (effect_list[i].ticks > 0) effect_list[i].ticks--;
			if (effect_list[i].ticks == 0) {
				if (effect_list[i].animation && effect_list[i].animation_name != "") {
					anim->decreaseCount(effect_list[i].animation_name);
					delete effect_list[i].animation;
				}
				effect_list.erase(effect_list.begin()+i);
			}
		}
	}
}

void EffectManager::addEffect(int _id, int _icon, int _duration, std::string _type, std::string _animation) {
	for (unsigned i=0; i<effect_list.size(); i++) {
		if (effect_list[i].id == _id) return; // we already have this effect
	}

	Effect e;

	e.id = _id;
	e.icon = _icon;
	e.ticks = e.duration = _duration;
	e.type = _type;

	if (_animation != "") {
		e.animation = loadAnimation(_animation);
		if (e.animation) {
			anim->increaseCount(_animation);
			e.animation_name = _animation;
		}
	}

	effect_list.push_back(e);
}

Animation* EffectManager::loadAnimation(std::string &s) {
	if (s != "") {
		AnimationSet *animationSet = anim->getAnimationSet(s);
		return animationSet->getAnimation(animationSet->starting_animation);
	}
	return NULL;
}

