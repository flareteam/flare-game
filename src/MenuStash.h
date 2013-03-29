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
 * class MenuStash
 */


#pragma once
#ifndef MENU_STASH_H
#define MENU_STASH_H

#include "MenuItemStorage.h"
#include "WidgetLabel.h"

#include <SDL.h>
#include <SDL_image.h>

#include <string>

class InputState;
class NPC;
class StatBlock;
class WidgetButton;

class MenuStash : public Menu {
private:
	ItemManager *items;
	StatBlock *stats;
	WidgetButton *closeButton;

	int STASH_SLOTS;

	// label and widget positions
	Point close_pos;
	LabelInfo title;
	int slots_cols;
	int slots_rows;
	SDL_Color color_normal;

public:
	MenuStash(ItemManager *items, StatBlock *stats);
	~MenuStash();

	void update();
	void logic();
	void render();
	ItemStack click(InputState * input);
	void itemReturn(ItemStack stack);
	void add(ItemStack stack);
	TooltipData checkTooltip(Point mouse);
	bool full(int item);
	void drop(Point mouse, ItemStack stack);
	void add(ItemStack stack, int slot);

	SDL_Rect slots_area;
	MenuItemStorage stock;
	bool updated;
};


#endif
