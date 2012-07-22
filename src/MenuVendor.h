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
 * class MenuVendor
 */

#ifndef MENU_VENDOR_H
#define MENU_VENDOR_H

#include "MenuItemStorage.h"

#include <SDL.h>
#include <SDL_image.h>

#include <string>

class InputState;
class NPC;
class StatBlock;
class WidgetButton;

class MenuVendor : public Menu {
private:
	ItemManager *items;
	StatBlock *stats;
	WidgetButton *closeButton;

	void loadGraphics();
	SDL_Surface *background;
	MenuItemStorage stock; // items the vendor currently has in stock

	int VENDOR_SLOTS;

	// label and widget positions
	Point close_pos;
	Point title_pos;
	Point name_pos;
	int slots_cols;
	int slots_rows;

public:
	MenuVendor(ItemManager *items, StatBlock *stats);
	~MenuVendor();

	NPC *npc;

	void update();
	void loadMerchant(const std::string&);
	void logic();
	void render();
	ItemStack click(InputState * input);
	void itemReturn(ItemStack stack);
	void add(ItemStack stack);
	TooltipData checkTooltip(Point mouse);
	bool full();
	void setInventory();
	void saveInventory();

	bool visible;
	bool talker_visible;
	SDL_Rect slots_area;
};


#endif
