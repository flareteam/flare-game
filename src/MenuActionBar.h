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
 * class ActionBar
 *
 * Handles the config, display, and usage of the 0-9 hotkeys, mouse buttons, and menu calls
 */

#ifndef MENU_ACTION_BAR_H
#define MENU_ACTION_BAR_H

#include "Utils.h"

#include <SDL.h>
#include <SDL_image.h>
#include "WidgetLabel.h"

class PowerManager;
class StatBlock;
class TooltipData;
class WidgetLabel;

const int MENU_CHARACTER = 0;
const int MENU_INVENTORY = 1;
const int MENU_POWERS = 2;
const int MENU_LOG = 3;

class MenuActionBar : public Menu {
private:
	void renderCooldowns();
	void renderItemCounts();

	SDL_Surface *background;
	SDL_Surface *emptyslot;
	SDL_Surface *icons;
	SDL_Surface *disabled;
    SDL_Surface *attention;

	StatBlock *hero;
	PowerManager *powers;
	SDL_Rect src;

	WidgetLabel *labels[16];
	int default_M1;
	Point last_mouse;

public:

	MenuActionBar(PowerManager *_powers, StatBlock *hero, SDL_Surface *icons);
	~MenuActionBar();
	void loadGraphics();
	void renderIcon(int icon_id, int x, int y);
	void renderAttention(int menu_id);
	void logic();
	void render();
	int checkAction(Point mouse);
	int checkDrag(Point mouse);
	void checkMenu(Point mouse, bool &menu_c, bool &menu_i, bool &menu_p, bool &menu_l);
	void drop(Point mouse, int power_index, bool rearranging);
	void actionReturn(int power_index);
	void remove(Point mouse);
	void set(int power_id[12]);
	void clear();
	void update();

	TooltipData checkTooltip(Point mouse);

	int hotkeys[12]; // refer to power_index in PowerManager
	int actionbar[12]; // temp for shapeshifting
	bool locked[12]; // if slot is locked, you cannot drop it
	SDL_Rect slots[12]; // the location of hotkey slots
	SDL_Rect menus[4]; // the location of the menu buttons
	int slot_item_count[12]; // -1 means this power isn't item based.  0 means out of items.  1+ means sufficient items.
	bool slot_enabled[12];
    bool requires_attention[4];

	// these store the area occupied by these hotslot sections.
	// useful for detecting mouse interactions on those locations
	SDL_Rect numberArea;
	SDL_Rect mouseArea;
	SDL_Rect menuArea;
	int drag_prev_slot;

};

#endif
