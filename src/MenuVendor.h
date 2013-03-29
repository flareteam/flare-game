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


#pragma once
#ifndef MENU_VENDOR_H
#define MENU_VENDOR_H

#include "MenuItemStorage.h"
#include "WidgetLabel.h"

#include <SDL.h>
#include <SDL_image.h>

#include <string>

class InputState;
class ItemStorage;
class NPC;
class StatBlock;
class WidgetButton;
class WidgetTabControl;

class MenuVendor : public Menu {
private:
	ItemManager *items;
	StatBlock *stats;
	WidgetButton *closeButton;
	WidgetTabControl *tabControl;

	MenuItemStorage stock[2]; // items the vendor currently has in stock

	int VENDOR_SLOTS;

	// label and widget positions
	Point close_pos;
	LabelInfo title;
	int slots_cols;
	int slots_rows;
	int activetab;
	SDL_Color color_normal;

public:
	MenuVendor(ItemManager *items, StatBlock *stats);
	~MenuVendor();

	NPC *npc;
	ItemStorage buyback_stock;

	void update();
	void loadMerchant(const std::string&);
	void logic();
	void tabsLogic();
	void setTab(int tab);
	int getTab() {return activetab;}
	void render();
	ItemStack click(InputState * input);
	void itemReturn(ItemStack stack);
	void add(ItemStack stack);
	TooltipData checkTooltip(Point mouse);
	void setInventory();
	void saveInventory();
	void sort(int type);

	bool talker_visible;
	SDL_Rect slots_area;
};


#endif
