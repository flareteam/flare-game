/*
Copyright © 2011-2012 Clint Bellanger
Copyright © 2012 Stefan Beller

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

#include "Loot.h"

Loot::Loot() {
	stack.item = 0;
	stack.quantity = 0;
	pos.x = 0;
	pos.y = 0;
	animation = NULL;
	currency = 0;
	tip.clear();
	gfx = "";
}

Loot::Loot(const Loot &other) {
	stack.item = other.stack.item;
	stack.quantity = other.stack.quantity;
	pos.x = other.pos.x;
	pos.y = other.pos.y;
	loadAnimation(other.gfx);
	animation->syncTo(other.animation);
	currency = other.currency;
	tip = other.tip;
}

// The assignment operator mainly used in internal vector managing,
// e.g. in vector::erase()
Loot& Loot::operator= (const Loot &other) {
	delete animation;
	loadAnimation(other.gfx);
	animation->syncTo(other.animation);

	stack.item = other.stack.item;
	stack.quantity = other.stack.quantity;
	pos.x = other.pos.x;
	pos.y = other.pos.y;
	currency = other.currency;
	tip = other.tip;

	return *this;
}

void Loot::loadAnimation(std::string _gfx) {
	gfx = _gfx;
	if (gfx != "") {
			anim->increaseCount(gfx);
			AnimationSet *as = anim->getAnimationSet(gfx);
			animation = as->getAnimation(as->starting_animation);
	}
}

bool Loot::isFlying() {
	 return !animation->isLastFrame();
}

Loot::~Loot() {
	if (gfx != "")
			anim->decreaseCount(gfx);
	delete animation;
}

