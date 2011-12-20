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
 * class MenuPowers
 */

#ifndef MENU_POWERS_H
#define MENU_POWERS_H

#include "InputState.h"
#include "Utils.h"
#include "FontEngine.h"
#include "StatBlock.h"
#include "WidgetTooltip.h"
#include "PowerManager.h"
#include "WidgetButton.h"
#include "SharedResources.h"

#include <SDL.h>
#include <SDL_image.h>

class MenuPowers {
private:
	StatBlock *stats;
	PowerManager *powers;

	SDL_Surface *background;
	SDL_Surface *powers_step;
	SDL_Surface *powers_unlock;
	WidgetButton *closeButton;
	
	WidgetLabel label_powers;
	WidgetLabel label_p1;
	WidgetLabel label_p2;
	WidgetLabel label_m1;
	WidgetLabel label_m2;
	WidgetLabel label_o1;
	WidgetLabel label_o2;
	WidgetLabel label_d1;
	WidgetLabel label_d2;
	WidgetLabel stat_po;
	WidgetLabel stat_pd;
	WidgetLabel stat_mo;
	WidgetLabel stat_md;
	
	void loadGraphics();
	void displayBuild(int value, int x);

public:
	MenuPowers(StatBlock *_stats, PowerManager *_powers);
	~MenuPowers();
	void logic();
	void render();
	TooltipData checkTooltip(Point mouse);
	bool requirementsMet(int power_index);
	int click(Point mouse);
	
	bool visible;
	SDL_Rect slots[20]; // the location of power slots

};

#endif
