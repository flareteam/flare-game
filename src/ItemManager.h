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
 * class ItemManager
 */

#ifndef ITEM_MANAGER_H
#define ITEM_MANAGER_H

#include "UtilsParsing.h"
#include "StatBlock.h"
#include "WidgetTooltip.h"
#include "SharedResources.h"

#include <SDL.h>
#include <SDL_image.h>
#include <SDL_mixer.h>

#include <string>


const int MAX_ITEM_ID = 10000;

const int ICON_SIZE_32 = 32;
const int ICON_SIZE_64 = 64;

const int ITEM_TYPE_OTHER = -1;
const int ITEM_TYPE_MAIN = 0;
const int ITEM_TYPE_BODY = 1;
const int ITEM_TYPE_OFF = 2;
const int ITEM_TYPE_ARTIFACT = 3;
const int ITEM_TYPE_CONSUMABLE = 4;
const int ITEM_TYPE_GEM = 5;
const int ITEM_TYPE_QUEST = 6;

const int REQUIRES_PHYS = 0;
const int REQUIRES_MENT = 1;
const int REQUIRES_OFF = 2;
const int REQUIRES_DEF = 3;

const int SFX_NONE = -1;
const int SFX_BOOK = 0;
const int SFX_CLOTH = 1;
const int SFX_COINS = 2;
const int SFX_GEM = 3;
const int SFX_LEATHER = 4;
const int SFX_METAL = 5;
const int SFX_PAGE = 6;
const int SFX_MAILLE = 7;
const int SFX_OBJECT = 8;
const int SFX_HEAVY = 9;
const int SFX_WOOD = 10;
const int SFX_POTION = 11;

const int ITEM_QUALITY_LOW = 0;
const int ITEM_QUALITY_NORMAL = 1;
const int ITEM_QUALITY_HIGH = 2;
const int ITEM_QUALITY_EPIC = 3;

const int ITEM_MAX_BONUSES = 8;

struct Item {
	std::string name;          // item name displayed on long and short tool tips
	int level;            // rough estimate of quality, used in the loot algorithm
	int quality;          // low, normal, high, epic; corresponds to item name color
	int type;             // equipment slot or base item type
	int icon32;           // icon index on the 32x32 pixel sheet
	int icon64;           // icon index on the 64x64 pixel sheet (used for equippable items)
	int dmg_min;          // minimum damage amount (weapons only)
	int dmg_max;          // maximum damage amount (weapons only)
	int abs_min;          // minimum absorb amount (armors and shields only)
	int abs_max;          // maximum absorb amount (armors and shields only)
	int req_stat;         // physical, mental, offense, defense
	int req_val;          // 1-5 (used with req_stat)
	std::string *bonus_stat;   // stat to increase/decrease e.g. hp, accuracy, speed
	int *bonus_val;       // amount to increase (used with bonus_stat)
	int sfx;              // the item sound when it hits the floor or inventory, etc
	std::string gfx;           // the sprite layer shown when this item is equipped
	std::string loot;          // the flying loot animation for this item
	int power;            // this item can be dragged to the action bar and used as a power
	int power_mod;        // alter powers when this item is equipped (e.g. shoot arrows from bows)
	std::string power_desc;    // shows up in green text on the tooltip
	int price;            // if price = 0 the item cannot be sold
	int max_quantity;     // max count per stack
	int rand_loot;        // max amount appearing in a loot stack
	int rand_vendor;      // max amount appearing in a vendor stack
	std::string pickup_status; // when this item is picked up, set a campaign state (usually for quest items)
	std::string stepfx;        // sound effect played when walking (armors only)

	Item() {
		name = "";
		level = 0;
		quality = ITEM_QUALITY_NORMAL;
		icon32 = 0;
		icon64 = 0;
		type = -1;
		dmg_min = 0;
		dmg_max = 0;
		abs_min = 0;
		abs_max = 0;
		req_stat = 0;
		req_val = 0;
		sfx = SFX_NONE;
		gfx = "";
		loot = "";
		power = -1;
		power_mod = -1;
		power_desc = "";
		price = 0;
		max_quantity = 1;
		rand_loot = 1;
		rand_vendor = 1;
		pickup_status = "";
		stepfx = "";
	}
};

struct ItemStack {
	int item;
	int quantity;
	bool operator > (ItemStack param);
};

class ItemManager {
private:
	SDL_Surface *icons32;
	SDL_Surface *icons64; // item db is the only module that currently uses the 64px icons
	SDL_Rect src;
	SDL_Rect dest;
	Mix_Chunk *sfx[12];

public:
	ItemManager();
	~ItemManager();
	void load(const std::string& filename);
	void loadAll();
	void loadSounds();
	void loadIcons();
	void renderIcon(ItemStack stack, int x, int y, int size);
	void playSound(int item);
	void playCoinsSound();
	TooltipData getTooltip(int item, StatBlock *stats, bool vendor_view);
	TooltipData getShortTooltip(ItemStack item);

	Item *items;
	int vendor_ratio;
};

#endif
