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

#pragma once
#ifndef LOOT_H
#define LOOT_H

#include <iostream>
#include "ItemManager.h"
#include "Settings.h"

class Loot {
private:
	std::string gfx;

public:
	ItemStack stack;
	Point pos;
	Animation *animation;
	int currency;
	TooltipData tip;

	Loot();
	Loot(const Loot &other);
	Loot& operator= (const Loot &other);
	~Loot();

	void loadAnimation(std::string _gfx);

	/**
	 * If an item is flying, it hasn't completed its "flying loot" animation.
	 * Only allow loot to be picked up if it is grounded.
	 */
	bool isFlying();
};

#endif // LOOT_H
