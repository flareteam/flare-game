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
 * MenuActiveEffects
 *
 * Handles the display of active effects (buffs/debuffs)
 */

#ifndef MENU_ACTIVE_EFFECTS_H
#define MENU_ACTIVE_EFFECTS_H

#include "Utils.h"

#include <SDL.h>
#include <SDL_image.h>
#include <vector>

class StatBlock;

class MenuActiveEffects {
private:
	SDL_Surface *icons;
	SDL_Surface *timer;
	std::vector<std::string> effects;

	struct limits {
		int slow;
		int bleed;
		int stun;
		int immobilize;
		int immunity;
		int transform;
		int haste;
		int hot;
		int shield;
	}limits;

	void renderIcon(int icon_id, int index, int current, int max);
	
public:
	MenuActiveEffects(SDL_Surface *_icons);
	~MenuActiveEffects();
	void loadGraphics();
	void render(StatBlock *stats);
};

#endif
