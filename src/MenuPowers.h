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
 * class MenuPowers
 */

#ifndef MENU_POWERS_H
#define MENU_POWERS_H

#include "Utils.h"
#include "WidgetButton.h"

#include <SDL.h>
#include <SDL_image.h>

#include <vector>

class PowerManager;
class StatBlock;
class TooltipData;

struct Power_Menu_Cell {
	int id;
	Point pos;
	int requires_physoff;
	int requires_physdef;
	int requires_mentoff;
	int requires_mentdef;
	bool requires_point;
};

class MenuPowers : public Menu {
private:
	StatBlock *stats;
	PowerManager *powers;
	Power_Menu_Cell power_cell[20];

	SDL_Surface *background;
	SDL_Surface *icons;
	SDL_Surface *powers_tree;
	SDL_Surface *powers_unlock;
	WidgetButton *closeButton;
	WidgetButton *plusButton[20];
	Point close_pos;
	Point unspent_pos;
	int points_left;

	WidgetLabel label_powers;
	WidgetLabel stat_up;

	void loadGraphics();
	void displayBuild(int power_id);
	void renderIcon(int icon_id, int x, int y);
	bool powerUnlockable(int power_index);

public:
	MenuPowers(StatBlock *_stats, PowerManager *_powers, SDL_Surface *_icons);
	~MenuPowers();
	void update();
	void logic();
	void render();
	TooltipData checkTooltip(Point mouse);
	bool requirementsMet(int power_index);
	int click(Point mouse);

	bool visible;
	SDL_Rect slots[20]; // the location of power slots
	std::vector<int> powers_list;

};

#endif
