/*
Copyright © 2011-2012 Clint Bellanger
Copyright © 2012 Igor Paliychuk
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
 * class ItemManager
 */


#pragma once
#ifndef ITEM_MANAGER_H
#define ITEM_MANAGER_H

#include "WidgetTooltip.h"

#include <SDL.h>
#include <SDL_image.h>
#include <SDL_mixer.h>

#include <map>
#include <string>
#include <stdint.h>
#include <vector>

#define VENDOR_BUY 0
#define VENDOR_SELL 1
#define PLAYER_INV 2

class StatBlock;

const int REQUIRES_PHYS = 0;
const int REQUIRES_MENT = 1;
const int REQUIRES_OFF = 2;
const int REQUIRES_DEF = 3;

const int ITEM_QUALITY_LOW = 0;
const int ITEM_QUALITY_NORMAL = 1;
const int ITEM_QUALITY_HIGH = 2;
const int ITEM_QUALITY_EPIC = 3;

const int ITEM_MAX_BONUSES = 8;

class Set_bonus {
public:
	int requirement;
	std::string bonus_stat;
	int bonus_val;
	Set_bonus()
		: requirement(0)
		, bonus_stat("")
		, bonus_val(0)
	{}
};

class Item {
public:
	std::string name;     // item name displayed on long and short tool tips
	std::string flavor;   // optional flavor text describing the item
	int level;            // rough estimate of quality, used in the loot algorithm
	int set;              // item can be attached to item set
	int quality;          // low, normal, high, epic; corresponds to item name color
	std::string type;     // equipment slot or base item type
	int icon;             // icon index on small pixel sheet
	int dmg_melee_min;    // minimum damage amount (melee)
	int dmg_melee_max;    // maximum damage amount (melee)
	int dmg_ranged_min;   // minimum damage amount (ranged)
	int dmg_ranged_max;   // maximum damage amount (ranged)
	int dmg_ment_min;     // minimum damage amount (mental)
	int dmg_ment_max;     // maximum damage amount (mental)
	int abs_min;          // minimum absorb amount
	int abs_max;          // maximum absorb amount
	int req_stat;         // physical, mental, offense, defense
	int req_val;          // 1-5 (used with req_stat)
	std::vector<std::string> bonus_stat;   // stat to increase/decrease e.g. hp, accuracy, speed
	std::vector<int> bonus_val;       // amount to increase (used with bonus_stat)
	SoundManager::SoundID sfx;        // the item sound when it hits the floor or inventory, etc
	std::string gfx;           // the sprite layer shown when this item is equipped
	std::string loot_animation;// the flying loot animation for this item
	int power;            // this item can be dragged to the action bar and used as a power
	int power_mod;        // alter powers when this item is equipped (e.g. shoot arrows from bows)
	std::string power_desc;    // shows up in green text on the tooltip
	int price;            // if price = 0 the item cannot be sold
	int price_sell;       // if price_sell = 0, the sell price is price*vendor_ratio
	int max_quantity;     // max count per stack
	int rand_loot;        // max amount appearing in a loot stack
	int rand_vendor;      // max amount appearing in a vendor stack
	std::string pickup_status; // when this item is picked up, set a campaign state (usually for quest items)
	std::string stepfx;        // sound effect played when walking (armors only)

	int getSellPrice();

	Item()
	: name("")
	, flavor("")
	, level(0)
	, set(0)
	, quality(ITEM_QUALITY_NORMAL)
	, type("other")
	, icon(0)
	, dmg_melee_min(0)
	, dmg_melee_max(0)
	, dmg_ranged_min(0)
	, dmg_ranged_max(0)
	, dmg_ment_min(0)
	, dmg_ment_max(0)
	, abs_min(0)
	, abs_max(0)
	, req_stat(0)
	, req_val(0)
	, sfx(0)
	, gfx("")
	, loot_animation("")
	, power(0)
	, power_mod(0)
	, power_desc("")
	, price(0)
	, price_sell(0)
	, max_quantity(1)
	, rand_loot(1)
	, rand_vendor(1)
	, pickup_status("")
	, stepfx("")
	{}

	~Item() {
	}
};

class ItemSet {
public:
	std::string name;            // item set name displayed on long and short tool tips
	std::vector<int> items;      // items, included into set
	std::vector<Set_bonus> bonus;// vector with stats to increase/decrease
	SDL_Color color;

	ItemSet()
	: name("")
	{
		color.r = 255;
		color.g = 255;
		color.b = 255;
	}

	~ItemSet() {
	}
};

class ItemStack {
public:
	ItemStack()
		: item(0)
		, quantity(0)
	{}
	~ItemStack() {}
	int item;
	int quantity;
	bool operator > (const ItemStack &param) const;
};

class ItemManager {
private:
	SDL_Surface *icons;

	void load(const std::string& filename);
	void loadTypes(const std::string& filename);
	void loadSets(const std::string& filename);
	void loadAll();
	void loadIcons();

	SDL_Color color_normal;
	SDL_Color color_low;
	SDL_Color color_high;
	SDL_Color color_epic;
	SDL_Color color_bonus;
	SDL_Color color_penalty;
	SDL_Color color_requirements_not_met;
	SDL_Color color_flavor;

public:
	ItemManager();
	~ItemManager();
	void renderIcon(ItemStack stack, int x, int y, int size);
	void playSound(int item, Point pos = Point(0,0));
	TooltipData getTooltip(int item, StatBlock *stats, int context);
	TooltipData getShortTooltip(ItemStack item);
	std::string getItemType(std::string _type);

	std::vector<Item> items;
	std::map<std::string,std::string> item_types;
	std::vector<ItemSet> item_sets;

	std::vector<std::string> item_class_names; // a vector of all defined classes of items
	// belongs to the item_class_names vector and contains a vector of item ids which belong to that specific class.
	std::vector<std::vector<unsigned int> > item_class_items;
};

#endif
