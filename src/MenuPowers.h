/*
Copyright © 2011-2012 Clint Bellanger
Copyright © 2012 Igor Paliychuk
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

/**
 * class MenuPowers
 */


#pragma once
#ifndef MENU_POWERS_H
#define MENU_POWERS_H

#include "Menu.h"
#include "Utils.h"
#include "WidgetButton.h"
#include "WidgetLabel.h"
#include "WidgetTabControl.h"

#include <SDL.h>
#include <SDL_image.h>

#include <vector>

class PowerManager;
class StatBlock;
class TooltipData;

class Power_Menu_Cell {
public:
	short id;
	short tab;
	Point pos;
	short requires_physoff;
	short requires_physdef;
	short requires_mentoff;
	short requires_mentdef;
	short requires_defense;
	short requires_offense;
	short requires_physical;
	short requires_mental;
	short requires_level;
	short requires_power;
	bool requires_point;
	bool passive_on;
	Power_Menu_Cell() {
		id = -1;
		tab = 0;
		pos.x = 0;
		pos.y = 0;
		requires_mentdef = 0;
		requires_mentoff = 0;
		requires_physoff = 0;
		requires_physdef = 0;
		requires_defense = 0;
		requires_offense = 0;
		requires_physical = 0;
		requires_mental = 0;
		requires_level = 0;
		requires_power = 0;
		requires_point = false;
		passive_on = false;
	}
};

class MenuPowers : public Menu {
private:
	StatBlock *stats;
	PowerManager *powers;
	std::vector<Power_Menu_Cell> power_cell;

	SDL_Surface *background;
	SDL_Surface *icons;
	std::vector<SDL_Surface*> tree_surf;
	SDL_Surface *powers_unlock;
	SDL_Surface *overlay_disabled;
	WidgetButton *closeButton;
	bool pressed;

	LabelInfo title;
	LabelInfo unspent_points;
	Point close_pos;
	SDL_Rect tab_area;

	short points_left;
	short tabs_count;
	std::vector<std::string> tab_titles;
	std::vector<std::string> tree_image_files;

	WidgetLabel label_powers;
	WidgetLabel stat_up;
	WidgetTabControl *tabControl;

	void loadGraphics();
	void displayBuild(int power_id);
	void renderIcon(int icon_id, int x, int y);
	bool powerUnlockable(int power_index);
	void renderPowers(int tab_num);

	SDL_Color color_bonus;
	SDL_Color color_penalty;

	short id_by_powerIndex(short power_index);

public:
	static MenuPowers *getInstance();
	MenuPowers(StatBlock *_stats, PowerManager *_powers, SDL_Surface *_icons);
	~MenuPowers();
	void update();
	void logic();
	void render();
	TooltipData checkTooltip(Point mouse);
	bool baseRequirementsMet(int power_index);
	bool requirementsMet(int power_index);
	int click(Point mouse);
	bool unlockClick(Point mouse);
	bool meetsUsageStats(unsigned powerid);
	short getUnspent() { return points_left; }

	std::vector<SDL_Rect> slots; // the location of power slots

};
extern MenuPowers *menuPowers;
#endif
