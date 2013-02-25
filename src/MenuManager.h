/*
Copyright © 2011-2012 Clint Bellanger
Copyright © 2013 Henrik Andersson

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
 * class MenuManager
 */


#pragma once
#ifndef MENU_MANAGER_H
#define MENU_MANAGER_H

#include <SDL.h>
#include <SDL_image.h>
#include <SDL_mixer.h>

#include "ItemManager.h"
#include "WidgetTooltip.h"

class Menu;
class MenuInventory;
class MenuPowers;
class MenuCharacter;
class MenuLog;
class MenuHUDLog;
class MenuActionBar;
class MenuStatBar;
class MenuMiniMap;
class MenuNPCActions;
class MenuEnemy;
class MenuVendor;
class MenuTalker;
class MenuExit;
class MenuActiveEffects;
class MenuStash;
class CampaignManager;
class ItemManager;
class PowerManager;
class StatBlock;

const int DRAG_SRC_POWERS = 1;
const int DRAG_SRC_INVENTORY = 2;
const int DRAG_SRC_ACTIONBAR = 3;
const int DRAG_SRC_VENDOR = 4;
const int DRAG_SRC_STASH = 5;

class MenuManager {
private:

	SDL_Surface *icons;

	PowerManager *powers;
	StatBlock *stats;
	CampaignManager *camp;

	TooltipData tip_buf;

	bool key_lock;
	void loadIcons();

	bool dragging;
	ItemStack drag_stack;
	int drag_power;
	int drag_src;

	bool done;

public:
	MenuManager(PowerManager *powers, StatBlock *stats, CampaignManager *camp, ItemManager *items);
	MenuManager(const MenuManager &copy); // not implemented
	~MenuManager();
	void logic();
	void render();
	void renderIcon(int icon_id, int x, int y);
	void closeAll();
	void closeLeft();
	void closeRight();

	std::vector<Menu*> menus;
	ItemManager *items;
	MenuInventory *inv;
	MenuPowers *pow;
	MenuCharacter *chr;
	MenuLog *log;
	MenuHUDLog *hudlog;
	MenuActionBar *act;
	MenuStatBar *hp;
	MenuStatBar *mp;
	MenuStatBar *xp;
	WidgetTooltip *tip;
	MenuMiniMap *mini;
	MenuNPCActions *npc;
	MenuEnemy *enemy;
	MenuVendor *vendor;
	MenuTalker *talker;
	MenuExit *exit;
	MenuActiveEffects *effects;
	MenuStash *stash;

	bool pause;
	bool menus_open;
	ItemStack drop_stack;

	bool requestingExit() { return done; }
};

#endif
