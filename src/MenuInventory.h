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
 * class MenuInventory
 */

#ifndef MENU_INVENTORY_H
#define MENU_INVENTORY_H

#include "MenuItemStorage.h"
#include "WidgetLabel.h"

#include <SDL.h>
#include <SDL_image.h>

#include <string>

class InputState;
class ItemManager;
class PowerManager;
class StatBlock;
class WidgetButton;

const int EQUIPMENT = 0;
const int CARRIED = 1;

class MenuInventory : public Menu {
private:
	ItemManager *items;
	StatBlock *stats;
	PowerManager *powers;

	void loadGraphics();
	int areaOver(Point mouse);
	void updateEquipment(int slot);

	SDL_Surface *background;
	WidgetButton *closeButton;

	int MAX_EQUIPPED;
	int MAX_CARRIED;

	// label and widget positions
	Point close_pos;
	LabelInfo title;
	LabelInfo currency_lbl;
	SDL_Rect help_pos;
	int carried_cols;
	int carried_rows;
	SDL_Color color_normal;
	SDL_Color color_high;

public:
	MenuInventory(ItemManager *items, StatBlock *stats, PowerManager *powers);
	~MenuInventory();
	void update();
	void logic();
	void render();
	TooltipData checkTooltip(Point mouse);

	ItemStack click(InputState * input);
	void itemReturn(ItemStack stack);
	void drop(Point mouse, ItemStack stack);
	void activate(InputState * input);

	void add( ItemStack stack, int area = CARRIED, int slot = -1);
	void remove(int item);
	void addCurrency(int count);
	bool buy(ItemStack stack, int tab);
	bool sell(ItemStack stack);
	bool stashAdd(ItemStack stack);

	bool full(int item);
	int getItemCountCarried(int item);
	bool isItemEquipped(int item);
	bool requirementsMet(int item);

	void applyEquipment(ItemStack *equipped);
	void applyItemStats(ItemStack *equipped);
	void applyItemSetBonuses(ItemStack *equipped);

	void clearHighlight();

	SDL_Rect carried_area;
	std::vector<SDL_Rect> equipped_area;
	std::vector<std::string> slot_type;
	std::vector<std::string> slot_desc;

	MenuItemStorage inventory[2];
	int currency;
	int drag_prev_src;

	// the following two are separate because artifacts don't display on the hero.
	// so we only update the hero sprites when non-artifact changes occur.
	bool changed_equipment;
	bool changed_artifact;

	std::string log_msg;

};

#endif

