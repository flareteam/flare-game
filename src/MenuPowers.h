/*
Copyright © 2011-2012 Clint Bellanger
Copyright © 2012 Igor Paliychuk

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

const int POWER_SLOTS_COUNT = 60;

struct Power_Menu_Cell {
	int id;
	int tab;
	Point pos;
	int requires_physoff;
	int requires_physdef;
	int requires_mentoff;
	int requires_mentdef;
	int requires_defense;
	int requires_offense;
	int requires_physical;
	int requires_mental;
	int requires_level;
	int requires_power;
	bool requires_point;
};

class MenuPowers : public Menu {
private:
	StatBlock *stats;
	PowerManager *powers;
	Power_Menu_Cell power_cell[POWER_SLOTS_COUNT];

	SDL_Surface *background;
	SDL_Surface *icons;
	std::vector<SDL_Surface*> tree_surf;
	SDL_Surface *powers_unlock;
	SDL_Surface *overlay_disabled;
	WidgetButton *closeButton;
	bool pressed;

	LabelInfo title_lbl;
	Point close_pos;
	Point unspent_pos;
	SDL_Rect tab_area;

	int points_left;
	int tabs_count;
	std::vector<std::string> tab_titles;
	std::vector<std::string> tree_image_files;

	WidgetLabel label_powers;
	WidgetLabel stat_up;
	WidgetTabControl * tabControl;

	void loadGraphics();
	void displayBuild(int power_id);
	void renderIcon(int icon_id, int x, int y);
	bool powerUnlockable(int power_index);
	void renderPowers(int tab_num);

public:
	static MenuPowers *getInstance();
	MenuPowers(StatBlock *_stats, PowerManager *_powers, SDL_Surface *_icons);
	~MenuPowers();
	void update();
	void logic();
	void render();
	TooltipData checkTooltip(Point mouse);
	bool requirementsMet(int power_index);
	int click(Point mouse);
	void unlock_click(Point mouse);
	bool meetsUsageStats(unsigned powerid);

	bool visible;
	SDL_Rect slots[POWER_SLOTS_COUNT]; // the location of power slots
	std::vector<int> powers_list;

};
extern MenuPowers *menuPowers;
#endif
